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

#ifndef ZTD_TEXT_FORWARD_HPP
#define ZTD_TEXT_FORWARD_HPP

#include <ztd/text/version.hpp>

#include <ztd/idk/charN_t.hpp>
#include <ztd/platform/version.hpp>

#include <ztd/prologue.hpp>

namespace ztd { namespace text {
	ZTD_TEXT_INLINE_ABI_NAMESPACE_OPEN_I_

	namespace __txt_detail {
		class __empty_state;

		template <typename, typename, typename, typename>
		class __scheme_handler;
		template <typename, typename>
		class __progress_handler;
		template <typename, typename>
		class __forwarding_handler;
	} // namespace __txt_detail

	namespace __txt_impl {
		class __utf8_tag;
		class __utf16_tag;
		class __utf32_tag;

		template <typename, typename, typename, typename, typename, bool, bool, bool>
		class __utf8_with;
		template <typename, typename, typename, bool>
		class __utf16_with;
		template <typename, typename, typename, bool, bool>
		class __utf32_with;

		class __execution_cuchar;
		class __execution_mac_os;
		class __execution_iconv;

		class __wide_execution_windows;
		class __wide_execution_iso10646;
		class __wide_execution_iconv;
		class __wide_execution_cwchar;

		class __unicode_code_point;
		class __unicode_scalar_value;
	} // namespace __txt_impl

#if ZTD_IS_ON(ZTD_TEXT_UNICODE_CODE_POINT_DISTINCT_TYPE)
	using unicode_code_point = __txt_impl::__unicode_code_point;
#else
	using unicode_code_point   = char32_t;
#endif

#if ZTD_IS_ON(ZTD_TEXT_UNICODE_SCALAR_VALUE_DISTINCT_TYPE)
	using unicode_scalar_value = __txt_impl::__unicode_scalar_value;
#else
	using unicode_scalar_value = char32_t;
#endif

	class assume_valid_handler_t;
	class pass_through_handler;
	template <typename, typename>
	class basic_incomplete_handler;
	class replacement_handler_t;
	class skip_handler_t;
	class default_handler_t;

	template <typename, typename>
	class basic_ascii;
	using ascii_t = basic_ascii<char, unicode_code_point>;
	template <typename, typename>
	class basic_utf8;
	template <typename, typename>
	class basic_utf16;
	template <typename, typename>
	class basic_utf32;
	using compat_utf8_t = basic_utf8<char, unicode_code_point>;
	using utf8_t        = basic_utf8<uchar8_t, unicode_code_point>;
	using utf16_t       = basic_utf16<char16_t, unicode_code_point>;
	using utf32_t       = basic_utf32<char32_t, unicode_code_point>;
	class execution_t;
	class wide_execution_t;

	class nfc;
	class nfd;
	class nfkc;
	class nfkd;

	template <typename>
	class pivot;

	template <typename, typename>
	class stateless_transcode_result;
	template <typename, typename, typename, typename>
	class transcode_result;
	template <typename, typename>
	class stateless_decode_result;
	template <typename, typename, typename>
	class decode_result;
	template <typename, typename>
	class stateless_encode_result;
	template <typename, typename, typename>
	class encode_result;

	template <typename, typename, typename, typename, typename>
	class basic_text_view;

	template <typename, typename, typename>
	class basic_text;

	template <typename _Input, typename _Encoding, typename _Output, typename _ErrorHandler, typename _State>
	constexpr auto decode_one_into_raw(_Input&& __input, _Encoding&& __encoding, _Output&& __output,
		_ErrorHandler&& __error_handler, _State& __state);

	template <typename _Input, typename _Encoding, typename _Output, typename _ErrorHandler, typename _State>
	constexpr auto encode_one_into_raw(_Input&& __input, _Encoding&& __encoding, _Output&& __output,
		_ErrorHandler&& __error_handler, _State& __state);

	template <typename _Input, typename _Encoding, typename _Output, typename _ErrorHandler, typename _State>
	constexpr auto basic_decode_into_raw(_Input&& __input, _Encoding&& __encoding, _Output&& __output,
		_ErrorHandler&& __error_handler, _State& __state);

	template <typename _Input, typename _Encoding, typename _Output, typename _ErrorHandler, typename _State>
	constexpr auto basic_encode_into_raw(_Input&& __input, _Encoding&& __encoding, _Output&& __output,
		_ErrorHandler&& __error_handler, _State& __state);

	template <typename _Input, typename _Output, typename _FromEncoding, typename _ToEncoding,
		typename _FromErrorHandler, typename _ToErrorHandler, typename _FromState, typename _ToState,
		typename _PivotRange>
	constexpr auto basic_transcode_one_into_raw(_Input&& __input, _FromEncoding&& __from_encoding, _Output&& __output,
		_ToEncoding&& __to_encoding, _FromErrorHandler&& __from_error_handler, _ToErrorHandler&& __to_error_handler,
		_FromState& __from_state, _ToState& __to_state, pivot<_PivotRange>& __pivot);

	template <typename _Input, typename _FromEncoding, typename _Output, typename _ToEncoding,
		typename _FromErrorHandler, typename _ToErrorHandler, typename _FromState, typename _ToState,
		typename _PivotRange>
	constexpr auto basic_transcode_into_raw(_Input&& __input, _FromEncoding&& __from_encoding, _Output&& __output,
		_ToEncoding&& __to_encoding, _FromErrorHandler&& __from_error_handler, _ToErrorHandler&& __to_error_handler,
		_FromState& __from_state, _ToState& __to_state, pivot<_PivotRange>& __pivot);

	ZTD_TEXT_INLINE_ABI_NAMESPACE_CLOSE_I_
}} // namespace ztd::text

#include <ztd/epilogue.hpp>

#endif
