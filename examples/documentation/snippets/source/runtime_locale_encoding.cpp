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

#include <ztd/text.hpp>

#if !defined(_LIBCPP_VERSION) && ZTD_IS_OFF(ZTD_PLATFORM_MAC_OS)
// This example doesn't work on Apple/libc++ because they don't have
// standard C or C++ headers.

#include <cstddef>
#if defined(__has_include) && __has_include(<cuchar>)
#include <cuchar>
#define UCHAR_ACCESS ::std::
#else
extern "C" {
#include <uchar.h>
}
#define UCHAR_ACCESS ::
#endif

#if defined(_WIN32_)
#define NOMINMAX 1
#define WIN32_LEAN_AND_MEAN 1
#include <windows.h>
#else

#endif

class runtime_locale {
public:
	using is_decode_injective = std::false_type;
	using is_encode_injective = std::false_type;
	using code_point          = ztd::text::unicode_code_point;
	using code_unit           = char;
	inline static constexpr std::size_t max_code_points = 1;
	inline static constexpr std::size_t max_code_units  = MB_LEN_MAX;

	struct decode_state {
		std::mbstate_t c_stdlib_state;

		decode_state() noexcept : c_stdlib_state() {
			// properly set for mbrtoc32 state
			code_point ghost_ouput[2] {};
			UCHAR_ACCESS mbrtoc32(ghost_ouput, "\0", 1, &c_stdlib_state);
		}
	};

	struct encode_state {
		std::mbstate_t c_stdlib_state;

		encode_state() noexcept : c_stdlib_state() {
			// properly set for c32rtomb state
			code_unit ghost_ouput[MB_LEN_MAX] {};
			UCHAR_ACCESS c32rtomb(ghost_ouput, U'\0', &c_stdlib_state);
		}
	};

	bool contains_unicode_encoding() const noexcept {
#if defined(_WIN32)
		CPINFOEXW cp_info {};
		BOOL success = GetCPInfoExW(CP_THREAD_ACP, 0, &cp_info);
		if (success == 0) {
			return false;
		}
		switch (cp_info.CodePage) {
		case 65001: // UTF-8
		            // etc. etc. …
			return true;
		default:
			break;
		}
#else
		std::string_view locale_name = std::setlocale(LC_CTYPE, NULL);
		if (locale_name == "UTF-8" || locale_name == "UTF-7"
		     // etc. etc …
		) {
			return true;
		}
#endif
		// unknown: not unicode, maybe
		return false;
	}

	ztd::span<const code_unit> replacement_code_units() const noexcept {
		if (this->contains_unicode_encoding()) {
			// Probably CESU-8 or UTF-8!
			static const char replacement[3] = { '\xEF', '\xBF', '\xBD' };
			return replacement;
		}
		else {
			// Uh... well, it probably has this? ¯\_(ツ)_/¯
			static const char replacement[1] = { '?' };
			return replacement;
		}
	}

private:
	using rtl_decode_result = ztd::text::decode_result<ztd::span<const code_unit>,
	     ztd::span<code_point>, decode_state>;
	using rtl_encode_result = ztd::text::encode_result<ztd::span<const code_point>,
	     ztd::span<code_unit>, encode_state>;
	using rtl_decode_error_handler
	     = std::function<rtl_decode_result(const runtime_locale&, rtl_decode_result,
	          ztd::span<const char>, ztd::span<const char32_t>)>;
	using rtl_encode_error_handler
	     = std::function<rtl_encode_result(const runtime_locale&, rtl_encode_result,
	          ztd::span<const char32_t>, ztd::span<const char>)>;

