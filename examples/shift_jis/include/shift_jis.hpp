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

#pragma once

#ifndef ZTD_TEXT_EXAMPLES_SHIFT_JIS_SHIFT_JIS_HPP
#define ZTD_TEXT_EXAMPLES_SHIFT_JIS_SHIFT_JIS_HPP

#include <ztd/text.hpp>
#include <cstddef>
#include <functional>

// the shift-jis encoding
struct shift_jis {
	using code_unit  = char;
	using code_point = char32_t;
	struct state { };

	static constexpr inline std::size_t max_code_points = 1;
	static constexpr inline std::size_t max_code_units  = 2;

	constexpr ztd::span<const code_unit, 1> replacement_code_units() const noexcept {
		return ztd::span<const code_unit, 1>(&"?"[0], 1);
	}

	using is_decode_injective = std::true_type;

	// clang-format off
	using sjis_encode_result = ztd::text::span_encode_result_for<
		shift_jis
	>;

	using sjis_decode_result = ztd::text::span_decode_result_for<
		shift_jis
	>;

	using sjis_encode_error_handler = std::function<sjis_encode_result(
		const shift_jis&,
		sjis_encode_result,
		ztd::span<const code_point>,
		ztd::span<const code_unit>
	)>;

	using sjis_decode_error_handler = std::function<sjis_decode_result(
		const shift_jis&,
		sjis_decode_result,
		ztd::span<const code_unit>,
		ztd::span<const code_point>
	)>;
	// clang-format on

	sjis_encode_result encode_one(ztd::span<const code_point> input, ztd::span<code_unit> output,
	     sjis_encode_error_handler error_handler, state& current_state) const;

	sjis_decode_result decode_one(ztd::span<const code_unit> input, ztd::span<code_point> output,
	     sjis_decode_error_handler error_handler, state& current_state) const;
};

#endif
