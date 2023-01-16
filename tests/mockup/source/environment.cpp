// =============================================================================
//
// ztd.text
// Copyright © 2022-2023 JeanHeyd "ThePhD" Meneide and Shepherd's Oasis, LLC
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

#include <ztd/text/encoding.hpp>
#include <ztd/text/transcode.hpp>

#include <ztd/text/tests/basic_unicode_strings.hpp>

#include <catch2/catch_all.hpp>

#include <string>
#include <string_view>

#if ZTD_IS_OFF(ZTD_NATIVE_CHAR8_T)
namespace std {
	using u8string      = ::std::basic_string<::ztd::uchar8_t, ::ztd::unsigned_char_traits>;
	using u8string_view = ::std::basic_string_view<::ztd::uchar8_t, ::ztd::unsigned_char_traits>;
} // namespace std
#endif

inline namespace ztd_text_tests_mockup_environment {
	class wide_ucs2_t
	: public ::ztd::text::__txt_impl::__utf16_with<wide_ucs2_t, wchar_t, ztd::text::unicode_code_point, true> {
	} constexpr wide_ucs2 = {};

	class environment_variable {
	public:
		using native_char_type = wchar_t;

	private:
		// Wide UCS-2: surrogates allowed, 2 leading surrogates in a row.
		inline static constexpr const wchar_t __totally_secret_windows_environment_data[]
		     = { L'a', L'\xDC00', L'\xD800', L'z' };

	public:
		ztd::span<const native_char_type> native() const noexcept {
			return __totally_secret_windows_environment_data;
		}

		std::string string() const {
			return ztd::text::transcode(
			     this->native(), wide_ucs2, ztd::text::windows_1252, ztd::text::replacement_handler);
		}

		std::wstring wstring() const {
			return ztd::text::transcode(this->native(), wide_ucs2, wide_ucs2, ztd::text::replacement_handler);
		}

		std::u8string u8string() const {
			return ztd::text::transcode<std::u8string>(
			     this->native(), wide_ucs2, ztd::text::utf8, ztd::text::replacement_handler);
		}

		std::u16string u16string() const {
			return ztd::text::transcode(this->native(), wide_ucs2, ztd::text::utf16, ztd::text::replacement_handler);
		}

		std::u32string u32string() const {
			return ztd::text::transcode(this->native(), wide_ucs2, ztd::text::utf32, ztd::text::replacement_handler);
		}
	};

	inline constexpr static ztd::uchar8_t u8_expected_env_value_storage[]
	     = { u8'a', (unsigned char)'\xEF', (unsigned char)'\xBF', (unsigned char)'\xBD', (unsigned char)'\xEF',
		       (unsigned char)'\xBF', (unsigned char)'\xBD', u8'z', u8'\0' };
} // namespace ztd_text_tests_mockup_environment

TEST_CASE("text/mockup/environment",
     "A fake environment which can meet the specifications Tom Honermann stated on January 5th, 2023") {

	constexpr const std::string_view expected_env_value        = "a??z";
	constexpr const std::wstring_view w_expected_env_value     = L"a\xDC00\xD800z";
	constexpr const std::u8string_view u8_expected_env_value   = u8_expected_env_value_storage;
	constexpr const std::u16string_view u16_expected_env_value = u"a\uFFFD\uFFFDz";
	constexpr const std::u32string_view u32_expected_env_value = U"a\uFFFD\uFFFDz";

	environment_variable env_var {};
	const std::string env_value        = env_var.string();
	const std::wstring w_env_value     = env_var.wstring();
	const std::u8string u8_env_value   = env_var.u8string();
	const std::u16string u16_env_value = env_var.u16string();
	const std::u32string u32_env_value = env_var.u32string();

	REQUIRE(env_value == expected_env_value);
	REQUIRE(w_env_value == w_expected_env_value);
	REQUIRE(u8_env_value == u8_expected_env_value);
	REQUIRE(u16_env_value == u16_expected_env_value);
	REQUIRE(u32_env_value == u32_expected_env_value);
}
