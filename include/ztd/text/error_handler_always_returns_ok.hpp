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

#ifndef ZTD_TEXT_ERROR_HANDLER_ALWAYS_RETURNS_OK_HPP
#define ZTD_TEXT_ERROR_HANDLER_ALWAYS_RETURNS_OK_HPP

#include <ztd/text/version.hpp>

#include <ztd/text/forward.hpp>
#include <ztd/text/pass_handler.hpp>
#include <ztd/text/throw_handler.hpp>
#include <ztd/text/assume_valid_handler.hpp>
#include <ztd/text/is_code_units_replaceable.hpp>
#include <ztd/text/is_code_points_replaceable.hpp>
#include <ztd/text/is_unicode_code_point.hpp>

#include <ztd/idk/type_traits.hpp>

#include <ztd/prologue.hpp>

namespace ztd { namespace text {
	ZTD_TEXT_INLINE_ABI_NAMESPACE_OPEN_I_

	namespace __txt_detail {
		template <typename _Type>
		using __detect_decode_always_ok = decltype(_Type::decode_always_ok);

		template <typename _Type>
		using __detect_encode_always_ok = decltype(_Type::encode_always_ok);

		template <typename _Encoding, typename _ErrorHandler>
		struct __decode_error_handler_always_returns_ok
		: public ::std::integral_constant<bool, is_detected_v<__detect_decode_always_ok, _ErrorHandler>> { };

		template <typename _Encoding>
		struct __decode_error_handler_always_returns_ok<_Encoding, replacement_handler_t>
		: public ::std::integral_constant<bool,
			  (is_code_points_replaceable_v<_Encoding> && !is_code_points_maybe_replaceable_v<_Encoding>)
			       || is_unicode_code_point_v<code_point_t<_Encoding>>> { };

		template <typename _Encoding>
		struct __decode_error_handler_always_returns_ok<_Encoding, throw_handler_t> : public ::std::true_type { };

		template <typename _Encoding, typename _ErrorHandler>
		struct __decode_error_handler_always_returns_ok<_Encoding, basic_incomplete_handler<_Encoding, _ErrorHandler>>
		: public __decode_error_handler_always_returns_ok<_Encoding,
			  typename basic_incomplete_handler<_Encoding, _ErrorHandler>::error_handler> { };

		template <typename _Encoding>
		struct __decode_error_handler_always_returns_ok<_Encoding, default_handler_t>
#if ZTD_IS_ON(ZTD_TEXT_DEFAULT_HANDLER_THROWS)
		: public __decode_error_handler_always_returns_ok<_Encoding, throw_handler_t> {
		};
#else
		: public __decode_error_handler_always_returns_ok<_Encoding, replacement_handler_t> {
		};
#endif
		template <typename _Encoding>
		struct __decode_error_handler_always_returns_ok<_Encoding, assume_valid_handler_t> : public ::std::true_type {
		};

		template <typename _Encoding, typename _ErrorHandler>
		struct __encode_error_handler_always_returns_ok
		: public ::std::integral_constant<bool, is_detected_v<__detect_encode_always_ok, _ErrorHandler>> { };

		template <typename _Encoding>
		struct __encode_error_handler_always_returns_ok<_Encoding, replacement_handler_t>
		: public ::std::integral_constant<bool,
			  (is_code_units_replaceable_v<_Encoding> && !is_code_units_maybe_replaceable_v<_Encoding>)
			       || is_unicode_code_point_v<code_unit_t<_Encoding>>> { };

		template <typename _Encoding>
		struct __encode_error_handler_always_returns_ok<_Encoding, throw_handler_t> : public ::std::true_type { };

		template <typename _Encoding, typename _ErrorHandler>
		struct __encode_error_handler_always_returns_ok<_Encoding, basic_incomplete_handler<_Encoding, _ErrorHandler>>
		: public __encode_error_handler_always_returns_ok<_Encoding,
			  typename basic_incomplete_handler<_Encoding, _ErrorHandler>::error_handler> { };

		template <typename _Encoding>
		struct __encode_error_handler_always_returns_ok<_Encoding, default_handler_t>
#if ZTD_IS_ON(ZTD_TEXT_DEFAULT_HANDLER_THROWS)
		: public __encode_error_handler_always_returns_ok<_Encoding, throw_handler_t> {
		};
#else
		: public __encode_error_handler_always_returns_ok<_Encoding, replacement_handler_t> {
		};

#endif
		template <typename _Encoding>
		struct __encode_error_handler_always_returns_ok<_Encoding, assume_valid_handler_t> : ::std::true_type { };

	} // namespace __txt_detail

	//////
	/// @addtogroup ztd_text_properties Property and Trait Helpers
	///
	/// @{
	/////

	//////
	/// @brief Whether or not the given `_Encoding` and `_Input` with the provided `_ErrorHandler` will always
	/// return ztd::text::encoding_error::ok for any failure that is not related to an output being too small
	/// (ztd::text::encoding_error::insufficient_output_space).
	///
	/// @tparam _Encoding The encoding type whose `decode_one` function will be used with the error handler.
	/// @tparam _Input The input range that will be used with the `decode_one` function of the encoding.
	/// @tparam _ErrorHandler The error handler that will be called with the given `_Encoding` object and `_Input`
	/// range.
	///
	/// @remarks This is a compile time assertion. If the encoding may exhibit different behavior at runtime based on
	/// runtime conditions, then this should return false. This is meant for cases where it is provable at compile
	/// time, this should return true. For example, if the ztd::text::replacement_handler_t is used in conjunction with
	/// ztd::text::utf8_t, then this will return true as
	template <typename _Encoding, typename _ErrorHandler>
	class decode_error_handler_always_returns_ok
	: public __txt_detail::__decode_error_handler_always_returns_ok<_Encoding, _ErrorHandler> { };

	//////
	/// @brief An alias of the inner `value` for ztd::text::decode_error_handler_always_returns_ok_v
	template <typename _Encoding, typename _ErrorHandler>
	inline constexpr bool decode_error_handler_always_returns_ok_v
		= decode_error_handler_always_returns_ok<_Encoding, _ErrorHandler>::value;

	//////
	/// @brief Whether or not the given `_Type` is an error handler that can be ignored.
	template <typename _Encoding, typename _ErrorHandler>
	class encode_error_handler_always_returns_ok
	: public __txt_detail::__encode_error_handler_always_returns_ok<_Encoding, _ErrorHandler> { };

	//////
	/// @brief An alias of the inner `value` for ztd::text::decode_error_handler_always_returns_ok_v
	template <typename _Encoding, typename _ErrorHandler>
	inline constexpr bool encode_error_handler_always_returns_ok_v
		= encode_error_handler_always_returns_ok<_Encoding, _ErrorHandler>::value;

	//////
	/// @}
	/////

	ZTD_TEXT_INLINE_ABI_NAMESPACE_CLOSE_I_
}} // namespace ztd::text

#include <ztd/epilogue.hpp>

#endif
