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

#define CATCH_CONFIG_RUNNER
#include <catch2/catch.hpp>

#include <ztd/text/tests/basic_unicode_strings.hpp>

#include <iostream>
#include <clocale>

struct static_hook {
	static_hook() {
#ifdef _WIN32
		char* res = std::setlocale(LC_ALL, ".65001");
		if (res == nullptr) {
			std::cout << "cannot set the locale-based encoding in Windows to UTF8" << std::endl;
		}
#elif defined(macintosh) || defined(Macintosh) || (__APPLE__)
		// who knows what the hell goes on in Apple: assume UTF-8 already
#else
		char* res = std::setlocale(LC_ALL, "en_US.utf8");
		if (res == nullptr) {
			std::cout << "cannot set the locale-based encoding in non-Windows to UTF8" << std::endl;
		}
#endif
	}
} inline ztd_text_tests_utf8_startup {};

int main(int argc, char* argv[]) {
	std::cout << "=== Encoding Names ===" << std::endl;
	std::cout << "Literal Encoding: "
	          << ztd::text::__txt_detail::to_name(
	                  ztd::text::__txt_detail::__to_encoding_id(ZTD_TEXT_COMPILE_TIME_ENCODING_NAME_GET_I_()))
	          << std::endl;
	std::cout << "Wide Literal Encoding: "
	          << ztd::text::__txt_detail::to_name(
	                  ztd::text::__txt_detail::__to_encoding_id(ZTD_TEXT_COMPILE_TIME_WIDE_ENCODING_NAME_GET_I_()))
	          << std::endl;
	int result = Catch::Session().run(argc, argv);
	return result;
}
