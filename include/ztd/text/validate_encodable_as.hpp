// =============================================================================
//
// ztd.text
// Copyright © 2022-2023 JeanHeyd "ThePhD" Meneide and Shepherd's Oasis, LLC
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

#ifndef ZTD_TEXT_VALIDATE_ENCODABLE_AS_HPP
#define ZTD_TEXT_VALIDATE_ENCODABLE_AS_HPP

#include <ztd/text/version.hpp>

#include <ztd/text/code_unit.hpp>
#include <ztd/text/code_point.hpp>
#include <ztd/text/max_units.hpp>
#include <ztd/text/default_encoding.hpp>
#include <ztd/text/validate_result.hpp>
#include <ztd/text/error_handler.hpp>
#include <ztd/text/state.hpp>
#include <ztd/text/recode_one.hpp>
#include <ztd/text/detail/is_lossless.hpp>
#include <ztd/text/detail/encoding_range.hpp>
#include <ztd/text/char_predicates.hpp>

#include <ztd/ranges/adl.hpp>
#include <ztd/ranges/algorithm.hpp>
#include <ztd/idk/span.hpp>
#include <ztd/idk/type_traits.hpp>
#include <ztd/idk/tag.hpp>

#include <algorithm>
#include <string_view>

#include <ztd/prologue.hpp>

namespace ztd { namespace text {
	ZTD_TEXT_INLINE_ABI_NAMESPACE_OPEN_I_

	//////
	/// @addtogroup ztd_text_validate_encodable_as ztd::text::validate_encodable_as
	/// @brief These functions check if the given input of code points will encode without an error. Note that this
	/// does not mean that an error handler will be invoked that can "smooth over" any possible errors: this checks
	/// solely if it will encode from code points into code units cleanly.
	/// @{

