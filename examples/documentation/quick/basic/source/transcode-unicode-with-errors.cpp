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

#include <vector>
#include <list>
#include <deque>
#include <string>
#include <string_view>

int main(int, char*[]) {
	constexpr const char32_t input[]                        = U"Ba\xD800rk!";
	constexpr const char expected_default_output[]          = u8"Ba�rk!";
	constexpr const char expected_replacement_output[]      = u8"Ba�rk!";
	constexpr const char expected_skip_output[]             = u8"Bark!";
	constexpr const char expected_pass_output[]             = u8"Ba";
	constexpr const char32_t expected_pass_leftover_input[] = U"\xD800rk!";

	std::string utf8_string_with_default
	     = ztd::text::transcode(input, ztd::text::compat_utf8);
	std::string utf8_string_with_replacement
	     = ztd::text::transcode(input, ztd::text::utf32, ztd::text::compat_utf8,
	          ztd::text::replacement_handler);
	std::string utf8_string_with_skip = ztd::text::transcode(input,
	     ztd::text::utf32, ztd::text::compat_utf8, ztd::text::skip_handler);
	std::string utf8_string_with_pass = ztd::text::transcode(input,
	     ztd::text::utf32, ztd::text::compat_utf8, ztd::text::pass_handler);

	ZTD_TEXT_ASSERT(utf8_string_with_default == expected_default_output);
	ZTD_TEXT_ASSERT(utf8_string_with_replacement == expected_replacement_output);
	ZTD_TEXT_ASSERT(utf8_string_with_skip == expected_skip_output);
	ZTD_TEXT_ASSERT(utf8_string_with_pass == expected_pass_output);

	auto utf8_string_with_default_result
	     = ztd::text::transcode_to(input, ztd::text::compat_utf8);
	ZTD_TEXT_ASSERT(utf8_string_with_default_result.error_code
	     == ztd::text::encoding_error::ok);
	ZTD_TEXT_ASSERT(utf8_string_with_default_result.errors_were_handled());
	ZTD_TEXT_ASSERT(utf8_string_with_default_result.error_count == 1);
	ZTD_TEXT_ASSERT(
	     utf8_string_with_default_result.output == expected_default_output);
	ZTD_TEXT_ASSERT(ztd::ranges::empty(utf8_string_with_default_result.input));

	auto utf8_string_with_replacement_result
	     = ztd::text::transcode_to(input, ztd::text::utf32,
	          ztd::text::compat_utf8, ztd::text::replacement_handler);
	ZTD_TEXT_ASSERT(utf8_string_with_replacement_result.error_code
	     == ztd::text::encoding_error::ok);
	ZTD_TEXT_ASSERT(utf8_string_with_replacement_result.errors_were_handled());
	ZTD_TEXT_ASSERT(utf8_string_with_replacement_result.error_count == 1);
	ZTD_TEXT_ASSERT(utf8_string_with_replacement_result.output
	     == expected_replacement_output);
	ZTD_TEXT_ASSERT(
	     ztd::ranges::empty(utf8_string_with_replacement_result.input));

	auto utf8_string_with_skip_result = ztd::text::transcode_to(input,
	     ztd::text::utf32, ztd::text::compat_utf8, ztd::text::skip_handler);
	ZTD_TEXT_ASSERT(utf8_string_with_skip_result.error_code
	     == ztd::text::encoding_error::ok);
	ZTD_TEXT_ASSERT(utf8_string_with_skip_result.errors_were_handled());
	ZTD_TEXT_ASSERT(utf8_string_with_skip_result.error_count == 1);
	ZTD_TEXT_ASSERT(utf8_string_with_skip_result.output == expected_skip_output);
	ZTD_TEXT_ASSERT(ztd::ranges::empty(utf8_string_with_skip_result.input));

	auto utf8_string_with_pass_result = ztd::text::transcode_to(input,
	     ztd::text::utf32, ztd::text::compat_utf8, ztd::text::pass_handler);
	ZTD_TEXT_ASSERT(utf8_string_with_pass_result.error_code
	     == ztd::text::encoding_error::invalid_sequence);
	ZTD_TEXT_ASSERT(utf8_string_with_pass_result.errors_were_handled());
	ZTD_TEXT_ASSERT(utf8_string_with_pass_result.error_count == 1);
	ZTD_TEXT_ASSERT(utf8_string_with_pass_result.output == expected_pass_output);
	ZTD_TEXT_ASSERT(
	     utf8_string_with_pass_result.input == expected_pass_leftover_input);

	return 0;
}
