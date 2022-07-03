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

#include <ztd/text/encoding_scheme.hpp>
#include <ztd/text/any_encoding.hpp>
#include <ztd/text/decode.hpp>

#include <ztd/idk/c_string_view.hpp>

#include <catch2/catch_all.hpp>

#include <ztd/text/tests/basic_unicode_strings.hpp>

#include <algorithm>

inline namespace ztd_text_tests_basic_runtime_any_encoding_decode {
	template <ztd::endian Endian = ztd::endian::native, typename BaseEncoding, typename BasicInput,
	     typename UnicodeInput>
	void check_decode(BaseEncoding&& base_encoding, BasicInput& input0, UnicodeInput& input1) {
		using UBaseEncoding = std::remove_cv_t<std::remove_reference_t<BaseEncoding>>;
		using Encoding      = ztd::text::any_encoding;

		const auto& expected0 = ztd::tests::u32_basic_source_character_set;
		const auto& expected1 = ztd::tests::u32_unicode_sequence_truth_native_endian;

		ztd::text::any_encoding encoding(
		     ztd::text::encoding_scheme<UBaseEncoding, Endian>(std::forward<BaseEncoding>(base_encoding)));

		ztd::text::any_encoding::decode_state state0 = ztd::text::make_decode_state(encoding);
		std::basic_string<char32_t> result0_storage(
		     std::size(input0) * ztd::text::max_code_points_v<Encoding>, char32_t {});
		ztd::span<char32_t> result0_storage_view(result0_storage.data(), result0_storage.size());
		auto result0
		     = ztd::text::decode_into(input0, encoding, result0_storage_view, ztd::text::replacement_handler, state0);
		std::basic_string_view<char32_t> result0_view(result0_storage_view.data(),
		     static_cast<std::size_t>(result0.output.data() - result0_storage_view.data()));
		bool is_equal0 = std::equal(result0_view.begin(), result0_view.end(), expected0.begin(), expected0.end());
		REQUIRE(result0.error_code == ztd::text::encoding_error::ok);
		REQUIRE_FALSE(result0.errors_were_handled());
		REQUIRE(std::size(result0.input) == 0);
		REQUIRE(is_equal0);

		if (ztd::text::contains_unicode_encoding(encoding)) {
			ztd::text::any_encoding::decode_state state1 = ztd::text::make_decode_state(encoding);
			std::basic_string<char32_t> result1_storage(
			     std::size(input1) * ztd::text::max_code_points_v<Encoding>, char32_t {});
			ztd::span<char32_t> result1_storage_view(result1_storage.data(), result1_storage.size());
			auto result1 = ztd::text::decode_into(
			     input1, encoding, result1_storage_view, ztd::text::replacement_handler, state1);
			std::basic_string_view<char32_t> result1_view(result1_storage_view.data(),
			     static_cast<std::size_t>(result1.output.data() - result1_storage_view.data()));
			bool is_equal1
			     = std::equal(result1_view.begin(), result1_view.end(), expected1.begin(), expected1.end());
			REQUIRE(result1.error_code == ztd::text::encoding_error::ok);
			REQUIRE_FALSE(result1.errors_were_handled());
			REQUIRE(std::size(result1.input) == 0);
			REQUIRE(is_equal1);
		}
	}
} // namespace ztd_text_tests_basic_runtime_any_encoding_decode