	//////
	/// @brief Validates the code points of the `__input` according to the `__encoding` with the given states @p
	/// __encode_state and `__decode_state`.
	///
	/// @param[in] __input The input range of code points to validate is possible for encoding into code units.
	/// @param[in] __encoding The encoding to verify can properly encode the input of code units.
	/// @param[in] __encode_state The state to use for the encoding portion of the validation check.
	/// @param[in] __decode_state The state to use for the decoding portion of the validation check, if needed.
	///
	/// @remarks This function explicitly does not check any of the extension points. It defers to doing a typical loop
	/// over the code points to verify it can be encoded into code units, and then decoded into code points, with no
	/// errors.
	template <typename _Input, typename _Encoding, typename _EncodeState, typename _DecodeState>
	constexpr auto basic_validate_encodable_as(
		_Input&& __input, _Encoding&& __encoding, _EncodeState& __encode_state, _DecodeState& __decode_state) {
		using _WorkingInput = ::ztd::ranges::subrange_for_t<__txt_detail::__span_reconstruct_t<_Input, _Input>>;
		using _UEncoding    = remove_cvref_t<_Encoding>;
		using _Result       = validate_transcode_result<_WorkingInput, _EncodeState, _DecodeState>;

		_WorkingInput __working_input = __txt_detail::__span_reconstruct<_Input>(::std::forward<_Input>(__input));

		if constexpr (is_detected_v<__txt_detail::__detect_adl_text_validate_encodable_as_one, _WorkingInput,
			              _Encoding, _EncodeState, _DecodeState>) {
			for (;;) {
				auto __result = text_validate_encodable_as_one(::ztd::tag<_UEncoding> {},
					::std::move(__working_input), __encoding, __encode_state, __decode_state);
				if (!__result.valid) {
					return _Result(::std::move(__result.input), false, __encode_state, __decode_state);
				}
				__working_input = ::std::move(__result.input);
				if (::ztd::ranges::empty(__working_input)) {
					if (!::ztd::text::is_state_complete(__encoding, __decode_state)) {
						continue;
					}
					if (!::ztd::text::is_state_complete(__encoding, __encode_state)) {
						continue;
					}
					break;
				}
			}
			return _Result(::std::move(__working_input), true, __encode_state, __decode_state);
		}
		else if constexpr (is_detected_v<__txt_detail::__detect_adl_text_validate_encodable_as_one, _WorkingInput,
			                   _Encoding, _EncodeState>) {
			for (;;) {
				auto __result = text_validate_encodable_as_one(
					::ztd::tag<_UEncoding> {}, ::std::move(__working_input), __encoding, __encode_state);
				if (!__result.valid) {
					return _Result(::std::move(__result.input), false, __encode_state, __decode_state);
				}
				__working_input = ::std::move(__result.input);
				if (::ztd::ranges::empty(__working_input)) {
					if (!::ztd::text::is_state_complete(__encoding, __decode_state)) {
						continue;
					}
					if (!::ztd::text::is_state_complete(__encoding, __encode_state)) {
						continue;
					}
					break;
				}
			}
			return _Result(::std::move(__working_input), true, __encode_state, __decode_state);
		}
		else if constexpr (is_detected_v<__txt_detail::__detect_adl_internal_text_validate_encodable_as_one,
			                   _WorkingInput, _Encoding, _EncodeState, _DecodeState>) {
			for (;;) {
				auto __result = text_validate_encodable_as_one(::ztd::tag<_UEncoding> {},
					::std::move(__working_input), __encoding, __encode_state, __decode_state);
				if (!__result.valid) {
					return _Result(::std::move(__result.input), false, __encode_state, __decode_state);
				}
				__working_input = ::std::move(__result.input);
				if (::ztd::ranges::empty(__working_input)) {
					if (!::ztd::text::is_state_complete(__encoding, __decode_state)) {
						continue;
					}
					if (!::ztd::text::is_state_complete(__encoding, __encode_state)) {
						continue;
					}
					break;
				}
			}
			return _Result(::std::move(__working_input), true, __encode_state, __decode_state);
		}
		else if constexpr (is_detected_v<__txt_detail::__detect_adl_internal_text_validate_encodable_as_one,
			                   _WorkingInput, _Encoding, _EncodeState>) {
			for (;;) {
				auto __result = __text_validate_encodable_as_one(
					::ztd::tag<_UEncoding> {}, ::std::move(__working_input), __encoding, __encode_state);
				if (!__result.valid) {
					return _Result(::std::move(__result.input), false, __encode_state, __decode_state);
				}
				__working_input = ::std::move(__result.input);
				if (::ztd::ranges::empty(__working_input)) {
					if (!::ztd::text::is_state_complete(__encoding, __decode_state)) {
						continue;
					}
					if (!::ztd::text::is_state_complete(__encoding, __encode_state)) {
						continue;
					}
					break;
				}
			}
			return _Result(::std::move(__working_input), true, __encode_state, __decode_state);
		}
		else if constexpr (is_detected_v<__txt_detail::__detect_adl_internal_text_validate_encodable_as_one,
			                   _WorkingInput, _Encoding, _EncodeState>) {
			for (;;) {
				auto __result = __text_validate_encodable_as_one(
					::ztd::tag<_UEncoding> {}, ::std::move(__working_input), __encoding, __encode_state);
				if (!__result.valid) {
					return _Result(::std::move(__result.input), false, __encode_state, __decode_state);
				}
				__working_input = ::std::move(__result.input);
				if (::ztd::ranges::empty(__working_input)) {
					if (!::ztd::text::is_state_complete(__encoding, __decode_state)) {
						continue;
					}
					if (!::ztd::text::is_state_complete(__encoding, __encode_state)) {
						continue;
					}
					break;
				}
			}
			return _Result(::std::move(__working_input), true, __encode_state, __decode_state);
		}
		else {
			constexpr ::std::size_t __code_point_max = max_recode_code_points_v<_UEncoding, _UEncoding>;
			constexpr ::std::size_t __code_unit_max  = max_code_units_v<_UEncoding>;
			using _CodeUnit                          = code_unit_t<_UEncoding>;
			using _CodePoint                         = code_point_t<_UEncoding>;
			using _Pivot                             = ::ztd::text::pivot<::ztd::span<_CodeUnit, __code_unit_max>>;

			_CodePoint __code_point_buf[__code_point_max] {};
			_CodeUnit __code_unit_buf[__code_unit_max] {};
			::ztd::span<_CodePoint, __code_point_max> __code_point_view(__code_point_buf);
			::ztd::span<_CodeUnit, __code_unit_max> __code_unit_view(__code_unit_buf);
			_Pivot __pivot { ::std::move(__code_unit_view), encoding_error::ok };

			for (;;) {
				auto __result
					= ::ztd::text::recode_one_into_raw(::std::move(__working_input), __encoding, __code_point_view,
					     __encoding, pass_handler, pass_handler, __encode_state, __decode_state, __pivot);
				if (__result.error_code != encoding_error::ok) {
					return _Result(::std::move(__result.input), false, __encode_state, __decode_state);
				}
				const bool __is_recode_roundtrip_okay = ::ztd::ranges::equal(::ztd::ranges::cbegin(__working_input),
					::ztd::ranges::cbegin(__result.input), __code_point_view.data(),
					::ztd::to_address(::ztd::ranges::cbegin(__result.output)), ::ztd::text::equal_to_char);
				if (!__is_recode_roundtrip_okay) {
					return _Result(::std::move(__result.input), false, __encode_state, __decode_state);
				}
				__working_input = ::std::move(__result.input);
				if (::ztd::ranges::empty(__working_input)) {
					if (!::ztd::text::is_state_complete(__encoding, __encode_state)) {
						continue;
					}
					if (!::ztd::text::is_state_complete(__encoding, __decode_state)) {
						continue;
					}
					break;
				}
			}
			return _Result(::std::move(__working_input), true, __encode_state, __decode_state);
		}
	}

