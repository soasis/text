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

#include <benchmark/benchmark.h>

#include <ztd/cuneicode.h>
#include <ztd/cuneicode/shared/simdutf/registry.hpp>
#include <barrier/barrier.h>

#include <memory>
#include <vector>

struct conversion_deleter {
	void operator()(cnc_conversion* conv) const noexcept {
		cnc_conv_delete(conv);
	}
};

struct registry_deleter {
	void operator()(cnc_conversion_registry* registry) const noexcept {
		cnc_delete_registry(registry);
	}
};

#define UTF_CONVERSION_BENCHMARK(FROM_N, TO_N)                                                                      \
	template <bool Fast, bool Unbounded>                                                                           \
	static void utf##FROM_N##_to_utf##TO_N##_well_formed_cuneicode_registry_core(benchmark::State& state) {        \
		const std::vector<ztd_char##FROM_N##_t> input_data(c_span_char##FROM_N##_t_data(u##FROM_N##_data),        \
		     c_span_char##FROM_N##_t_data(u##FROM_N##_data) + c_span_char##FROM_N##_t_size(u##FROM_N##_data));    \
		std::vector<ztd_char##TO_N##_t> output_data(c_span_char##TO_N##_t_size(u##TO_N##_data));                  \
		/* create registry to use, and conversion descriptor too */                                               \
		std::unique_ptr<cnc_conversion_registry, registry_deleter> registry = nullptr;                            \
		std::unique_ptr<cnc_conversion, conversion_deleter> conversion      = nullptr;                            \
		{                                                                                                         \
			cnc_conversion_registry* raw_registry = nullptr;                                                     \
			cnc_conversion* raw_conversion        = nullptr;                                                     \
			cnc_conversion_info info              = {};                                                          \
			const cnc_open_error err              = cnc_new_registry(&raw_registry, CNC_REGISTRY_OPTIONS_NONE);  \
			if (err != CNC_OPEN_ERROR_OK) {                                                                      \
				/* something went wrong, get out of here quick! */                                              \
				state.SkipWithError("bad benchmark result");                                                    \
				return;                                                                                         \
			}                                                                                                    \
			registry.reset(raw_registry);                                                                        \
			if constexpr (Fast) {                                                                                \
				if (!add_simdutf_to_registry(registry.get())) {                                                 \
					/* something went wrong, get out of here quick! */                                         \
					state.SkipWithError("bad benchmark result");                                               \
					return;                                                                                    \
				}                                                                                               \
			}                                                                                                    \
			const cnc_open_error conv_err = cnc_conv_new(registry.get(), (const ztd_char8_t*)u8"UTF-" #FROM_N,   \
			     (const ztd_char8_t*)u8"UTF-" #TO_N, &raw_conversion, &info);                                    \
			if (conv_err != CNC_OPEN_ERROR_OK) {                                                                 \
				/* something went wrong, get out of here quick! */                                              \
				state.SkipWithError("bad benchmark result");                                                    \
				return;                                                                                         \
			}                                                                                                    \
			conversion.reset(raw_conversion);                                                                    \
		}                                                                                                         \
		bool result = true;                                                                                       \
		for (auto _ : state) {                                                                                    \
			size_t input_size                   = input_data.size() * sizeof(*input_data.data());                \
			const unsigned char* input          = (const unsigned char*)input_data.data();                       \
			[[maybe_unused]] size_t output_size = output_data.size() * sizeof(*output_data.data());              \
			unsigned char* output               = (unsigned char*)output_data.data();                            \
			cnc_mcerror err                                                                                      \
			     = cnc_conv(conversion.get(), Unbounded ? nullptr : &output_size, &output, &input_size, &input); \
			if (err != CNC_MCERROR_OK) {                                                                         \
				result = false;                                                                                 \
			}                                                                                                    \
		}                                                                                                         \
		const bool is_equal                                                                                       \
		     = std::equal(output_data.cbegin(), output_data.cend(), c_span_char##TO_N##_t_data(u##TO_N##_data),   \
		          c_span_char##TO_N##_t_data(u##TO_N##_data) + c_span_char##TO_N##_t_size(u##TO_N##_data));       \
		if (!result || !is_equal) {                                                                               \
			state.SkipWithError("bad benchmark result");                                                         \
		}                                                                                                         \
	}                                                                                                              \
                                                                                                                    \
	static void utf##FROM_N##_to_utf##TO_N##_well_formed_cuneicode_registry(benchmark::State& state) {             \
		utf##FROM_N##_to_utf##TO_N##_well_formed_cuneicode_registry_core<true, false>(state);                     \
	}                                                                                                              \
	static void utf##FROM_N##_to_utf##TO_N##_well_formed_cuneicode_registry_unbounded(benchmark::State& state) {   \
		utf##FROM_N##_to_utf##TO_N##_well_formed_cuneicode_registry_core<true, true>(state);                      \
	}                                                                                                              \
	static_assert(true, "")

UTF_CONVERSION_BENCHMARK(16, 8);
UTF_CONVERSION_BENCHMARK(16, 32);

UTF_CONVERSION_BENCHMARK(32, 8);
UTF_CONVERSION_BENCHMARK(32, 16);

UTF_CONVERSION_BENCHMARK(8, 16);
UTF_CONVERSION_BENCHMARK(8, 32);

#undef UTF_CONVERSION_BENCHMARK

BENCHMARK(utf8_to_utf16_well_formed_cuneicode_registry);
BENCHMARK(utf8_to_utf16_well_formed_cuneicode_registry_unbounded);
BENCHMARK(utf16_to_utf8_well_formed_cuneicode_registry);
BENCHMARK(utf16_to_utf8_well_formed_cuneicode_registry_unbounded);

BENCHMARK(utf8_to_utf32_well_formed_cuneicode_registry);
BENCHMARK(utf8_to_utf32_well_formed_cuneicode_registry_unbounded);
BENCHMARK(utf32_to_utf8_well_formed_cuneicode_registry);
BENCHMARK(utf32_to_utf8_well_formed_cuneicode_registry_unbounded);

BENCHMARK(utf16_to_utf32_well_formed_cuneicode_registry);
BENCHMARK(utf16_to_utf32_well_formed_cuneicode_registry_unbounded);
BENCHMARK(utf32_to_utf16_well_formed_cuneicode_registry);
BENCHMARK(utf32_to_utf16_well_formed_cuneicode_registry_unbounded);
