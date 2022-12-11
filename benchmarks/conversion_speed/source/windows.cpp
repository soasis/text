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

#include <ztd/version.hpp>

#include <ztd/text/benchmarks/version.hpp>

#if ZTD_IS_ON(ZTD_TEXT_BENCHMARKS_CONVERSION_SPEED_WINDOWS_BENCHMARKS)

#if ZTD_IS_ON(ZTD_PLATFORM_WINDOWS)

#include <benchmark/benchmark.h>

#include <ztd/cuneicode/shared/unicode_range.hpp>
#include <barrier/barrier.h>

#include <Windows.h>

#include <vector>

static void utf16_to_utf8_well_formed_windows_api(benchmark::State& state) {
	const std::vector<ztd_char16_t> input_data(
	     c_span_char16_t_data(u16_data), c_span_char16_t_data(u16_data) + c_span_char16_t_size(u16_data));
	std::vector<ztd_char8_t> output_data(c_span_char8_t_size(u8_data));
	bool result = true;
	for (auto _ : state) {
		size_t input_size    = input_data.size();
		const wchar_t* input = (const wchar_t*)input_data.data();
		size_t output_size   = output_data.size();
		char* output         = (char*)output_data.data();
		int err              = WideCharToMultiByte(
		                  CP_UTF8, WC_ERR_INVALID_CHARS, input, (int)input_size, output, (int)output_size, 0, 0);
		if (err == 0) {
			result = false;
		}
	}
	const bool is_equal = std::equal(output_data.cbegin(), output_data.cend(), c_span_char8_t_data(u8_data),
	     c_span_char8_t_data(u8_data) + c_span_char8_t_size(u8_data));
	if (!result) {
		state.SkipWithError("conversion failed with an error");
	}
	else if (!is_equal) {
		state.SkipWithError("conversion succeeded but produced illegitimate data");
	}
}

static void utf8_to_utf16_well_formed_windows_api(benchmark::State& state) {
	const std::vector<ztd_char8_t> input_data(
	     c_span_char8_t_data(u8_data), c_span_char8_t_data(u8_data) + c_span_char8_t_size(u8_data));
	std::vector<ztd_char16_t> output_data(c_span_char16_t_size(u16_data));
	bool result = true;
	for (auto _ : state) {
		size_t input_size  = input_data.size();
		const char* input  = (const char*)input_data.data();
		size_t output_size = output_data.size();
		wchar_t* output    = (wchar_t*)output_data.data();
		int err
		     = MultiByteToWideChar(CP_UTF8, MB_ERR_INVALID_CHARS, input, (int)input_size, output, (int)output_size);
		if (err == 0) {
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
		return;
	}
}

BENCHMARK(utf16_to_utf8_well_formed_windows_api);
BENCHMARK(utf8_to_utf16_well_formed_windows_api);

#endif // Windows-only

#endif
