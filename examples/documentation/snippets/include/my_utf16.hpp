// =============================================================================
//
// ztd.text
// Copyright © 2022 JeanHeyd "ThePhD" Meneide and Shepherd's Oasis, LLC
// Contact: opensource@soasis.org
//
// Commercial License Usage
// Licensees holding valid commercial ztd.text licenses may use this file
// in accordance with the commercial license agreement provided with the
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

// start my_utf16-snippet
#include <ztd/text.hpp>

#include <utility>

class my_utf16 : private ztd::text::utf16_t {
private:
	using base_t = ztd::text::utf16_t;

public:
	// Lucky 7 Members
	static inline constexpr std::size_t max_code_points = 1;
	static inline constexpr std::size_t max_code_units  = 2;
	using state      = ztd::text::decode_state_t<ztd::text::utf16_t>;
	using code_point = char32_t;
	using code_unit  = char16_t;
	// Extension definitions
	using is_unicode_encoding = std::true_type; // UTF-16 is Unicode
	using is_injective        = std::true_type; // conversion is not lossy

	// Import base implementation here,
	// to save on the implementation work!
	using base_t::decode_one;
	using base_t::encode_one;

	// Import additional methods
	using base_t::replacement_code_points;
	using base_t::replacement_code_units;

	// ❗ Special input skip member!!
	// If this function is present and callable, it will
	// allow us to skip over bad input.
	template <typename Input, typename Output, typename State>
	constexpr auto skip_input_error(
	     ztd::text::decode_result<Input, Output, State> result) const noexcept {
		// If we are decoding a UTF-16 sequence,
		// we can have 1 or 2 UTF-16 code units.
		// they are identifiable as leading and trailing surrogates
		constexpr char16_t last_utf16_lead_surrogate = u'\xDBFF';
		auto it   = ztd::ranges::begin(result.input);
		auto last = ztd::ranges::end(result.input);
		if (it != last) {
			// We can skip all trailing surrogates, until we find a leading one.
			do {
				++it;
			} while (it != last && *it > last_utf16_lead_surrogate);
		}
		// put input range back together, return in constructed result object
		using SubInput = ztd::ranges::subrange_for_t<Input>;
		using Result   = ztd::text::decode_result<SubInput, Output, State>;
		return Result(
		     // subrange of input
		     SubInput(std::move(it), std::move(last)),
		     // move the output
		     std::move(result.output),
		     // pass state along
		     result.state,
		     // existing error code
		     result.error_code,
		     // existing error count
		     result.error_count);
	}
};
