// ============================================================================
//
// ztd.text
// Copyright © 2022-2023 JeanHeyd "ThePhD" Meneide and Shepherd's Oasis, LLC
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

#pragma once

#ifndef ZTD_TEXT_BENCHMARKS_BARRIER_CONVERT_H
#define ZTD_TEXT_BENCHMARKS_BARRIER_CONVERT_H

#include <barrier/version.h>

#include <barrier/data.h>

#include <ztd/cuneicode.h>
#include <ztd/idk/charN_t.h>

ZTD_C_LANGUAGE_LINKAGE_I_
ZTD_TEXT_BENCHMARKS_BARRIER_API_LINKAGE_I_ cnc_mcerror err_pptr_psize(
     ztd_char8_t** p_output_ptr, size_t* p_output_size, const ztd_char32_t** p_input_ptr, size_t* p_input_size);
ZTD_C_LANGUAGE_LINKAGE_I_
ZTD_TEXT_BENCHMARKS_BARRIER_API_LINKAGE_I_ cnc_mcerror err_pptr_ptr(ztd_char8_t** p_output_ptr,
     ztd_char8_t* output_ptr_last, const ztd_char32_t** p_input_ptr, const ztd_char32_t* input_ptr_last);
ZTD_C_LANGUAGE_LINKAGE_I_
ZTD_TEXT_BENCHMARKS_BARRIER_API_LINKAGE_I_ error_size_size size_struct_ptr_size(
     ztd_char8_t* output_ptr, size_t output_size, const ztd_char32_t* p_input_ptr, size_t input_size);
ZTD_C_LANGUAGE_LINKAGE_I_
ZTD_TEXT_BENCHMARKS_BARRIER_API_LINKAGE_I_ error_ptr_ptr ptr_struct_ptr_size(
     ztd_char8_t* output_ptr, size_t output_size, const ztd_char32_t* p_input_ptr, size_t input_size);
ZTD_C_LANGUAGE_LINKAGE_I_
ZTD_TEXT_BENCHMARKS_BARRIER_API_LINKAGE_I_ error_size_size size_struct_ptr_ptr(ztd_char8_t* output_ptr,
     ztd_char8_t* output_ptr_last, const ztd_char32_t* input_ptr, const ztd_char32_t* input_ptr_last);
ZTD_C_LANGUAGE_LINKAGE_I_
ZTD_TEXT_BENCHMARKS_BARRIER_API_LINKAGE_I_ error_ptr_ptr ptr_struct_ptr_ptr(ztd_char8_t* output_ptr,
     ztd_char8_t* output_ptr_last, const ztd_char32_t* input_ptr, const ztd_char32_t* input_ptr_last);

#endif
