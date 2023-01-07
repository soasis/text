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

int main(int, char*[]) {
	// properly-typed input picks the right encoding automatically
	std::u16string utf16_emoji_string
	     = ztd::text::transcode(input, ztd::text::utf16);
	// explicitly pick the input encoding
	std::u16string utf16_emoji_string_explicit
	     = ztd::text::transcode(input, ztd::text::utf32, ztd::text::utf16);
	// must use explicit handler because "wide execution" may be
	// a lossy encoding! See:
	// https://ztdtext.rtfd.io/en/latest/design/error%20handling/lossy%20protection.html
	std::u16string utf16_korean_string_explicit
	     = ztd::text::transcode(wide_input, ztd::text::wide_execution,
	          ztd::text::utf16, ztd::text::replacement_handler);
	// result in the same strings, but different encodings!
	ZTD_TEXT_ASSERT(utf16_emoji_string == utf16_emoji_string_explicit);
	ZTD_TEXT_ASSERT(utf16_emoji_string == u"🐶🐶");
	ZTD_TEXT_ASSERT(utf16_korean_string_explicit == u"안녕하세요");
	return 0;
}
