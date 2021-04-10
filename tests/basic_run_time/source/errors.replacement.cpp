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

#include <ztd/text/encoding.hpp>
#include <ztd/text/detail/algorithm.hpp>

#include <catch2/catch.hpp>

#include <ztd/text/tests/basic_unicode_strings.hpp>

inline namespace ztd_text_tests_basic_run_time_errors_replacement {
	template <typename Encoding, typename Input, typename Expected>
	void decode_one_replace_check(Encoding& encoding, Input& container, Expected& expected) {
		constexpr std::size_t input_max  = ztd::text::max_code_units_v<Encoding>;
		constexpr std::size_t output_max = ztd::text::max_code_points_v<Encoding>;
		using code_point                 = ztd::text::code_point_t<Encoding>;
		using code_unit                  = ztd::text::code_unit_t<Encoding>;
		using state                      = ztd::text::decode_state_t<Encoding>;

		code_point output_buffer[output_max] {};
		ztd::text::span<const code_unit> input(container.data(), container.size());
		ztd::text::span<code_point> output(output_buffer, output_max);
		state s {};
		auto result   = encoding.decode_one(input, output, ztd::text::replacement_handler {}, s);
		bool is_equal = ztd::text::__txt_detail::__equal(
		     output.data(), result.output.data(), expected.begin(), expected.end());
		REQUIRE(is_equal);
	}

	template <typename Encoding, typename Input, typename Expected>
	void encode_one_replace_check(Encoding& encoding, Input& container, Expected& expected) {
		constexpr std::size_t input_max  = ztd::text::max_code_points_v<Encoding>;
		constexpr std::size_t output_max = ztd::text::max_code_units_v<Encoding>;
		using code_point                 = ztd::text::code_point_t<Encoding>;
		using code_unit                  = ztd::text::code_unit_t<Encoding>;
		using state                      = ztd::text::encode_state_t<Encoding>;

		code_unit output_buffer[output_max] {};
		ztd::text::span<const code_point> input(container.data(), container.size());
		ztd::text::span<code_unit> output(output_buffer, output_max);
		state s {};
		auto result   = encoding.encode_one(input, output, ztd::text::replacement_handler {}, s);
		bool is_equal = ztd::text::__txt_detail::__equal(
		     output.data(), result.output.data(), expected.begin(), expected.end());
		REQUIRE(is_equal);
	}
} // namespace ztd_text_tests_basic_run_time_errors_replacement

TEST_CASE("text/encoding/errors/replacement", "invalid characters are properly replacement") {
	SECTION("decode") {
		SECTION("utf8") {
			ztd::text::utf8 encoding;
			decode_one_replace_check(encoding, ztd::text::tests::u8_unicode_invalid_input,
			     ztd::text::tests::u32_unicode_replacement_truth);
		}
		SECTION("utf16") {
			ztd::text::utf16 encoding;
			decode_one_replace_check(encoding, ztd::text::tests::u16_unicode_invalid_input,
			     ztd::text::tests::u32_unicode_replacement_truth);
		}
		SECTION("utf32") {
			ztd::text::utf32 encoding;
			decode_one_replace_check(encoding, ztd::text::tests::u32_unicode_invalid_input,
			     ztd::text::tests::u32_unicode_replacement_truth);
		}
	}
	SECTION("encode") {
		SECTION("utf8") {
			ztd::text::utf8 encoding;
			encode_one_replace_check(encoding, ztd::text::tests::u32_unicode_invalid_input,
			     ztd::text::tests::u8_unicode_replacement_truth);
		}
		SECTION("utf16") {
			ztd::text::utf16 encoding;
			encode_one_replace_check(encoding, ztd::text::tests::u32_unicode_invalid_input,
			     ztd::text::tests::u16_unicode_replacement_truth);
		}
		SECTION("utf32") {
			ztd::text::utf32 encoding;
			encode_one_replace_check(encoding, ztd::text::tests::u32_unicode_invalid_input,
			     ztd::text::tests::u32_unicode_replacement_truth);
		}
	}
}
