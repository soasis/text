// ============================================================================
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

#if ZTD_IS_ON(ZTD_TEXT_BENCHMARKS_CONVERSION_SPEED_SIMDUTF_BENCHMARKS)

#include <benchmark/benchmark.h>

#include <simdutf.h>

#include <ztd/idk/c_span.h>
#include <ztd/idk/endian.hpp>
#include <ztd/idk/charN_t.hpp>
#include <barrier/barrier.h>

#include <vector>

#define UTF_CONVERSION_BENCHMARK(FROM_N, TO_N, BIG_FROM_SUFFIX, LIL_FROM_SUFFIX, BIG_TO_SUFFIX, LIL_TO_SUFFIX)    \
	static void utf##FROM_N##_to_utf##TO_N##_well_formed_simdutf_unchecked(benchmark::State& state) {            \
		using from_char_t = ::std::conditional_t<FROM_N == 8, char, ztd_char##FROM_N##_t>;                      \
		using to_char_t   = ::std::conditional_t<TO_N == 8, char, ztd_char##TO_N##_t>;                          \
		const std::vector<ztd_char##FROM_N##_t> input_data(c_span_char##FROM_N##_t_data(u##FROM_N##_data),      \
		     c_span_char##FROM_N##_t_data(u##FROM_N##_data) + c_span_char##FROM_N##_t_size(u##FROM_N##_data));  \
		std::vector<ztd_char##TO_N##_t> output_data(c_span_char##TO_N##_t_size(u##TO_N##_data));                \
		bool result = true;                                                                                     \
		for (auto _ : state) {                                                                                  \
			size_t input_size        = input_data.size();                                                      \
			const from_char_t* input = (from_char_t*)input_data.data();                                        \
			size_t output_size       = output_data.size();                                                     \
			to_char_t* output        = (to_char_t*)output_data.data();                                         \
                                                                                                                  \
			size_t output_written = ztd::endian::native == ztd::endian::little                                 \
			     ? simdutf::convert_valid_utf##FROM_N##LIL_FROM_SUFFIX##_to_utf##TO_N##LIL_TO_SUFFIX(          \
			          input, input_size, output)                                                               \
			     : simdutf::convert_valid_utf##FROM_N##BIG_FROM_SUFFIX##_to_utf##TO_N##BIG_TO_SUFFIX(          \
			          input, input_size, output);                                                              \
			if (output_written != output_data.size()) {                                                        \
				result = false;                                                                               \
			}                                                                                                  \
		}                                                                                                       \
		const bool is_equal                                                                                     \
		     = std::equal(output_data.cbegin(), output_data.cend(), c_span_char##TO_N##_t_data(u##TO_N##_data), \
		          c_span_char##TO_N##_t_data(u##TO_N##_data) + c_span_char##TO_N##_t_size(u##TO_N##_data));     \
		if (!result) {                                                                                          \
			state.SkipWithError("conversion failed with an error");                                            \
		}                                                                                                       \
		else if (!is_equal) {                                                                                   \
			state.SkipWithError("conversion succeeded but produced illegitimate data");                        \
		}                                                                                                       \
	}                                                                                                            \
	static_assert(true, "")

UTF_CONVERSION_BENCHMARK(8, 16, , , be, le);
UTF_CONVERSION_BENCHMARK(8, 32, , , , );

UTF_CONVERSION_BENCHMARK(16, 8, be, le, , );
UTF_CONVERSION_BENCHMARK(16, 32, be, le, , );

UTF_CONVERSION_BENCHMARK(32, 8, , , , );
UTF_CONVERSION_BENCHMARK(32, 16, , , be, le);

#undef UTF_CONVERSION_BENCHMARK

BENCHMARK(utf8_to_utf16_well_formed_simdutf_unchecked);
BENCHMARK(utf8_to_utf32_well_formed_simdutf_unchecked);

BENCHMARK(utf16_to_utf8_well_formed_simdutf_unchecked);
BENCHMARK(utf16_to_utf32_well_formed_simdutf_unchecked);

BENCHMARK(utf32_to_utf8_well_formed_simdutf_unchecked);
BENCHMARK(utf32_to_utf16_well_formed_simdutf_unchecked);

#endif
