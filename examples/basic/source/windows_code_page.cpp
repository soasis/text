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
#include <ztd/text/windows_code_page.hpp>
#include <ztd/text/encoding.hpp>
#include <ztd/text/assert.hpp>

#if ZTD_IS_ON(ZTD_PLATFORM_WINDOWS)

#include <algorithm>

int main(int, char*[]) {
	// Odia text:
	// ଜତି, ଭଷ ଓ ସାହିତ୍ୟର ପରିଚୟ ଏବଂ ଏକ ସଂଗେ ଯୋଡ
	const std::string_view input = reinterpret_cast<const char*>(&u8"ଜତି, ଭଷ ଓ ସାହିତ୍ୟର ପରିଚୟ ଏବଂ ଏକ ସଂଗେ ଯୋଡ"[0]);
	const std::u32string_view expected_intermediate = U"ଜତି, ଭଷ ଓ ସାହିତ୍ୟର ପରିଚୟ ଏବଂ ଏକ ସଂଗେ ଯୋଡ";
	const std::string_view expected_output = reinterpret_cast<const char*>(&u8"ଜତି, ଭଷ ଓ ସାହିତ୍ୟର ପରିଚୟ ଏବଂ ଏକ ସଂଗେ ଯୋଡ");

	constexpr int32_t win32_odia_code_page = 57007;

	std::u32string intermediate = ztd::text::decode(input, ztd::text::compat_utf8);
	std::string output          = ztd::text::encode(
          intermediate, ztd::text::windows_code_page(win32_odia_code_page), ztd::text::unchecked_default_handler);

	ZTD_TEXT_ASSERT(expected_intermediate.size() == intermediate.size());
	ZTD_TEXT_ASSERT(std::equal(expected_intermediate.cbegin(), expected_intermediate.cend(), intermediate.begin()));
	ZTD_TEXT_ASSERT(expected_output.size() == output.size());
	ZTD_TEXT_ASSERT(std::equal(expected_output.cbegin(), expected_output.cend(), output.begin()));

	return 0;
}

#else

#include <iostream>

int main(int, char*[]) {
	std::cout << "This is not Windows, so there is no output or test here! Sorry :<" << std::endl;

	return 0;
}

#endif
