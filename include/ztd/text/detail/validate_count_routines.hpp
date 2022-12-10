// =============================================================================
//
// ztd.text
// Copyright © 2022 JeanHeyd "ThePhD" Meneide and Shepherd's Oasis, LLC
// Contact: opensource@soasis.org
//
// Commercial License Usage
// Licensees holding valid commercial ztd.text licenses may use this file in
// accordance with the commercial license agreement provided with the
// Software or, alternatively, in accordance with the terms contained in
// a written agreement between you and Shepherd's Oasis, LLC.
// For licensing terms and conditions see your agreement. For
// further information contact opensource@soasis.org.
//
// Apache License Version 2 Usage
// Alternatively, this file may be used under the terms of Apache License
// Version 2.0 (the "License") for non-commercial use; you may not use this
// file except in compliance with the License. You may obtain a copy of the
// License at
//
//		http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
//
// ============================================================================ //

#pragma once

#ifndef ZTD_TEXT_DETAIL_VALIDATE_COUNT_ROUTINES_HPP
#define ZTD_TEXT_DETAIL_VALIDATE_COUNT_ROUTINES_HPP

#include <ztd/text/version.hpp>

#include <ztd/text/validate_result.hpp>
#include <ztd/text/count_result.hpp>
#include <ztd/text/state.hpp>
#include <ztd/text/code_point.hpp>
#include <ztd/text/code_unit.hpp>
#include <ztd/text/transcode.hpp>
#include <ztd/text/detail/pass_through_handler.hpp>
#include <ztd/text/detail/progress_handler.hpp>
#include <ztd/text/detail/is_lossless.hpp>

#include <ztd/idk/span.hpp>
#include <ztd/idk/tag.hpp>
#include <ztd/ranges/adl.hpp>
#include <ztd/ranges/unbounded.hpp>
#include <ztd/ranges/subrange.hpp>
#include <ztd/ranges/blackhole_iterator.hpp>
#include <ztd/ranges/algorithm.hpp>

#include <array>
#include <algorithm>

#include <ztd/prologue.hpp>

namespace ztd { namespace text {
	ZTD_TEXT_INLINE_ABI_NAMESPACE_OPEN_I_

	namespace __txt_detail {

		template <typename _Input, typename _Encoding, typename _Output, typename _DecodeState, typename _EncodeState,
			typename _PivotRange>
		constexpr auto __basic_validate_decodable_as_one(_Input&& __input, _Encoding&& __encoding, _Output& __output,
			_DecodeState& __decode_state, _EncodeState& __encode_state, pivot<_PivotRange> __pivot) {

			using _UInput  = remove_cvref_t<_Input>;
			using _UOutput = remove_cvref_t<_Output>;
			using _InSubRange
				= ranges::subrange<ranges::range_iterator_t<_UInput>, ranges::range_sentinel_t<_UInput>>;
			using _OutSubRange
				= ranges::subrange<ranges::range_iterator_t<_UOutput>, ranges::range_sentinel_t<_UOutput>>;

			_InSubRange __working_input(::std::forward<_Input>(__input));
			_OutSubRange __working_output(__output);
			__pass_through_handler __error_handler {};

			using _Result = validate_result<ranges::tag_range_reconstruct_t<_UInput, _InSubRange>, _DecodeState>;

			auto __transcode_result = transcode_one_into(__working_input, __encoding, __working_output, __encoding,
				__error_handler, __error_handler, __decode_state, __encode_state, __pivot);
			if (__transcode_result.error_code != encoding_error::ok) {
				return _Result(
					ranges::reconstruct(::std::in_place_type<_UInput>, ::std::move(__transcode_result.input)),
					false, __decode_state);
			}

			const bool __is_equal_transcode = ranges::__rng_detail::__equal(::ztd::ranges::begin(__working_input),
				::ztd::ranges::begin(__transcode_result.input), ::ztd::ranges::begin(__working_output),
				::ztd::ranges::begin(__transcode_result.output));
			if (!__is_equal_transcode) {
				return _Result(
					ranges::reconstruct(::std::in_place_type<_UInput>, ::std::move(__transcode_result.input)),
					false, __decode_state);
			}
			return _Result(ranges::reconstruct(::std::in_place_type<_UInput>, ::std::move(__transcode_result.input)),
				true, __decode_state);
		}

