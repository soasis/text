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

#if ZTD_IS_ON(ZTD_TEXT_BENCHMARKS_CONVERSION_SPEED_CUNEICODE_BASIC_BENCHMARKS)

#include <benchmark/benchmark.h>

#include <ztd/cuneicode.h>
#include <barrier/barrier.h>

#include <vector>

#define UTF_CONVERSION_BENCHMARK(FROM_N, TO_N)                                                                      \
	static void utf##FROM_N##_to_utf##TO_N##_well_formed_cuneicode_single(benchmark::State& state) {               \
		const std::vector<ztd_char##FROM_N##_t> input_data(c_span_char##FROM_N##_t_data(u##FROM_N##_data),        \
		     c_span_char##FROM_N##_t_data(u##FROM_N##_data) + c_span_char##FROM_N##_t_size(u##FROM_N##_data));    \
		std::vector<ztd_char##TO_N##_t> output_data(c_span_char##TO_N##_t_size(u##TO_N##_data));                  \
		bool result = true;                                                                                       \
		for (auto _ : state) {                                                                                    \
			size_t input_size                 = input_data.size();                                               \
			const ztd_char##FROM_N##_t* input = input_data.data();                                               \
			size_t output_size                = output_data.size();                                              \
			ztd_char##TO_N##_t* output        = output_data.data();                                              \
			cnc_mcstate_t state               = {};                                                              \
			for (const ztd_char##FROM_N##_t* const input_last = input + input_size;                              \
			     input != input_last && !cnc_mcstate_is_complete(&state);) {                                     \
				cnc_mcerr err                                                                                 \
				     = cnc_c##FROM_N##nrtoc##TO_N##n(&output_size, &output, &input_size, &input, &state);       \
				if (err != cnc_mcerr_ok) {                                                                    \
					result = false;                                                                            \
					break;                                                                                     \
				}                                                                                               \
			}                                                                                                    \
		}                                                                                                         \
		const bool is_equal                                                                                       \
		     = std::equal(output_data.cbegin(), output_data.cend(), c_span_char##TO_N##_t_data(u##TO_N##_data),   \
		          c_span_char##TO_N##_t_data(u##TO_N##_data) + c_span_char##TO_N##_t_size(u##TO_N##_data));       \
		if (!result) {                                                                                            \
			state.SkipWithError("conversion failed with an error");                                              \
		}                                                                                                         \
		else if (!is_equal) {                                                                                     \
			state.SkipWithError("conversion succeeded but produced illegitimate data");                          \
		}                                                                                                         \
	}                                                                                                              \
                                                                                                                    \
	static void utf##FROM_N##_to_utf##TO_N##_well_formed_cuneicode_single_unbounded(benchmark::State& state) {     \
		const std::vector<ztd_char##FROM_N##_t> input_data(c_span_char##FROM_N##_t_data(u##FROM_N##_data),        \
		     c_span_char##FROM_N##_t_data(u##FROM_N##_data) + c_span_char##FROM_N##_t_size(u##FROM_N##_data));    \
		std::vector<ztd_char##TO_N##_t> output_data(c_span_char##TO_N##_t_size(u##TO_N##_data));                  \
		bool result = true;                                                                                       \
		for (auto _ : state) {                                                                                    \
			size_t input_size                 = input_data.size();                                               \
			const ztd_char##FROM_N##_t* input = input_data.data();                                               \
			ztd_char##TO_N##_t* output        = output_data.data();                                              \
			cnc_mcstate_t state               = {};                                                              \
			for (const ztd_char##FROM_N##_t* const input_last = input + input_size;                              \
			     input != input_last && !cnc_mcstate_is_complete(&state);) {                                     \
				cnc_mcerr err = cnc_c##FROM_N##nrtoc##TO_N##n(nullptr, &output, &input_size, &input, &state); \
				if (err != cnc_mcerr_ok) {                                                                    \
					result = false;                                                                            \
					break;                                                                                     \
				}                                                                                               \
			}                                                                                                    \
		}                                                                                                         \
		const bool is_equal                                                                                       \
		     = std::equal(output_data.cbegin(), output_data.cend(), c_span_char##TO_N##_t_data(u##TO_N##_data),   \
		          c_span_char##TO_N##_t_data(u##TO_N##_data) + c_span_char##TO_N##_t_size(u##TO_N##_data));       \
		if (!result) {                                                                                            \
			state.SkipWithError("conversion failed with an error");                                              \
		}                                                                                                         \
		else if (!is_equal) {                                                                                     \
			state.SkipWithError("conversion succeeded but produced illegitimate data");                          \
		}                                                                                                         \
	}                                                                                                              \
	static_assert(true, "")

UTF_CONVERSION_BENCHMARK(8, 16);
UTF_CONVERSION_BENCHMARK(16, 8);

UTF_CONVERSION_BENCHMARK(8, 32);
UTF_CONVERSION_BENCHMARK(32, 8);

UTF_CONVERSION_BENCHMARK(16, 32);
UTF_CONVERSION_BENCHMARK(32, 16);

#undef UTF_CONVERSION_BENCHMARK

BENCHMARK(utf8_to_utf16_well_formed_cuneicode_single);
BENCHMARK(utf8_to_utf16_well_formed_cuneicode_single_unbounded);
BENCHMARK(utf16_to_utf8_well_formed_cuneicode_single);
BENCHMARK(utf16_to_utf8_well_formed_cuneicode_single_unbounded);

BENCHMARK(utf8_to_utf32_well_formed_cuneicode_single);
BENCHMARK(utf8_to_utf32_well_formed_cuneicode_single_unbounded);
BENCHMARK(utf32_to_utf8_well_formed_cuneicode_single);
BENCHMARK(utf32_to_utf8_well_formed_cuneicode_single_unbounded);

BENCHMARK(utf16_to_utf32_well_formed_cuneicode_single);
BENCHMARK(utf16_to_utf32_well_formed_cuneicode_single_unbounded);
BENCHMARK(utf32_to_utf16_well_formed_cuneicode_single);
BENCHMARK(utf32_to_utf16_well_formed_cuneicode_single_unbounded);

#endif
