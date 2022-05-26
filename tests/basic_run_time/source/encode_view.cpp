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

#include <ztd/text/encode_view.hpp>

#include <catch2/catch_all.hpp>

#include <ztd/text/tests/basic_unicode_strings.hpp>

template <typename Encoding, typename Input, typename Expected>
void check_encode_view(const Input& input, const Expected& expected_output) {
	ztd::text::encode_view<Encoding> result0_view(input);
	auto result0_it         = result0_view.begin();
	const auto result0_last = result0_view.end();
	auto truth0_it          = std::cbegin(expected_output);
	const auto truth0_last  = std::cend(expected_output);
	for (; result0_it != result0_last; ++result0_it, (void)++truth0_it) {
		REQUIRE(result0_it.error_code() == ztd::text::encoding_error::ok);
		REQUIRE(truth0_it != truth0_last);
		const auto truth0_val  = *truth0_it;
		const auto result0_val = *result0_it;
		REQUIRE(truth0_val == result0_val);
	}
}

TEST_CASE("text/encode_view/basic", "basic usages of encode_view type do not explode") {
	SECTION("execution") {
		ztd::text::execution_t encoding {};

		check_encode_view<ztd::text::execution_t>(
		     ztd::tests::u32_basic_source_character_set, ztd::tests::basic_source_character_set);

		if (ztd::text::contains_unicode_encoding(encoding)) {
			check_encode_view<ztd::text::execution_t>(ztd::tests::u32_unicode_sequence_truth_native_endian,
			     ztd::tests::unicode_sequence_truth_native_endian);
		}
	}
	SECTION("wide_execution") {
		ztd::text::wide_execution_t encoding {};

		check_encode_view<ztd::text::wide_execution_t>(
		     ztd::tests::u32_basic_source_character_set, ztd::tests::w_basic_source_character_set);

		if (ztd::text::contains_unicode_encoding(encoding)) {
			check_encode_view<ztd::text::wide_execution_t>(ztd::tests::u32_unicode_sequence_truth_native_endian,
			     ztd::tests::w_unicode_sequence_truth_native_endian);
		}
	}
	SECTION("literal") {
		ztd::text::literal_t encoding {};

		check_encode_view<ztd::text::literal_t>(
		     ztd::tests::u32_basic_source_character_set, ztd::tests::basic_source_character_set);

		if (ztd::text::contains_unicode_encoding(encoding)) {
			check_encode_view<ztd::text::literal_t>(ztd::tests::u32_unicode_sequence_truth_native_endian,
			     ztd::tests::unicode_sequence_truth_native_endian);
		}
	}
	SECTION("wide_literal") {
		ztd::text::wide_literal_t encoding {};

		check_encode_view<ztd::text::wide_literal_t>(
		     ztd::tests::u32_basic_source_character_set, ztd::tests::w_basic_source_character_set);

		if (ztd::text::contains_unicode_encoding(encoding)) {
			check_encode_view<ztd::text::wide_literal_t>(ztd::tests::u32_unicode_sequence_truth_native_endian,
			     ztd::tests::w_unicode_sequence_truth_native_endian);
		}
	}
	SECTION("utf8") {
		check_encode_view<ztd::text::utf8_t>(
		     ztd::tests::u32_basic_source_character_set, ztd::tests::u8_basic_source_character_set);
		check_encode_view<ztd::text::utf8_t>(ztd::tests::u32_unicode_sequence_truth_native_endian,
		     ztd::tests::u8_unicode_sequence_truth_native_endian);
	}
	SECTION("utf16") {
		check_encode_view<ztd::text::utf16_t>(
		     ztd::tests::u32_basic_source_character_set, ztd::tests::u16_basic_source_character_set);
		check_encode_view<ztd::text::utf16_t>(ztd::tests::u32_unicode_sequence_truth_native_endian,
		     ztd::tests::u16_unicode_sequence_truth_native_endian);
	}
	SECTION("utf32") {
		check_encode_view<ztd::text::utf32_t>(
		     ztd::tests::u32_basic_source_character_set, ztd::tests::u32_basic_source_character_set);
		check_encode_view<ztd::text::utf32_t>(ztd::tests::u32_unicode_sequence_truth_native_endian,
		     ztd::tests::u32_unicode_sequence_truth_native_endian);
	}
}
