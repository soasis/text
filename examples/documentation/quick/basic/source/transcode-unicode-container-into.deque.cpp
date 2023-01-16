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

#include <ztd/text.hpp>

#include <ztd/idk/span.hpp>

#include <string>
#include <string_view>
#include <deque>

int main(int, char*[]) {
	constexpr const ztd_char8_t input[] = u8"bark🐶⛄🐶🔔bark!";
	constexpr const std::u16string_view expected_output = u"bark🐶⛄🐶🔔bark!";

	// Get a deque with a pre-ordained size.
	std::deque<char16_t> utf16_deque(expected_output.size());
	// Subrange indicating available space to write into
	auto utf16_deque_output_view
	     = ztd::ranges::make_subrange(utf16_deque.begin(), utf16_deque.end());
	// SAFE by default: if the container runs out of space, will not write more!
	auto utf16_deque_result = ztd::text::transcode_into(input, ztd::text::utf8,
	     utf16_deque_output_view, ztd::text::utf16, ztd::text::pass_handler,
	     ztd::text::pass_handler);

	// Ensure that the error code indicates success.
	ZTD_TEXT_ASSERT(
	     utf16_deque_result.error_code == ztd::text::encoding_error::ok);
	// there were no errors handled for us while processing
	ZTD_TEXT_ASSERT(!utf16_deque_result.errors_were_handled());
	// We had (exactly enough) space.
	ZTD_TEXT_ASSERT(ztd::ranges::equal(expected_output, utf16_deque));
	// There is no more input or output space left
	ZTD_TEXT_ASSERT(ztd::ranges::empty(utf16_deque_result.input));
	ZTD_TEXT_ASSERT(ztd::ranges::empty(utf16_deque_result.output));

	return 0;
}
