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

#include <ztd/text/encoding.hpp>

#include <catch2/catch_all.hpp>

#include <ztd/text/tests/basic_unicode_strings.hpp>

#include <ztd/idk/span.hpp>

#include <algorithm>

inline namespace ztd_text_tests_basic_run_time_encoding {
	template <typename Encoding, typename Input, typename Expected>
	void check_one(const Encoding& encoding, const Input& encoded, const Expected& decoded) {
		constexpr const auto decode_output_max = ztd::text::max_code_points_v<Encoding>;
		constexpr const auto encode_output_max = ztd::text::max_code_units_v<Encoding>;
		using CodePoint                        = ztd::text::code_point_t<Encoding>;
		using CodeUnit                         = ztd::text::code_unit_t<Encoding>;

		CodePoint decode_output_storage[decode_output_max] {};
		ztd::span<CodePoint, decode_output_max> decode_output_buffer(decode_output_storage, decode_output_max);
		ztd::text::decode_state_t<Encoding> decode_state {};
		auto decode_result
		     = encoding.decode_one(encoded, decode_output_buffer, ztd::text::replacement_handler, decode_state);
		ztd::span<CodePoint> decode_output(decode_output_buffer.data(), decode_result.output.data());
		bool decode_output_equal = std::equal(
		     decode_output.begin(), decode_output.end(), decoded.begin(), decoded.begin() + decode_output.size());
		REQUIRE(decode_result.error_code == ztd::text::encoding_error::ok);
		REQUIRE_FALSE(decode_result.errors_were_handled());
		REQUIRE(decode_output_equal);

		CodeUnit encode_output_storage[encode_output_max] {};
		ztd::span<CodeUnit, encode_output_max> encode_output_buffer(encode_output_storage, encode_output_max);
		ztd::text::encode_state_t<Encoding> encode_state {};
		auto encode_result
		     = encoding.encode_one(decoded, encode_output_buffer, ztd::text::replacement_handler, encode_state);
		ztd::span<CodeUnit> encode_output(encode_output_buffer.data(), encode_result.output.data());
		bool encode_output_equal = std::equal(
		     encode_output.begin(), encode_output.end(), encoded.begin(), encoded.begin() + encode_output.size());
		REQUIRE(encode_result.error_code == ztd::text::encoding_error::ok);
		REQUIRE_FALSE(encode_result.errors_were_handled());
		REQUIRE(encode_output_equal);

		bool decode_result_input_okay = std::equal(decode_result.input.begin(), decode_result.input.end(),
		     encoded.begin() + encode_output.size(), encoded.end());
		bool encode_result_input_okay = std::equal(encode_result.input.begin(), encode_result.input.end(),
		     decoded.begin() + decode_output.size(), decoded.end());
		REQUIRE(decode_result_input_okay);
		REQUIRE(encode_result_input_okay);

		bool decode_result_output_okay = std::equal(decode_result.output.begin(), decode_result.output.end(),
		     decode_output_buffer.begin() + decode_output.size(), decode_output_buffer.end());
		bool encode_result_output_okay = std::equal(encode_result.output.begin(), encode_result.output.end(),
		     encode_output_buffer.begin() + encode_output.size(), encode_output_buffer.end());
		REQUIRE(decode_result_output_okay);
		REQUIRE(encode_result_output_okay);
	}
} // namespace ztd_text_tests_basic_run_time_encoding

TEST_CASE("text/encoding/core", "basic usages of encoding do not explode") {
	check_one(
	     ztd::text::execution, ztd::tests::basic_source_character_set, ztd::tests::u32_basic_source_character_set);
	check_one(ztd::text::wide_execution, ztd::tests::w_basic_source_character_set,
	     ztd::tests::u32_basic_source_character_set);
	check_one(ztd::text::literal, ztd::tests::basic_source_character_set, ztd::tests::u32_basic_source_character_set);
	check_one(ztd::text::wide_literal, ztd::tests::w_basic_source_character_set,
	     ztd::tests::u32_basic_source_character_set);
	check_one(ztd::text::utf8, ztd::tests::u8_basic_source_character_set, ztd::tests::u32_basic_source_character_set);
	check_one(
	     ztd::text::utf16, ztd::tests::u16_basic_source_character_set, ztd::tests::u32_basic_source_character_set);
	check_one(
	     ztd::text::utf32, ztd::tests::u32_basic_source_character_set, ztd::tests::u32_basic_source_character_set);

	if (ztd::text::contains_unicode_encoding(ztd::text::execution)) {
		check_one(ztd::text::execution, ztd::tests::unicode_sequence_truth_native_endian,
		     ztd::tests::u32_unicode_sequence_truth_native_endian);
	}
	if (ztd::text::contains_unicode_encoding(ztd::text::wide_execution)) {
		check_one(ztd::text::wide_execution, ztd::tests::w_unicode_sequence_truth_native_endian,
		     ztd::tests::u32_unicode_sequence_truth_native_endian);
	}
	if (ztd::text::contains_unicode_encoding(ztd::text::literal)) {
		check_one(ztd::text::literal, ztd::tests::unicode_sequence_truth_native_endian,
		     ztd::tests::u32_unicode_sequence_truth_native_endian);
	}
	if (ztd::text::contains_unicode_encoding(ztd::text::wide_literal)) {
		check_one(ztd::text::wide_literal, ztd::tests::w_unicode_sequence_truth_native_endian,
		     ztd::tests::u32_unicode_sequence_truth_native_endian);
	}
	check_one(ztd::text::utf8, ztd::tests::u8_unicode_sequence_truth_native_endian,
	     ztd::tests::u32_unicode_sequence_truth_native_endian);
	check_one(ztd::text::utf16, ztd::tests::u16_unicode_sequence_truth_native_endian,
	     ztd::tests::u32_unicode_sequence_truth_native_endian);
	check_one(ztd::text::utf32, ztd::tests::u32_unicode_sequence_truth_native_endian,
	     ztd::tests::u32_unicode_sequence_truth_native_endian);
}
