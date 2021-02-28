// =============================================================================
//
// ztd.text
// Copyright © 2021 JeanHeyd "ThePhD" Meneide and Shepherd's Oasis, LLC
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
// ============================================================================>

#pragma once

#ifndef ZTD_TEXT_VALIDATE_CODE_UNITS_HPP
#define ZTD_TEXT_VALIDATE_CODE_UNITS_HPP

#include <ztd/text/code_unit.hpp>
#include <ztd/text/code_point.hpp>
#include <ztd/text/default_encoding.hpp>
#include <ztd/text/validate_result.hpp>
#include <ztd/text/error_handler.hpp>
#include <ztd/text/state.hpp>
#include <ztd/text/detail/is_lossless.hpp>
#include <ztd/text/subrange.hpp>

#include <ztd/text/detail/encoding_range.hpp>
#include <ztd/text/detail/type_traits.hpp>
#include <ztd/text/detail/transcode_one.hpp>
#include <ztd/text/detail/span.hpp>

#include <algorithm>
#include <string_view>

namespace ztd { namespace text {
	ZTD_TEXT_INLINE_ABI_NAMESPACE_OPEN_I_

	//////
	/// @addtogroup ztd_text_validate_code_units ztd::text::validate_code_units
	/// @brief These functions check if the given input of code points will decode without an error. Note that this
	/// does not mean that an error handler will be invoked that can "smooth over" any possible errors: this checks
	/// solely if it will decode from code units into code points cleanly.
	/// @{
	//////

	//////
	/// @brief Validates the code units of the @p __input according to the @p __encoding with the given states @p
	/// __decode_state and @p __encode_state to see if it can be turned into code points.
	///
	/// @param[in] __input The input range of code units to validate is possible for encoding into code points.
	/// @param[in] __encoding The encoding to verify can properly encode the input of code units.
	/// @param[in] __decode_state The state to use for the decoding portion of the validation check.
	/// @param[in] __encode_state The state to use for the encoding portion of the validation check.
	///
	/// @remarks This function explicitly does not call any extension points. It defers to doing a typical loop over
	/// the code points to verify it can be decoded into code points, and then encoded back into code units, with no
	/// errors and with the exact same value sequence as the original.
	//////
	template <typename _Input, typename _Encoding, typename _DecodeState, typename _EncodeState>
	constexpr auto basic_validate_code_units(
		_Input&& __input, _Encoding&& __encoding, _DecodeState& __decode_state, _EncodeState& __encode_state) {
		using _UInput         = __txt_detail::__remove_cvref_t<_Input>;
		using _InputValueType = __txt_detail::__range_value_type_t<_UInput>;
		using _WorkingInput   = __txt_detail::__reconstruct_t<::std::conditional_t<::std::is_array_v<_UInput>,
               ::std::conditional_t<__txt_detail::__is_character_v<_InputValueType>,
                    ::std::basic_string_view<_InputValueType>, ::ztd::text::span<const _InputValueType>>,
               _UInput>>;
		using _UEncoding      = __txt_detail::__remove_cvref_t<_Encoding>;
		using _Result         = validate_result<_WorkingInput, _DecodeState>;

		_WorkingInput __working_input(
			__txt_detail::__reconstruct(::std::in_place_type<_WorkingInput>, ::std::forward<_Input>(__input)));

		if constexpr (__txt_detail::__is_detected_v<__txt_detail::__detect_object_validate_code_units_one, _Encoding,
			              _WorkingInput, _DecodeState>) {
			(void)__encode_state;
			for (;;) {
				auto __result = __encoding.validate_code_units_one(__working_input, __decode_state);
				if (!__result.valid) {
					return _Result(::std::move(__result.input), false, __decode_state);
				}
				__working_input = ::std::move(__result.input);
				if (__txt_detail::__adl::__adl_empty(__working_input)) {
					break;
				}
			}
			return _Result(::std::move(__working_input), true, __decode_state);
		}
		else {
			using _CodeUnit = code_unit_t<_UEncoding>;

			using _CodeUnit  = code_unit_t<_UEncoding>;
			using _CodePoint = code_point_t<_UEncoding>;

			_CodePoint __code_point_buf[max_code_points_v<_UEncoding>] {};
			_CodeUnit __code_unit_buf[max_code_units_v<_UEncoding>] {};

			for (;;) {
				auto __validate_result = __txt_detail::__basic_validate_code_units_one(__working_input, __encoding,
					__code_unit_buf, __code_point_buf, __decode_state, __encode_state);
				if (!__validate_result.valid) {
					return _Result(__txt_detail::__reconstruct(
						               ::std::in_place_type<_WorkingInput>, ::std::move(__working_input)),
						false, __decode_state);
				}
				__working_input = ::std::move(__validate_result.input);
				if (__txt_detail::__adl::__adl_empty(__working_input)) {
					break;
				}
			}
			return _Result(
				__txt_detail::__reconstruct(::std::in_place_type<_WorkingInput>, ::std::move(__working_input)),
				true, __decode_state);
		}
	}

