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
#include <ztd/text/any_encoding.hpp>
#include <ztd/text/encode.hpp>

#include <catch2/catch.hpp>

#include <ztd/text/tests/basic_unicode_strings.hpp>

#include <algorithm>

TEST_CASE("text/encode/any_encoding/encoding_scheme", "encode to byte arrays through any_encoding interface") {
	const auto& input0 = ztd::text::tests::u32_ansi_sequence_truth;
	const auto& input1 = ztd::text::tests::u32_unicode_sequence_truth;

	SECTION("endian::native") {
		SECTION("execution") {
			const auto& output0 = ztd::text::tests::ansi_sequence_bytes_truth;

			ztd::text::any_encoding encoding(
			     ztd::text::encoding_scheme<ztd::text::execution, ztd::text::endian::native> {});

			ztd::text::any_encoding::encode_state state0(encoding);
			std::vector<std::byte> result0_storage(std::size(input0) * encoding.max_code_units, std::byte {});
			std::span<std::byte> result0_storage_view(result0_storage);
			auto result0 = ztd::text::encode_into(
			     input0, encoding, result0_storage_view, ztd::text::default_handler {}, state0);
			std::span<std::byte> result0_view(result0_storage_view.begin(), result0.output.begin());
			bool is_equal0
			     = std::equal(result0_view.cbegin(), result0_view.cend(), output0.cbegin(), output0.cend());
			REQUIRE(is_equal0);
		}
		SECTION("wide_execution") {
			const auto& output0 = ztd::text::tests::w_ansi_sequence_bytes_truth;
			const auto& output1 = ztd::text::tests::w_unicode_sequence_bytes_truth;

			ztd::text::any_encoding encoding(
			     ztd::text::encoding_scheme<ztd::text::wide_execution, ztd::text::endian::native> {});

			ztd::text::any_encoding::encode_state state0(encoding);
			std::vector<std::byte> result0_storage(std::size(input0) * encoding.max_code_units, std::byte {});
			std::span<std::byte> result0_storage_view(result0_storage);
			auto result0 = ztd::text::encode_into(
			     input0, encoding, result0_storage_view, ztd::text::default_handler {}, state0);
			std::span<std::byte> result0_view(result0_storage_view.begin(), result0.output.begin());
			bool is_equal0
			     = std::equal(result0_view.cbegin(), result0_view.cend(), output0.cbegin(), output0.cend());
			REQUIRE(is_equal0);

			ztd::text::any_encoding::encode_state state1(encoding);
			std::vector<std::byte> result1_storage(std::size(input1) * encoding.max_code_units, std::byte {});
			std::span<std::byte> result1_storage_view(result1_storage);
			auto result1 = ztd::text::encode_into(
			     input1, encoding, result1_storage_view, ztd::text::default_handler {}, state1);
			std::span<std::byte> result1_view(result1_storage_view.begin(), result1.output.begin());
			bool is_equal1
			     = std::equal(result1_view.cbegin(), result1_view.cend(), output1.cbegin(), output1.cend());
			REQUIRE(is_equal1);
		}
		SECTION("utf8") {
			const auto& output0 = ztd::text::tests::u8_ansi_sequence_bytes_truth;
			const auto& output1 = ztd::text::tests::u8_unicode_sequence_bytes_truth;

			ztd::text::any_encoding encoding(
			     ztd::text::encoding_scheme<ztd::text::utf8, ztd::text::endian::native> {});

			ztd::text::any_encoding::encode_state state0(encoding);
			std::vector<std::byte> result0_storage(std::size(input0) * encoding.max_code_units, std::byte {});
			std::span<std::byte> result0_storage_view(result0_storage);
			auto result0 = ztd::text::encode_into(
			     input0, encoding, result0_storage_view, ztd::text::default_handler {}, state0);
			std::span<std::byte> result0_view(result0_storage_view.begin(), result0.output.begin());
			bool is_equal0
			     = std::equal(result0_view.cbegin(), result0_view.cend(), output0.cbegin(), output0.cend());
			REQUIRE(is_equal0);

			ztd::text::any_encoding::encode_state state1(encoding);
			std::vector<std::byte> result1_storage(std::size(input1) * encoding.max_code_units, std::byte {});
			std::span<std::byte> result1_storage_view(result1_storage);
			auto result1 = ztd::text::encode_into(
			     input1, encoding, result1_storage_view, ztd::text::default_handler {}, state1);
			std::span<std::byte> result1_view(result1_storage_view.begin(), result1.output.begin());
			bool is_equal1
			     = std::equal(result1_view.cbegin(), result1_view.cend(), output1.cbegin(), output1.cend());
			REQUIRE(is_equal1);
		}
		SECTION("utf16") {
			const auto& output0 = ztd::text::tests::u16_ansi_sequence_bytes_truth;
			const auto& output1 = ztd::text::tests::u16_unicode_sequence_bytes_truth;

			ztd::text::any_encoding encoding(
			     ztd::text::encoding_scheme<ztd::text::utf16, ztd::text::endian::native> {});

			ztd::text::any_encoding::encode_state state0(encoding);
			std::vector<std::byte> result0_storage(std::size(input0) * encoding.max_code_units, std::byte {});
			std::span<std::byte> result0_storage_view(result0_storage);
			auto result0 = ztd::text::encode_into(
			     input0, encoding, result0_storage_view, ztd::text::default_handler {}, state0);
			std::span<std::byte> result0_view(result0_storage_view.begin(), result0.output.begin());
			bool is_equal0
			     = std::equal(result0_view.cbegin(), result0_view.cend(), output0.cbegin(), output0.cend());
			REQUIRE(is_equal0);

			ztd::text::any_encoding::encode_state state1(encoding);
			std::vector<std::byte> result1_storage(std::size(input1) * encoding.max_code_units, std::byte {});
			std::span<std::byte> result1_storage_view(result1_storage);
			auto result1 = ztd::text::encode_into(
			     input1, encoding, result1_storage_view, ztd::text::default_handler {}, state1);
			std::span<std::byte> result1_view(result1_storage_view.begin(), result1.output.begin());
			bool is_equal1
			     = std::equal(result1_view.cbegin(), result1_view.cend(), output1.cbegin(), output1.cend());
			REQUIRE(is_equal1);
		}
		SECTION("utf32") {
			const auto& output0 = ztd::text::tests::u32_ansi_sequence_bytes_truth;
			const auto& output1 = ztd::text::tests::u32_unicode_sequence_bytes_truth;

			ztd::text::any_encoding encoding(
			     ztd::text::encoding_scheme<ztd::text::utf32, ztd::text::endian::native> {});

			ztd::text::any_encoding::encode_state state0(encoding);
			std::vector<std::byte> result0_storage(std::size(input0) * encoding.max_code_units, std::byte {});
			std::span<std::byte> result0_storage_view(result0_storage);
			auto result0 = ztd::text::encode_into(
			     input0, encoding, result0_storage_view, ztd::text::default_handler {}, state0);
			std::span<std::byte> result0_view(result0_storage_view.begin(), result0.output.begin());
			bool is_equal0
			     = std::equal(result0_view.cbegin(), result0_view.cend(), output0.cbegin(), output0.cend());
			REQUIRE(is_equal0);

			ztd::text::any_encoding::encode_state state1(encoding);
			std::vector<std::byte> result1_storage(std::size(input1) * encoding.max_code_units, std::byte {});
			std::span<std::byte> result1_storage_view(result1_storage);
			auto result1 = ztd::text::encode_into(
			     input1, encoding, result1_storage_view, ztd::text::default_handler {}, state1);
			std::span<std::byte> result1_view(result1_storage_view.begin(), result1.output.begin());
			bool is_equal1
			     = std::equal(result1_view.cbegin(), result1_view.cend(), output1.cbegin(), output1.cend());
			REQUIRE(is_equal1);
		}
	}
}
