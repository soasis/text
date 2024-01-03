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

#if ZTD_IS_ON(ZTD_TEXT_BENCHMARKS_CONVERSION_SPEED_CUNEICODE_BASIC_UNCHECKED_BENCHMARKS)

#include <benchmark/benchmark.h>

#include <ztd/cuneicode.h>
#include <ztd/cuneicode/shared/simdutf/registry.hpp>
#include <ztd/idk/size.hpp>
#include <barrier/barrier.h>

#include <ztd/text/benchmarks/cuneicode_helpers.hpp>

#include <vector>

#define UTF_CONVERSION_BENCHMARK(FROM_N, TO_N)                                                                         \
	template <bool Fast, bool Unbounded>                                                                              \
	static void utf##FROM_N##_to_utf##TO_N##__well_formed_cuneicode_registry_single_unchecked_core(                   \
	     benchmark::State& state) {                                                                                   \
		const auto input_begin = c_span_char##FROM_N##_t_data(u##FROM_N##_data);                                     \
		const auto input_end                                                                                         \
		     = c_span_char##FROM_N##_t_data(u##FROM_N##_data) + c_span_char##FROM_N##_t_size(u##FROM_N##_data);      \
		const auto expected_begin = c_span_char##TO_N##_t_data(u##TO_N##_data);                                      \
		const auto expected_end                                                                                      \
		     = c_span_char##TO_N##_t_data(u##TO_N##_data) + c_span_char##TO_N##_t_size(u##TO_N##_data);              \
		const auto expected_size = c_span_char##TO_N##_t_size(u##TO_N##_data);                                       \
		const std::vector<ztd_char##FROM_N##_t> input_data(input_begin, input_end);                                  \
		std::vector<ztd_char##TO_N##_t> output_data(expected_size);                                                  \
		/* create registry to use, and conversion descriptor too */                                                  \
		std::unique_ptr<cnc_conversion_registry, registry_deleter> registry = nullptr;                               \
		std::unique_ptr<cnc_conversion, conversion_deleter> conversion      = nullptr;                               \
		{                                                                                                            \
			cnc_conversion_registry* raw_registry = nullptr;                                                        \
			cnc_conversion* raw_conversion        = nullptr;                                                        \
			cnc_conversion_info info              = {};                                                             \
			const cnc_open_err err                = cnc_registry_new(&raw_registry, cnc_registry_options_none);     \
			if (err != cnc_open_err_ok) {                                                                           \
				/* something went wrong, get out of here quick! */                                                 \
				state.SkipWithError("could not open registry");                                                    \
				return;                                                                                            \
			}                                                                                                       \
			registry.reset(raw_registry);                                                                           \
			if constexpr (Fast) {                                                                                   \
				if (!cnc_shared_add_simdutf_to_registry(registry.get())) {                                         \
					/* something went wrong, get out of here quick! */                                            \
					state.SkipWithError("could not add conversions to registry");                                 \
					return;                                                                                       \
				}                                                                                                  \
			}                                                                                                       \
			const cnc_open_err conv_err                                                                             \
			     = cnc_conv_new_c8(registry.get(), (const ztd_char8_t*)u8"UTF-" #FROM_N "-unchecked",               \
			          (const ztd_char8_t*)u8"UTF-" #TO_N "-unchecked", &raw_conversion, &info);                     \
			if (conv_err != cnc_open_err_ok) {                                                                      \
				/* something went wrong, get out of here quick! */                                                 \
				state.SkipWithError("could not open conversion descriptor");                                       \
				return;                                                                                            \
			}                                                                                                       \
			conversion.reset(raw_conversion);                                                                       \
		}                                                                                                            \
		bool result = true;                                                                                          \
		for (auto _ : state) {                                                                                       \
			size_t input_size                   = input_data.size() * sizeof(*input_data.data());                   \
			const unsigned char* input          = (const unsigned char*)input_data.data();                          \
			[[maybe_unused]] size_t output_size = output_data.size() * sizeof(*output_data.data());                 \
			unsigned char* output               = (unsigned char*)output_data.data();                               \
			for (; input_size > 0 || !cnc_conv_state_is_complete(conversion.get());) {                              \
				cnc_mcerr err = cnc_conv_one(                                                                      \
				     conversion.get(), Unbounded ? nullptr : &output_size, &output, &input_size, &input);          \
				if (err != cnc_mcerr_ok) {                                                                         \
					result = false;                                                                               \
					break;                                                                                        \
				}                                                                                                  \
			}                                                                                                       \
		}                                                                                                            \
		const auto mismatch_result                                                                                   \
		     = std::mismatch(output_data.cbegin(), output_data.cend(), expected_begin, expected_end);                \
		const bool is_equal = mismatch_result.first == output_data.cend() && mismatch_result.second == expected_end; \
		if (!result) {                                                                                               \
			if (!state.skipped()) {                                                                                 \
				state.SkipWithError("conversion failed with an error");                                            \
			}                                                                                                       \
		}                                                                                                            \
		else if (!is_equal) {                                                                                        \
			if (!state.skipped()) {                                                                                 \
				state.SkipWithError("conversion succeeded but produced illegitimate data at index "                \
				     + std::to_string(mismatch_result.second - expected_begin));                                   \
			}                                                                                                       \
		}                                                                                                            \
	}                                                                                                                 \
                                                                                                                       \
	static void utf##FROM_N##_to_utf##TO_N##_well_formed_cuneicode_registry_single_unchecked(                         \
	     benchmark::State& state) {                                                                                   \
		utf##FROM_N##_to_utf##TO_N##__well_formed_cuneicode_registry_single_unchecked_core<true, false>(state);      \
	}                                                                                                                 \
	static void utf##FROM_N##_to_utf##TO_N##_well_formed_cuneicode_registry_single_unchecked_unbounded(               \
	     benchmark::State& state) {                                                                                   \
		utf##FROM_N##_to_utf##TO_N##__well_formed_cuneicode_registry_single_unchecked_core<true, true>(state);       \
	}                                                                                                                 \
                                                                                                                       \
	template <bool Fast, bool Unbounded>                                                                              \
	static void utf##FROM_N##_to_utf##TO_N##_init_well_formed_cuneicode_registry_single_unchecked_core(               \
	     benchmark::State& state) {                                                                                   \
		const auto input_begin = c_span_char##FROM_N##_t_data(u##FROM_N##_data);                                     \
		const auto input_end                                                                                         \
		     = c_span_char##FROM_N##_t_data(u##FROM_N##_data) + c_span_char##FROM_N##_t_size(u##FROM_N##_data);      \
		const auto expected_begin = c_span_char##TO_N##_t_data(u##TO_N##_data);                                      \
		const auto expected_end                                                                                      \
		     = c_span_char##TO_N##_t_data(u##TO_N##_data) + c_span_char##TO_N##_t_size(u##TO_N##_data);              \
		const auto expected_size = c_span_char##TO_N##_t_size(u##TO_N##_data);                                       \
		const std::vector<ztd_char##FROM_N##_t> input_data(input_begin, input_end);                                  \
		std::vector<ztd_char##TO_N##_t> output_data(expected_size);                                                  \
		bool result = true;                                                                                          \
		std::pmr::monotonic_buffer_resource mbr(static_cast<std::size_t>(4096));                                     \
		cnc_conversion_heap mbr_heap = create_monotonic_buffer_heap(mbr);                                            \
		for (auto _ : state) {                                                                                       \
			/* create registry to use, and conversion descriptor too */                                             \
			std::unique_ptr<cnc_conversion_registry, registry_deleter> registry = nullptr;                          \
			std::unique_ptr<cnc_conversion, conversion_deleter> conversion      = nullptr;                          \
			{                                                                                                       \
				cnc_conversion_registry* raw_registry = nullptr;                                                   \
				cnc_conversion* raw_conversion        = nullptr;                                                   \
				cnc_conversion_info info              = {};                                                        \
				const cnc_open_err err = cnc_registry_open(&raw_registry, &mbr_heap, cnc_registry_options_none);   \
				if (err != cnc_open_err_ok) {                                                                      \
					/* something went wrong, get out of here quick! */                                            \
					result = false;                                                                               \
					break;                                                                                        \
				}                                                                                                  \
				registry.reset(raw_registry);                                                                      \
				if constexpr (Fast) {                                                                              \
					if (!cnc_shared_add_simdutf_to_registry(registry.get())) {                                    \
						/* something went wrong, get out of here quick! */                                       \
						result = false;                                                                          \
						state.SkipWithError("could not add conversions to registry");                            \
						break;                                                                                   \
					}                                                                                             \
				}                                                                                                  \
				const cnc_open_err conv_err                                                                        \
				     = cnc_conv_new_c8(registry.get(), (const ztd_char8_t*)u8"UTF-" #FROM_N "-unchecked",          \
				          (const ztd_char8_t*)u8"UTF-" #TO_N "-unchecked", &raw_conversion, &info);                \
				if (conv_err != cnc_open_err_ok) {                                                                 \
					state.SkipWithError("could not open conversion descriptor");                                  \
					result = false;                                                                               \
					return;                                                                                       \
				}                                                                                                  \
				conversion.reset(raw_conversion);                                                                  \
			}                                                                                                       \
			size_t input_size                   = input_data.size() * sizeof(*input_data.data());                   \
			const unsigned char* input          = (const unsigned char*)input_data.data();                          \
			[[maybe_unused]] size_t output_size = output_data.size() * sizeof(*output_data.data());                 \
			unsigned char* output               = (unsigned char*)output_data.data();                               \
			for (; input_size > 0 || !cnc_conv_state_is_complete(conversion.get());) {                              \
				cnc_mcerr err = cnc_conv_one(                                                                      \
				     conversion.get(), Unbounded ? nullptr : &output_size, &output, &input_size, &input);          \
				if (err != cnc_mcerr_ok) {                                                                         \
					result = false;                                                                               \
					break;                                                                                        \
				}                                                                                                  \
			}                                                                                                       \
		}                                                                                                            \
		const auto mismatch_result                                                                                   \
		     = std::mismatch(output_data.cbegin(), output_data.cend(), expected_begin, expected_end);                \
		const bool is_equal = mismatch_result.first == output_data.cend() && mismatch_result.second == expected_end; \
		if (!result) {                                                                                               \
			if (!state.skipped()) {                                                                                 \
				state.SkipWithError("conversion failed with an error");                                            \
			}                                                                                                       \
		}                                                                                                            \
		else if (!is_equal) {                                                                                        \
			if (!state.skipped()) {                                                                                 \
				state.SkipWithError("conversion succeeded but produced illegitimate data at index "                \
				     + std::to_string(mismatch_result.second - expected_begin));                                   \
			}                                                                                                       \
		}                                                                                                            \
	}                                                                                                                 \
                                                                                                                       \
	static void utf##FROM_N##_to_utf##TO_N##_init_well_formed_cuneicode_registry_single_unchecked(                    \
	     benchmark::State& state) {                                                                                   \
		utf##FROM_N##_to_utf##TO_N##_init_well_formed_cuneicode_registry_single_unchecked_core<true, false>(state);  \
	}                                                                                                                 \
	static void utf##FROM_N##_to_utf##TO_N##_init_well_formed_cuneicode_registry_single_unchecked_unbounded(          \
	     benchmark::State& state) {                                                                                   \
		utf##FROM_N##_to_utf##TO_N##_init_well_formed_cuneicode_registry_single_unchecked_core<true, true>(state);   \
	}                                                                                                                 \
	static_assert(true, "")

