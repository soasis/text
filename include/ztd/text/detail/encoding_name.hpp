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

#ifndef ZTD_TEXT_DETAIL_ENCODING_NAME_HPP
#define ZTD_TEXT_DETAIL_ENCODING_NAME_HPP

#include <ztd/text/version.hpp>

#include <ztd/text/utf8.hpp>
#include <ztd/text/utf16.hpp>
#include <ztd/text/utf32.hpp>
#include <ztd/text/encoding_scheme.hpp>
#include <ztd/text/ascii.hpp>
#include <ztd/text/no_encoding.hpp>

#include <string_view>

namespace ztd { namespace text {
	ZTD_TEXT_INLINE_ABI_NAMESPACE_OPEN_I_

	namespace __detail {

		inline constexpr bool __is_encoding_name_equal(
			::std::string_view __left, ::std::string_view __right) noexcept {
			constexpr std::string_view __readable_characters
				= "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuuvwxyz1234567890";
			constexpr std::string_view __uncased_characters = "abcdefghijklmnopqrstuuvwxyz";
			constexpr std::string_view __cased_characters   = "ABCDEFGHIJKLMNOPQRSTUVWXYZ";
			::std::size_t __left_size                       = __left.size();
			::std::size_t __right_size                      = __right.size();
			const char* __left_ptr                          = __left.data();
			const char* __right_ptr                         = __right.data();
			::std::size_t __left_index                      = 0;
			::std::size_t __right_index                     = 0;
			for (; __left_index < __left_size && __right_index < __right_size;) {
				// find the first non-ignorable character we can read
				::std::size_t __left_first_index = __left.find_first_of(__readable_characters, __left_index);
				if (__left_first_index == ::std::string_view::npos) {
					return __left_index == __right_index;
				}
				__left_index                      = __left_first_index + 1;
				::std::size_t __right_first_index = __right.find_first_of(__readable_characters, __right_index);
				if (__right_first_index == ::std::string_view::npos) {
					return __left_index == __right_index;
				}
				__right_index  = __right_first_index + 1;
				char __left_c  = __left_ptr[__left_first_index];
				char __right_c = __right_ptr[__right_first_index];
				// make sure we eliminate casing differences
				::std::size_t __left_c_casing_index = __uncased_characters.find(__left_c);
				if (__left_c_casing_index != ::std::string_view::npos) {
					__left_c = __cased_characters[__left_c_casing_index];
				}
				::std::size_t __right_c_casing_index = __uncased_characters.find(__right_c);
				if (__right_c_casing_index != ::std::string_view::npos) {
					__right_c = __cased_characters[__right_c_casing_index];
				}
				// finally, check
				if (__left_c == __right_c) {
					continue;
				}
				return false;
			}
			return true;
		}

		inline constexpr bool __is_unicode_encoding_name(std::string_view __encoding_name) noexcept {
			constexpr const char* __unicode_names[]
				= { "UTF-7", "UTF-7-IMAP", "UTF-8", "UTF-16", "UTF-32", "UTF-16LE", "UTF-16BE", "UTF-32LE",
					  "UTF-32BE", "UTF-EBCDIC", "UTF-8-EBCDIC", "MUTF-8", "WTF-8", "GB18030", "CESU-8", "UTF-1" };
			constexpr ::std::size_t __unicode_names_count = sizeof(__unicode_names) / sizeof(__unicode_names[0]);
			for (::std::size_t __index = 0; __index < __unicode_names_count; ++__index) {
				std::string_view __unicode_name = __unicode_names[__index];
				if (__is_encoding_name_equal(__encoding_name, __unicode_name)) {
					return true;
				}
			}
			return false;
		}

		enum class __encoding_id {
			__unknown = 0,
			__utf7imap,
			__utf7,
			__utfebcdic,
			__utf8,
			__mutf8,
			__wtf8,
			__utf16,
			__utf16le,
			__utf16be,
			__utf32,
			__utf32le,
			__utf32be,
			__gb18030,
			__utf1,
			__cesu8,
			__ascii
		};

