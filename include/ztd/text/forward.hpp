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

#ifndef ZTD_TEXT_FORWARD_HPP
#define ZTD_TEXT_FORWARD_HPP

#include <ztd/text/version.hpp>

#include <ztd/text/detail/prologue.hpp>

namespace ztd { namespace text {
	ZTD_TEXT_INLINE_ABI_NAMESPACE_OPEN_I_

	namespace __txt_detail {
		class __empty_state;
	} // namespace __txt_detail

	namespace __impl {
		class __utf8_tag;
		class __utf16_tag;
		class __utf32_tag;

		template <typename, typename, typename, bool, bool, bool>
		class __utf8_with;
		template <typename, typename, typename, bool>
		class __utf16_with;
		template <typename, typename, typename, bool>
		class __utf32_with;

		class __unicode_code_point;
		class __unicode_scalar_value;
	} // namespace __impl

#if ZTD_TEXT_IS_ON(ZTD_TEXT_UNICODE_CODE_POINT_DISTINCT_TYPE_I_)
	using unicode_code_point = __impl::__unicode_code_point;
#else
	using unicode_code_point   = char32_t;
#endif

#if ZTD_TEXT_IS_ON(ZTD_TEXT_UNICODE_SCALAR_VALUE_DISTINCT_TYPE_I_)
	using unicode_scalar_value = __impl::__unicode_scalar_value;
#else
	using unicode_scalar_value = char32_t;
#endif

	using uchar8_t = ZTD_TEXT_CHAR8_T_I_;

	class assume_valid_handler;
	class pass_through_handler;
	template <typename, typename>
	class incomplete_handler;
	class replacement_handler;
	class default_handler;

	template <typename, typename>
	class basic_ascii;
	using ascii = basic_ascii<char, unicode_code_point>;
	class execution;
	class wide_execution;
	template <typename, typename>
	class basic_utf8;
	template <typename, typename>
	class basic_utf16;
	template <typename, typename>
	class basic_utf32;
	using utf8  = basic_utf8<uchar8_t, unicode_code_point>;
	using utf16 = basic_utf16<char16_t, unicode_code_point>;
	using utf32 = basic_utf32<char32_t, unicode_code_point>;

	class nfc;
	class nfd;
	class nfkc;
	class nfkd;

	template <typename, typename, typename, typename>
	class basic_text_view;

	template <typename, typename, typename, typename>
	class basic_text;

	ZTD_TEXT_INLINE_ABI_NAMESPACE_CLOSE_I_
}} // namespace ztd::text

#include <ztd/text/detail/epilogue.hpp>

#endif // ZTD_TEXT_FORWARD_HPP