UTF_CONVERSION_BENCHMARK(16, 8);
UTF_CONVERSION_BENCHMARK(16, 32);

UTF_CONVERSION_BENCHMARK(32, 8);
UTF_CONVERSION_BENCHMARK(32, 16);

UTF_CONVERSION_BENCHMARK(8, 16);
UTF_CONVERSION_BENCHMARK(8, 32);

#undef UTF_CONVERSION_BENCHMARK

BENCHMARK(utf8_to_utf16_well_formed_cuneicode_registry_single_unchecked);
BENCHMARK(utf8_to_utf16_well_formed_cuneicode_registry_single_unchecked_unbounded);
BENCHMARK(utf16_to_utf8_well_formed_cuneicode_registry_single_unchecked);
BENCHMARK(utf16_to_utf8_well_formed_cuneicode_registry_single_unchecked_unbounded);

BENCHMARK(utf8_to_utf32_well_formed_cuneicode_registry_single_unchecked);
BENCHMARK(utf8_to_utf32_well_formed_cuneicode_registry_single_unchecked_unbounded);
BENCHMARK(utf32_to_utf8_well_formed_cuneicode_registry_single_unchecked);
BENCHMARK(utf32_to_utf8_well_formed_cuneicode_registry_single_unchecked_unbounded);

