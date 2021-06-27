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

#include <ztd/text/examples/utf8_startup.hpp>

#include "shift_jis.hpp"

#include <ztd/text.hpp>

#include <fstream>
#include <string>
#include <vector>

int main() {
	std::string file_name          = "greeting.shift-jis.txt";
	std::string expected_file_name = "greeting.utf-8.txt";
	// open the desired files
	std::ifstream input_file(file_name, std::ios_base::binary);
	std::ifstream expected_input_file(expected_file_name, std::ios_base::binary);
	// if opening the file failed,
	// then leave with an exit code of 1
	if (!input_file) {
		std::cerr << "Could not initialize the input stream with the file " << file_name << std::endl;
		return 1;
	}
	if (!input_file) {
		std::cerr << "Could not initialize the expected input stream with the file " << expected_file_name
		          << std::endl;
		return 1;
	}
	std::vector<char> shift_jis_bytes(std::istreambuf_iterator<char>(input_file), std::istreambuf_iterator<char> {});
	std::vector<char> utf8_bytes(
	     std::istreambuf_iterator<char>(expected_input_file), std::istreambuf_iterator<char> {});

	ztd::text::span<char> shift_jis_input(shift_jis_bytes);
	std::string utf8_string      = ztd::text::transcode(shift_jis_input, shift_jis(), ztd::text::compat_utf8(),
          ztd::text::replacement_handler(), ztd::text::replacement_handler());
	std::string_view utf8_input  = utf8_string;
	std::string shift_jis_string = ztd::text::transcode(utf8_input, ztd::text::compat_utf8(), shift_jis(),
	     ztd::text::replacement_handler(), ztd::text::replacement_handler());

	// check that they are equivalent!
	ZTD_TEXT_ASSERT(std::equal(utf8_string.cbegin(), utf8_string.cend(), utf8_bytes.cbegin(), utf8_bytes.cend()));
	ZTD_TEXT_ASSERT(std::equal(
	     shift_jis_string.cbegin(), shift_jis_string.cend(), shift_jis_bytes.cbegin(), shift_jis_bytes.cend()));
	std::cout << utf8_string << std::endl;

	return 0;
}