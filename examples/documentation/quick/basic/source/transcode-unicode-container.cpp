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

#include <vector>
#include <list>
#include <deque>
#include <string>
#include <string_view>

int main(int, char*[]) {
	constexpr const char32_t input[] = U"🐶⛄🐶🔔";
	constexpr const std::u16string_view utf16_expected_output = u"🐶⛄🐶🔔";

	// a vector instead of a std::u16string
	std::vector<char16_t> utf16_emoji_vector
	     = ztd::text::transcode<std::vector<char16_t>>(input, ztd::text::utf16);

	// a list (doubly-linked list) instead of a std::u16string
	std::list<char16_t> utf16_emoji_list
	     = ztd::text::transcode<std::list<char16_t>>(input, ztd::text::utf16);

	// manually hand-serializing into a std::deque
	std::deque<char16_t> utf16_emoji_deque {};
	auto deque_result = ztd::text::transcode_into(input, ztd::text::utf16,
	     ztd::ranges::unbounded_view(std::back_inserter(utf16_emoji_deque)));
	// transcode_into_raw returns a ztd::text::transcode_result<…>
	// which we can inspect for error codes and more!
	// the error_code should be "ok"
	ZTD_TEXT_ASSERT(deque_result.error_code == ztd::text::encoding_error::ok);
	// No errors should have occured, even if they were "handled" and still
	// returned "ok"
	ZTD_TEXT_ASSERT(!deque_result.errors_were_handled());
	// The input should be completely empty
	ZTD_TEXT_ASSERT(deque_result.input.empty());

	// The results should all be the same, despite the container!
	ZTD_TEXT_ASSERT(
	     ztd::ranges::equal(utf16_emoji_vector, utf16_expected_output));
	ZTD_TEXT_ASSERT(ztd::ranges::equal(utf16_emoji_list, utf16_expected_output));
	ZTD_TEXT_ASSERT(ztd::ranges::equal(utf16_emoji_deque, utf16_expected_output));
	return 0;
}
