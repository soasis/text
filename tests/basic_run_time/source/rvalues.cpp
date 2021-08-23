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

#include <ztd/text/decode.hpp>
#include <ztd/text/encode.hpp>
#include <ztd/text/transcode.hpp>

#include <catch2/catch.hpp>

#include <ztd/text/tests/basic_unicode_strings.hpp>

#include <string>
#include <sstream>

TEST_CASE("text/r-values", "r-values placed into top-level commands do not cause extreme compiler vomit") {
	std::stringstream message_stream;
	message_stream << ztd::tests::basic_source_character_set;

	SECTION("decode") {
		std::u32string result = ztd::text::decode(message_stream.str(), ztd::text::compat_utf8);
		REQUIRE(result == ztd::tests::u32_basic_source_character_set);
	}
	SECTION("decode_to") {
		auto result = ztd::text::decode_to<std::u32string>(message_stream.str(), ztd::text::compat_utf8);
		REQUIRE(result.error_code == ztd::text::encoding_error::ok);
		REQUIRE(result.handled_errors == 0);
		REQUIRE(result.input.empty());
		REQUIRE(result.output == ztd::tests::u32_basic_source_character_set);
	}
	SECTION("encode") {
		std::array<char32_t, ztd::tests::u32_basic_source_character_set.size()> input {};
		std::copy(ztd::tests::u32_basic_source_character_set.begin(),
		     ztd::tests::u32_basic_source_character_set.end(), input.begin());
		std::string result = ztd::text::encode(std::move(input), ztd::text::compat_utf8);
		REQUIRE(result == ztd::tests::basic_source_character_set);
	}
	SECTION("encode_to") {
		std::array<char32_t, ztd::tests::u32_basic_source_character_set.size()> input {};
		std::copy(ztd::tests::u32_basic_source_character_set.begin(),
		     ztd::tests::u32_basic_source_character_set.end(), input.begin());
		const std::vector<ztd::uchar8_t> expected(
		     ztd::tests::u8_basic_source_character_set.begin(), ztd::tests::u8_basic_source_character_set.end());
		auto result = ztd::text::encode_to<std::vector<ztd::uchar8_t>>(std::move(input), ztd::text::compat_utf8);
		REQUIRE(result.error_code == ztd::text::encoding_error::ok);
		REQUIRE(result.handled_errors == 0);
		REQUIRE(result.input.empty());
		REQUIRE(result.output == expected);
	}
	SECTION("transcode") {
		std::string result
		     = ztd::text::transcode(message_stream.str(), ztd::text::compat_utf8, ztd::text::compat_utf8);
		REQUIRE(result == ztd::tests::basic_source_character_set);
	}
	SECTION("transcode_to") {
		auto result = ztd::text::transcode_to<std::string>(
		     message_stream.str(), ztd::text::compat_utf8, ztd::text::compat_utf8);
		REQUIRE(result.error_code == ztd::text::encoding_error::ok);
		REQUIRE(result.handled_errors == 0);
		REQUIRE(result.input.empty());
		REQUIRE(result.output == ztd::tests::basic_source_character_set);
	}
}
