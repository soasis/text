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

int main(int argc, char* argv[]) {
	if (argc < 0)
		return 0;

	// overlong encoded null
	// (https://ztdtext.rtfd.io/en/latest/api/encodings/mutf8.html)
	const char mutf8_text[] = u8"meow\xc0\x80meow!";
	const auto is_valid_mutf8_text
	     = ztd::text::validate_decodable_as(mutf8_text, ztd::text::compat_mutf8);

	std::cout << "The input text is "
	          << (is_valid_mutf8_text.valid ? "valid " : "not valid ")
	          << "MUTF-8 text!" << std::endl;

	return 0;
}
