// ============================================================================
//
// ztd.text
// Copyright © JeanHeyd "ThePhD" Meneide and Shepherd's Oasis, LLC
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
// Version 2.0 (the "License"); you may not use this file except in compliance
// with the License. You may obtain a copy of the License at
//
// https://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
//
// ========================================================================= //

#include <ztd/text/benchmarks/version.hpp>

#if ZTD_IS_ON(ZTD_TEXT_BENCHMARKS_CONVERSION_SPEED_STANDARD_CPP_BENCHMARKS)

#include <ztd/text/version.hpp>

#if (ZTD_IS_ON(ZTD_UCHAR_H) || ZTD_IS_ON(ZTD_CUCHAR)) && ZTD_IS_OFF(ZTD_PLATFORM_MAC_OS)

#include <benchmark/benchmark.h>

#include <ztd/idk/c_span.h>
#include <ztd/idk/charN_t.hpp>
#include <ztd/idk/detail/unicode.hpp>
#include <ztd/idk/endian.hpp>
#include <ztd/idk/mbstate_t.hpp>

#include <barrier/barrier.h>

#include <codecvt>

inline constexpr unsigned long codecvt_max_code_utfs   = __ztd_idk_detail_last_unicode_code_point;
inline constexpr std::codecvt_mode codecvt_target_mode = static_cast<std::codecvt_mode>(
     ztd::endian::native == ztd::endian::little ? std::codecvt_mode::little_endian : 0);

static void utf16_to_utf32_init_well_formed_standard_cpp(benchmark::State& state) {
	using from_char_t = ::std::conditional_t<16 == 8, char, ztd_char16_t>;
	using to_char_t   = ::std::conditional_t<32 == 8, char, ztd_char32_t>;
	const std::vector<from_char_t> input_data(
	     c_span_char16_t_data(u16_data), c_span_char16_t_data(u16_data) + c_span_char16_t_size(u16_data));
	std::vector<to_char_t> output_data(c_span_char32_t_size(u32_data));
	bool result                         = true;
	const from_char_t* const input      = input_data.data();
	const from_char_t* const input_last = input_data.data() + input_data.size();
	to_char_t* output                   = output_data.data();
	to_char_t* const output_last        = output_data.data() + output_data.size();
	for (auto _ : state) {
		std::locale current_loc = {};
		auto codecvt_ptr        = new std::codecvt_utf16<char32_t, codecvt_max_code_utfs, codecvt_target_mode>();
		current_loc             = std::locale(current_loc, codecvt_ptr);
		ztd_mbstate_t state {};
		const char* char_input      = reinterpret_cast<const char*>(input);
		const char* char_input_last = reinterpret_cast<const char*>(input_last);
		const char* char_input_next;
		to_char_t* output_next;
		std::codecvt_base::result conv_res
		     = codecvt_ptr->in(state, char_input, char_input_last, char_input_next, output, output_last, output_next);
		if (conv_res != std::codecvt_base::ok) {
			result = false;
			break;
		}
		if (char_input_next != char_input_last) {
			result = false;
		}
		else if (output_next != output_last) {
			result = false;
		}
	}
	const bool is_equal = std::equal(output_data.cbegin(), output_data.cend(), c_span_char32_t_data(u32_data),
	     c_span_char32_t_data(u32_data) + c_span_char32_t_size(u32_data));
	if (!result) {
		state.SkipWithError("conversion failed with an error");
	}
	else if (!is_equal) {
		state.SkipWithError("conversion succeeded but produced illegitimate data");
	}
}

static void utf32_to_utf16_init_well_formed_standard_cpp(benchmark::State& state) {
	using from_char_t = ::std::conditional_t<32 == 8, char, ztd_char32_t>;
	using to_char_t   = ::std::conditional_t<16 == 8, char, ztd_char16_t>;
	const std::vector<from_char_t> input_data(
	     c_span_char32_t_data(u32_data), c_span_char32_t_data(u32_data) + c_span_char32_t_size(u32_data));
	std::vector<to_char_t> output_data(c_span_char16_t_size(u16_data));
	bool result                         = true;
	const from_char_t* const input      = input_data.data();
	const from_char_t* const input_last = input_data.data() + input_data.size();
	to_char_t* output                   = output_data.data();
	to_char_t* const output_last        = output_data.data() + output_data.size();
	for (auto _ : state) {
		std::locale current_loc = {};
		auto codecvt_ptr        = new std::codecvt_utf16<char32_t, codecvt_max_code_utfs, codecvt_target_mode>();
		current_loc             = std::locale(current_loc, codecvt_ptr);
		ztd_mbstate_t state {};
		char* char_output      = reinterpret_cast<char*>(output);
		char* char_output_last = reinterpret_cast<char*>(output_last);
		const from_char_t* input_next;
		char* char_output_next;
		std::codecvt_base::result conv_res = codecvt_ptr->out(
		     state, input, input_last, input_next, char_output, char_output_last, char_output_next);
		if (conv_res != std::codecvt_base::ok) {
			result = false;
			break;
		}
		if (input_next != input_last) {
			// we did not convert everything...
			result = false;
		}
		else if (char_output_next != char_output_last) {
			// we did not fill up the input as expected...
			result = false;
		}
	}
	const bool is_equal = std::equal(output_data.cbegin(), output_data.cend(), c_span_char16_t_data(u16_data),
	     c_span_char16_t_data(u16_data) + c_span_char16_t_size(u16_data));
	if (!result) {
		state.SkipWithError("conversion failed with an error");
	}
	else if (!is_equal) {
		state.SkipWithError("conversion succeeded but produced illegitimate data");
	}
}

