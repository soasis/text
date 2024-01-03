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

#if ZTD_IS_ON(ZTD_TEXT_BENCHMARKS_CONVERSION_SPEED_BOOST_TEXT_BENCHMARKS)

#include <benchmark/benchmark.h>

#include <ztd/cuneicode.h>
#include <barrier/barrier.h>
#if ZTD_IS_ON(ZTD_COMPILER_CLANG) || ZTD_IS_ON(ZTD_COMPILER_GCC)
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wunused-variable"
#pragma clang diagnostic ignored "-Wunused-parameter"
#endif
#include <boost/text/text.hpp>
#if ZTD_IS_ON(ZTD_COMPILER_CLANG) || ZTD_IS_ON(ZTD_COMPILER_GCC)
#pragma clang diagnostic pop
#endif

#include <vector>
#include <algorithm>
#include <iostream>

#define UTF_CONVERSION_BENCHMARK(FROM_N, TO_N)                                                                       \
	static void utf##FROM_N##_to_utf##TO_N##_well_formed_boost_text(benchmark::State& state) {                      \
		using from_char_t = ::std::conditional_t<FROM_N == 8, char, ztd_char##FROM_N##_t>;                         \
		using to_char_t   = ::std::conditional_t<TO_N == 8, char, ztd_char##TO_N##_t>;                             \
		const std::vector<from_char_t> input_data(c_span_char##FROM_N##_t_data(u##FROM_N##_data),                  \
		     c_span_char##FROM_N##_t_data(u##FROM_N##_data) + c_span_char##FROM_N##_t_size(u##FROM_N##_data));     \
		std::vector<to_char_t> output_data(c_span_char##TO_N##_t_size(u##TO_N##_data));                            \
		bool result = true;                                                                                        \
		for (auto _ : state) {                                                                                     \
			const size_t input_size  = input_data.size();                                                         \
			const from_char_t* input = input_data.data();                                                         \
			const size_t output_size = output_data.size();                                                        \
			to_char_t* output        = output_data.data();                                                        \
			auto position            = boost::text::transcode_to_utf##TO_N(input, input + input_size, output);    \
			if (position.in != (input + input_size) || position.out != (output + output_size)) {                  \
				result = false;                                                                                  \
			}                                                                                                     \
		}                                                                                                          \
		const bool is_equal = std::equal(output_data.cbegin(), output_data.cend(),                                 \
		     (to_char_t*)c_span_char##TO_N##_t_data(u##TO_N##_data),                                               \
		     (to_char_t*)c_span_char##TO_N##_t_data(u##TO_N##_data) + c_span_char##TO_N##_t_size(u##TO_N##_data)); \
		if (!result) {                                                                                             \
			state.SkipWithError("conversion failed with an error");                                               \
		}                                                                                                          \
		else if (!is_equal) {                                                                                      \
			state.SkipWithError("conversion succeeded but produced illegitimate data");                           \
			return;                                                                                               \
		}                                                                                                          \
	}                                                                                                               \
                                                                                                                     \
	static void utf##FROM_N##_to_utf##TO_N##_well_formed_boost_text_view(benchmark::State& state) {                 \
		using from_char_t = ::std::conditional_t<FROM_N == 8, char, ztd_char##FROM_N##_t>;                         \
		using to_char_t   = ::std::conditional_t<TO_N == 8, char, ztd_char##TO_N##_t>;                             \
		const std::vector<from_char_t> input_data(c_span_char##FROM_N##_t_data(u##FROM_N##_data),                  \
		     c_span_char##FROM_N##_t_data(u##FROM_N##_data) + c_span_char##FROM_N##_t_size(u##FROM_N##_data));     \
		std::vector<to_char_t> output_data(c_span_char##TO_N##_t_size(u##TO_N##_data));                            \
		bool result = true;                                                                                        \
		for (auto _ : state) {                                                                                     \
			const size_t input_size  = input_data.size();                                                         \
			const from_char_t* input = input_data.data();                                                         \
			const size_t output_size = output_data.size();                                                        \
			to_char_t* output        = output_data.data();                                                        \
			auto view                = boost::text::as_utf##TO_N(input, input + input_size);                      \
			for (const to_char_t value : view) {                                                                  \
				*output = value;                                                                                 \
				++output;                                                                                        \
			}                                                                                                     \
			if (output != (output_data.data() + output_size)) {                                                   \
				result = false;                                                                                  \
			}                                                                                                     \
		}                                                                                                          \
		const bool is_equal = std::equal(output_data.cbegin(), output_data.cend(),                                 \
		     (to_char_t*)c_span_char##TO_N##_t_data(u##TO_N##_data),                                               \
		     (to_char_t*)c_span_char##TO_N##_t_data(u##TO_N##_data) + c_span_char##TO_N##_t_size(u##TO_N##_data)); \
		if (!result) {                                                                                             \
			state.SkipWithError("conversion failed with an error");                                               \
		}                                                                                                          \
		else if (!is_equal) {                                                                                      \
			/*const auto mismatch = std::mismatch(output_data.cbegin(), output_data.cend(),                       \
			     (to_char_t*)c_span_char##TO_N##_t_data(u##TO_N##_data),                                          \
			     (to_char_t*)c_span_char##TO_N##_t_data(u##TO_N##_data)                                           \
			          + c_span_char##TO_N##_t_size(u##TO_N##_data));                                              \
			std::cout << "Diff:" << std::endl                                                                     \
			          << "\t" << (mismatch.first - output_data.cbegin()) << std::endl                             \
			          << "\t"                                                                                     \
			          << (mismatch.second                                                                         \
			                  - (to_char_t*)c_span_char##TO_N##_t_data(u##TO_N##_data))                           \
			          << std::endl;*/                                                                             \
			state.SkipWithError("conversion succeeded but produced illegitimate data");                           \
			return;                                                                                               \
		}                                                                                                          \
	}                                                                                                               \
	static_assert(true, "")

UTF_CONVERSION_BENCHMARK(8, 16);
UTF_CONVERSION_BENCHMARK(8, 32);

UTF_CONVERSION_BENCHMARK(16, 8);
UTF_CONVERSION_BENCHMARK(16, 32);

UTF_CONVERSION_BENCHMARK(32, 8);
UTF_CONVERSION_BENCHMARK(32, 16);

#undef UTF_CONVERSION_BENCHMARK

BENCHMARK(utf16_to_utf32_well_formed_boost_text);
BENCHMARK(utf16_to_utf32_well_formed_boost_text_view);
BENCHMARK(utf32_to_utf16_well_formed_boost_text);
BENCHMARK(utf32_to_utf16_well_formed_boost_text_view);

BENCHMARK(utf8_to_utf32_well_formed_boost_text);
BENCHMARK(utf8_to_utf32_well_formed_boost_text_view);
BENCHMARK(utf32_to_utf8_well_formed_boost_text);
BENCHMARK(utf32_to_utf8_well_formed_boost_text_view);

BENCHMARK(utf16_to_utf8_well_formed_boost_text);
BENCHMARK(utf16_to_utf8_well_formed_boost_text_view);
BENCHMARK(utf8_to_utf16_well_formed_boost_text);
BENCHMARK(utf8_to_utf16_well_formed_boost_text_view);

#endif
