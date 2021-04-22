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
#include <ztd/text/transcode.hpp>

#include <ztd/text/tests/basic_unicode_strings.hpp>

#include <catch2/catch.hpp>

#include <algorithm>

inline namespace ztd_text_tests_transcode_containers {
	template <typename FromEncoding, typename ToEncoding, template <class...> typename Container = std::vector>
	void check_container_roundtrip(FromEncoding& from_encoding, ToEncoding& to_encoding) {
		constexpr bool DecodeInjective = ztd::text::is_decode_injective_v<FromEncoding>;
		constexpr bool EncodeInjective = ztd::text::is_encode_injective_v<ToEncoding>;
		using ToCodeUnit               = ztd::text::code_unit_t<ToEncoding>;
		using FromCodeUnit             = ztd::text::code_unit_t<FromEncoding>;
		{
			Container<FromCodeUnit> in_container = { (FromCodeUnit)0, (FromCodeUnit)0, (FromCodeUnit)0 };
			Container<ToCodeUnit> out_container  = { (ToCodeUnit)0, (ToCodeUnit)0, (ToCodeUnit)0 };
			auto invoker                         = [&]() {
                    if constexpr (DecodeInjective && EncodeInjective) {
                         return ztd::text::transcode_into(in_container, from_encoding, out_container, to_encoding);
                    }
                    else {
                         return ztd::text::transcode_into(in_container, from_encoding, out_container, to_encoding,
                              ztd::text::pass_handler {}, ztd::text::pass_handler {});
                    }
			};
			auto result = invoker();
			REQUIRE(result.error_code == ztd::text::encoding_error::ok);
			REQUIRE(result.handled_errors == 0);
			REQUIRE_FALSE(result.errors_were_handled());
			REQUIRE(std::equal(std::begin(in_container), std::end(in_container), std::begin(out_container),
			     std::end(out_container)));
		}
		{
			Container<FromCodeUnit> in_container = { (FromCodeUnit)0, (FromCodeUnit)0, (FromCodeUnit)0 };
			auto invoker                         = [&]() {
                    if constexpr (DecodeInjective && EncodeInjective) {
                         return ztd::text::transcode_to<Container<ToCodeUnit>>(
                              in_container, from_encoding, to_encoding);
                    }
                    else {
                         return ztd::text::transcode_to<Container<ToCodeUnit>>(in_container, from_encoding, to_encoding,
                              ztd::text::pass_handler {}, ztd::text::pass_handler {});
                    }
			};
			auto result = invoker();
			REQUIRE(result.error_code == ztd::text::encoding_error::ok);
			REQUIRE(result.handled_errors == 0);
			REQUIRE_FALSE(result.errors_were_handled());
			REQUIRE(std::equal(std::begin(in_container), std::end(in_container), std::begin(result.output),
			     std::end(result.output)));
		}
		{
			Container<FromCodeUnit> in_container = { (FromCodeUnit)0, (FromCodeUnit)0, (FromCodeUnit)0 };
			auto invoker                         = [&]() {
                    if constexpr (DecodeInjective && EncodeInjective) {
                         return ztd::text::transcode(in_container, from_encoding, to_encoding);
                    }
                    else {
                         return ztd::text::transcode(in_container, from_encoding, to_encoding,
                              ztd::text::pass_handler {}, ztd::text::pass_handler {});
                    }
			};
			auto result = invoker();
			REQUIRE(
			     std::equal(std::begin(in_container), std::end(in_container), std::begin(result), std::end(result)));
		}
	}
} // namespace ztd_text_tests_transcode_containers

TEST_CASE(
     "text/transcode/roundtrip/containers", "transcode can handle container-like types at many levels of the API") {
	SECTION("ascii") {
		ztd::text::ascii from;
		ztd::text::ascii to;
		check_container_roundtrip(from, to);
	}
	SECTION("execution") {
		ztd::text::execution from;
		ztd::text::execution to;
		check_container_roundtrip(from, to);
	}
	SECTION("wide_execution") {
		ztd::text::wide_execution from;
		ztd::text::wide_execution to;
		check_container_roundtrip(from, to);
	}
	SECTION("literal") {
		ztd::text::literal from;
		ztd::text::literal to;
		check_container_roundtrip(from, to);
	}
	SECTION("wide_literal") {
		ztd::text::wide_literal from;
		ztd::text::wide_literal to;
		check_container_roundtrip(from, to);
	}
	SECTION("utf8") {
		ztd::text::utf8 from;
		ztd::text::utf8 to;
		check_container_roundtrip(from, to);
	}
	SECTION("utf16") {
		ztd::text::utf16 from;
		ztd::text::utf16 to;
		check_container_roundtrip(from, to);
	}
	SECTION("utf32") {
		ztd::text::utf32 from;
		ztd::text::utf32 to;
		check_container_roundtrip(from, to);
	}
}
