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

#include <benchmark/benchmark.h>

#include <barrier/barrier.h>
#include <ctre_utf8_cxx17.hpp>

#include <vector>
#include <string_view>
#include <algorithm>

static void utf8_to_utf32_unchecked_well_formed_ctre(benchmark::State& state) {
	const std::vector<ztd_char8_t> input_data(
	     c_span_char8_t_data(u8_data), c_span_char8_t_data(u8_data) + c_span_char8_t_size(u8_data));
	std::vector<ztd_char32_t> output_data(c_span_char32_t_size(u32_data));
	bool result = true;
	for (auto _ : state) {
		ctre::utf8_range view(std::basic_string_view<unsigned char>(input_data.data(), input_data.size()));
		auto out_it = output_data.begin();
		auto last   = view.end();
		for (auto it = view.begin(); it != last; (void)++it, ++out_it) {
			*out_it = *it;
		}
		if (out_it != output_data.end()) {
			result = false;
		}
	}
	const bool is_equal = std::equal(output_data.cbegin(), output_data.cend(), c_span_char32_t_data(u32_data),
	     c_span_char32_t_data(u32_data) + c_span_char32_t_size(u32_data));
	if (!result || !is_equal) {
		state.SkipWithError("bad benchmark result");
		return;
	}
}

BENCHMARK(utf8_to_utf32_unchecked_well_formed_ctre);
