// =============================================================================
//
// ztd.text
// Copyright © JeanHeyd "ThePhD" Meneide and Shepherd's Oasis, LLC
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
// https://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
//
// ============================================================================ //

#pragma once

#ifndef ZTD_TEXT_DETAIL_TRANSCODE_ROUTINES_HPP
#define ZTD_TEXT_DETAIL_TRANSCODE_ROUTINES_HPP

#include <ztd/text/version.hpp>

#include <ztd/text/encode_result.hpp>
#include <ztd/text/decode_result.hpp>
#include <ztd/text/state.hpp>
#include <ztd/text/code_point.hpp>
#include <ztd/text/code_unit.hpp>
#include <ztd/text/is_ignorable_error_handler.hpp>
#include <ztd/text/detail/is_lossless.hpp>
#include <ztd/text/detail/encoding_range.hpp>

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

		enum class __consume : unsigned char { __no = 0, __embrace_the_void = 1 };
		enum class __transaction : unsigned char { __encode = 0, __decode = 1 };

		template <__consume _ConsumeIntoTheNothingness, typename _Encoding, typename _Input, typename _Output,
			typename _ErrorHandler, typename _State>
		constexpr auto __basic_decode_one(_Input&& __input, _Encoding&& __encoding, _Output&& __output,
			_ErrorHandler& __error_handler, _State& __state) {
			using _UOutput   = remove_cvref_t<_Output>;
			using _UEncoding = remove_cvref_t<_Encoding>;

			if constexpr (_ConsumeIntoTheNothingness == __consume::__embrace_the_void
				&& __is_decode_range_category_output_v<_UEncoding>) {
				(void)__output;
				using _Blackhole = ranges::unbounded_view<ranges::blackhole_iterator>;
				_Blackhole __output_range(ranges::blackhole_iterator {});
				return __encoding.decode_one(
					::std::forward<_Input>(__input), __output_range, __error_handler, __state);
			}
			else if constexpr ((!ranges::is_range_contiguous_range_v<_UOutput>
				                   || ranges::is_range_input_or_output_range_exactly_v<
				                        _UOutput>)&&__is_decode_range_category_contiguous_v<_UEncoding>) {
				code_point_t<_UEncoding> __intermediate_output_storage[max_code_points_v<_UEncoding>];
				::ztd::span<code_point_t<_UEncoding>, max_code_points_v<_UEncoding>> __intermediate_output(
					__intermediate_output_storage);
				auto __intermediate_result = __encoding.decode_one(
					::std::forward<_Input>(__input), __intermediate_output, __error_handler, __state);
				using _ReconstructedResult = decode_result<decltype(__intermediate_result.input),
					ranges::range_reconstruct_t<_Output>, _State>;
				if (__intermediate_result.error_code != encoding_error::ok) {
					auto __intermediate_last = __intermediate_result.output.data();
					auto __out_it            = ::ztd::ranges::begin(::std::move(__output));
					auto __out_last          = ::ztd::ranges::end(::std::move(__output));
					for (auto __intermediate_it = __intermediate_output.data();
						__intermediate_it != __intermediate_last;) {
						*__out_it = *__intermediate_it;
						++__intermediate_it;
						++__out_it;
					}
					return _ReconstructedResult(::std::move(__intermediate_result.input),
						ranges::reconstruct(::std::in_place_type<_UOutput>, ::std::forward<_Output>(__output),
						     ::std::move(__out_it), ::std::move(__out_last)),
						__state, __intermediate_result.error_code, __intermediate_result.error_count);
				}
				return _ReconstructedResult(::std::move(__intermediate_result.input),
					::std::forward<_Output>(__output), __state, __intermediate_result.error_code,
					__intermediate_result.error_count);
			}
			else {
				return __encoding.decode_one(
					::std::forward<_Input>(__input), ::std::forward<_Output>(__output), __error_handler, __state);
			}
		}

		template <__consume _ConsumeIntoTheNothingness, typename _Encoding, typename _Input, typename _Output,
			typename _ErrorHandler, typename _State>
		constexpr auto __basic_encode_one(_Input&& __input, _Encoding&& __encoding, _Output&& __output,
			_ErrorHandler& __error_handler, _State& __state) {
			using _UOutput   = remove_cvref_t<_Output>;
			using _UEncoding = remove_cvref_t<_Encoding>;

			if constexpr (_ConsumeIntoTheNothingness == __consume::__embrace_the_void
				&& __is_encode_range_category_output_v<_UEncoding>) {
				(void)__output;
				using _Blackhole = ranges::unbounded_view<ranges::blackhole_iterator>;
				_Blackhole __output_range(ranges::blackhole_iterator {});
				return __encoding.encode_one(
					::std::forward<_Input>(__input), __output_range, __error_handler, __state);
			}
			else if constexpr ((!ranges::is_range_contiguous_range_v<_UOutput>
				                   || ranges::is_range_input_or_output_range_exactly_v<
				                        _UOutput>)&&__is_encode_range_category_contiguous_v<_UEncoding>) {
				constexpr ::std::size_t _IntermediateMax = max_code_units_v<_UEncoding>;
				code_unit_t<_UEncoding> __intermediate_output_storage[_IntermediateMax];
				::ztd::span<code_unit_t<_UEncoding>, _IntermediateMax> __intermediate_output(
					__intermediate_output_storage);
				auto __intermediate_result = __encoding.encode_one(
					::std::forward<_Input>(__input), __intermediate_output, __error_handler, __state);
				using _ReconstructedResult = encode_result<decltype(__intermediate_result.input),
					ranges::range_reconstruct_t<_Output>, _State>;
				if (__intermediate_result.error_code != encoding_error::ok) {
					auto __intermediate_last = __intermediate_result.output.data();
					auto __out_it            = ::ztd::ranges::begin(::std::move(__output));
					auto __out_last          = ::ztd::ranges::end(::std::move(__output));
					for (auto __intermediate_it = __intermediate_output.data();
						__intermediate_it != __intermediate_last;) {
						*__out_it = *__intermediate_it;
						++__intermediate_it;
						++__out_it;
					}
					return _ReconstructedResult(::std::move(__intermediate_result.input),
						ranges::reconstruct(::std::in_place_type<_UOutput>, ::std::forward<_Output>(__output),
						     ::std::move(__out_it), ::std::move(__out_last)),
						__state, __intermediate_result.error_code, __intermediate_result.error_count);
				}
				return _ReconstructedResult(::std::move(__intermediate_result.input),
					::std::forward<_Output>(__output), __state, __intermediate_result.error_code,
					__intermediate_result.error_count);
			}
			else {
				return __encoding.encode_one(
					::std::forward<_Input>(__input), ::std::forward<_Output>(__output), __error_handler, __state);
			}
		}

		template <__consume _ConsumeIntoTheNothingness, __transaction __encode_or_decode, typename _Encoding,
			typename _Input, typename _OutputContainer, typename _ErrorHandler, typename _State>
		constexpr auto __basic_encode_or_decode_one(_Input&& __input, _Encoding&& __encoding,
			_OutputContainer& __output, _ErrorHandler& __error_handler, _State& __state) {
			if constexpr (__encode_or_decode == __transaction::__decode) {
				return __basic_decode_one<_ConsumeIntoTheNothingness>(::std::forward<_Input>(__input),
					::std::forward<_Encoding>(__encoding), __output, __error_handler, __state);
			}
			else {
				return __basic_encode_one<_ConsumeIntoTheNothingness>(::std::forward<_Input>(__input),
					::std::forward<_Encoding>(__encoding), __output, __error_handler, __state);
			}
		}

	} // namespace __txt_detail

	ZTD_TEXT_INLINE_ABI_NAMESPACE_CLOSE_I_
}} // namespace ztd::text

#include <ztd/epilogue.hpp>

#endif
