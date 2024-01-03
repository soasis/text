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

#include <barrier/data.h>

#include <barrier/barrier.h>

#include <ztd/cuneicode.h>
#include <ztd/idk/size.h>
#include <ztd/idk/c_span.h>
#include <ztd/idk/detail/unicode.h>

#include <ztd/tests/basic_unicode_strings.hpp>

#include <vector>
#include <stdexcept>

static auto init_u8_unicode_data() {
	std::vector<ztd_char8_t> data;
	data.reserve(static_cast<std::size_t>(__ztd_idk_detail_last_unicode_code_point * 3));
	for (ztd_char32_t c = 0; c < __ztd_idk_detail_last_unicode_code_point; ++c) {
		if (__ztd_idk_detail_is_surrogate(c)) {
			continue;
		}
		ztd_char8_t output_buffer[6];
		ztd_char8_t* output       = output_buffer;
		const ztd_char32_t* input = &c;
		size_t input_size         = 1;
		size_t output_size        = ztdc_c_array_size(output_buffer);
		cnc_mcerr err             = cnc_c32ntoc8n(&output_size, &output, &input_size, &input);
		if (err != cnc_mcerr_ok) {
			throw std::runtime_error("bad idea");
		}
		size_t written_output_size = ztdc_c_array_size(output_buffer) - output_size;
		if (written_output_size > 4) {
			throw std::runtime_error("bad idea");
		}
		for (std::size_t i = 0; i < written_output_size; ++i) {
			data.push_back(output_buffer[i]);
		}
	}
	return data;
}

static auto init_u16_unicode_data() {
	std::vector<ztd_char16_t> data;
	data.reserve(static_cast<std::size_t>(__ztd_idk_detail_last_unicode_code_point * 3));
	for (ztd_char32_t c = 0; c < __ztd_idk_detail_last_unicode_code_point; ++c) {
		if (__ztd_idk_detail_is_surrogate(c)) {
			continue;
		}
		ztd_char16_t output_buffer[6];
		ztd_char16_t* output      = output_buffer;
		const ztd_char32_t* input = &c;
		size_t input_size         = 1;
		size_t output_size        = ztdc_c_array_size(output_buffer);
		cnc_mcerr err             = cnc_c32ntoc16n(&output_size, &output, &input_size, &input);
		if (err != cnc_mcerr_ok) {
			throw std::runtime_error("bad idea");
		}
		size_t written_output_size = ztdc_c_array_size(output_buffer) - output_size;
		if (written_output_size > 2) {
			throw std::runtime_error("bad idea");
		}
		for (std::size_t i = 0; i < written_output_size; ++i) {
			data.push_back(output_buffer[i]);
		}
	}
	return data;
}

static auto init_u32_unicode_data() {
	std::vector<ztd_char32_t> data;
	data.reserve(static_cast<std::size_t>(__ztd_idk_detail_last_unicode_code_point));
	for (ztd_char32_t c = 0; c < __ztd_idk_detail_last_unicode_code_point; ++c) {
		if (__ztd_idk_detail_is_surrogate(c))
			continue;
		data.push_back(c);
	}
	return data;
}

static auto u8_unicode_vec  = init_u8_unicode_data();
static auto u16_unicode_vec = init_u16_unicode_data();
static auto u32_unicode_vec = init_u32_unicode_data();


ZTD_TEXT_BENCHMARKS_BARRIER_API_LINKAGE_I_ c_span_char8_t u8_unicode_data
     = make_c_span_char8_t(u8_unicode_vec.data(), u8_unicode_vec.data() + u8_unicode_vec.size());

ZTD_TEXT_BENCHMARKS_BARRIER_API_LINKAGE_I_ c_span_char16_t u16_unicode_data
     = make_c_span_char16_t(u16_unicode_vec.data(), u16_unicode_vec.data() + u16_unicode_vec.size());

ZTD_TEXT_BENCHMARKS_BARRIER_API_LINKAGE_I_ c_span_char32_t u32_unicode_data
     = make_c_span_char32_t(u32_unicode_vec.data(), u32_unicode_vec.data() + u32_unicode_vec.size());

static auto init_u8_basic_source_data() {
	std::vector<ztd_char8_t> data(
	     ztd::tests::u8_basic_source_character_set.cbegin(), ztd::tests::u8_basic_source_character_set.cend());
	return data;
}

static auto init_u16_basic_source_data() {
	std::vector<ztd_char16_t> data(
	     ztd::tests::u16_basic_source_character_set.cbegin(), ztd::tests::u16_basic_source_character_set.cend());
	return data;
}

static auto init_u32_basic_source_data() {
	std::vector<ztd_char32_t> data(
	     ztd::tests::u32_basic_source_character_set.cbegin(), ztd::tests::u32_basic_source_character_set.cend());
	return data;
}

static auto u8_basic_source_vec  = init_u8_basic_source_data();
static auto u16_basic_source_vec = init_u16_basic_source_data();
static auto u32_basic_source_vec = init_u32_basic_source_data();


ZTD_TEXT_BENCHMARKS_BARRIER_API_LINKAGE_I_ c_span_char8_t u8_basic_source_data
     = make_c_span_char8_t(u8_basic_source_vec.data(), u8_basic_source_vec.data() + u8_basic_source_vec.size());

ZTD_TEXT_BENCHMARKS_BARRIER_API_LINKAGE_I_ c_span_char16_t u16_basic_source_data
     = make_c_span_char16_t(u16_basic_source_vec.data(), u16_basic_source_vec.data() + u16_basic_source_vec.size());

ZTD_TEXT_BENCHMARKS_BARRIER_API_LINKAGE_I_ c_span_char32_t u32_basic_source_data
     = make_c_span_char32_t(u32_basic_source_vec.data(), u32_basic_source_vec.data() + u32_basic_source_vec.size());