	using empty_code_unit_span  = ztd::span<const code_unit, 0>;
	using empty_code_point_span = ztd::span<const code_point, 0>;

public:
	rtl_decode_result decode_one(ztd::span<const code_unit> input,
	     ztd::span<code_point> output, rtl_decode_error_handler error_handler,
	     decode_state& current // decode-based state
	) const {
		if (output.size() < 1) {
			return error_handler(*this,
			     rtl_decode_result(input, output, current,
			          ztd::text::encoding_error::insufficient_output_space),
			     empty_code_unit_span(), empty_code_point_span());
		}
		std::size_t result = UCHAR_ACCESS mbrtoc32(
		     output.data(), input.data(), input.size(), &current.c_stdlib_state);
		switch (result) {
		case (std::size_t)0:
			// '\0' was encountered in the input
			// current.c_stdlib_state was "cleared"
			// '\0' character was written to output
			return rtl_decode_result(
			     input.subspan(1), output.subspan(1), current);
			break;
		case (std::size_t)-3:
			// no input read, pre-stored character
			// was written out
			return rtl_decode_result(input, output.subspan(1), current);
		case (std::size_t)-2:
			// input was an incomplete sequence
			return error_handler(*this,
			     rtl_decode_result(input, output, current,
			          ztd::text::encoding_error::incomplete_sequence),
			     empty_code_unit_span(), empty_code_point_span());
			break;
		case (std::size_t)-1:
			// invalid sequence!
			return error_handler(*this,
			     rtl_decode_result(input, output, current,
			          ztd::text::encoding_error::invalid_sequence),
			     empty_code_unit_span(), empty_code_point_span());
		}
		// everything as fine, then
		return rtl_decode_result(
		     input.subspan(result), output.subspan(1), current);
	}

	rtl_encode_result encode_one(ztd::span<const code_point> input,
	     ztd::span<code_unit> output, rtl_encode_error_handler error_handler,
	     encode_state& current // encode-based state
	) const {
		// saved, in case we need to go
		// around mulitple times to get
		// an output character
		ztd::span<const code_point> original_input = input;
		// The C standard library assumes
		// it can write out MB_CUR_MAX characters to the buffer:
		// we have no guarantee our output buffer is that big, so it
		// needs to go into an intermediate buffer instead
		code_unit intermediate_buffer[MB_LEN_MAX];

		for ([[maybe_unused]] int times_around = 0;; ++times_around) {
			if (input.size() < 1) {
				// no more input: everything is fine
				return rtl_encode_result(input, output, current);
			}
			std::size_t result = UCHAR_ACCESS c32rtomb(
			     intermediate_buffer, *input.data(), &current.c_stdlib_state);
			if (result == (std::size_t)-1) {
				// invalid sequence!
				return error_handler(*this,
				     rtl_encode_result(original_input, output, current,
				          ztd::text::encoding_error::invalid_sequence),
				     empty_code_point_span(), empty_code_unit_span());
			}
			else if (result == (std::size_t)0) {
				// this means nothing was output
				// we should probably go-around again,
				// after modifying input
				input = input.subspan(1);
				continue;
			}
			// otherwise, we got something written out!
			if (output.size() < result) {
				// can't fit!!
				return error_handler(*this,
				     rtl_encode_result(original_input, output, current,
				          ztd::text::encoding_error::insufficient_output_space),
				     empty_code_point_span(), empty_code_unit_span());
			}
			::std::memcpy(output.data(), intermediate_buffer,
			     sizeof(*intermediate_buffer) * result);
			input  = input.subspan(1);
			output = output.subspan(result);
			break;
		}
		return rtl_encode_result(input, output, current);
	}
};

int main(int argc, char* argv[]) {
	if (argc < 1) {
		return 0;
	}
	// Text coming in from the command line / program arguments
	// is (usually) encoded by the runtime locale
	runtime_locale encoding {};
	std::string_view first_arg       = argv[0];
	std::u32string decoded_first_arg = ztd::text::decode(
	     first_arg, encoding, ztd::text::replacement_handler_t {});

	// the first argument is (usually!!) the executable name,
	// so we'll see if we decoded that part nicely!
	ZTD_TEXT_ASSERT(decoded_first_arg.find(U"runtime_locale_encoding")
	     != std::u32string::npos);

	return 0;
}


#else
int main([[maybe_unused]] int argc, [[maybe_unused]] char* argv[]) {
	return 0;
}
#endif