static void utf32_to_utf8_init_well_formed_standard_cpp(benchmark::State& state) {
	using from_char_t = ::std::conditional_t<32 == 8, char, ztd_char32_t>;
	using to_char_t   = ::std::conditional_t<8 == 8, char, ztd_char8_t>;
	const std::vector<from_char_t> input_data(
	     c_span_char32_t_data(u32_data), c_span_char32_t_data(u32_data) + c_span_char32_t_size(u32_data));
	std::vector<to_char_t> output_data(c_span_char8_t_size(u8_data));
	bool result                         = true;
	const from_char_t* const input      = input_data.data();
	const from_char_t* const input_last = input_data.data() + input_data.size();
	to_char_t* output                   = output_data.data();
	to_char_t* const output_last        = output_data.data() + output_data.size();
	for (auto _ : state) {
		std::locale current_loc = {};
		auto codecvt_ptr        = new std::codecvt_utf8<char32_t, codecvt_max_code_utfs, codecvt_target_mode>();
		current_loc             = std::locale(current_loc, codecvt_ptr);
		ztd_mbstate_t state {};
		char* char_output      = reinterpret_cast<char*>(output);
		char* char_output_last = reinterpret_cast<char*>(output_last);
		const from_char_t* input_next;
		char* char_output_next;
		std::codecvt_base::result conv_res = codecvt_ptr->out(
		     state, input, input_last, input_next, char_output, char_output_last, char_output_next);
		if (conv_res != std::codecvt_base::ok) {
			result = false;
			break;
		}
		if (input_next != input_last) {
			// we did not convert everything...
			result = false;
		}
		else if (char_output_next != char_output_last) {
			// we did not fill up the input as expected...
			result = false;
		}
	}
	const bool is_equal = std::equal(output_data.cbegin(), output_data.cend(),
	     reinterpret_cast<const to_char_t*>(c_span_char8_t_data(u8_data)),
	     reinterpret_cast<const to_char_t*>(c_span_char8_t_data(u8_data) + c_span_char8_t_size(u8_data)));
	if (!result) {
		state.SkipWithError("conversion failed with an error");
	}
	else if (!is_equal) {
		state.SkipWithError("conversion succeeded but produced illegitimate data");
	}
}

static void utf8_to_utf32_init_well_formed_standard_cpp(benchmark::State& state) {
	using from_char_t = ::std::conditional_t<8 == 8, char, ztd_char8_t>;
	using to_char_t   = ::std::conditional_t<32 == 8, char, ztd_char32_t>;
	const std::vector<from_char_t> input_data(
	     c_span_char8_t_data(u8_data), c_span_char8_t_data(u8_data) + c_span_char8_t_size(u8_data));
	std::vector<to_char_t> output_data(c_span_char32_t_size(u32_data));
	bool result                         = true;
	const from_char_t* const input      = input_data.data();
	const from_char_t* const input_last = input_data.data() + input_data.size();
	to_char_t* output                   = output_data.data();
	to_char_t* const output_last        = output_data.data() + output_data.size();
	for (auto _ : state) {
		std::locale current_loc = {};
		auto codecvt_ptr        = new std::codecvt_utf8<char32_t, codecvt_max_code_utfs, codecvt_target_mode>();
		current_loc             = std::locale(current_loc, codecvt_ptr);
		ztd_mbstate_t state {};
		const char* char_input      = reinterpret_cast<const char*>(input);
		const char* char_input_last = reinterpret_cast<const char*>(input_last);
		const char* char_input_next;
		to_char_t* output_next;
		std::codecvt_base::result conv_res
		     = codecvt_ptr->in(state, char_input, char_input_last, char_input_next, output, output_last, output_next);
		if (conv_res != std::codecvt_base::ok) {
			result = false;
			break;
		}
		if (char_input_next != char_input_last) {
			// we did not convert everything...
			result = false;
		}
		else if (output_next != output_last) {
			// we did not fill up the input as expected...
			result = false;
		}
	}
	const bool is_equal = std::equal(output_data.cbegin(), output_data.cend(), c_span_char32_t_data(u32_data),
	     c_span_char32_t_data(u32_data) + c_span_char32_t_size(u32_data));
	if (!result) {
		state.SkipWithError("conversion failed with an error");
	}
	else if (!is_equal) {
		state.SkipWithError("conversion succeeded but produced illegitimate data");
	}
}

