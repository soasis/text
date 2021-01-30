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

#include <ztd/text/count_code_points.hpp>

#include <catch2/catch.hpp>

#include <ztd/text/tests/basic_unicode_strings.hpp>

TEST_CASE("text/count_code_points/core", "basic usages of count_code_points function do not explode") {
	std::size_t expected0 = std::size(ztd::text::tests::u32_ansi_sequence_truth);
	std::size_t expected1 = std::size(ztd::text::tests::u32_unicode_sequence_truth);
	SECTION("execution") {
		ztd::text::count_result result0
		     = ztd::text::count_code_points(ztd::text::tests::ansi_sequence_truth, ztd::text::execution {});
		REQUIRE_FALSE(result0.handled_error);
		REQUIRE(result0.count == expected0);
	}
	SECTION("wide_execution") {
		ztd::text::count_result result0
		     = ztd::text::count_code_points(ztd::text::tests::w_ansi_sequence_truth, ztd::text::wide_execution {});
		REQUIRE_FALSE(result0.handled_error);
		REQUIRE(result0.count == expected0);

		ztd::text::count_result result1
		     = ztd::text::count_code_points(ztd::text::tests::w_unicode_sequence_truth, ztd::text::wide_execution {});
		REQUIRE_FALSE(result1.handled_error);
		REQUIRE(result1.count == expected1);
	}
	SECTION("utf8") {
		ztd::text::count_result result0
		     = ztd::text::count_code_points(ztd::text::tests::u8_ansi_sequence_truth, ztd::text::utf8 {});
		REQUIRE_FALSE(result0.handled_error);
		REQUIRE(result0.count == expected0);

		ztd::text::count_result result1
		     = ztd::text::count_code_points(ztd::text::tests::u8_unicode_sequence_truth, ztd::text::utf8 {});
		REQUIRE_FALSE(result1.handled_error);
		REQUIRE(result1.count == expected1);
	}
	SECTION("utf16") {
		ztd::text::count_result result0
		     = ztd::text::count_code_points(ztd::text::tests::u16_ansi_sequence_truth, ztd::text::utf16 {});
		REQUIRE_FALSE(result0.handled_error);
		REQUIRE(result0.count == expected0);

		ztd::text::count_result result1
		     = ztd::text::count_code_points(ztd::text::tests::u16_unicode_sequence_truth, ztd::text::utf16 {});
		REQUIRE_FALSE(result1.handled_error);
		REQUIRE(result1.count == expected1);
	}
	SECTION("utf32") {
		ztd::text::count_result result0
		     = ztd::text::count_code_points(ztd::text::tests::u32_ansi_sequence_truth, ztd::text::utf16 {});
		REQUIRE_FALSE(result0.handled_error);
		REQUIRE(result0.count == expected0);

		ztd::text::count_result result1
		     = ztd::text::count_code_points(ztd::text::tests::u32_unicode_sequence_truth, ztd::text::utf32 {});
		REQUIRE_FALSE(result1.handled_error);
		REQUIRE(result1.count == expected1);
	}
}
