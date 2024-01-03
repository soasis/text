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

#include <ztd/text/decode.hpp>

#include <ztd/text/encoding.hpp>

#include <catch2/catch_all.hpp>

#include <ztd/text/tests/basic_unicode_strings.hpp>
#include <ztd/tests/compare_each.hpp>

TEST_CASE("text/decode/core", "basic usages of decode function do not explode") {
	SECTION("execution") {
		ztd::text::execution_t encoding {};

		std::u32string result0
		     = ztd::text::decode(ztd::tests::basic_source_character_set, encoding, ztd::text::replacement_handler);
		ztd::tests::compare_each(result0, ztd::tests::u32_basic_source_character_set);

		if (ztd::text::contains_unicode_encoding(encoding)) {
			std::u32string result1 = ztd::text::decode(
			     ztd::tests::unicode_sequence_truth_native_endian, encoding, ztd::text::replacement_handler);
			ztd::tests::compare_each(result1, ztd::tests::u32_unicode_sequence_truth_native_endian);
		}
	}
	SECTION("wide_execution") {
		ztd::text::wide_execution_t encoding {};
		std::u32string result0
		     = ztd::text::decode(ztd::tests::w_basic_source_character_set, encoding, ztd::text::replacement_handler);
		ztd::tests::compare_each(result0, ztd::tests::u32_basic_source_character_set);
	}
	SECTION("wide_execution-2") {
		ztd::text::wide_execution_t encoding {};
		if (ztd::text::contains_unicode_encoding(encoding)) {
			std::u32string result1 = ztd::text::decode(
			     ztd::tests::w_unicode_sequence_truth_native_endian, encoding, ztd::text::replacement_handler);
			ztd::tests::compare_each(result1, ztd::tests::u32_unicode_sequence_truth_native_endian);
		}
	}
	SECTION("literal") {
		ztd::text::literal_t encoding {};
		std::u32string result0 = ztd::text::decode(ztd::tests::basic_source_character_set, ztd::text::literal);
		ztd::tests::compare_each(result0, ztd::tests::u32_basic_source_character_set);

		if (ztd::text::contains_unicode_encoding(encoding)) {
			std::u32string result1 = ztd::text::decode(
			     ztd::tests::unicode_sequence_truth_native_endian, encoding, ztd::text::replacement_handler);
			ztd::tests::compare_each(result1, ztd::tests::u32_unicode_sequence_truth_native_endian);
		}
	}
	SECTION("wide_literal") {
		ztd::text::wide_literal_t encoding {};
		std::u32string result0
		     = ztd::text::decode(ztd::tests::w_basic_source_character_set, encoding, ztd::text::replacement_handler);
		ztd::tests::compare_each(result0, ztd::tests::u32_basic_source_character_set);

		if (ztd::text::contains_unicode_encoding(encoding)) {
			std::u32string result1 = ztd::text::decode(
			     ztd::tests::w_unicode_sequence_truth_native_endian, encoding, ztd::text::replacement_handler);
			ztd::tests::compare_each(result1, ztd::tests::u32_unicode_sequence_truth_native_endian);
		}
	}
	SECTION("utf8") {
		std::u32string result0 = ztd::text::decode(ztd::tests::u8_basic_source_character_set, ztd::text::utf8);
		ztd::tests::compare_each(result0, ztd::tests::u32_basic_source_character_set);

		std::u32string result1
		     = ztd::text::decode(ztd::tests::u8_unicode_sequence_truth_native_endian, ztd::text::utf8);
		ztd::tests::compare_each(result1, ztd::tests::u32_unicode_sequence_truth_native_endian);
	}
	SECTION("basic_utf8<std::byte>") {
		std::u32string result0
		     = ztd::text::decode(ztd::tests::u8_basic_source_character_set, ztd::text::basic_utf8<std::byte> {});
		ztd::tests::compare_each(result0, ztd::tests::u32_basic_source_character_set);

		std::u32string result1 = ztd::text::decode(
		     ztd::tests::u8_unicode_sequence_truth_native_endian, ztd::text::basic_utf8<std::byte> {});
		ztd::tests::compare_each(result1, ztd::tests::u32_unicode_sequence_truth_native_endian);
	}
	SECTION("utf16") {
		std::u32string result0 = ztd::text::decode(ztd::tests::u16_basic_source_character_set, ztd::text::utf16);
		ztd::tests::compare_each(result0, ztd::tests::u32_basic_source_character_set);

		std::u32string result1
		     = ztd::text::decode(ztd::tests::u16_unicode_sequence_truth_native_endian, ztd::text::utf16);
		ztd::tests::compare_each(result1, ztd::tests::u32_unicode_sequence_truth_native_endian);
	}
	SECTION("utf32") {
		std::u32string result0 = ztd::text::decode(ztd::tests::u32_basic_source_character_set, ztd::text::utf32);
		ztd::tests::compare_each(result0, ztd::tests::u32_basic_source_character_set);

		std::u32string result1
		     = ztd::text::decode(ztd::tests::u32_unicode_sequence_truth_native_endian, ztd::text::utf32);
		ztd::tests::compare_each(result1, ztd::tests::u32_unicode_sequence_truth_native_endian);
	}
}