BENCHMARK(utf16_to_utf32_well_formed_cuneicode_registry_single_unchecked);
BENCHMARK(utf16_to_utf32_well_formed_cuneicode_registry_single_unchecked_unbounded);
BENCHMARK(utf32_to_utf16_well_formed_cuneicode_registry_single_unchecked);
BENCHMARK(utf32_to_utf16_well_formed_cuneicode_registry_single_unchecked_unbounded);

BENCHMARK(utf8_to_utf16_init_well_formed_cuneicode_registry_single_unchecked);
BENCHMARK(utf8_to_utf16_init_well_formed_cuneicode_registry_single_unchecked_unbounded);
BENCHMARK(utf16_to_utf8_init_well_formed_cuneicode_registry_single_unchecked);
BENCHMARK(utf16_to_utf8_init_well_formed_cuneicode_registry_single_unchecked_unbounded);

BENCHMARK(utf8_to_utf32_init_well_formed_cuneicode_registry_single_unchecked);
BENCHMARK(utf8_to_utf32_init_well_formed_cuneicode_registry_single_unchecked_unbounded);
BENCHMARK(utf32_to_utf8_init_well_formed_cuneicode_registry_single_unchecked);
BENCHMARK(utf32_to_utf8_init_well_formed_cuneicode_registry_single_unchecked_unbounded);

BENCHMARK(utf16_to_utf32_init_well_formed_cuneicode_registry_single_unchecked);
BENCHMARK(utf16_to_utf32_init_well_formed_cuneicode_registry_single_unchecked_unbounded);
BENCHMARK(utf32_to_utf16_init_well_formed_cuneicode_registry_single_unchecked);
BENCHMARK(utf32_to_utf16_init_well_formed_cuneicode_registry_single_unchecked_unbounded);

#endif
