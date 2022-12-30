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
#if ZTD_IS_ON(ZTD_COMPILER_CLANG) || ZTD_IS_ON(ZTD_COMPILER_GCC)
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wunused-variable"
#pragma GCC diagnostic ignored "-Wunused-parameter"
#endif
#include <boost/text/unencoded_rope.hpp>
#if ZTD_IS_ON(ZTD_COMPILER_CLANG) || ZTD_IS_ON(ZTD_COMPILER_GCC)
#pragma GCC diagnostic pop
#endif
#include <cstdint>

int main() {
	using rope_t      = boost::text::basic_unencoded_rope<char16_t>;
	using rope_view_t = boost::text::basic_unencoded_rope_view<char16_t>;
	rope_t rope       = u"✨🌃⭐❣";
	rope_view_t rope_view(rope, 0, rope.size());
	ztd::text::transcode_view<ztd::text::utf16_t, ztd::text::compat_utf8_t, rope_view_t> utf8_rope_view { std::move(
		rope_view) };
	// allows us to iterate over a rope and print things
	for (const auto& code_unit : utf8_rope_view) {
		std::cout.write(&code_unit, 1);
	}
	std::cout << std::endl;
	return 0;
}
