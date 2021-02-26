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

#include <ztd/text/validate_code_units.hpp>
#include <ztd/text/encoding.hpp>

#include <catch2/catch.hpp>

#include <ztd/text/tests/basic_unicode_strings.hpp>

inline namespace ztd_text_tests_basic_run_time_validate_code_units {
	template <typename Input, typename Encoding>
	void validate_check(Input& input, Encoding& encoding) {
		auto result0 = ztd::text::validate_code_units(input);
		REQUIRE(result0);
		auto result1 = ztd::text::validate_code_units(input, encoding);
		REQUIRE(result1);
	}
} // namespace ztd_text_tests_basic_run_time_validate_code_units

TEST_CASE("text/validate_code_units/basic", "basic usages of validate_code_units function do not explode") {
	SECTION("execution") {
		ztd::text::execution encoding {};
		validate_check(ztd::text::tests::basic_source_character_set, encoding);
		if (ztd::text::contains_unicode_encoding(encoding)) {
			validate_check(ztd::text::tests::unicode_sequence_truth_native_endian, encoding);
		}
	}
	SECTION("wide_execution") {
		ztd::text::wide_execution encoding {};
		validate_check(ztd::text::tests::w_basic_source_character_set, encoding);
		if (ztd::text::contains_unicode_encoding(encoding)) {
			validate_check(ztd::text::tests::w_unicode_sequence_truth_native_endian, encoding);
		}
	}
	SECTION("literal") {
		ztd::text::literal encoding {};
		auto result0 = ztd::text::validate_code_units(ztd::text::tests::basic_source_character_set, encoding);
		REQUIRE(result0);
		if (ztd::text::contains_unicode_encoding(encoding)) {
			auto result1
			     = ztd::text::validate_code_units(ztd::text::tests::unicode_sequence_truth_native_endian, encoding);
			REQUIRE(result1);
		}
	}
	SECTION("wide_literal") {
		ztd::text::wide_literal encoding {};
		auto result0 = ztd::text::validate_code_units(ztd::text::tests::w_basic_source_character_set, encoding);
		REQUIRE(result0);
		if (ztd::text::contains_unicode_encoding(encoding)) {
			auto result1 = ztd::text::validate_code_units(
			     ztd::text::tests::w_unicode_sequence_truth_native_endian, encoding);
			REQUIRE(result1);
		}
	}
	SECTION("utf8") {
		ztd::text::utf8 encoding {};
		validate_check(ztd::text::tests::u8_basic_source_character_set, encoding);
		validate_check(ztd::text::tests::u8_unicode_sequence_truth_native_endian, encoding);
	}
	SECTION("utf16") {
		ztd::text::utf16 encoding {};
		validate_check(ztd::text::tests::u16_basic_source_character_set, encoding);
		validate_check(ztd::text::tests::u16_unicode_sequence_truth_native_endian, encoding);
	}
	SECTION("utf32") {
		ztd::text::utf32 encoding {};
		validate_check(ztd::text::tests::u32_basic_source_character_set, encoding);
		validate_check(ztd::text::tests::u32_unicode_sequence_truth_native_endian, encoding);
	}
}
