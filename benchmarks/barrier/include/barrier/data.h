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

#pragma once

#ifndef ZTD_TEXT_BENCHMARKS_BARRIER_DATA_H
#define ZTD_TEXT_BENCHMARKS_BARRIER_DATA_H

#include <barrier/version.h>

#include <ztd/cuneicode.h>
#include <ztd/idk/c_span.h>

#if ZTD_IS_ON(ZTD_C)
#include <stddef.h>
#else
#include <cstddef>
#endif

ZTD_C_LANGUAGE_LINKAGE_I_ typedef struct error_size_size {
	cnc_mcerror error;
	size_t input_read;
	size_t output_written;
} error_size_size;

ZTD_C_LANGUAGE_LINKAGE_I_ typedef struct error_ptr_ptr {
	cnc_mcerror error;
	const ztd_char32_t* input;
	ztd_char8_t* output;
} error_ptr_ptr;

ZTD_C_LANGUAGE_LINKAGE_I_
ZTD_TEXT_BENCHMARKS_BARRIER_API_LINKAGE_I_ c_span_char8_t u8_data;
ZTD_C_LANGUAGE_LINKAGE_I_
ZTD_TEXT_BENCHMARKS_BARRIER_API_LINKAGE_I_ c_span_char16_t u16_data;
ZTD_C_LANGUAGE_LINKAGE_I_
ZTD_TEXT_BENCHMARKS_BARRIER_API_LINKAGE_I_ c_span_char32_t u32_data;

#endif // ZTD_TEXT_BENCHMARKS_BARRIER_DATA_H
