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

#include <string_view>

int main(int, char*[]) {
	constexpr const std::u32string_view input = U"meow🐱moew🐱!";

	// At compile-time: returns a structure with (explicit) operator bool
	// to allow it to be used with ! and if() statements
	static_assert(!ztd::text::validate_encodable_as(input, ztd::text::ascii),
	     "Unfortunately, ASCII does not support emoji.");

	// At run-time: returns a structure
	auto validate_result
	     = ztd::text::validate_encodable_as(input, ztd::text::ascii);

	// Check if the result is valid (should not be valid).
	if (validate_result) {
		// Everyyhing was verified (not expected! ❌)
		std::cerr << "Unexpectedly, the input text was all valid ASCII."
		          << std::endl;
		return 1;
	}

	// Otherwise, everything was not verified (expected! ✅)
	std::cout << "As expected, the input text was not valid ASCII."
	          << "\n"
	          << "Here are the unicode hex values of the unvalidated UTF-32 code "
	             "points:\n\t";
	// use the structure to know where we left off.
	std::u32string_view unused_input(
	     validate_result.input.data(), validate_result.input.size());
	for (const auto& u32_codepoint : unused_input) {
		std::cout << "0x" << std::hex << u32_codepoint;
		if (&u32_codepoint != &unused_input.back()) {
			std::cout << " ";
		}
	}
	std::cout << std::endl;
	return 0;
}
