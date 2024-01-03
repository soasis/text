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

#include <ztd/idk/endian.hpp>

#include <fstream>
#include <iostream>
#include <vector>

TEST_CASE(
     "text/additional_encodings/shift_jis_x0208", "test a quick roundtrip using example SHIFT-JIS text to UTF-32") {
	constexpr const char* original_filename = "data/tests/additional_encodings/shift_jis_x0208/shift_jis_x0208.txt";
	std::ifstream original_ifstr(
	     original_filename, static_cast<std::ios_base::openmode>(std::ios_base::in | std::ios_base::binary));
	original_ifstr >> std::noskipws;
	std::vector<unsigned char> original_data(
	     std::istream_iterator<unsigned char>(original_ifstr), std::istream_iterator<unsigned char> {});

	constexpr const char* expected_filename = ztd::endian::native == ztd::endian::little
	     ? "data/tests/additional_encodings/shift_jis_x0208/utf-32-le.txt"
	     : "data/tests/additional_encodings/shift_jis_x0208/utf-32-be.txt";
	std::ifstream expected_ifstr(
	     expected_filename, static_cast<std::ios_base::openmode>(std::ios_base::in | std::ios_base::binary));
	expected_ifstr >> std::noskipws;
	std::vector<unsigned char> expected_data(
	     std::istream_iterator<unsigned char>(expected_ifstr), std::istream_iterator<unsigned char> {});

	std::string_view original(reinterpret_cast<const char*>(original_data.data()), original_data.size());
	std::u32string_view expected(
	     reinterpret_cast<const char32_t*>(expected_data.data()), expected_data.size() / sizeof(char32_t));

	auto decoded_result = ztd::text::decode_to(original, ztd::text::shift_jis_x0208, ztd::text::pass_handler);
	const std::u32string& decoded = decoded_result.output;
	REQUIRE(decoded_result.error_code == ztd::text::encoding_error::ok);
	REQUIRE_FALSE(decoded_result.errors_were_handled());
	REQUIRE(ztd::ranges::empty(decoded_result.input));
	REQUIRE(decoded == expected);
	auto encoded_result        = ztd::text::encode_to(decoded, ztd::text::shift_jis_x0208, ztd::text::pass_handler);
	const std::string& encoded = encoded_result.output;
	REQUIRE(encoded_result.error_code == ztd::text::encoding_error::ok);
	REQUIRE_FALSE(encoded_result.errors_were_handled());
	REQUIRE(ztd::ranges::empty(encoded_result.input));
	REQUIRE(encoded == original);
}
