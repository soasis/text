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

#include <ztd/text/count_code_units.hpp>

#include <catch2/catch.hpp>

#include <ztd/text/tests/basic_unicode_strings.hpp>

TEST_CASE("text/count_code_units/core", "basic usages of count_code_units function do not explode") {
	SECTION("execution") {
		auto result0
		     = ztd::text::count_code_units(ztd::text::tests::u32_basic_source_character_set, ztd::text::execution {});
		std::size_t expected0 = std::size(ztd::text::tests::basic_source_character_set);
		REQUIRE_FALSE(result0.handled_error);
		REQUIRE(result0.count == expected0);
	}
	SECTION("wide_execution") {
		auto result0 = ztd::text::count_code_units(
		     ztd::text::tests::u32_basic_source_character_set, ztd::text::wide_execution {});
		std::size_t expected0 = std::size(ztd::text::tests::w_basic_source_character_set);
		REQUIRE_FALSE(result0.handled_error);
		REQUIRE(result0.count == expected0);

		auto result1 = ztd::text::count_code_units(
		     ztd::text::tests::u32_unicode_sequence_truth_native_endian, ztd::text::wide_execution {});
		std::size_t expected1 = std::size(ztd::text::tests::w_unicode_sequence_truth_native_endian);
		REQUIRE_FALSE(result1.handled_error);
		REQUIRE(result1.count == expected1);
	}
	SECTION("utf8") {
		auto result0
		     = ztd::text::count_code_units(ztd::text::tests::u32_basic_source_character_set, ztd::text::utf8 {});
		std::size_t expected0 = std::size(ztd::text::tests::u8_basic_source_character_set);
		REQUIRE_FALSE(result0.handled_error);
		REQUIRE(result0.count == expected0);

		auto result1 = ztd::text::count_code_units(
		     ztd::text::tests::u32_unicode_sequence_truth_native_endian, ztd::text::utf8 {});
		std::size_t expected1 = std::size(ztd::text::tests::u8_unicode_sequence_truth_native_endian);
		REQUIRE_FALSE(result1.handled_error);
		REQUIRE(result1.count == expected1);
	}
	SECTION("utf16") {
		auto result0
		     = ztd::text::count_code_units(ztd::text::tests::u32_basic_source_character_set, ztd::text::utf16 {});
		std::size_t expected0 = std::size(ztd::text::tests::u16_basic_source_character_set);
		REQUIRE_FALSE(result0.handled_error);
		REQUIRE(result0.count == expected0);

		auto result1 = ztd::text::count_code_units(
		     ztd::text::tests::u32_unicode_sequence_truth_native_endian, ztd::text::utf16 {});
		std::size_t expected1 = std::size(ztd::text::tests::u16_unicode_sequence_truth_native_endian);
		REQUIRE_FALSE(result1.handled_error);
		REQUIRE(result1.count == expected1);
	}
	SECTION("utf32") {
		auto result0
		     = ztd::text::count_code_units(ztd::text::tests::u32_basic_source_character_set, ztd::text::utf32 {});
		std::size_t expected0 = std::size(ztd::text::tests::u32_basic_source_character_set);
		REQUIRE_FALSE(result0.handled_error);
		REQUIRE(result0.count == expected0);

		auto result1 = ztd::text::count_code_units(
		     ztd::text::tests::u32_unicode_sequence_truth_native_endian, ztd::text::utf32 {});
		std::size_t expected1 = std::size(ztd::text::tests::u32_unicode_sequence_truth_native_endian);
		REQUIRE_FALSE(result1.handled_error);
		REQUIRE(result1.count == expected1);
	}
}