	//////
	/// @brief Validates the code units of the @p __input according to the @p __encoding with the given states @p
	/// __decode_state and @p __encode_state to see if it can be turned into code points.
	///
	/// @param[in] __input The input range of code units to validate is possible for encoding into code points.
	/// @param[in] __encoding The encoding to verify can properly encode the input of code units.
	/// @param[in] __decode_state The state to use for the decoding portion of the validation check.
	/// @param[in] __encode_state The state to use for the encoding portion of the validation check.
	///
	/// @remarks This functions checks to see if extension points for @c text_validate_code_units is available taking
	/// the available 4 parameters. If so, it calls this. Otherwise, it defers to ztd::text::validate_code_units.
	//////
	template <typename _Input, typename _Encoding, typename _DecodeState, typename _EncodeState>
	constexpr auto validate_code_units(
		_Input&& __input, _Encoding&& __encoding, _DecodeState& __decode_state, _EncodeState& __encode_state) {
		if constexpr (__txt_detail::__is_detected_v<__txt_detail::__detect_adl_text_validate_code_units, _Encoding,
			              _Input, _DecodeState>) {
			(void)__encode_state;
			return text_validate_code_units(tag<__txt_detail::__remove_cvref_t<_Encoding>> {},
				::std::forward<_Input>(__input), ::std::forward<_Encoding>(__encoding), __decode_state);
		}
		else if constexpr (__txt_detail::__is_detected_v<__txt_detail::__detect_adl_text_validate_code_units,
			                   _Encoding, _Input, _DecodeState>) {
			return text_validate_code_units(tag<__txt_detail::__remove_cvref_t<_Encoding>> {},
				::std::forward<_Input>(__input), ::std::forward<_Encoding>(__encoding), __decode_state,
				__encode_state);
		}
		else if constexpr (__txt_detail::__is_detected_v<__txt_detail::__detect_adl_internal_text_validate_code_units,
			                   _Encoding, _Input, _DecodeState>) {
			(void)__encode_state;
			return __text_validate_code_units(tag<__txt_detail::__remove_cvref_t<_Encoding>> {},
				::std::forward<_Input>(__input), ::std::forward<_Encoding>(__encoding), __decode_state);
		}
		else if constexpr (__txt_detail::__is_detected_v<__txt_detail::__detect_adl_internal_text_validate_code_units,
			                   _Encoding, _Input, _DecodeState>) {
			return __text_validate_code_units(tag<__txt_detail::__remove_cvref_t<_Encoding>> {},
				::std::forward<_Input>(__input), ::std::forward<_Encoding>(__encoding), __decode_state,
				__encode_state);
		}
		else {
			using _UInput         = __txt_detail::__remove_cvref_t<_Input>;
			using _InputValueType = __txt_detail::__range_value_type_t<_UInput>;
			using _WorkingInput   = __txt_detail::__reconstruct_t<::std::conditional_t<::std::is_array_v<_UInput>,
                    ::std::conditional_t<__txt_detail::__is_character_v<_InputValueType>,
                         ::std::basic_string_view<_InputValueType>, ::ztd::text::span<const _InputValueType>>,
                    _UInput>>;
			using _UEncoding      = __txt_detail::__remove_cvref_t<_Encoding>;
			using _Result         = validate_result<_WorkingInput, _DecodeState>;

			_WorkingInput __working_input(
				__txt_detail::__reconstruct(::std::in_place_type<_WorkingInput>, ::std::forward<_Input>(__input)));

			if constexpr (__txt_detail::__is_detected_v<__txt_detail::__detect_object_validate_code_units_one,
				              _Encoding, _WorkingInput, _DecodeState>) {
				(void)__encode_state;
				for (;;) {
					auto __result = __encoding.validate_code_units_one(__working_input, __decode_state);
					if (!__result.valid) {
						return _Result(::std::move(__result.input), false, __decode_state);
					}
					__working_input = ::std::move(__result.input);
					if (__txt_detail::__adl::__adl_empty(__working_input)) {
						break;
					}
				}
				return _Result(::std::move(__working_input), true, __decode_state);
			}
			else {
				using _CodeUnit = code_unit_t<_UEncoding>;

				using _CodeUnit  = code_unit_t<_UEncoding>;
				using _CodePoint = code_point_t<_UEncoding>;

				_CodePoint __code_point_buf[max_code_points_v<_UEncoding>] {};
				_CodeUnit __code_unit_buf[max_code_units_v<_UEncoding>] {};

				for (;;) {
					auto __validate_result = __txt_detail::__basic_validate_code_units_one(__working_input,
						__encoding, __code_unit_buf, __code_point_buf, __decode_state, __encode_state);
					if (!__validate_result.valid) {
						return _Result(__txt_detail::__reconstruct(
							               ::std::in_place_type<_WorkingInput>, ::std::move(__working_input)),
							false, __decode_state);
					}
					__working_input = ::std::move(__validate_result.input);
					if (__txt_detail::__adl::__adl_empty(__working_input)) {
						break;
					}
				}
				return _Result(
					__txt_detail::__reconstruct(::std::in_place_type<_WorkingInput>, ::std::move(__working_input)),
					true, __decode_state);
			}
		}
	}