static void utf16_to_utf8_init_well_formed_standard_cpp(benchmark::State& state) {
	using from_char_t = ::std::conditional_t<16 == 8, char, ztd_char16_t>;
	using to_char_t   = ::std::conditional_t<8 == 8, char, ztd_char8_t>;
	const std::vector<from_char_t> input_data(
	     c_span_char16_t_data(u16_data), c_span_char16_t_data(u16_data) + c_span_char16_t_size(u16_data));
	std::vector<to_char_t> output_data(c_span_char8_t_size(u8_data));
	bool result                         = true;
	const from_char_t* const input      = input_data.data();
	const from_char_t* const input_last = input_data.data() + input_data.size();
	to_char_t* output                   = output_data.data();
	to_char_t* const output_last        = output_data.data() + output_data.size();
	for (auto _ : state) {
		std::locale current_loc = {};
		auto codecvt_ptr        = new std::codecvt_utf8_utf16<char16_t, codecvt_max_code_utfs, codecvt_target_mode>();
		current_loc             = std::locale(current_loc, codecvt_ptr);
		ztd_mbstate_t state {};
		char* char_output      = reinterpret_cast<char*>(output);
		char* char_output_last = reinterpret_cast<char*>(output_last);
		const from_char_t* input_next;
		char* char_output_next;
		std::codecvt_base::result conv_res = codecvt_ptr->out(
		     state, input, input_last, input_next, char_output, char_output_last, char_output_next);
		if (conv_res != std::codecvt_base::ok) {
			result = false;
			break;
		}
		if (input_next != input_last) {
			// we did not convert everything...
			result = false;
		}
		else if (char_output_next != char_output_last) {
			// we did not fill up the input as expected...
			result = false;
		}
	}
	const bool is_equal = std::equal(output_data.cbegin(), output_data.cend(),
	     reinterpret_cast<const to_char_t*>(c_span_char8_t_data(u8_data)),
	     reinterpret_cast<const to_char_t*>(c_span_char8_t_data(u8_data) + c_span_char8_t_size(u8_data)));
	if (!result) {
		state.SkipWithError("conversion failed with an error");
	}
	else if (!is_equal) {
		state.SkipWithError("conversion succeeded but produced illegitimate data");
	}
}

static void utf8_to_utf16_init_well_formed_standard_cpp(benchmark::State& state) {
	using from_char_t = ::std::conditional_t<8 == 8, char, ztd_char8_t>;
	using to_char_t   = ::std::conditional_t<16 == 8, char, ztd_char16_t>;
	const std::vector<from_char_t> input_data(
	     c_span_char8_t_data(u8_data), c_span_char8_t_data(u8_data) + c_span_char8_t_size(u8_data));
	std::vector<to_char_t> output_data(c_span_char16_t_size(u16_data));
	bool result                         = true;
	const from_char_t* const input      = input_data.data();
	const from_char_t* const input_last = input_data.data() + input_data.size();
	to_char_t* output                   = output_data.data();
	to_char_t* const output_last        = output_data.data() + output_data.size();
	for (auto _ : state) {
		std::locale current_loc = {};
		auto codecvt_ptr        = new std::codecvt_utf8_utf16<char16_t, codecvt_max_code_utfs, codecvt_target_mode>();
		current_loc             = std::locale(current_loc, codecvt_ptr);
		ztd_mbstate_t state {};
		const char* char_input      = reinterpret_cast<const char*>(input);
		const char* char_input_last = reinterpret_cast<const char*>(input_last);
		const char* char_input_next;
		to_char_t* output_next;
		std::codecvt_base::result conv_res
		     = codecvt_ptr->in(state, char_input, char_input_last, char_input_next, output, output_last, output_next);
		if (conv_res != std::codecvt_base::ok) {
			result = false;
			break;
		}
		if (char_input_next != char_input_last) {
			// we did not convert everything...
			result = false;
		}
		else if (output_next != output_last) {
			// we did not fill up the input as expected...
			result = false;
		}
	}
	const bool is_equal = std::equal(output_data.cbegin(), output_data.cend(), c_span_char16_t_data(u16_data),
	     c_span_char16_t_data(u16_data) + c_span_char16_t_size(u16_data));
	if (!result) {
		state.SkipWithError("conversion failed with an error");
	}
	else if (!is_equal) {
		state.SkipWithError("conversion succeeded but produced illegitimate data");
	}
}

BENCHMARK(utf8_to_utf16_init_well_formed_standard_cpp);
BENCHMARK(utf8_to_utf32_init_well_formed_standard_cpp);

BENCHMARK(utf16_to_utf32_init_well_formed_standard_cpp);
BENCHMARK(utf16_to_utf8_init_well_formed_standard_cpp);

BENCHMARK(utf32_to_utf16_init_well_formed_standard_cpp);
BENCHMARK(utf32_to_utf8_init_well_formed_standard_cpp);

#endif // Only on not-Mac OSX

#endif