TEST_CASE("text/decode/any_encoding/encoding_scheme", "decode from byte arrays with specific endianness") {
	SECTION("endian::native") {
		SECTION("ascii") {
			check_decode<ztd::endian::native>(ztd::text::ascii,
			     ztd::tests::basic_source_character_set_bytes_native_endian,
			     ztd::tests::unicode_sequence_bytes_truth_native_endian);
		}
		SECTION("execution") {
			check_decode<ztd::endian::native>(ztd::text::execution,
			     ztd::tests::basic_source_character_set_bytes_native_endian,
			     ztd::tests::unicode_sequence_bytes_truth_native_endian);
		}
		SECTION("wide_execution") {
			check_decode<ztd::endian::native>(ztd::text::wide_execution,
			     ztd::tests::w_basic_source_character_set_bytes_native_endian,
			     ztd::tests::w_unicode_sequence_bytes_truth_native_endian);
		}
		SECTION("literal") {
			check_decode<ztd::endian::native>(ztd::text::literal,
			     ztd::tests::basic_source_character_set_bytes_native_endian,
			     ztd::tests::unicode_sequence_bytes_truth_native_endian);
		}
		SECTION("wide_literal") {
			check_decode<ztd::endian::native>(ztd::text::wide_literal,
			     ztd::tests::w_basic_source_character_set_bytes_native_endian,
			     ztd::tests::w_unicode_sequence_bytes_truth_native_endian);
		}
		SECTION("utf8") {
			check_decode<ztd::endian::native>(ztd::text::utf8,
			     ztd::tests::u8_basic_source_character_set_bytes_native_endian,
			     ztd::tests::u8_unicode_sequence_bytes_truth_native_endian);
		}
		SECTION("utf16") {
			check_decode<ztd::endian::native>(ztd::text::utf16,
			     ztd::tests::u16_basic_source_character_set_bytes_native_endian,
			     ztd::tests::u16_unicode_sequence_bytes_truth_native_endian);
		}
		SECTION("utf32") {
			check_decode<ztd::endian::native>(ztd::text::utf32,
			     ztd::tests::u32_basic_source_character_set_bytes_native_endian,
			     ztd::tests::u32_unicode_sequence_bytes_truth_native_endian);
		}
	}
	SECTION("endian::little") {
		SECTION("ascii") {
			check_decode<ztd::endian::little>(ztd::text::ascii,
			     ztd::tests::basic_source_character_set_bytes_little_endian,
			     ztd::tests::unicode_sequence_bytes_truth_little_endian);
		}
		SECTION("execution") {
			check_decode<ztd::endian::little>(ztd::text::execution,
			     ztd::tests::basic_source_character_set_bytes_little_endian,
			     ztd::tests::unicode_sequence_bytes_truth_little_endian);
		}
		SECTION("wide_execution") {
			check_decode<ztd::endian::little>(ztd::text::wide_execution,
			     ztd::tests::w_basic_source_character_set_bytes_little_endian,
			     ztd::tests::w_unicode_sequence_bytes_truth_little_endian);
		}
		SECTION("literal") {
			check_decode<ztd::endian::little>(ztd::text::literal,
			     ztd::tests::basic_source_character_set_bytes_little_endian,
			     ztd::tests::unicode_sequence_bytes_truth_little_endian);
		}
		SECTION("wide_literal") {
			check_decode<ztd::endian::little>(ztd::text::wide_literal,
			     ztd::tests::w_basic_source_character_set_bytes_little_endian,
			     ztd::tests::w_unicode_sequence_bytes_truth_little_endian);
		}
		SECTION("utf8") {
			check_decode<ztd::endian::little>(ztd::text::utf8,
			     ztd::tests::u8_basic_source_character_set_bytes_little_endian,
			     ztd::tests::u8_unicode_sequence_bytes_truth_little_endian);
		}
		SECTION("utf16") {
			check_decode<ztd::endian::little>(ztd::text::utf16,
			     ztd::tests::u16_basic_source_character_set_bytes_little_endian,
			     ztd::tests::u16_unicode_sequence_bytes_truth_little_endian);
		}
		SECTION("utf32") {
			check_decode<ztd::endian::little>(ztd::text::utf32,
			     ztd::tests::u32_basic_source_character_set_bytes_little_endian,
			     ztd::tests::u32_unicode_sequence_bytes_truth_little_endian);
		}
	}
	SECTION("endian::big") {
		SECTION("ascii") {
			check_decode<ztd::endian::big>(ztd::text::ascii, ztd::tests::basic_source_character_set_bytes_big_endian,
			     ztd::tests::unicode_sequence_bytes_truth_big_endian);
		}
		SECTION("execution") {
			check_decode<ztd::endian::big>(ztd::text::execution,
			     ztd::tests::basic_source_character_set_bytes_big_endian,
			     ztd::tests::unicode_sequence_bytes_truth_big_endian);
		}
		SECTION("wide_execution") {
			check_decode<ztd::endian::big>(ztd::text::wide_execution,
			     ztd::tests::w_basic_source_character_set_bytes_big_endian,
			     ztd::tests::w_unicode_sequence_bytes_truth_big_endian);
		}
		SECTION("literal") {
			check_decode<ztd::endian::big>(ztd::text::literal,
			     ztd::tests::basic_source_character_set_bytes_big_endian,
			     ztd::tests::unicode_sequence_bytes_truth_big_endian);
		}
		SECTION("wide_literal") {
			check_decode<ztd::endian::big>(ztd::text::wide_literal,
			     ztd::tests::w_basic_source_character_set_bytes_big_endian,
			     ztd::tests::w_unicode_sequence_bytes_truth_big_endian);
		}
		SECTION("utf8") {
			check_decode<ztd::endian::big>(ztd::text::utf8,
			     ztd::tests::u8_basic_source_character_set_bytes_big_endian,
			     ztd::tests::u8_unicode_sequence_bytes_truth_big_endian);
		}
		SECTION("utf16") {
			check_decode<ztd::endian::big>(ztd::text::utf16,
			     ztd::tests::u16_basic_source_character_set_bytes_big_endian,
			     ztd::tests::u16_unicode_sequence_bytes_truth_big_endian);
		}
		SECTION("utf32") {
			check_decode<ztd::endian::big>(ztd::text::utf32,
			     ztd::tests::u32_basic_source_character_set_bytes_big_endian,
			     ztd::tests::u32_unicode_sequence_bytes_truth_big_endian);
		}
	}
}
