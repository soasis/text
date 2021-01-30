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
// =============================================================================

#include <ztd/text/decode.hpp>

#include <ztd/text/encoding.hpp>

#include <catch2/catch.hpp>

#include <ztd/text/tests/basic_unicode_strings.hpp>

TEST_CASE("text/decode/core", "basic usages of decode function do not explode") {
	SECTION("execution") {
		std::u32string result0 = ztd::text::decode(
		     ztd::text::tests::ansi_sequence_truth, ztd::text::execution {}, ztd::text::replacement_handler {});
		REQUIRE(result0 == ztd::text::tests::u32_ansi_sequence_truth);
	}
	SECTION("wide_execution") {
		std::u32string result0 = ztd::text::decode(ztd::text::tests::w_ansi_sequence_truth,
		     ztd::text::wide_execution {}, ztd::text::replacement_handler {});
		REQUIRE(result0 == ztd::text::tests::u32_ansi_sequence_truth);

		std::u32string result1 = ztd::text::decode(
		     ztd::text::tests::w_unicode_sequence_truth, ztd::text::wide_execution {}, ztd::text::default_handler {});
		REQUIRE(result1 == ztd::text::tests::u32_unicode_sequence_truth);
	}
	SECTION("utf8") {
		std::u32string result0 = ztd::text::decode(ztd::text::tests::u8_ansi_sequence_truth, ztd::text::utf8 {});
		REQUIRE(result0 == ztd::text::tests::u32_ansi_sequence_truth);

		std::u32string result1 = ztd::text::decode(ztd::text::tests::u8_unicode_sequence_truth, ztd::text::utf8 {});
		REQUIRE(result1 == ztd::text::tests::u32_unicode_sequence_truth);
	}
	SECTION("utf16") {
		std::u32string result0 = ztd::text::decode(ztd::text::tests::u16_ansi_sequence_truth, ztd::text::utf16 {});
		REQUIRE(result0 == ztd::text::tests::u32_ansi_sequence_truth);

		std::u32string result1 = ztd::text::decode(ztd::text::tests::u16_unicode_sequence_truth, ztd::text::utf16 {});
		REQUIRE(result1 == ztd::text::tests::u32_unicode_sequence_truth);
	}
	SECTION("utf32") {
		std::u32string result0 = ztd::text::decode(ztd::text::tests::u32_ansi_sequence_truth, ztd::text::utf32 {});
		REQUIRE(result0 == ztd::text::tests::u32_ansi_sequence_truth);

		std::u32string result1 = ztd::text::decode(ztd::text::tests::u32_unicode_sequence_truth, ztd::text::utf32 {});
		REQUIRE(result1 == ztd::text::tests::u32_unicode_sequence_truth);
	}
	SECTION("literal") {
		std::u32string result0
		     = ztd::text::decode(ztd::text::tests::basic_source_character_set, ztd::text::literal {});
		REQUIRE(result0 == ztd::text::tests::u32_basic_source_character_set);
	}
	SECTION("wide_literal") {
		std::u32string result0
		     = ztd::text::decode(ztd::text::tests::w_basic_source_character_set, ztd::text::wide_literal {});
		REQUIRE(result0 == ztd::text::tests::u32_basic_source_character_set);
	}
}
