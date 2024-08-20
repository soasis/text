// =============================================================================
//
// ztd.text
// Copyright © JeanHeyd "ThePhD" Meneide and Shepherd's Oasis, LLC
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
// https://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
//
// ============================================================================ //

#include <ztd/text.hpp>

#include <catch2/catch_all.hpp>

TEST_CASE("snippets/transcode_to/basic",
     "transcode_to with a pass handler doing UTF-8 through an any_encoding does not run afoul with pivot values") {
	SECTION("using span changes") {
		std::string input = "test";
		ztd::text::any_encoding input_encoding(ztd::text::utf8);
		auto result = ztd::text::transcode_to<std::string>(
		     std::as_bytes(std::span<char>(input)), input_encoding, ztd::text::utf8, ztd::text::pass_handler);
		REQUIRE(result.output == input);
		REQUIRE(result.error_code == ztd::text::encoding_error::ok);
		REQUIRE(!result.errors_were_handled());
		REQUIRE(result.error_count == 0);
		REQUIRE(ztd::ranges::empty(result.input));
		REQUIRE(ztd::text::is_state_complete(result.from_state.get()));
		REQUIRE(ztd::text::is_state_complete(result.to_state.get()));
	}
	SECTION("using any_byte_encoding") {
		std::string input = "test";
		ztd::text::any_byte_encoding<char> input_encoding(ztd::text::compat_utf8);
		auto result
		     = ztd::text::transcode_to<std::string>(input, input_encoding, ztd::text::utf8, ztd::text::pass_handler);
		REQUIRE(input == result.output);
		REQUIRE(result.error_code == ztd::text::encoding_error::ok);
		REQUIRE(!result.errors_were_handled());
		REQUIRE(result.error_count == 0);
		REQUIRE(ztd::ranges::empty(result.input));
		REQUIRE(ztd::text::is_state_complete(result.from_state.get()));
		REQUIRE(ztd::text::is_state_complete(result.to_state.get()));
	}
}