	//////
	/// @brief Validates the code points of the `__input` according to the `__encoding` with the given states @p
	/// __encode_state and `__decode_state`.
	///
	/// @param[in] __input The input range of code points to validate is possible for encoding into code units.
	/// @param[in] __encoding The encoding to verify can properly encode the input of code units.
	/// @param[in] __encode_state The state to use for the encoding portion of the validation check.
	/// @param[in] __decode_state The state to use for the decoding portion of the validation check, if needed.
	///
	/// @remarks This functions checks to see if extension points for `text_validate_encodable_as` is available taking
	/// the available 4 parameters. If so, it calls this. Otherwise, it defers to doing a typical loop over the code
	/// points to verify it can be encoded into code units, and then decoded into code points, with no errors.
	template <typename _Input, typename _Encoding, typename _EncodeState, typename _DecodeState>
	constexpr auto validate_encodable_as(
		_Input&& __input, _Encoding&& __encoding, _EncodeState& __encode_state, _DecodeState& __decode_state) {
		if constexpr (is_detected_v<__txt_detail::__detect_adl_text_validate_encodable_as, _Input, _Encoding,
			              _EncodeState, _DecodeState>) {
			return text_validate_encodable_as(::ztd::tag<remove_cvref_t<_Encoding>> {},
				::std::forward<_Input>(__input), ::std::forward<_Encoding>(__encoding), __encode_state,
				__decode_state);
		}
		else if constexpr (is_detected_v<__txt_detail::__detect_adl_text_validate_encodable_as, _Input, _Encoding,
			                   _EncodeState>) {
			(void)__decode_state;
			return text_validate_encodable_as(::ztd::tag<remove_cvref_t<_Encoding>> {},
				::std::forward<_Input>(__input), ::std::forward<_Encoding>(__encoding), __encode_state);
		}
		else if constexpr (is_detected_v<__txt_detail::__detect_adl_internal_text_validate_encodable_as, _Input,
			                   _Encoding, _EncodeState>) {
			(void)__decode_state;
			return __text_validate_encodable_as(::ztd::tag<remove_cvref_t<_Encoding>> {},
				::std::forward<_Input>(__input), ::std::forward<_Encoding>(__encoding), __encode_state);
		}
		else if constexpr (is_detected_v<__txt_detail::__detect_adl_internal_text_validate_encodable_as, _Input,
			                   _Encoding, _EncodeState, _DecodeState>) {
			return __text_validate_encodable_as(::ztd::tag<remove_cvref_t<_Encoding>> {},
				::std::forward<_Input>(__input), ::std::forward<_Encoding>(__encoding), __encode_state,
				__decode_state);
		}
		else {
			return basic_validate_encodable_as(::std::forward<_Input>(__input),
				::std::forward<_Encoding>(__encoding), __encode_state, __decode_state);
		}
	}

