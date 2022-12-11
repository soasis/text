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

#if ZTD_IS_ON(ZTD_TEXT_BENCHMARKS_CONVERSION_SPEED_ICU_BENCHMARKS)

#include <ztd/platform.hpp>

#if ZTD_IS_ON(ZTD_PLATFORM_ICU)

#include <benchmark/benchmark.h>

#include <barrier/barrier.h>
#include <ztd/idk/endian.hpp>

#include <unicode/stringpiece.h>
#include <unicode/utf8.h>
#include <unicode/uchar.h>
#include <unicode/ustring.h>
#include <unicode/unistr.h>
#include <unicode/ucnv.h>

#include <memory>
#include <vector>

inline namespace ztd_text_benchmarks_conversion_speed_icu_init {
	struct uconv_close_converter {
		void operator()(UConverter* conv) noexcept {
			ucnv_close(conv);
		}
	};
} // namespace ztd_text_benchmarks_conversion_speed_icu_init

#define UTF_CONVERSION_BENCHMARK(FROM_N, TO_N, BIG_FROM_SUFFIX, LIL_FROM_SUFFIX, BIG_TO_SUFFIX, LIL_TO_SUFFIX)    \
	static void utf##FROM_N##_to_utf##TO_N##_init_well_formed_icu_ucnv_convertEx(benchmark::State& state) {      \
		std::vector<ztd_char##FROM_N##_t> input_data(c_span_char##FROM_N##_t_data(u##FROM_N##_data),            \
		     c_span_char##FROM_N##_t_data(u##FROM_N##_data) + c_span_char##FROM_N##_t_size(u##FROM_N##_data));  \
		std::vector<ztd_char##TO_N##_t> output_data(c_span_char##TO_N##_t_size(u##TO_N##_data));                \
		bool result = true;                                                                                     \
		for (auto _ : state) {                                                                                  \
			std::unique_ptr<UConverter, uconv_close_converter> from_conv = nullptr;                            \
			{                                                                                                  \
				UErrorCode open_err = U_ZERO_ERROR;                                                           \
				UConverter* raw_conv                                                                          \
				     = ucnv_open(ztd::endian::native == ztd::endian::big ? "UTF-" #FROM_N #BIG_FROM_SUFFIX    \
				                                                         : "UTF-" #FROM_N #LIL_FROM_SUFFIX,   \
				          &open_err);                                                                         \
				if (!U_SUCCESS(open_err)) {                                                                   \
					state.SkipWithError("conversion succeeded but produced illegitimate data");              \
					return;                                                                                  \
				}                                                                                             \
				from_conv.reset(raw_conv);                                                                    \
			}                                                                                                  \
			std::unique_ptr<UConverter, uconv_close_converter> to_conv = nullptr;                              \
			{                                                                                                  \
				UErrorCode open_err = U_ZERO_ERROR;                                                           \
				UConverter* raw_conv                                                                          \
				     = ucnv_open(ztd::endian::native == ztd::endian::big ? "UTF-" #TO_N #BIG_TO_SUFFIX        \
				                                                         : "UTF-" #TO_N #LIL_TO_SUFFIX,       \
				          &open_err);                                                                         \
				if (!U_SUCCESS(open_err)) {                                                                   \
					state.SkipWithError("conversion succeeded but produced illegitimate data");              \
					return;                                                                                  \
				}                                                                                             \
				to_conv.reset(raw_conv);                                                                      \
			}                                                                                                  \
			UErrorCode err          = U_ZERO_ERROR;                                                            \
			const ztd_char_t* input = (const ztd_char_t*)input_data.data();                                    \
			int32_t input_size      = (int32_t)(input_data.size() * sizeof(input_data[0]));                    \
			ztd_char_t* output      = (ztd_char_t*)output_data.data();                                         \
			int32_t output_size     = (int32_t)(output_data.size() * sizeof(output_data[0]));                  \
			ucnv_convertEx(to_conv.get(), from_conv.get(), &output, output + output_size, &input,              \
			     input + input_size, nullptr, nullptr, nullptr, nullptr, true, true, &err);                    \
			if (!U_SUCCESS(err)) {                                                                             \
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


UTF_CONVERSION_BENCHMARK(8, 16, , , BE, LE);
UTF_CONVERSION_BENCHMARK(8, 32, , , BE, LE);

UTF_CONVERSION_BENCHMARK(16, 8, BE, LE, , );
UTF_CONVERSION_BENCHMARK(16, 32, BE, LE, BE, LE);

UTF_CONVERSION_BENCHMARK(32, 8, BE, LE, , );
UTF_CONVERSION_BENCHMARK(32, 16, BE, LE, BE, LE);

#undef UTF_CONVERSION_BENCHMARK


BENCHMARK(utf8_to_utf16_init_well_formed_icu_ucnv_convertEx);
BENCHMARK(utf8_to_utf32_init_well_formed_icu_ucnv_convertEx);

BENCHMARK(utf16_to_utf8_init_well_formed_icu_ucnv_convertEx);
BENCHMARK(utf16_to_utf32_init_well_formed_icu_ucnv_convertEx);

BENCHMARK(utf32_to_utf8_init_well_formed_icu_ucnv_convertEx);
BENCHMARK(utf32_to_utf16_init_well_formed_icu_ucnv_convertEx);

#endif

#endif
