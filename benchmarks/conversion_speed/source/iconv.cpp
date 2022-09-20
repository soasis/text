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

#include <ztd/platform.hpp>

#if ZTD_IS_ON(ZTD_PLATFORM_LIBICONV)

#include <benchmark/benchmark.h>

#include <barrier/barrier.h>
#include <ztd/idk/endian.hpp>

#include <vector>
#include <memory>

struct descriptor_deleter {

	struct pointer {
		ztd::plat::icnv::descriptor handle;

		pointer(nullptr_t = nullptr) noexcept : handle(ztd::plat::icnv::failure_descriptor) {
		}

		pointer(ztd::plat::icnv::descriptor desc) noexcept : handle(desc) {
		}

		pointer(const pointer&) noexcept = default;
		pointer(pointer&&) noexcept      = default;

		pointer& operator=(const pointer&) noexcept = default;
		pointer& operator=(pointer&&) noexcept      = default;

		explicit operator bool() const noexcept {
			return *this != nullptr;
		}

		operator ztd::plat::icnv::descriptor() const noexcept {
			return this->handle;
		}

		friend bool operator==(pointer p, nullptr_t) noexcept {
			return !ztd::plat::icnv::descriptor_is_valid(p.handle);
		}

		friend bool operator==(nullptr_t, pointer p) noexcept {
			return !ztd::plat::icnv::descriptor_is_valid(p.handle);
		}

		friend bool operator!=(pointer p, nullptr_t) noexcept {
			return ztd::plat::icnv::descriptor_is_valid(p.handle);
		}

		friend bool operator!=(nullptr_t, pointer p) noexcept {
			return ztd::plat::icnv::descriptor_is_valid(p.handle);
		}
	};

	void operator()(pointer convd) noexcept {
		const auto& iconv_functions = ztd::plat::icnv::functions();
		iconv_functions.close(convd.handle);
	}
};

#define UTF_CONVERSION_BENCHMARKS(FROM_N, TO_N, BIG_FROM_SUFFIX, LIL_FROM_SUFFIX, BIG_TO_SUFFIX, LIL_TO_SUFFIX)       \
	static void utf##FROM_N##_to_utf##TO_N##_well_formed_iconv(benchmark::State& state) {                            \
		const auto& iconv_functions = ztd::plat::icnv::functions();                                                 \
		const std::vector<ztd_char##FROM_N##_t> input_data(c_span_char##FROM_N##_t_data(u##FROM_N##_data),          \
		     c_span_char##FROM_N##_t_data(u##FROM_N##_data) + c_span_char##FROM_N##_t_size(u##FROM_N##_data));      \
		std::vector<ztd_char##TO_N##_t> output_data(c_span_char##TO_N##_t_size(u##TO_N##_data));                    \
		std::unique_ptr<ztd::plat::icnv::descriptor, descriptor_deleter> convd = nullptr;                           \
		{                                                                                                           \
			descriptor_deleter::pointer raw_convd = nullptr;                                                       \
			if (ztd::endian::native == ztd::endian::little) {                                                      \
				raw_convd = iconv_functions.open("UTF-" #TO_N #LIL_TO_SUFFIX, "UTF-" #FROM_N #LIL_FROM_SUFFIX);   \
			}                                                                                                      \
			else {                                                                                                 \
				raw_convd = iconv_functions.open("UTF-" #TO_N #BIG_TO_SUFFIX, "UTF-" #FROM_N #BIG_FROM_SUFFIX);   \
			}                                                                                                      \
			if (!ztd::plat::icnv::descriptor_is_valid(raw_convd)) {                                                \
				state.SkipWithError("bad benchmark result");                                                      \
				return;                                                                                           \
			}                                                                                                      \
			convd.reset(raw_convd);                                                                                \
		}                                                                                                           \
                                                                                                                      \
		bool result = true;                                                                                         \
		for (auto _ : state) {                                                                                      \
			size_t input_size  = input_data.size() * sizeof(input_data[0]);                                        \
			const char* input  = (const char*)input_data.data();                                                   \
			size_t output_size = output_data.size() * sizeof(output_data[0]);                                      \
			char* output       = (char*)output_data.data();                                                        \
			size_t conv_result = iconv_functions.convert(convd.get(), &input, &input_size, &output, &output_size); \
			if (conv_result != ztd::plat::icnv::conversion_success) {                                              \
				result = false;                                                                                   \
			}                                                                                                      \
		}                                                                                                           \
		const bool is_equal                                                                                         \
		     = std::equal(output_data.cbegin(), output_data.cend(), c_span_char##TO_N##_t_data(u##TO_N##_data),     \
		          c_span_char##TO_N##_t_data(u##TO_N##_data) + c_span_char##TO_N##_t_size(u##TO_N##_data));         \
		if (!result || !is_equal) {                                                                                 \
			state.SkipWithError("bad benchmark result");                                                           \
		}                                                                                                           \
	}                                                                                                                \
	static_assert(true, "")

UTF_CONVERSION_BENCHMARKS(8, 16, , , BE, LE);
UTF_CONVERSION_BENCHMARKS(16, 8, BE, LE, , );

UTF_CONVERSION_BENCHMARKS(8, 32, , , BE, LE);
UTF_CONVERSION_BENCHMARKS(32, 8, BE, LE, , );

UTF_CONVERSION_BENCHMARKS(16, 32, BE, LE, BE, LE);
UTF_CONVERSION_BENCHMARKS(32, 16, BE, LE, BE, LE);

#undef UTF_CONVERSION_BENCHMARK

BENCHMARK(utf8_to_utf16_well_formed_iconv);
BENCHMARK(utf8_to_utf32_well_formed_iconv);

BENCHMARK(utf16_to_utf8_well_formed_iconv);
BENCHMARK(utf16_to_utf32_well_formed_iconv);

BENCHMARK(utf32_to_utf8_well_formed_iconv);
BENCHMARK(utf32_to_utf16_well_formed_iconv);

#endif
