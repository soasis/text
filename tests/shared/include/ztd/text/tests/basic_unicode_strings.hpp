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

#ifndef ZTD_TEXT_TESTS_BASIC_UNICODE_STRINGS_HPP
#define ZTD_TEXT_TESTS_BASIC_UNICODE_STRINGS_HPP

#include <ztd/text/version.hpp>

#include <ztd/text/encoding.hpp>
#include <ztd/text/is_unicode_encoding.hpp>

#include <ztd/idk/charN_t.hpp>
#include <ztd/idk/endian.hpp>
#include <ztd/idk/span.hpp>
#include <ztd/tests/basic_unicode_strings.hpp>

#if ZTD_IS_ON(ZTD_COMPILER_VCXX)
#pragma warning(push)
#pragma warning(disable : 4310)
#endif

namespace ztd { namespace text { namespace tests {

	template <typename Encoding>
	constexpr auto basic_source_character_set_for() {
		using CodeUnit = ztd::text::code_unit_t<Encoding>;
		if constexpr (std::is_same_v<Encoding,
				         utf8_t> || std::is_same_v<Encoding, mutf8_t> || std::is_same_v<Encoding, wtf8_t>) {
			return ztd::tests::u8_basic_source_character_set;
		}
		else {
			if constexpr (std::is_same_v<CodeUnit, char>) {
				return ztd::tests::u8_basic_source_character_set;
			}
			else if constexpr (std::is_same_v<CodeUnit, wchar_t>) {
				return ztd::tests::w_basic_source_character_set;
			}
			else if constexpr (std::is_same_v<CodeUnit, ztd::uchar8_t>) {
				return ztd::tests::u8_basic_source_character_set;
			}
#if ZTD_IS_ON(ZTD_NATIVE_CHAR8_T)
			else if constexpr (std::is_same_v<CodeUnit, char8_t>) {
				return ztd::tests::u8_basic_source_character_set;
			}
#endif
			else if constexpr (std::is_same_v<CodeUnit, char16_t>) {
				return ztd::tests::u16_basic_source_character_set;
			}
			else if constexpr (std::is_same_v<CodeUnit, char32_t>) {
				return ztd::tests::u32_basic_source_character_set;
			}
			else {
				static_assert(ztd::always_false_v<Encoding>,
					"Cannot pick basic_source_character_set object for this Encoding");
			}
		}
	}

	template <typename Encoding>
	constexpr auto unicode_sequence_for() {
		using CodeUnit = ztd::text::code_unit_t<Encoding>;
		if constexpr (std::is_same_v<Encoding, utf16_le_t>) {
			return ztd::tests::u16_unicode_sequence_truth_little_endian;
		}
		else if constexpr (std::is_same_v<Encoding, utf16_be_t>) {
			return ztd::tests::u16_unicode_sequence_truth_big_endian;
		}
		else if constexpr (std::is_same_v<Encoding, utf16_ne_t>) {
			return ztd::tests::u16_unicode_sequence_truth_native_endian;
		}
		else if constexpr (std::is_same_v<Encoding, utf32_le_t>) {
			return ztd::tests::u32_unicode_sequence_truth_little_endian;
		}
		else if constexpr (std::is_same_v<Encoding, utf32_be_t>) {
			return ztd::tests::u32_unicode_sequence_truth_big_endian;
		}
		else if constexpr (std::is_same_v<Encoding, utf32_ne_t>) {
			return ztd::tests::u32_unicode_sequence_truth_native_endian;
		}
		else {
			if constexpr (std::is_same_v<CodeUnit, char>) {
				if constexpr (std::is_same_v<Encoding,
						         ztd::text::literal_t> && !ztd::text::is_unicode_encoding_v<Encoding>) {
					static_assert(ztd::always_false_v<Encoding>,
						"The encoding ztd::text::literal_t is not a unicode encoding, and therefore doesn't "
						"work here!");
				}
				return ztd::tests::unicode_sequence_truth_native_endian;
			}
			else if constexpr (std::is_same_v<CodeUnit, wchar_t>) {
				if constexpr (std::is_same_v<Encoding,
						         ztd::text::wide_literal_t> && !ztd::text::is_unicode_encoding_v<Encoding>) {
					static_assert(ztd::always_false_v<Encoding>,
						"The encoding ztd::text::literal_t is not a unicode encoding, and therefore doesn't "
						"work here!");
				}
				return ztd::tests::w_unicode_sequence_truth_native_endian;
			}
			else if constexpr (std::is_same_v<CodeUnit, uchar8_t>) {
				return ztd::tests::u8_unicode_sequence_truth_native_endian;
			}
#if ZTD_IS_ON(ZTD_NATIVE_CHAR8_T)
			else if constexpr (std::is_same_v<CodeUnit, char8_t>) {
				return ztd::tests::u8_unicode_sequence_truth_native_endian;
			}
#endif
			else if constexpr (std::is_same_v<CodeUnit, char16_t>) {
				return ztd::tests::u16_unicode_sequence_truth_native_endian;
			}
			else if constexpr (std::is_same_v<CodeUnit, char32_t>) {
				return ztd::tests::u32_unicode_sequence_truth_native_endian;
			}
			else {
				static_assert(ztd::always_false_v<Encoding>,
					"Cannot pick basic_source_character_set object for this Encoding");
			}
		}
	}
}}} // namespace ztd::text::tests

#if ZTD_IS_ON(ZTD_COMPILER_VCXX)
#pragma warning(pop)
#endif

#endif
