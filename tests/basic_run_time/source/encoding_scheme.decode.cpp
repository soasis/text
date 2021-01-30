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
// =============================================================================

#include <ztd/text/encoding_scheme.hpp>
#include <ztd/text/decode.hpp>

#include <catch2/catch.hpp>

#include <ztd/text/tests/basic_unicode_strings.hpp>

#include <algorithm>

TEST_CASE("text/decode/encoding_scheme", "decode from byte arrays with specific endianness") {
	SECTION("endian::native") {
		SECTION("execution") {
			ztd::text::encoding_scheme<ztd::text::execution, ztd::text::endian::native> encoding {};
			std::u32string result0 = ztd::text::decode(
			     ztd::text::tests::ansi_sequence_bytes_truth, encoding, ztd::text::replacement_handler {});
			bool is_equal0 = std::equal(result0.cbegin(), result0.cend(),
			     ztd::text::tests::u32_ansi_sequence_truth.begin(), ztd::text::tests::u32_ansi_sequence_truth.end());
			REQUIRE(is_equal0);
		}
		SECTION("wide_execution") {
			ztd::text::encoding_scheme<ztd::text::wide_execution, ztd::text::endian::native> encoding {};
			std::u32string result0 = ztd::text::decode(
			     ztd::text::tests::w_ansi_sequence_bytes_truth, encoding, ztd::text::replacement_handler {});
			bool is_equal0 = std::equal(result0.cbegin(), result0.cend(),
			     ztd::text::tests::u32_ansi_sequence_truth.begin(), ztd::text::tests::u32_ansi_sequence_truth.end());
			REQUIRE(is_equal0);

			std::u32string result1 = ztd::text::decode(
			     ztd::text::tests::w_unicode_sequence_bytes_truth, encoding, ztd::text::replacement_handler {});
			bool is_equal1
			     = std::equal(result1.cbegin(), result1.cend(), ztd::text::tests::u32_unicode_sequence_truth.begin(),
			          ztd::text::tests::u32_unicode_sequence_truth.end());
			REQUIRE(is_equal1);
		}
		SECTION("utf8") {
			ztd::text::encoding_scheme<ztd::text::utf8, ztd::text::endian::native> encoding {};
			std::u32string result0 = ztd::text::decode(ztd::text::tests::u8_ansi_sequence_bytes_truth, encoding);
			bool is_equal0         = std::equal(result0.cbegin(), result0.cend(),
                    ztd::text::tests::u32_ansi_sequence_truth.begin(), ztd::text::tests::u32_ansi_sequence_truth.end());
			REQUIRE(is_equal0);

			std::u32string result1 = ztd::text::decode(ztd::text::tests::u8_unicode_sequence_bytes_truth, encoding);
			bool is_equal1
			     = std::equal(result1.cbegin(), result1.cend(), ztd::text::tests::u32_unicode_sequence_truth.begin(),
			          ztd::text::tests::u32_unicode_sequence_truth.end());
			REQUIRE(is_equal1);
		}
		SECTION("utf16") {
			ztd::text::encoding_scheme<ztd::text::utf16, ztd::text::endian::native> encoding {};
			std::u32string result0 = ztd::text::decode(ztd::text::tests::u16_ansi_sequence_bytes_truth, encoding);
			bool is_equal0         = std::equal(result0.cbegin(), result0.cend(),
                    ztd::text::tests::u32_ansi_sequence_truth.begin(), ztd::text::tests::u32_ansi_sequence_truth.end());
			REQUIRE(is_equal0);

			std::u32string result1 = ztd::text::decode(ztd::text::tests::u16_unicode_sequence_bytes_truth, encoding);
			bool is_equal1
			     = std::equal(result1.cbegin(), result1.cend(), ztd::text::tests::u32_unicode_sequence_truth.begin(),
			          ztd::text::tests::u32_unicode_sequence_truth.end());
			REQUIRE(is_equal1);
		}
		SECTION("utf32") {
			ztd::text::encoding_scheme<ztd::text::utf32, ztd::text::endian::native> encoding {};
			std::u32string result0 = ztd::text::decode(ztd::text::tests::u32_ansi_sequence_bytes_truth, encoding);
			bool is_equal0         = std::equal(result0.cbegin(), result0.cend(),
                    ztd::text::tests::u32_ansi_sequence_truth.begin(), ztd::text::tests::u32_ansi_sequence_truth.end());
			REQUIRE(is_equal0);

			std::u32string result1 = ztd::text::decode(ztd::text::tests::u32_unicode_sequence_bytes_truth, encoding);
			bool is_equal1
			     = std::equal(result1.cbegin(), result1.cend(), ztd::text::tests::u32_unicode_sequence_truth.begin(),
			          ztd::text::tests::u32_unicode_sequence_truth.end());
			REQUIRE(is_equal1);
		}
	}
}
