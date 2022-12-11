// ============================================================================
//
// ztd.text
// Copyright © 2022-2022 JeanHeyd "ThePhD" Meneide and Shepherd's Oasis, LLC
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
// 		http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
//
// ========================================================================= //

#include <ztd/text/benchmarks/version.hpp>

#if ZTD_IS_ON(ZTD_TEXT_BENCHMARKS_CONVERSION_SPEED_STANDARD_C_BENCHMARKS)

#include <ztd/text/version.hpp>

#if (ZTD_IS_ON(ZTD_UCHAR) || ZTD_IS_ON(ZTD_CUCHAR)) && ZTD_IS_OFF(ZTD_PLATFORM_MAC_OS)

#include <benchmark/benchmark.h>

#include <ztd/idk/c_span.h>
#include <ztd/idk/charN_t.hpp>
#include <barrier/barrier.h>

#if ZTD_IS_ON(ZTD_CUCHAR)
#include <cuchar>
#else
#include <uchar.h>
#endif

static void utf16_to_utf32_well_formed_standard_c(benchmark::State& state) {
	using from_char_t = ::std::conditional_t<16 == 8, char, ztd_char16_t>;
	using to_char_t   = ::std::conditional_t<32 == 8, char, ztd_char32_t>;
	const std::vector<from_char_t> input_data(
	     c_span_char16_t_data(u16_data), c_span_char16_t_data(u16_data) + c_span_char16_t_size(u16_data));
	std::vector<to_char_t> output_data(c_span_char32_t_size(u32_data));
	bool result = true;
	for (auto _ : state) {
		mbstate_t from_state {};
		mbstate_t to_state {};
		char intermediate_data[MB_LEN_MAX * 2];
		to_char_t* output                   = output_data.data();
		to_char_t* output_last              = output_data.data() + output_data.size();
		char* intermediate                  = intermediate_data;
		const from_char_t* input            = input_data.data();
		const from_char_t* const input_last = input_data.data() + input_data.size();
		for (;;) {
			if (input != input_last) {
				const size_t from_result = ZTD_UCHAR_ACCESSOR_I_ c16rtomb(intermediate, *input, &from_state);
				switch (from_result) {
				case static_cast<size_t>(-1):
					// encoding error: everyhting has failed. bail.
					result = false;
					goto loop_break;
				case static_cast<size_t>(-3):
					// a single output was been written, but nothing has been processed
					// we must loop around.
					continue;
				default:
					break;
				}
				const size_t intermediate_size = from_result + (intermediate - intermediate_data);
				const size_t to_result
				     = ZTD_UCHAR_ACCESSOR_I_ mbrtoc32(output, intermediate_data, intermediate_size, &to_state);
				switch (to_result) {
				case static_cast<size_t>(-1):
					// encoding error: everyhting has failed. bail.
					result = false;
					goto loop_break;
				case static_cast<size_t>(-2):
					// this should never happen.... but it just might!!
					// loop back around to "accumulate" more data. If we do this twice, something is fucked.
					intermediate += intermediate_size;
					input += 1;
					continue;
				case static_cast<size_t>(-3):
					// a single output was been written, but nothing has been processed
					output += 1;
					break;
				case 0:
					// null character read/written
				default:
					input += 1;
					output += 1;
					break;
				}
				intermediate = intermediate_data;
			}
			else {
				// The C Standard is so fucking bad that you ACTUALLY cannot
				// finish processing text without having to check if you have cleaned out
				// the state. You have to call the function more to make sure
				// you have drained off all the surrogates...
				while (ZTD_UCHAR_ACCESSOR_I_ mbsinit(&to_state) == 0) {
					// try to drain the (last?) input out...
					to_char_t last_c;
					const size_t last_to_result = ZTD_UCHAR_ACCESSOR_I_ mbrtoc32(&last_c, "", 1, &to_state);
					if (last_to_result == static_cast<size_t>(-3)) {
						// we had extra, vomit it out...
						*output = last_c;
						++output;
					}
					else {
						break;
					}
				}
				break;
			}
		}
	loop_break:;
		if (output != output_last) {
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

static void utf32_to_utf16_well_formed_standard_c(benchmark::State& state) {
	using from_char_t = ::std::conditional_t<32 == 8, char, ztd_char32_t>;
	using to_char_t   = ::std::conditional_t<16 == 8, char, ztd_char16_t>;
	const std::vector<from_char_t> input_data(
	     c_span_char32_t_data(u32_data), c_span_char32_t_data(u32_data) + c_span_char32_t_size(u32_data));
	std::vector<to_char_t> output_data(c_span_char16_t_size(u16_data));
	bool result = true;
	for (auto _ : state) {
		mbstate_t from_state {};
		mbstate_t to_state {};
		char intermediate_data[MB_LEN_MAX * 2];
		to_char_t* output                   = output_data.data();
		to_char_t* output_last              = output_data.data() + output_data.size();
		char* intermediate                  = intermediate_data;
		const from_char_t* input            = input_data.data();
		const from_char_t* const input_last = input_data.data() + input_data.size();
		for (;;) {
			if (input != input_last) {
				const size_t from_result = ZTD_UCHAR_ACCESSOR_I_ c32rtomb(intermediate, *input, &from_state);
				switch (from_result) {
				case static_cast<size_t>(-1):
					// encoding error: everyhting has failed. bail.
					result = false;
					goto loop_break;
				case static_cast<size_t>(-3):
					// a single output was been written, but nothing has been processed
					// we must loop around.
					continue;
				default:
					break;
				}
				const size_t intermediate_size = from_result + (intermediate - intermediate_data);
				const size_t to_result
				     = ZTD_UCHAR_ACCESSOR_I_ mbrtoc16(output, intermediate_data, intermediate_size, &to_state);
				switch (to_result) {
				case static_cast<size_t>(-1):
					// encoding error: everyhting has failed. bail.
					result = false;
					goto loop_break;
				case static_cast<size_t>(-2):
					// this should never happen.... but it just might!!
					// loop back around to "accumulate" more data. If we do this twice, something is fucked.
					intermediate += intermediate_size;
					input += 1;
					continue;
				case static_cast<size_t>(-3):
					// a single output was been written, but nothing has been processed: we
					// this happens with surrogate pairs.
					output += 1;
					break;
				case 0:
					// null character read/written
				default:
					input += 1;
					output += 1;
					break;
				}
				intermediate = intermediate_data;
			}
			else {
				// The C Standard is so fucking bad that you ACTUALLY cannot
				// finish processing text without having to check if you have cleaned out
				// the state. You have to call the function more to make sure
				// you have drained off all the surrogates...
				while (ZTD_UCHAR_ACCESSOR_I_ mbsinit(&to_state) == 0) {
					// try to drain the (last?) input out...
					to_char_t last_c;
					const size_t last_to_result = ZTD_UCHAR_ACCESSOR_I_ mbrtoc16(&last_c, "", 1, &to_state);
					if (last_to_result == static_cast<size_t>(-3)) {
						// we had extra, vomit it out...
						*output = last_c;
						++output;
					}
					else {
						break;
					}
				}
				break;
			}
		}
	loop_break:;
		if (output != output_last) {
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

static void utf32_to_utf8_well_formed_standard_c(benchmark::State& state) {
	using from_char_t = ::std::conditional_t<32 == 8, char, ztd_char32_t>;
	using to_char_t   = ::std::conditional_t<8 == 8, char, ztd_char8_t>;
	const std::vector<from_char_t> input_data(
	     c_span_char32_t_data(u32_data), c_span_char32_t_data(u32_data) + c_span_char32_t_size(u32_data));
	std::vector<to_char_t> output_data(c_span_char8_t_size(u8_data));
	bool result = true;
	for (auto _ : state) {
		mbstate_t from_state {};
		to_char_t* output                   = output_data.data();
		to_char_t* output_last              = output_data.data() + output_data.size();
		const from_char_t* input            = input_data.data();
		const from_char_t* const input_last = input_data.data() + input_data.size();
		for (; input != input_last;) {
			const size_t from_result = ZTD_UCHAR_ACCESSOR_I_ c32rtomb(output, *input, &from_state);
			switch (from_result) {
			case static_cast<size_t>(-1):
				// encoding error: everyhting has failed. bail.
				result = false;
				goto loop_break;
			default:
				output += from_result;
				input += 1;
				break;
			}
		}
	loop_break:;
		if (output != output_last) {
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

static void utf8_to_utf32_well_formed_standard_c(benchmark::State& state) {
	using from_char_t = ::std::conditional_t<8 == 8, char, ztd_char8_t>;
	using to_char_t   = ::std::conditional_t<32 == 8, char, ztd_char32_t>;
	const std::vector<from_char_t> input_data(
	     c_span_char8_t_data(u8_data), c_span_char8_t_data(u8_data) + c_span_char8_t_size(u8_data));
	std::vector<to_char_t> output_data(c_span_char32_t_size(u32_data));
	bool result = true;
	for (auto _ : state) {
		mbstate_t to_state {};
		const from_char_t* input            = input_data.data();
		const from_char_t* const input_last = input_data.data() + input_data.size();
		to_char_t* output                   = output_data.data();
		to_char_t* output_last              = output_data.data() + output_data.size();
		for (;;) {
			if (input != input_last) {
				const std::size_t input_size = input_last - input;
				const size_t to_result       = ZTD_UCHAR_ACCESSOR_I_ mbrtoc32(output, input, input_size, &to_state);
				switch (to_result) {
				case static_cast<size_t>(-1):
					// encoding error: everyhting has failed. bail.
					result = false;
					goto loop_break;
				case static_cast<size_t>(-2):
					// this should never happen.... but it just might!!
					// loop back around to "accumulate" more data. If we do this twice, something is fucked.
					result = false;
					goto loop_break;
				case static_cast<size_t>(-3):
					output += 1;
					break;
				case 0:
					input += 1;
					output += 1;
					break;
				default:
					input += to_result;
					output += 1;
					break;
				}
			}
			else {
				// The C Standard is so fucking bad that you ACTUALLY cannot
				// finish processing text without having to check if you have cleaned out
				// the state. You have to call the function more to make sure
				// you have drained off all the surrogates...
				while (ZTD_UCHAR_ACCESSOR_I_ mbsinit(&to_state) == 0) {
					// try to drain the (last?) input out...
					to_char_t last_c;
					const size_t last_to_result = ZTD_UCHAR_ACCESSOR_I_ mbrtoc32(&last_c, "", 1, &to_state);
					if (last_to_result == static_cast<size_t>(-3)) {
						// we had extra, vomit it out...
						*output = last_c;
						++output;
					}
					else {
						break;
					}
				}
				break;
			}
		}
	loop_break:;
		if (output != output_last) {
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

static void utf16_to_utf8_well_formed_standard_c(benchmark::State& state) {
	using from_char_t = ::std::conditional_t<16 == 8, char, ztd_char16_t>;
	using to_char_t   = ::std::conditional_t<8 == 8, char, ztd_char8_t>;
	const std::vector<from_char_t> input_data(
	     c_span_char16_t_data(u16_data), c_span_char16_t_data(u16_data) + c_span_char16_t_size(u16_data));
	std::vector<to_char_t> output_data(c_span_char8_t_size(u8_data));
	bool result = true;
	for (auto _ : state) {
		mbstate_t from_state {};
		to_char_t* output                   = output_data.data();
		to_char_t* output_last              = output_data.data() + output_data.size();
		const from_char_t* input            = input_data.data();
		const from_char_t* const input_last = input_data.data() + input_data.size();
		for (; input != input_last;) {
			const size_t from_result = ZTD_UCHAR_ACCESSOR_I_ c16rtomb(output, *input, &from_state);
			switch (from_result) {
			case static_cast<size_t>(-1):
				// encoding error: everyhting has failed. bail.
				result = false;
				goto loop_break;
			default:
				output += from_result;
				input += 1;
				break;
			}
		}
	loop_break:;
		if (output != output_last) {
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

static void utf8_to_utf16_well_formed_standard_c(benchmark::State& state) {
	using from_char_t = ::std::conditional_t<8 == 8, char, ztd_char8_t>;
	using to_char_t   = ::std::conditional_t<16 == 8, char, ztd_char16_t>;
	const std::vector<from_char_t> input_data(
	     c_span_char8_t_data(u8_data), c_span_char8_t_data(u8_data) + c_span_char8_t_size(u8_data));
	std::vector<to_char_t> output_data(c_span_char16_t_size(u16_data));
	bool result = true;
	for (auto _ : state) {
		mbstate_t to_state {};
		const from_char_t* input            = input_data.data();
		const from_char_t* const input_last = input_data.data() + input_data.size();
		to_char_t* output                   = output_data.data();
		to_char_t* output_last              = output_data.data() + output_data.size();
		for (;;) {
			if (input != input_last) {
				const std::size_t input_size = input_last - input;
				const size_t to_result       = ZTD_UCHAR_ACCESSOR_I_ mbrtoc16(output, input, input_size, &to_state);
				switch (to_result) {
				case static_cast<size_t>(-1):
					// encoding error: everyhting has failed. bail.
					result = false;
					goto loop_break;
				case static_cast<size_t>(-2):
					// this should never happen.... but it just might!!
					// loop back around to "accumulate" more data. If we do this twice, something is fucked.
					result = false;
					goto loop_break;
				case static_cast<size_t>(-3):
					output += 1;
					break;
				case 0:
					input += 1;
					output += 1;
					break;
				default:
					input += to_result;
					output += 1;
					break;
				}
			}
			else {
				// The C Standard is so fucking bad that you ACTUALLY cannot
				// finish processing text without having to check if you have cleaned out
				// the state. You have to call the function more to make sure
				// you have drained off all the surrogates...
				while (ZTD_UCHAR_ACCESSOR_I_ mbsinit(&to_state) == 0) {
					// try to drain the (last?) input out...
					to_char_t last_c;
					const size_t last_to_result = ZTD_UCHAR_ACCESSOR_I_ mbrtoc16(&last_c, "", 1, &to_state);
					if (last_to_result == static_cast<size_t>(-3)) {
						// we had extra, vomit it out...
						*output = last_c;
						++output;
					}
					else {
						break;
					}
				}
				break;
			}
		}
	loop_break:;
		if (output != output_last) {
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

BENCHMARK(utf8_to_utf16_well_formed_standard_c);
BENCHMARK(utf8_to_utf32_well_formed_standard_c);

BENCHMARK(utf16_to_utf32_well_formed_standard_c);
BENCHMARK(utf16_to_utf8_well_formed_standard_c);

BENCHMARK(utf32_to_utf16_well_formed_standard_c);
BENCHMARK(utf32_to_utf8_well_formed_standard_c);

#endif // Only on not-Mac OSX with uchar and cuchar

#endif
