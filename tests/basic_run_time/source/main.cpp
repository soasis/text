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

#include <clocale>
#include <ztd/text/tests/basic_unicode_strings.hpp>

int main(int argc, char* argv[]) {
#ifdef _WIN32
	char* res = std::setlocale(LC_ALL, ".65001");
	if (res == nullptr) {
		std::cerr << "cannot set the locale-based encoding in Windows to UTF8" << std::endl;
		return -1;
	}
#else
	char* res = std::setlocale(LC_ALL, "en_US.utf8");
	if (res == nullptr) {
		std::cerr << "cannot set the locale-based encoding in non-Windows to UTF8" << std::endl;
		return -1;
	}
#endif

	int result = Catch::Session().run(argc, argv);
	return result;
}
