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

#include <ztd/text.hpp>

#include <catch2/catch_all.hpp>

#include <ztd/idk/endian.hpp>

#include <fstream>
#include <iostream>
#include <vector>

TEST_CASE("text/additional_encodings/shift_jis", "test a quick roundtrip using example SHIFT-JIS text to UTF-32") {
	constexpr const char* original_filename = "data/shift_jis/shift_jis.txt";
	std::ifstream original_ifstr(original_filename, std::ios_base::beg | std::ios_base::binary);
	original_ifstr >> std::noskipws;
	std::vector<unsigned char> original_data(
	     std::istream_iterator<unsigned char>(original_ifstr), std::istream_iterator<unsigned char> {});

	constexpr const char* expected_filename = ztd::endian::native == ztd::endian::little
	     ? "data/shift_jis/utf-32-le.txt"
	     : "data/shift_jis/utf-32-be.txt";
	std::ifstream expected_ifstr(expected_filename, std::ios_base::beg | std::ios_base::binary);
	expected_ifstr >> std::noskipws;
	std::vector<unsigned char> expected_data(
	     std::istream_iterator<unsigned char>(expected_ifstr), std::istream_iterator<unsigned char> {});

	std::string_view original(reinterpret_cast<const char*>(original_data.data()), original_data.size());
	std::u32string_view expected(
	     reinterpret_cast<const char32_t*>(expected_data.data()), expected_data.size() / sizeof(char32_t));

	std::u32string decoded = ztd::text::decode(original, ztd::text::shift_jis);
	REQUIRE(decoded == expected);
	std::string encoded = ztd::text::encode(decoded, ztd::text::shift_jis, ztd::text::replacement_handler);
	REQUIRE(encoded == original);
}
