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

#include <benchmark/benchmark.h>

#include <ztd/idk/c_span.h>
#include <ztd/idk/endian.hpp>
#include <ztd/idk/charN_t.hpp>
#include <barrier/barrier.h>

#include <cstring>
#include <vector>

#define UTF_CONVERSION_BENCHMARK(FROM_N, TO_N)                                                                    \
	static void utf##FROM_N##_to_utf##TO_N##_well_formed_memcpy(benchmark::State& state) {                       \
		using to_char_t = ztd_char##TO_N##_t;                                                                   \
		std::vector<to_char_t> output_data(c_span_char##TO_N##_t_size(u##TO_N##_data));                         \
		bool result = true;                                                                                     \
		for (auto _ : state) {                                                                                  \
			std::memcpy(output_data.data(), c_span_char##TO_N##_t_data(u##TO_N##_data),                        \
			     output_data.size() * sizeof(*output_data.data()));                                            \
		}                                                                                                       \
		const bool is_equal                                                                                     \
		     = std::equal(output_data.cbegin(), output_data.cend(), c_span_char##TO_N##_t_data(u##TO_N##_data), \
		          c_span_char##TO_N##_t_data(u##TO_N##_data) + c_span_char##TO_N##_t_size(u##TO_N##_data));     \
		if (!result) {                                                                                          \
			state.SkipWithError("memcmp failed somehow???");                                                   \
		}                                                                                                       \
	}                                                                                                            \
	static void utf##FROM_N##_to_utf##TO_N##_init_well_formed_memcpy(benchmark::State& state) {                  \
		using to_char_t = ztd_char##TO_N##_t;                                                                   \
		std::vector<to_char_t> output_data(c_span_char##TO_N##_t_size(u##TO_N##_data));                         \
		bool result = true;                                                                                     \
		for (auto _ : state) {                                                                                  \
			std::memcpy(output_data.data(), c_span_char##TO_N##_t_data(u##TO_N##_data),                        \
			     output_data.size() * sizeof(*output_data.data()));                                            \
		}                                                                                                       \
		const bool is_equal                                                                                     \
		     = std::equal(output_data.cbegin(), output_data.cend(), c_span_char##TO_N##_t_data(u##TO_N##_data), \
		          c_span_char##TO_N##_t_data(u##TO_N##_data) + c_span_char##TO_N##_t_size(u##TO_N##_data));     \
		if (!result) {                                                                                          \
			state.SkipWithError("memcmp failed somehow???");                                                   \
		}                                                                                                       \
	}                                                                                                            \
	static void utf##FROM_N##_to_utf##TO_N##_well_formed_unchecked_memcpy(benchmark::State& state) {             \
		using to_char_t = ztd_char##TO_N##_t;                                                                   \
		std::vector<to_char_t> output_data(c_span_char##TO_N##_t_size(u##TO_N##_data));                         \
		bool result = true;                                                                                     \
		for (auto _ : state) {                                                                                  \
			std::memcpy(output_data.data(), c_span_char##TO_N##_t_data(u##TO_N##_data),                        \
			     output_data.size() * sizeof(*output_data.data()));                                            \
		}                                                                                                       \
		const bool is_equal                                                                                     \
		     = std::equal(output_data.cbegin(), output_data.cend(), c_span_char##TO_N##_t_data(u##TO_N##_data), \
		          c_span_char##TO_N##_t_data(u##TO_N##_data) + c_span_char##TO_N##_t_size(u##TO_N##_data));     \
		if (!result) {                                                                                          \
			state.SkipWithError("memcmp failed somehow???");                                                   \
		}                                                                                                       \
	}                                                                                                            \
	static_assert(true, "")

UTF_CONVERSION_BENCHMARK(8, 8);
UTF_CONVERSION_BENCHMARK(16, 16);
UTF_CONVERSION_BENCHMARK(32, 32);

UTF_CONVERSION_BENCHMARK(8, 16);
UTF_CONVERSION_BENCHMARK(16, 8);

UTF_CONVERSION_BENCHMARK(32, 16);
UTF_CONVERSION_BENCHMARK(16, 32);

UTF_CONVERSION_BENCHMARK(8, 32);
UTF_CONVERSION_BENCHMARK(32, 8);

#undef UTF_CONVERSION_BENCHMARK

BENCHMARK(utf8_to_utf8_well_formed_memcpy);
BENCHMARK(utf16_to_utf16_well_formed_memcpy);
BENCHMARK(utf32_to_utf32_well_formed_memcpy);

BENCHMARK(utf8_to_utf16_well_formed_memcpy);
BENCHMARK(utf8_to_utf32_well_formed_memcpy);
BENCHMARK(utf16_to_utf32_well_formed_memcpy);
BENCHMARK(utf16_to_utf8_well_formed_memcpy);
BENCHMARK(utf32_to_utf16_well_formed_memcpy);
BENCHMARK(utf32_to_utf8_well_formed_memcpy);

BENCHMARK(utf8_to_utf16_init_well_formed_memcpy);
BENCHMARK(utf8_to_utf32_init_well_formed_memcpy);
BENCHMARK(utf16_to_utf32_init_well_formed_memcpy);
BENCHMARK(utf16_to_utf8_init_well_formed_memcpy);
BENCHMARK(utf32_to_utf16_init_well_formed_memcpy);
BENCHMARK(utf32_to_utf8_init_well_formed_memcpy);