	//////
	/// @brief Validates the code points of the `__input` according to the `__encoding` with the given states @p
	/// "__encode_state".
	///
	/// @param[in] __input The input range of code points to validate is possible for encoding into code units.
	/// @param[in] __encoding The encoding to verify can properly encode the input of code units.
	/// @param[in] __encode_state The state for encoding to use.
	///
	/// @remarks This functions checks to see if extension points for `text_validate_encodable_as` is available taking
	/// the available 3 parameters. If so, it calls this. Otherwise, it defers to ztd::text::validate_encodable_as.
	template <typename _Input, typename _Encoding, typename _EncodeState>
	constexpr auto validate_encodable_as(_Input&& __input, _Encoding&& __encoding, _EncodeState& __encode_state) {
		using _UEncoding = remove_cvref_t<_Encoding>;
		if constexpr (is_detected_v<__txt_detail::__detect_adl_text_validate_encodable_as, _Input, _Encoding,
			              _EncodeState>) {
			return text_validate_encodable_as(::ztd::tag<remove_cvref_t<_Encoding>> {},
				::std::forward<_Input>(__input), ::std::forward<_Encoding>(__encoding), __encode_state);
		}
		else if constexpr (is_detected_v<__txt_detail::__detect_adl_internal_text_validate_encodable_as, _Input,
			                   _Encoding, _EncodeState>) {
			return __text_validate_encodable_as(::ztd::tag<remove_cvref_t<_Encoding>> {},
				::std::forward<_Input>(__input), ::std::forward<_Encoding>(__encoding), __encode_state);
		}
		else {
			using _State = decode_state_t<_UEncoding>;

			_State __decode_state  = make_decode_state_with(__encoding, __encode_state);
			auto __stateful_result = validate_encodable_as(::std::forward<_Input>(__input),
				::std::forward<_Encoding>(__encoding), __encode_state, __decode_state);
			if constexpr (is_specialization_of_v<decltype(__stateful_result), validate_result>) {
				return __stateful_result;
			}
			else {
				return __txt_detail::__drop_single_state(::std::move(__stateful_result));
			}
		}
	}

	//////
	/// @brief Validates the code points of the `__input` according to the @p "__encoding".
	///
	/// @param[in] __input The input range of code points to validate is possible for encoding into code units.
	/// @param[in] __encoding The encoding to verify can properly encode the input of code units.
	template <typename _Input, typename _Encoding>
	constexpr auto validate_encodable_as(_Input&& __input, _Encoding&& __encoding) {
		using _UEncoding = remove_cvref_t<_Encoding>;
		using _State     = encode_state_t<_UEncoding>;

		_State __state = make_encode_state(__encoding);
		auto __stateful_result
			= validate_encodable_as(::std::forward<_Input>(__input), ::std::forward<_Encoding>(__encoding), __state);
		return __txt_detail::__slice_to_stateless(::std::move(__stateful_result));
	}

	//////
	/// @brief Validates the code points of the input.
	///
	/// @param[in] __input The input range of code points to validate is possible for encoding into code units.
	///
	/// @remarks This passes the default encoding as inferred from the discernible `value_type` of the input range
	/// input into the ztd::text::default_code_point_encoding.
	template <typename _Input>
	constexpr auto validate_encodable_as(_Input&& __input) {
		using _UInput    = remove_cvref_t<_Input>;
		using _CodePoint = remove_cvref_t<ranges::range_value_type_t<_UInput>>;
#if ZTD_IS_ON(ZTD_STD_LIBRARY_IS_CONSTANT_EVALUATED)
		if (::std::is_constant_evaluated()) {
			// Use literal encoding instead, if we meet the right criteria
			using _Encoding = default_consteval_code_point_encoding_t<_CodePoint>;
			_Encoding __encoding {};
			return validate_encodable_as(::std::forward<_Input>(__input), __encoding);
		}
		else
#endif
		{
			using _Encoding = default_code_point_encoding_t<_CodePoint>;
			_Encoding __encoding {};
			return validate_encodable_as(::std::forward<_Input>(__input), __encoding);
		}
	}

	//////
	/// @}

	ZTD_TEXT_INLINE_ABI_NAMESPACE_CLOSE_I_
}} // namespace ztd::text

#include <ztd/epilogue.hpp>

#endif
