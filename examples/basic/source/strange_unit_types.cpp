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

#include <iostream>

int main(int, char*[]) {
#if ZTD_IS_ON(ZTD_COMPILER_CLANG)
	// Only Clang compilers have _BitInt(N) right now.
	using weird_9bit_utf8_t                           = ztd::text::basic_utf8<_BitInt(9), unsigned _BitInt(21)>;
	constexpr const weird_9bit_utf8_t weird_9bit_utf8 = {};

	auto weird_data_vec = ztd::text::transcode<std::vector<_BitInt(9)>>(u"🎉 Even this works?! 🤪 🎉", weird_9bit_utf8);
	for (const auto& utf8_9_code_unit : weird_data_vec) {
		std::cout.put(static_cast<char>(utf8_9_code_unit));
	}
	std::cout << std::endl;
#endif
	return 0;
}
