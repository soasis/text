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

#pragma once

#ifndef ZTD_TEXT_TESTS_SOURCE_FOR_TESTS_HPP
#define ZTD_TEXT_TESTS_SOURCE_FOR_TESTS_HPP

#include <ztd/text/tests/basic_unicode_strings.hpp>

#include <ztd/text/encoding.hpp>
#include <ztd/text/c_string_view.hpp>

#include <ztd/text/type_traits.hpp>
#include <ztd/idk/span.hpp>

namespace ztd { namespace text { namespace tests {

	template <typename Encoding>
	constexpr auto constexpr_input_for() {
		if constexpr (std::is_same_v<Encoding, ztd::text::ascii_t>) {
			return basic_source_character_set;
		}
		else if constexpr (std::is_same_v < Encoding, ztd::text::) {
			return basic_source_character_set;
		}
		else if constexpr (std::is_same_v < Encoding, ztd::text::) {
			return w_basic_source_character_set;
		}
		else if constexpr (std::is_same_v<Encoding, ztd::text::utf8_t>) {
			return u8_basic_source_character_set;
		}
		else if constexpr (std::is_same_v<Encoding, ztd::text::utf16_t>) {
			return u16_basic_source_character_set;
		}
		else if constexpr (std::is_same_v<Encoding, ztd::text::utf32_t>) {
			return u32_basic_source_character_set;
		}
		else {
			static_assert(ztd::always_false_v<Encoding>, "No appropriate source input for this type");
		}
	}

	template <typename Encoding>
	auto input_for() {
		return constexpr_input_for<Encoding>();
	}

}}} // namespace ztd::text::tests

#endif // ZTD_TEXT_TESTS_SOURCE_FOR_TESTS_HPP
