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

#if ZTD_IS_ON(ZTD_TEXT_BENCHMARKS_CONVERSION_SPEED_ZTD_TEXT_BENCHMARKS)

#include <benchmark/benchmark.h>

#include <ztd/text/benchmarks/fast_transcode.ztd.text.hpp>

#include <ztd/text.hpp>
#include <barrier/barrier.h>

#include <vector>

#define UTF_CONVERSION_BENCHMARK(FROM_N, TO_N)                                                                                \
	static void utf##FROM_N##_to_utf##TO_N##_well_formed_ztd_text(benchmark::State& state) {                                 \
		using from_char_t = ztd_char##FROM_N##_t;                                                                           \
		using to_char_t   = ztd_char##TO_N##_t;                                                                             \
		const std::vector<from_char_t> input_data(c_span_char##FROM_N##_t_data(u##FROM_N##_data),                           \
		     c_span_char##FROM_N##_t_data(u##FROM_N##_data) + c_span_char##FROM_N##_t_size(u##FROM_N##_data));              \
		std::vector<to_char_t> output_data(c_span_char##TO_N##_t_size(u##TO_N##_data));                                     \
		bool result = true;                                                                                                 \
		for (auto _ : state) {                                                                                              \
			size_t input_size        = input_data.size();                                                                  \
			const from_char_t* input = input_data.data();                                                                  \
			size_t output_size       = output_data.size();                                                                 \
			to_char_t* output        = output_data.data();                                                                 \
			auto err                 = ztd::text::transcode_into_raw(                                                      \
                    ztd::span(input, input_size), ztd::text::utf##TO_N, ztd::span(output, output_size));      \
			if (err.error_code != ztd::text::encoding_error::ok) {                                                         \
				result = false;                                                                                           \
			}                                                                                                              \
		}                                                                                                                   \
		const bool is_equal                                                                                                 \
		     = std::equal(output_data.cbegin(), output_data.cend(), c_span_char##TO_N##_t_data(u##TO_N##_data),             \
		          c_span_char##TO_N##_t_data(u##TO_N##_data) + c_span_char##TO_N##_t_size(u##TO_N##_data));                 \
		if (!result) {                                                                                                      \
			state.SkipWithError("conversion failed with an error");                                                        \
		}                                                                                                                   \
		else if (!is_equal) {                                                                                               \
			state.SkipWithError("conversion succeeded but produced illegitimate data");                                    \
		}                                                                                                                   \
	}                                                                                                                        \
                                                                                                                              \
	static void utf##FROM_N##_to_utf##TO_N##_well_formed_ztd_text_single(benchmark::State& state) {                          \
		using from_char_t = ztd_char##FROM_N##_t;                                                                           \
		using to_char_t   = ztd_char##TO_N##_t;                                                                             \
		const std::vector<from_char_t> input_data(c_span_char##FROM_N##_t_data(u##FROM_N##_data),                           \
		     c_span_char##FROM_N##_t_data(u##FROM_N##_data) + c_span_char##FROM_N##_t_size(u##FROM_N##_data));              \
		std::vector<to_char_t> output_data(c_span_char##TO_N##_t_size(u##TO_N##_data));                                     \
		bool result = true;                                                                                                 \
		for (auto _ : state) {                                                                                              \
			size_t input_size            = input_data.size();                                                              \
			const from_char_t* input_ptr = input_data.data();                                                              \
			size_t output_size           = output_data.size();                                                             \
			to_char_t* output_ptr        = output_data.data();                                                             \
			auto input                   = ztd::span(input_ptr, input_size);                                               \
			auto output                  = ztd::span(output_ptr, output_size);                                             \
			for (; !input.empty();) {                                                                                      \
				auto err = ztd::text::transcode_one_into(input, ztd::text::utf##TO_N, output);                            \
				if (err.error_code != ztd::text::encoding_error::ok) {                                                    \
					result = false;                                                                                      \
					break;                                                                                               \
				}                                                                                                         \
				input  = ::std::move(err.input);                                                                          \
				output = ::std::move(err.output);                                                                         \
			}                                                                                                              \
		}                                                                                                                   \
		const bool is_equal                                                                                                 \
		     = std::equal(output_data.cbegin(), output_data.cend(), c_span_char##TO_N##_t_data(u##TO_N##_data),             \
		          c_span_char##TO_N##_t_data(u##TO_N##_data) + c_span_char##TO_N##_t_size(u##TO_N##_data));                 \
		if (!result) {                                                                                                      \
			state.SkipWithError("conversion failed with an error");                                                        \
		}                                                                                                                   \
		else if (!is_equal) {                                                                                               \
			state.SkipWithError("conversion succeeded but produced illegitimate data");                                    \
		}                                                                                                                   \
	}                                                                                                                        \
                                                                                                                              \
	static void utf##FROM_N##_to_utf##TO_N##_well_formed_ztd_text_unbounded(benchmark::State& state) {                       \
		using from_char_t = ztd_char##FROM_N##_t;                                                                           \
		using to_char_t   = ztd_char##TO_N##_t;                                                                             \
		const std::vector<from_char_t> input_data(c_span_char##FROM_N##_t_data(u##FROM_N##_data),                           \
		     c_span_char##FROM_N##_t_data(u##FROM_N##_data) + c_span_char##FROM_N##_t_size(u##FROM_N##_data));              \
		std::vector<to_char_t> output_data(c_span_char##TO_N##_t_size(u##TO_N##_data));                                     \
		bool result = true;                                                                                                 \
		for (auto _ : state) {                                                                                              \
			size_t input_size        = input_data.size();                                                                  \
			const from_char_t* input = input_data.data();                                                                  \
			to_char_t* output        = output_data.data();                                                                 \
			auto err                 = ztd::text::transcode_into(                                                          \
                    ztd::span(input, input_size), ztd::text::utf##TO_N, ztd::ranges::unbounded_view(output)); \
			if (err.error_code != ztd::text::encoding_error::ok) {                                                         \
				result = false;                                                                                           \
			}                                                                                                              \
		}                                                                                                                   \
		const bool is_equal                                                                                                 \
		     = std::equal(output_data.cbegin(), output_data.cend(), c_span_char##TO_N##_t_data(u##TO_N##_data),             \
		          c_span_char##TO_N##_t_data(u##TO_N##_data) + c_span_char##TO_N##_t_size(u##TO_N##_data));                 \
		if (!result) {                                                                                                      \
			state.SkipWithError("conversion failed with an error");                                                        \
		}                                                                                                                   \
		else if (!is_equal) {                                                                                               \
			state.SkipWithError("conversion succeeded but produced illegitimate data");                                    \
		}                                                                                                                   \
	}                                                                                                                        \
                                                                                                                              \
	static void utf##FROM_N##_to_utf##TO_N##_well_formed_ztd_text_single_unbounded(benchmark::State& state) {                \
		using from_char_t = ztd_char##FROM_N##_t;                                                                           \
		using to_char_t   = ztd_char##TO_N##_t;                                                                             \
		const std::vector<from_char_t> input_data(c_span_char##FROM_N##_t_data(u##FROM_N##_data),                           \
		     c_span_char##FROM_N##_t_data(u##FROM_N##_data) + c_span_char##FROM_N##_t_size(u##FROM_N##_data));              \
		std::vector<to_char_t> output_data(c_span_char##TO_N##_t_size(u##TO_N##_data));                                     \
		bool result = true;                                                                                                 \
		for (auto _ : state) {                                                                                              \
			size_t input_size            = input_data.size();                                                              \
			const from_char_t* input_ptr = input_data.data();                                                              \
			to_char_t* output_ptr        = output_data.data();                                                             \
			auto input                   = ztd::span(input_ptr, input_size);                                               \
			auto output                  = ztd::ranges::unbounded_view(output_ptr);                                        \
			for (; !input.empty();) {                                                                                      \
				auto err = ztd::text::transcode_one_into(input, ztd::text::utf##TO_N, output);                            \
				if (err.error_code != ztd::text::encoding_error::ok) {                                                    \
					result = false;                                                                                      \
					break;                                                                                               \
				}                                                                                                         \
				input  = ::std::move(err.input);                                                                          \
				output = ::std::move(err.output);                                                                         \
			}                                                                                                              \
		}                                                                                                                   \
		const bool is_equal                                                                                                 \
		     = std::equal(output_data.cbegin(), output_data.cend(), c_span_char##TO_N##_t_data(u##TO_N##_data),             \
		          c_span_char##TO_N##_t_data(u##TO_N##_data) + c_span_char##TO_N##_t_size(u##TO_N##_data));                 \
		if (!result) {                                                                                                      \
			state.SkipWithError("conversion failed with an error");                                                        \
		}                                                                                                                   \
		else if (!is_equal) {                                                                                               \
			state.SkipWithError("conversion succeeded but produced illegitimate data");                                    \
		}                                                                                                                   \
	}                                                                                                                        \
                                                                                                                              \
	static void utf##FROM_N##_to_utf##TO_N##_well_formed_ztd_text_view(benchmark::State& state) {                            \
		using from_char_t = ztd_char##FROM_N##_t;                                                                           \
		using to_char_t   = ztd_char##TO_N##_t;                                                                             \
		const std::vector<from_char_t> input_data(c_span_char##FROM_N##_t_data(u##FROM_N##_data),                           \
		     c_span_char##FROM_N##_t_data(u##FROM_N##_data) + c_span_char##FROM_N##_t_size(u##FROM_N##_data));              \
		std::vector<to_char_t> output_data(c_span_char##TO_N##_t_size(u##TO_N##_data));                                     \
		const ::std::size_t output_size = output_data.size();                                                               \
		bool result                     = true;                                                                             \
		for (auto _ : state) {                                                                                              \
			ztd::text::transcode_view<ztd::text::utf##FROM_N##_t, ztd::text::utf##TO_N##_t,                                \
			     ::ztd::span<const from_char_t>>                                                                           \
			     view(input_data);                                                                                         \
			auto output = output_data.data();                                                                              \
			for (const to_char_t value : view) {                                                                           \
				*output = value;                                                                                          \
				++output;                                                                                                 \
			}                                                                                                              \
			if (output != (output_data.data() + output_size)) {                                                            \
				result = false;                                                                                           \
			}                                                                                                              \
		}                                                                                                                   \
		const bool is_equal                                                                                                 \
		     = std::equal(output_data.cbegin(), output_data.cend(), c_span_char##TO_N##_t_data(u##TO_N##_data),             \
		          c_span_char##TO_N##_t_data(u##TO_N##_data) + c_span_char##TO_N##_t_size(u##TO_N##_data));                 \
		if (!result) {                                                                                                      \
			state.SkipWithError("conversion failed with an error");                                                        \
		}                                                                                                                   \
		else if (!is_equal) {                                                                                               \
			state.SkipWithError("conversion succeeded but produced illegitimate data");                                    \
		}                                                                                                                   \
	}                                                                                                                        \
	static_assert(true, "")

// UTF_CONVERSION_BENCHMARK(8, 16);
// UTF_CONVERSION_BENCHMARK(16, 8);

UTF_CONVERSION_BENCHMARK(8, 32);
UTF_CONVERSION_BENCHMARK(32, 8);

UTF_CONVERSION_BENCHMARK(16, 32);
UTF_CONVERSION_BENCHMARK(32, 16);

#undef UTF_CONVERSION_BENCHMARK

BENCHMARK(utf8_to_utf32_well_formed_ztd_text);
BENCHMARK(utf8_to_utf32_well_formed_ztd_text_unbounded);
BENCHMARK(utf8_to_utf32_well_formed_ztd_text_single);
BENCHMARK(utf8_to_utf32_well_formed_ztd_text_single_unbounded);
BENCHMARK(utf8_to_utf32_well_formed_ztd_text_view);
BENCHMARK(utf32_to_utf8_well_formed_ztd_text);
BENCHMARK(utf32_to_utf8_well_formed_ztd_text_unbounded);
BENCHMARK(utf32_to_utf8_well_formed_ztd_text_single);
BENCHMARK(utf32_to_utf8_well_formed_ztd_text_single_unbounded);
BENCHMARK(utf32_to_utf8_well_formed_ztd_text_view);

BENCHMARK(utf16_to_utf32_well_formed_ztd_text);
BENCHMARK(utf16_to_utf32_well_formed_ztd_text_single);
BENCHMARK(utf16_to_utf32_well_formed_ztd_text_unbounded);
BENCHMARK(utf16_to_utf32_well_formed_ztd_text_single_unbounded);
BENCHMARK(utf16_to_utf32_well_formed_ztd_text_view);
BENCHMARK(utf32_to_utf16_well_formed_ztd_text);
BENCHMARK(utf32_to_utf16_well_formed_ztd_text_single);
BENCHMARK(utf32_to_utf16_well_formed_ztd_text_unbounded);
BENCHMARK(utf32_to_utf16_well_formed_ztd_text_single_unbounded);
BENCHMARK(utf32_to_utf16_well_formed_ztd_text_view);

#endif