		template <typename _Input, typename _Encoding, typename _DecodeState, typename _EncodeState>
		constexpr auto __basic_validate_decodable_as_one(
			_Input&& __input, _Encoding&& __encoding, _DecodeState& __decode_state, _EncodeState& __encode_state) {
			using _UEncoding = remove_cvref_t<_Encoding>;
			using _CodeUnit  = code_unit_t<_UEncoding>;
			using _CodePoint = code_point_t<_UEncoding>;

			_CodePoint __intermediate_storage[max_code_points_v<_UEncoding>] {};
			::ztd::span<_CodePoint, max_code_points_v<_UEncoding>> __intermediate(__intermediate_storage);
			_CodeUnit __output_storage[max_code_units_v<_UEncoding>] {};
			::ztd::span<_CodeUnit, max_code_units_v<_UEncoding>> __output(__output_storage);

			pivot<::ztd::span<_CodePoint, max_code_points_v<_UEncoding>>> __pivot { __intermediate,
				encoding_error::ok };
			return __basic_validate_decodable_as_one(::std::forward<_Input>(__input),
				::std::forward<_Encoding>(__encoding), __output, __decode_state, __encode_state, __pivot);
		}

		template <typename _Input, typename _Encoding, typename _CodeUnitContainer, typename _EncodeState,
			typename _DecodeState, typename _Intermediate>
		constexpr auto __basic_validate_encodable_as_one(_Input&& __input, _Encoding&& __encoding,
			_CodeUnitContainer& __output, _EncodeState& __encode_state, _DecodeState& __decode_state,
			_Intermediate& __intermediate) {

			using _UInput        = remove_cvref_t<_Input>;
			using _UIntermediate = remove_cvref_t<_Intermediate>;
			using _InSubRange
				= ranges::subrange<ranges::range_iterator_t<_UInput>, ranges::range_sentinel_t<_UInput>>;
			using _IntermediateSubRange = ranges::subrange<ranges::range_iterator_t<_UIntermediate>,
				ranges::range_sentinel_t<_UIntermediate>>;
			using _Result = validate_result<ranges::tag_range_reconstruct_t<_UInput, _InSubRange>, _EncodeState>;

			__pass_through_handler __error_handler {};
			_InSubRange __working_input(::std::forward<_Input>(__input));

			auto __encode_result = __basic_encode_one<__consume::__no>(
				__working_input, __encoding, __output, __error_handler, __encode_state);
			if (__encode_result.error_code != encoding_error::ok) {
				return _Result(
					ranges::reconstruct(::std::in_place_type<_UInput>, ::std::move(__encode_result.input)), false,
					__encode_state);
			}

			_IntermediateSubRange __working_intermediate(__intermediate);

			auto __decode_result = __basic_decode_one<__consume::__no>(
				__output, __encoding, __working_intermediate, __error_handler, __decode_state);
			if (__decode_result.error_code != encoding_error::ok) {
				return _Result(
					ranges::reconstruct(::std::in_place_type<_UInput>, ::std::move(__encode_result.input)), false,
					__encode_state);
			}

			const bool __is_equal_transcode = ranges::__rng_detail::__equal(::ztd::ranges::begin(__working_input),
				::ztd::ranges::begin(__encode_result.input), ::ztd::ranges::begin(__working_intermediate),
				::ztd::ranges::begin(__decode_result.output));
			if (!__is_equal_transcode) {
				return _Result(
					ranges::reconstruct(::std::in_place_type<_UInput>, ::std::move(__encode_result.input)), false,
					__encode_state);
			}
			return _Result(ranges::reconstruct(::std::in_place_type<_UInput>, ::std::move(__encode_result.input)),
				true, __encode_state);
		}

		template <typename _Input, typename _Encoding, typename _EncodeState, typename _DecodeState>
		constexpr auto __basic_validate_encodable_as_one(
			_Input&& __input, _Encoding&& __encoding, _EncodeState& __encode_state, _DecodeState& __decode_state) {
			using _UEncoding = remove_cvref_t<_Encoding>;
			using _CodePoint = code_point_t<_UEncoding>;
			using _CodeUnit  = code_unit_t<_UEncoding>;

			_CodeUnit __output_storage[max_code_units_v<_UEncoding>] {};
			::ztd::span<_CodeUnit, max_code_units_v<_UEncoding>> __output(__output_storage);
			_CodePoint __intermediate_storage[max_code_points_v<_UEncoding>] {};
			::ztd::span<_CodePoint, max_code_points_v<_UEncoding>> __intermediate(__intermediate_storage);
			return __basic_validate_encodable_as_one(::std::forward<_Input>(__input),
				::std::forward<_Encoding>(__encoding), __output, __encode_state, __decode_state, __intermediate);
		}

