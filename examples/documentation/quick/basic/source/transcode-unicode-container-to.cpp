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

#include <ztd/idk/span.hpp>

#include <string>
#include <string_view>

int main(int, char*[]) {
	constexpr const char input[] = u8"bark🐶⛄🐶🔔bark!";
	constexpr std::size_t input_last_exclamation_mark_index
	     = ztd_c_string_array_size(input) - 1;
	constexpr const std::u16string_view full_expected_output = u"bark🐶⛄🐶🔔bark!";
	constexpr std::size_t truncated_input_size               = 15;
	// string_view containing: "bark🐶⛄🐶🔔bark" (no ending exclamation point)
	constexpr const std::u16string_view truncated_expected_ouput
	     = full_expected_output.substr(0, truncated_input_size);

	// SAFE by default: if the string runs out of space, will not write more!
	std::u16string truncated_utf16_string(truncated_input_size, u'\0');
	// Span indicating available space to write into
	ztd::span<char16_t> truncated_utf16_string_output(truncated_utf16_string);
	auto truncated_utf16_string_result = ztd::text::transcode_into(input,
	     ztd::text::compat_utf8, truncated_utf16_string_output, ztd::text::utf16);

	// We only had space for sixteen UTF-16 code units; expect as much from output
	ZTD_TEXT_ASSERT(truncated_expected_ouput == truncated_utf16_string);
	// The sequence was correct, but there wasn't enough output space for the full
	// sequence!
	ZTD_TEXT_ASSERT(truncated_utf16_string_result.error_code
	     == ztd::text::encoding_error::insufficient_output_space);
	ZTD_TEXT_ASSERT(truncated_utf16_string_result.errors_were_handled());
	// There is no more output space
	ZTD_TEXT_ASSERT(ztd::ranges::empty(truncated_utf16_string_result.output));
	// There is still input left
	ZTD_TEXT_ASSERT(!ztd::ranges::empty(truncated_utf16_string_result.input));
	// We left only enough space for everything except the last '\0':
	// check to see if that's what happened in the input
	ZTD_TEXT_ASSERT(truncated_utf16_string_result.input[0] == '!');
	ZTD_TEXT_ASSERT(truncated_utf16_string_result.input[0]
	     == input[input_last_exclamation_mark_index]);
	// No copies of the input were made:
	// points to the same data as it was given.
	ZTD_TEXT_ASSERT(&truncated_utf16_string_result.input[0]
	     == &input[input_last_exclamation_mark_index]);

	return 0;
}
