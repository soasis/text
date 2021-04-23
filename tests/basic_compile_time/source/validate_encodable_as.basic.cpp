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

#include <ztd/text/tests/basic_unicode_strings.hpp>

#include <ztd/text/validate_encodable_as.hpp>
#include <ztd/text/encoding.hpp>
#include <ztd/text/is_unicode_encoding.hpp>

// There is an infinite recursion that needs to be investigated for Clang here...
#if ZTD_TEXT_IS_OFF(ZTD_TEXT_COMPILER_CLANG_I_)


inline namespace ztd_text_tests_basic_compile_time_validate_encodable_as_basic {

	template <typename T>
	static void delayed() {
		// Basic Source Character Set, using defaults
		static_assert(ztd::text::validate_encodable_as(ztd::text::tests::u32_basic_source_character_set));

		// Basic Source Character Set, explicit encoding
		static_assert(ztd::text::validate_encodable_as(
		     ztd::text::tests::u32_basic_source_character_set, ztd::text::literal()));
		static_assert(ztd::text::validate_encodable_as(
		     ztd::text::tests::u32_basic_source_character_set, ztd::text::wide_literal()));
		static_assert(
		     ztd::text::validate_encodable_as(ztd::text::tests::u32_basic_source_character_set, ztd::text::utf8()));
		static_assert(
		     ztd::text::validate_encodable_as(ztd::text::tests::u32_basic_source_character_set, ztd::text::utf16()));
		static_assert(
		     ztd::text::validate_encodable_as(ztd::text::tests::u32_basic_source_character_set, ztd::text::utf32()));
	}

	void instantiate() {
		delayed<void>();
	}
} // namespace ztd_text_tests_basic_compile_time_validate_encodable_as_basic

#endif
