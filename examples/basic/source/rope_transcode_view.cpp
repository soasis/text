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

#include <ztd/text/transcode_view.hpp>
#include <ztd/text/encoding.hpp>

#if ZTD_IS_ON(ZTD_CXX_STDLIB_STDCXX)

// libstdc++ (gcc) has a rope class!

#include <ext/rope>

#include <algorithm>
#include <iostream>

int main(int, char*[]) {
	const std::string_view input              = reinterpret_cast<const char*>(&u8"приветствие"[0]);
	const std::u16string_view expected_output = u"приветствие";

	using rope = __gnu_cxx::crope; // rope of char
	rope rope_text;
	rope_text += input.data();

	using iterator      = typename rope::const_iterator;
	using rope_subrange = ztd::ranges::subrange<iterator, iterator>;
	rope_subrange rope_view(rope_text.begin(), rope_text.end());

	// View the char-based UTF-8 of the rope
	// as a sequence of UTF-16 characters
	using utf16_from_utf8_rope
	     = ztd::text::transcode_view<ztd::text::compat_utf8_t, ztd::text::utf16_t, rope_subrange>;
	utf16_from_utf8_rope transcoded_rope_view(rope_view);

	std::cout << "UTF-16 Code Units, from the Rope:" << std::endl;
	for (const auto& c16 : transcoded_rope_view) {
		std::cout << std::hex << "0x" << (int)c16 << " ";
	}
	std::cout << std::endl;

	ZTD_TEXT_ASSERT(std::equal(expected_output.cbegin(), expected_output.cend(), transcoded_rope_view.begin()));

	return 0;
}

#else

#include <iostream>

int main(int, char*[]) {
	std::cout << "There is no rope extension for this standard library we can use for this example. Sorry! :<"
	          << std::endl;

	return 0;
}

#endif
