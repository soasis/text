// =============================================================================
//
// ztd.text
// Copyright © 2022-2023 JeanHeyd "ThePhD" Meneide and Shepherd's Oasis, LLC
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

class my_encoding {
public:
	struct empty { };
	// the regular Lucky 7 members
	static inline constexpr std::size_t max_code_points = 1;
	static inline constexpr std::size_t max_code_units  = 1;
	using state                                         = empty;
	using code_point                                    = char32_t;
	using code_unit                                     = char;

	template <typename Input, typename Output, typename ErrorHandler>
	constexpr static auto decode_one(Input&& input, Output&& output,
	     state& current_state, ErrorHandler&& error_handler) noexcept {
		// decoding implementation here !
		return decode_result<ztd::remove_cvref_t<Input>,
		     ztd::remove_cvref_t<Output>, state>(
		     input, output, current_state, ztd::text::encoding_error::ok);
	}

	template <typename Input, typename Output, typename ErrorHandler>
	static constexpr auto decode_one(Input&& input, Output&& output,
	     state& current_state, ErrorHandler&& error_handler) noexcept {
		// encoding implementation here !
		return encode_result<ztd::remove_cvref_t<Input>,
		     ztd::remove_cvref_t<Output>, state>(
		     input, output, current_state, ztd::text::encoding_error::ok);
	}

	// ❗ Special input skip member here
	template <typename Input, typename Output, typename State>
	static constexpr auto skip_input_error(
	     decode_result<Input, Output, State> result) noexcept {
		// manipulate "result" here,
		// for any failures in the decode routine.
		return result;
	}

	template <typename Input, typename Output, typename State>
	static constexpr auto skip_input_error(
	     encode_result<Input, Output, State> result) noexcept {
		// manipulate "result" here,
		// for any failures in the encode routine.
		return result;
	}
};
