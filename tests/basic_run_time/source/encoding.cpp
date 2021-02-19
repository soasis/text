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

#include <catch2/catch.hpp>

#include <ztd/text/tests/basic_unicode_strings.hpp>

TEST_CASE("text/encoding/core", "basic usages of encoding do not explode") {
	constexpr const std::size_t encode_output_max = 16;
	constexpr const std::size_t decode_output_max = 2;

	SECTION("decode") {
		SECTION("execution") {
			ztd::text::execution enc;

			ztd::text::unicode_code_point output0[decode_output_max] {};
			std::span<ztd::text::unicode_code_point> output_buffer0(output0, 1);
			ztd::text::execution::decode_state s0 {};
			auto result0 = enc.decode_one(std::string_view(ztd::text::tests::basic_source_character_set),
			     std::span<ztd::text::unicode_code_point>(output0, 1), ztd::text::default_handler {}, s0);
			REQUIRE(result0.error_code == ztd::text::encoding_error::ok);
			REQUIRE(std::u32string_view(output0) == ztd::text::tests::u32_basic_source_character_set);
		}
		SECTION("wide_execution") {
			ztd::text::wide_execution enc;

			ztd::text::unicode_code_point output0[decode_output_max] {};
			std::span<ztd::text::unicode_code_point> output_buffer0(output0, 1);
			ztd::text::wide_execution::decode_state s0 {};
			auto result0 = enc.decode_one(std::wstring_view(ztd::text::tests::w_basic_source_character_set),
			     std::span<ztd::text::unicode_code_point>(output0, 1), ztd::text::default_handler {}, s0);
			REQUIRE(result0.error_code == ztd::text::encoding_error::ok);
			REQUIRE(std::u32string_view(output0) == ztd::text::tests::u32_basic_source_character_set);
		}
		SECTION("utf8") {
			ztd::text::utf8 enc;

			ztd::text::unicode_code_point output0[decode_output_max] {};
			std::span<ztd::text::unicode_code_point> output_buffer0(output0, 1);
			ztd::text::utf8::state s0 {};
			auto result0 = enc.decode_one(std::u8string_view(ztd::text::tests::u8_basic_source_character_set),
			     output_buffer0, ztd::text::default_handler {}, s0);
			REQUIRE(result0.error_code == ztd::text::encoding_error::ok);
			REQUIRE(std::u32string_view(output0) == ztd::text::tests::u32_basic_source_character_set);

			ztd::text::unicode_code_point output1[decode_output_max] {};
			std::span<ztd::text::unicode_code_point> output_buffer1(output1, 1);
			ztd::text::utf8::state s1 {};
			auto result1 = enc.decode_one(std::u8string_view(ztd::text::tests::u8_basic_source_character_set),
			     output_buffer1, ztd::text::default_handler {}, s1);
			REQUIRE(result1.error_code == ztd::text::encoding_error::ok);
			REQUIRE(std::u32string_view(output1) == ztd::text::tests::u32_basic_source_character_set);
		}
		SECTION("utf16") {
			ztd::text::utf16 enc;

			ztd::text::unicode_code_point output0[decode_output_max] {};
			std::span<ztd::text::unicode_code_point> output_buffer0(output0, 1);
			ztd::text::utf16::state s0 {};
			auto result0 = enc.decode_one(std::u16string_view(ztd::text::tests::u16_basic_source_character_set),
			     output_buffer0, ztd::text::default_handler {}, s0);
			REQUIRE(result0.error_code == ztd::text::encoding_error::ok);
			REQUIRE(std::u32string_view(output0) == ztd::text::tests::u32_basic_source_character_set);

			ztd::text::unicode_code_point output1[decode_output_max] {};
			std::span<ztd::text::unicode_code_point> output_buffer1(output1, 1);
			ztd::text::utf16::state s1 {};
			auto result1 = enc.decode_one(std::u16string_view(ztd::text::tests::u16_basic_source_character_set),
			     output_buffer1, ztd::text::default_handler {}, s1);
			REQUIRE(result1.error_code == ztd::text::encoding_error::ok);
			REQUIRE(std::u32string_view(output1) == ztd::text::tests::u32_basic_source_character_set);
		}
		SECTION("utf32") {
			ztd::text::utf32 enc;

			ztd::text::unicode_code_point output0[decode_output_max] {};
			std::span<ztd::text::unicode_code_point> output_buffer0(output0, 1);
			ztd::text::utf32::state s0 {};
			auto result0 = enc.decode_one(std::u32string_view(ztd::text::tests::u32_basic_source_character_set),
			     output_buffer0, ztd::text::default_handler {}, s0);
			REQUIRE(result0.error_code == ztd::text::encoding_error::ok);
			REQUIRE(std::u32string_view(output0) == ztd::text::tests::u32_basic_source_character_set);
		}
	}
	SECTION("encode") {
		SECTION("execution") {
			ztd::text::execution enc;

			char output0[encode_output_max] {};
			std::span<char> output_buffer0(output0, encode_output_max);
			ztd::text::execution::encode_state s0 {};
			auto result0 = enc.encode_one(std::u32string_view(ztd::text::tests::u32_basic_source_character_set),
			     output_buffer0, ztd::text::default_handler {}, s0);
			REQUIRE(result0.error_code == ztd::text::encoding_error::ok);
			REQUIRE(std::string_view(output0) == ztd::text::tests::basic_source_character_set);
		}
		SECTION("wide_execution") {
			ztd::text::wide_execution enc;

			wchar_t output0[encode_output_max] {};
			std::span<wchar_t> output_buffer0(output0, encode_output_max);
			ztd::text::wide_execution::encode_state s0 {};
			auto result0 = enc.encode_one(std::u32string_view(ztd::text::tests::u32_basic_source_character_set),
			     output_buffer0, ztd::text::default_handler {}, s0);
			REQUIRE(result0.error_code == ztd::text::encoding_error::ok);
			REQUIRE(std::wstring_view(output0) == ztd::text::tests::w_basic_source_character_set);
		}
		SECTION("utf8") {
			ztd::text::utf8 enc;

			char8_t output0[encode_output_max] {};
			std::span<char8_t> output_buffer0(output0, encode_output_max);
			ztd::text::utf8::state s0 {};
			auto result0 = enc.encode_one(std::u32string_view(ztd::text::tests::u32_basic_source_character_set),
			     output_buffer0, ztd::text::default_handler {}, s0);
			REQUIRE(result0.error_code == ztd::text::encoding_error::ok);
			REQUIRE(std::u8string_view(output0) == ztd::text::tests::u8_basic_source_character_set);
		}
		SECTION("utf16") {
			ztd::text::utf16 enc;

			char16_t output0[encode_output_max] {};
			std::span<char16_t> output_buffer0(output0, encode_output_max);
			ztd::text::utf16::state s0 {};
			auto result0 = enc.encode_one(std::u32string_view(ztd::text::tests::u32_basic_source_character_set),
			     output_buffer0, ztd::text::default_handler {}, s0);
			REQUIRE(result0.error_code == ztd::text::encoding_error::ok);
			REQUIRE(std::u16string_view(output0) == ztd::text::tests::u16_basic_source_character_set);
		}
		SECTION("utf32") {
			ztd::text::utf32 enc;

			char32_t output0[encode_output_max] {};
			std::span<char32_t> output_buffer0(output0, encode_output_max);
			ztd::text::utf32::state s0 {};
			auto result0 = enc.encode_one(std::u32string_view(ztd::text::tests::u32_basic_source_character_set),
			     output_buffer0, ztd::text::default_handler {}, s0);
			REQUIRE(result0.error_code == ztd::text::encoding_error::ok);
			REQUIRE(std::u32string_view(output0) == ztd::text::tests::u32_basic_source_character_set);
		}
		SECTION("literal") {
			ztd::text::literal enc;

			char output0[encode_output_max] {};
			std::span<char> output_buffer0(output0, encode_output_max);
			ztd::text::literal::encode_state s0 {};
			auto result0 = enc.encode_one(std::u32string_view(ztd::text::tests::u32_basic_source_character_set),
			     output_buffer0, ztd::text::default_handler {}, s0);
			REQUIRE(result0.error_code == ztd::text::encoding_error::ok);
			REQUIRE(std::string_view(output0) == ztd::text::tests::basic_source_character_set);
		}
		SECTION("wide_literal") {
			ztd::text::wide_literal enc;

			wchar_t output0[encode_output_max] {};
			std::span<wchar_t> output_buffer0(output0, encode_output_max);
			ztd::text::wide_literal::encode_state s0 {};
			auto result0 = enc.encode_one(std::u32string_view(ztd::text::tests::u32_basic_source_character_set),
			     output_buffer0, ztd::text::default_handler {}, s0);
			REQUIRE(result0.error_code == ztd::text::encoding_error::ok);
			REQUIRE(std::wstring_view(output0) == ztd::text::tests::w_basic_source_character_set);
		}
	}
}
