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

#include <ztd/idk/size.hpp>

#include <fstream>
#include <iostream>
#include <vector>

TEST_CASE("text/additional_encodings/gb18030", "test a quick roundtrip using example GB18030 text to UTF-32") {
	constexpr const unsigned char original_data[] = { 0x47, 0x42, 0x31, 0x38, 0x30, 0x33, 0x30, 0x20, 0xB7, 0xFB, 0xBA,
		0xCF, 0xD0, 0xD4, 0xCE, 0xCA, 0xD3, 0xEB, 0xB4, 0xF0, 0x20, 0xA1, 0xAA, 0x20, 0xD0, 0xC7, 0xD0, 0xC7, 0xD6,
		0xAE, 0xBB, 0xF0, 0xA3, 0xAC, 0xBF, 0xC9, 0xD2, 0xD4, 0xC1, 0xC7, 0xD4, 0xAD, 0xA1, 0xA3, 0x94, 0x39, 0xB2,
		0x39, 0x81, 0x38, 0xAC, 0x32, 0x94, 0x39, 0xB3, 0x30 };
	constexpr const char32_t expected_data[]      = { 0x00000047, 0x00000042, 0x00000031, 0x00000038, 0x00000030,
		     0x00000033, 0x00000030, 0x00000020, 0x00007b26, 0x00005408, 0x00006027, 0x000095ee, 0x00004e0e, 0x00007b54,
		     0x00000020, 0x00002014, 0x00000020, 0x0000661f, 0x0000661f, 0x00004e4b, 0x0000706b, 0x0000ff0c, 0x000053ef,
		     0x00004ee5, 0x000071ce, 0x0000539f, 0x00003002, 0x0001f31f, 0x00002b50, 0x0001f320 };

	std::basic_string_view<unsigned char> original(original_data + 0, ztdc_c_array_size(original_data));
	std::u32string_view expected(expected_data + 0, ztdc_c_array_size(expected_data));

	auto decoded_result           = ztd::text::decode_to(original, ztd::text::gb18030, ztd::text::pass_handler);
	const std::u32string& decoded = decoded_result.output;
	REQUIRE(decoded_result.error_code == ztd::text::encoding_error::ok);
	REQUIRE_FALSE(decoded_result.errors_were_handled());
	REQUIRE(ztd::ranges::empty(decoded_result.input));
	REQUIRE(decoded == expected);
	auto encoded_result = ztd::text::encode_to<std::basic_string<unsigned char>>(
	     decoded, ztd::text::gb18030, ztd::text::pass_handler);
	const auto& encoded = encoded_result.output;
	REQUIRE(encoded_result.error_code == ztd::text::encoding_error::ok);
	REQUIRE_FALSE(encoded_result.errors_were_handled());
	REQUIRE(ztd::ranges::empty(encoded_result.input));
	auto it = std::mismatch(encoded.begin(), encoded.end(), original.begin(), original.end());
	[[maybe_unused]] auto encoded_index  = it.first - encoded.begin();
	[[maybe_unused]] auto original_index = it.second - original.begin();
	REQUIRE(encoded == original);
}