	//////
	/// @brief Validates the code units of the @p __input according to the @p __encoding with the given state @p
	/// __decode_state to see if it can be turned into code points.
	///
	/// @param[in] __input The input range of code units to validate is possible for encoding into code points.
	/// @param[in] __encoding The encoding to verify can properly encode the input of code units.
	/// @param[in] __decode_state The state to use for the decoding portion of the validation check.
	///
	/// @remarks This functions checks to see if extension points for @c text_validate_code_units is available taking
	/// the available 3 parameters. If so, it calls this. Otherwise, it creates an encoding state through
	/// ztd::text::make_encode_state before calling ztd::text::validate_code_units(__input, __encoding,
	/// __decode_state,
	/// __encode_state).
	//////
	template <typename _Input, typename _Encoding, typename _DecodeState>
	constexpr auto validate_code_units(_Input&& __input, _Encoding&& __encoding, _DecodeState& __decode_state) {
		using _UEncoding = __txt_detail::__remove_cvref_t<_Encoding>;
		if constexpr (__txt_detail::__is_detected_v<__txt_detail::__detect_adl_text_validate_code_units, _Encoding,
			              _Input, _DecodeState>) {
			return text_validate_code_units(tag<__txt_detail::__remove_cvref_t<_Encoding>> {},
				::std::forward<_Input>(__input), ::std::forward<_Encoding>(__encoding), __decode_state);
		}
		else if constexpr (__txt_detail::__is_detected_v<__txt_detail::__detect_adl_internal_text_validate_code_units,
			                   _Encoding, _Input, _DecodeState>) {
			return __text_validate_code_units(tag<__txt_detail::__remove_cvref_t<_Encoding>> {},
				::std::forward<_Input>(__input), ::std::forward<_Encoding>(__encoding), __decode_state);
		}
		else {
			using _State = encode_state_t<_UEncoding>;

			_State __encode_state = make_encode_state(__encoding);
			return validate_code_units(::std::forward<_Input>(__input), ::std::forward<_Encoding>(__encoding),
				__decode_state, __encode_state);
		}
	}

	//////
	/// @brief Validates the code units of the @p __input according to the @p __encoding to see if they can be turned
	/// into code points.
	///
	/// @param[in] __input The input range of code units to validate is possible for encoding into code points.
	/// @param[in] __encoding The encoding to verify can properly encode the input of code units.
	///
	/// @remarks This functions creates an encoding @c state through ztd::text::make_decode_state before calling
	/// the next overload of ztd::text::validate_code_units.
	//////
	template <typename _Input, typename _Encoding>
	constexpr auto validate_code_units(_Input&& __input, _Encoding&& __encoding) {
		using _UEncoding = __txt_detail::__remove_cvref_t<_Encoding>;
		using _State     = decode_state_t<_UEncoding>;

		_State __state = make_decode_state(__encoding);
		auto __stateful_result
			= validate_code_units(::std::forward<_Input>(__input), ::std::forward<_Encoding>(__encoding), __state);
		return __txt_detail::__slice_to_stateless(::std::move(__stateful_result));
	}

	//////
	/// @brief Validates the code units of the @p __input to see if it can be turned into code points.
	///
	/// @param[in] __input The input range of code units to validate is possible for encoding into code points.
	///
	/// @returns A ztd::text::stateless_validate_result detailing whether or not the input code points can be turned
	/// into code units of the corresponding encoding.
	///
	/// @remarks This functions creates an @c encoding by passing the @c value_type of the @c __input range through
	/// ztd::text::default_code_unit_encoding.
	//////
	template <typename _Input>
	constexpr auto validate_code_units(_Input&& __input) {
		using _UInput   = __txt_detail::__remove_cvref_t<_Input>;
		using _CodeUnit = __txt_detail::__remove_cvref_t<__txt_detail::__range_value_type_t<_UInput>>;
#if ZTD_TEXT_IS_ON(ZTD_TEXT_STD_LIBRARY_IS_CONSTANT_EVALUATED_I_)
		if (::std::is_constant_evaluated()) {
			// Use literal encoding instead, if we meet the right criteria
			using _Encoding = default_compile_time_code_unit_encoding_t<_CodeUnit>;
			_Encoding __encoding {};
			return validate_code_units(::std::forward<_Input>(__input), __encoding);
		}
		else
#endif
		{
			using _Encoding = default_code_unit_encoding_t<_CodeUnit>;
			_Encoding __encoding {};
			return validate_code_units(::std::forward<_Input>(__input), __encoding);
		}
	}

	//////
	/// @}
	//////

	ZTD_TEXT_INLINE_ABI_NAMESPACE_CLOSE_I_
}} // namespace ztd::text

#endif // ZTD_TEXT_VALIDATE_CODE_UNITS_HPP
