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

#include <ztd/text.hpp>

#include <cuchar>

class runtime_locale {
public:
	using code_point = ztd::text::unicode_code_point;
	using code_unit  = char;

	// State to use for encode_one function
	struct decode_state {
		std::mbstate_t c_stdlib_state;

		decode_state() noexcept : c_stdlib_state() {
			// properly set for mbrtoc32 state
			code_point ghost_ouput[2];
			std::mbrtoc32(ghost_ouput, "\0", 1, &c_stdlib_state);
		}
	};

	// State to use for decode_one function
	struct encode_state {
		std::mbstate_t c_stdlib_state;

		encode_state() noexcept : c_stdlib_state() {
			// properly set for c32rtomb state
			code_unit ghost_ouput[MB_LEN_MAX];
			std::c32rtomb(ghost_ouput, U'\0', &c_stdlib_state);
		}
	};

	// Other, non-state Lucky 7 members...
	inline static constexpr std::size_t max_code_points = 1;
	inline static constexpr std::size_t max_code_units  = MB_LEN_MAX;

private:
	// convenience definitions!
	using rtl_decode_result
	     = ztd::text::decode_result<std::span<const code_unit>, std::span<code_point>, decode_state>;
	using rtl_encode_result
	     = ztd::text::encode_result<std::span<const code_point>, std::span<code_unit>, encode_state>;
	using rtl_decode_error_handler
	     = std::function<rtl_decode_result(const runtime_locale&, rtl_decode_result, std::span<char>)>;
	using rtl_encode_error_handler
	     = std::function<rtl_encode_result(const runtime_locale&, rtl_encode_result, std::span<char32_t>)>;

public:
	rtl_decode_result decode_one(
	     std::span<const code_unit> input, std::span<code_point> output, rtl_decode_error_handler error_handler,
	     decode_state& current // decode-based state
	) {
		// implementation ...
		return rtl_decode_result(input, output, current);
	}

	rtl_encode_result encode_one(
	     std::span<const code_point> input, std::span<code_unit> output, rtl_encode_error_handler error_handler,
	     encode_state& current // encode-based state
	) {
		// implementation ...
		return rtl_encode_result(input, output, current);
	}
};

#include <iostream>

int main(int argc, char* argv[]) {

	// Text coming in from the command line / program arguments
	// is (usually) encoded by the runtime locale
	std::string_view first_arg = argv[0];
	std::u32string decoded_first_arg
	     = ztd::text::decode(first_arg, runtime_locale {}, ztd::text::replacement_handler {});

	// use it as you like...
	(void)decoded_first_arg;

	return 0;
}