		inline constexpr __encoding_id __to_encoding_id(::std::string_view __name) {
			if (__is_encoding_name_equal(__name, "UTF-8")) {
				return __encoding_id::__utf8;
			}
			else if (__is_encoding_name_equal(__name, "UTF-16") || __is_encoding_name_equal(__name, "UCS-2-INTERNAL")
				|| __is_encoding_name_equal(__name, "UCS-2")) {
				return __encoding_id::__utf16;
			}
			else if (__is_encoding_name_equal(__name, "UTF-16LE")
				|| __is_encoding_name_equal(__name, "UCS-2LE-INTERNAL")
				|| __is_encoding_name_equal(__name, "UCS-2LE")) {
				return __encoding_id::__utf16le;
			}
			else if (__is_encoding_name_equal(__name, "UTF-16BE")
				|| __is_encoding_name_equal(__name, "UCS-2BE-INTERNAL")
				|| __is_encoding_name_equal(__name, "UCS-2BE")) {
				return __encoding_id::__utf16be;
			}
			else if (__is_encoding_name_equal(__name, "UTF-32") || __is_encoding_name_equal(__name, "UCS-4-INTERNAL")
				|| __is_encoding_name_equal(__name, "UCS-4")) {
				return __encoding_id::__utf32;
			}
			else if (__is_encoding_name_equal(__name, "UTF-32LE")
				|| __is_encoding_name_equal(__name, "UCS-4LE-INTERNAL")
				|| __is_encoding_name_equal(__name, "UCS-4LE")) {
				return __encoding_id::__utf32le;
			}
			else if (__is_encoding_name_equal(__name, "UTF-32BE")
				|| __is_encoding_name_equal(__name, "UCS-4BE-INTERNAL")
				|| __is_encoding_name_equal(__name, "UCS-4BE")) {
				return __encoding_id::__utf32be;
			}
			else if (__is_encoding_name_equal(__name, "ASCII")
				|| __is_encoding_name_equal(__name, "ANSI_X3.4-1968")) {
				return __encoding_id::__ascii;
			}
			else if (__is_encoding_name_equal(__name, "UTF-EBCDIC")
				|| __is_encoding_name_equal(__name, "UTF-8-EBCDIC")) {
				return __encoding_id::__utfebcdic;
			}
			else if (__is_encoding_name_equal(__name, "WTF-8")) {
				return __encoding_id::__wtf8;
			}
			else if (__is_encoding_name_equal(__name, "MUTF-8")) {
				return __encoding_id::__mutf8;
			}
			else if (__is_encoding_name_equal(__name, "UTF-7")) {
				return __encoding_id::__utf7;
			}
			else if (__is_encoding_name_equal(__name, "UTF-7-IMAP")) {
				return __encoding_id::__utf7imap;
			}
			else {
				return __encoding_id::__unknown;
			}
		}

		inline constexpr bool __is_unicode_encoding_id(__encoding_id __id) noexcept {
			switch (__id) {
			case __encoding_id::__utf7:
			case __encoding_id::__utf7imap:
			case __encoding_id::__utfebcdic:
			case __encoding_id::__utf8:
			case __encoding_id::__utf16:
			case __encoding_id::__utf16le:
			case __encoding_id::__utf16be:
			case __encoding_id::__utf32:
			case __encoding_id::__utf32le:
			case __encoding_id::__utf32be:
			case __encoding_id::__gb18030:
			case __encoding_id::__wtf8:
			case __encoding_id::__mutf8:
			case __encoding_id::__utf1:
			case __encoding_id::__cesu8:
				return true;
			default:
				return false;
			}
		}

		template <typename _CharType, __encoding_id _Id>
		constexpr auto __select_encoding() {
			if constexpr (_Id == __encoding_id::__utf8) {
				return basic_utf8<_CharType> {};
			}
			else if constexpr (_Id == __encoding_id::__mutf8) {
				return basic_mutf8<_CharType> {};
			}
			else if constexpr (_Id == __encoding_id::__wtf8) {
				return basic_wtf8<_CharType> {};
			}
			else if constexpr (_Id == __encoding_id::__utf16) {
				return basic_utf16<_CharType> {};
			}
			else if constexpr (_Id == __encoding_id::__utf16le) {
				// TODO: beef up encoding_scheme to handle this better...!
				return basic_utf16_le<_CharType> {};
			}
			else if constexpr (_Id == __encoding_id::__utf16be) {
				// TODO: beef up encoding_scheme to handle this better...!
				return basic_utf16_be<_CharType> {};
			}
			else if constexpr (_Id == __encoding_id::__utf32) {
				return basic_utf32<_CharType> {};
			}
			else if constexpr (_Id == __encoding_id::__utf32le) {
				// TODO: beef up encoding_scheme to handle this better...!
				return basic_utf32_le<_CharType> {};
			}
			else if constexpr (_Id == __encoding_id::__utf32be) {
				// TODO: beef up encoding_scheme to handle this better...!
				return basic_utf32_be<_CharType> {};
			}
			else if constexpr (_Id == __encoding_id::__ascii) {
				return basic_ascii<_CharType> {};
			}
			else {
				return basic_no_encoding<_CharType, unicode_code_point> {};
			}
		}

	} // namespace __detail

	ZTD_TEXT_INLINE_ABI_NAMESPACE_CLOSE_I_
}} // namespace ztd::text

#endif // ZTD_TEXT_DETAIL_ENCODING_NAME_HPP