		template <typename _Input, typename _Encoding, typename _ErrorHandler, typename _State,
			typename _Intermediate>
		constexpr auto __basic_count_as_encoded_one(_Input&& __input, _Encoding&& __encoding,
			_ErrorHandler&& __error_handler, _State& __state, _Intermediate& __intermediate) {
			using _Result = count_result<ranges::reconstruct_t<_Input>, _State>;

			auto __intermediate_result = __basic_encode_one<__consume::__no>(::std::forward<_Input>(__input),
				::std::forward<_Encoding>(__encoding), __intermediate,
				::std::forward<_ErrorHandler>(__error_handler), __state);
			::std::size_t __written    = static_cast<::std::size_t>(
                    ::ztd::ranges::data(__intermediate_result.output) - ::ztd::ranges::data(__intermediate));

			return _Result(::std::move(__intermediate_result.input), __written, __intermediate_result.state,
				__intermediate_result.error_code, __intermediate_result.error_count);
		}

		template <typename _Input, typename _Encoding, typename _ErrorHandler, typename _State>
		constexpr auto __basic_count_as_encoded_one(
			_Input&& __input, _Encoding&& __encoding, _ErrorHandler&& __error_handler, _State& __state) {
			using _UEncoding                                  = remove_cvref_t<_Encoding>;
			using _IntermediateCodeUnit                       = code_unit_t<_UEncoding>;
			constexpr ::std::size_t _IntermediateMaxCodeUnits = max_code_units_v<_UEncoding>;

			_IntermediateCodeUnit __intermediate_storage[_IntermediateMaxCodeUnits] {};
			::ztd::span<_IntermediateCodeUnit, _IntermediateMaxCodeUnits> __intermediate(__intermediate_storage);
			return __basic_count_as_encoded_one(::std::forward<_Input>(__input),
				::std::forward<_Encoding>(__encoding), ::std::forward<_ErrorHandler>(__error_handler), __state,
				__intermediate);
		}

		template <typename _Input, typename _Encoding, typename _ErrorHandler, typename _State,
			typename _Intermediate>
		constexpr auto __basic_count_as_decoded_one(_Input&& __input, _Encoding&& __encoding,
			_ErrorHandler&& __error_handler, _State& __state, _Intermediate& __intermediate) {
			using _Result = count_result<ranges::reconstruct_t<_Input>, _State>;

			auto __intermediate_result = __basic_decode_one<__consume::__no>(::std::forward<_Input>(__input),
				::std::forward<_Encoding>(__encoding), __intermediate,
				::std::forward<_ErrorHandler>(__error_handler), __state);
			::std::size_t __written    = static_cast<::std::size_t>(
                    ::ztd::ranges::data(__intermediate_result.output) - ::ztd::ranges::data(__intermediate));

			return _Result(::std::move(__intermediate_result.input), __written, __intermediate_result.state,
				__intermediate_result.error_code, __intermediate_result.error_count);
		}

		template <typename _Input, typename _Encoding, typename _ErrorHandler, typename _State>
		constexpr auto __basic_count_as_decoded_one(
			_Input&& __input, _Encoding&& __encoding, _ErrorHandler&& __error_handler, _State& __state) {
			using _UEncoding                                   = remove_cvref_t<_Encoding>;
			using _IntermediateCodePoint                       = code_point_t<_UEncoding>;
			constexpr ::std::size_t _IntermediateMaxCodePoints = max_code_points_v<_UEncoding>;


			_IntermediateCodePoint __intermediate_storage[_IntermediateMaxCodePoints] {};
			::ztd::span<_IntermediateCodePoint, _IntermediateMaxCodePoints> __intermediate(__intermediate_storage);
			return __basic_count_as_decoded_one(::std::forward<_Input>(__input),
				::std::forward<_Encoding>(__encoding), ::std::forward<_ErrorHandler>(__error_handler), __state,
				__intermediate);
		}
	} // namespace __txt_detail

	ZTD_TEXT_INLINE_ABI_NAMESPACE_CLOSE_I_
}} // namespace ztd::text

#include <ztd/epilogue.hpp>

#endif
