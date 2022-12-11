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

#include <barrier/convert.h>

#include <ztd/idk/size.h>
#include <ztd/idk/detail/unicode.h>


inline constexpr ztd_char32_t __first_4byte_unicode_code_point = 0x10000;
inline constexpr ztd_char32_t __first_3byte_unicode_code_point = 0x800;
inline constexpr ztd_char32_t __first_2byte_unicode_code_point = 0x80;
inline constexpr ztd_char32_t __first_1byte_unicode_code_point = 0x0;

ZTD_EXTERN_C_I_ ZTD_API_LINKAGE_I_ cnc_mcerror err_pptr_psize(
     ztd_char8_t** p_output_ptr, size_t* p_output_size, const ztd_char32_t** p_input_ptr, size_t* p_input_size) {
	const bool _IsCounting  = p_output_ptr == nullptr && *p_output_ptr == nullptr;
	const bool _IsUnbounded = p_output_size == nullptr;
	if (p_input_ptr == nullptr || *p_input_ptr == nullptr) {
		return CNC_MCERROR_OK;
	}
	const bool _IsInputUnbounded = p_input_size == nullptr;
	if (!_IsInputUnbounded) {
		if (*p_input_size < 1) {
			return CNC_MCERROR_OK;
		}
	}
	ztd_char32_t __c32 = **p_input_ptr;
	if (__c32 > __ztd_idk_detail_last_unicode_code_point) {
		return CNC_MCERROR_INVALID_SEQUENCE;
	}
	if (__c32 < __first_2byte_unicode_code_point) {
		// 1 byte
		if (!_IsUnbounded) {
			if (*p_output_size < 1) {
				return CNC_MCERROR_INSUFFICIENT_OUTPUT;
			}
			*p_output_size -= 1;
		}
		if (!_IsCounting) {
			**p_output_ptr = (ztd_char8_t)__c32;
			++*p_output_ptr;
		}
	}
	else if (__c32 < __first_3byte_unicode_code_point) {
		// 2 bytes
		if (!_IsUnbounded) {
			if (*p_output_size < 2) {
				return CNC_MCERROR_INSUFFICIENT_OUTPUT;
			}
			*p_output_size -= 2;
		}
		if (!_IsCounting) {
			**p_output_ptr = (ztd_char8_t)(((__c32 >> 6) & 0b00011111) | (0b11000000));
			++*p_output_ptr;
			**p_output_ptr = (ztd_char8_t)(((__c32 >> 0) & 0b00111111) | (0b10000000));
			++*p_output_ptr;
		}
	}
	else if (__c32 < __first_4byte_unicode_code_point) {
		// 3 bytes
		if (!_IsUnbounded) {
			if (*p_output_size < 3) {
				return CNC_MCERROR_INSUFFICIENT_OUTPUT;
			}
			*p_output_size -= 3;
		}
		if (!_IsCounting) {
			**p_output_ptr = (ztd_char8_t)(((__c32 >> 0xC) & 0b00001111u) | 0b11100000u);
			++*p_output_ptr;
			**p_output_ptr = (ztd_char8_t)(((__c32 >> 0x6) & 0b00111111u) | 0b10000000u);
			++*p_output_ptr;
			**p_output_ptr = (ztd_char8_t)(((__c32 >> 0x0) & 0b00111111u) | 0b10000000u);
			++*p_output_ptr;
		}
	}
	else {
		// 4 bytes
		if (!_IsUnbounded) {
			if (*p_output_size < 4) {
				return CNC_MCERROR_INSUFFICIENT_OUTPUT;
			}
			*p_output_size -= 4;
		}
		if (!_IsCounting) {
			**p_output_ptr = (ztd_char8_t)(((__c32 >> 0x12) & 0b00000111u) | 0b11110000u);
			++*p_output_ptr;
			**p_output_ptr = (ztd_char8_t)(((__c32 >> 0x0C) & 0b00111111u) | 0b10000000u);
			++*p_output_ptr;
			**p_output_ptr = (ztd_char8_t)(((__c32 >> 0x06) & 0b00111111u) | 0b10000000u);
			++*p_output_ptr;
			**p_output_ptr = (ztd_char8_t)(((__c32 >> 0x00) & 0b00111111u) | 0b10000000u);
			++*p_output_ptr;
		}
	}
	*p_input_ptr += 1;
	*p_input_size -= 1;
	return CNC_MCERROR_OK;
}

ZTD_EXTERN_C_I_ ZTD_API_LINKAGE_I_ cnc_mcerror err_pptr_ptr(ztd_char8_t** p_output_ptr, ztd_char8_t* output_ptr_last,
     const ztd_char32_t** p_input_ptr, const ztd_char32_t* input_ptr_last) {
	const bool _IsCounting       = p_output_ptr == nullptr && *p_output_ptr == nullptr;
	const bool _IsUnbounded      = output_ptr_last == nullptr;
	const bool _IsInputUnbounded = input_ptr_last == nullptr;
	if (p_input_ptr == nullptr || *p_input_ptr == nullptr) {
		return CNC_MCERROR_OK;
	}
	if (!_IsInputUnbounded) {
		if (*p_input_ptr == input_ptr_last) {
			return CNC_MCERROR_OK;
		}
	}
	ztd_char32_t __c32 = **p_input_ptr;
	if (__c32 > __ztd_idk_detail_last_unicode_code_point) {
		return CNC_MCERROR_INVALID_SEQUENCE;
	}
	if (__c32 < __first_2byte_unicode_code_point) {
		// 1 byte
		if (!_IsCounting) {
			if (!_IsUnbounded) {
				ptrdiff_t output_size = output_ptr_last - *p_output_ptr;
				if (output_size < 1) {
					return CNC_MCERROR_INSUFFICIENT_OUTPUT;
				}
			}
			**p_output_ptr = (ztd_char8_t)(((__c32 >> 0) & 0b01111111) | (0b00000000));
			++*p_output_ptr;
		}
	}
	else if (__c32 < __first_3byte_unicode_code_point) {
		// 2 bytes
		if (!_IsCounting) {
			if (!_IsUnbounded) {
				ptrdiff_t output_size = output_ptr_last - *p_output_ptr;
				if (output_size < 2) {
					return CNC_MCERROR_INSUFFICIENT_OUTPUT;
				}
			}
			**p_output_ptr = (ztd_char8_t)(((__c32 >> 6) & 0b00011111) | (0b11000000));
			++*p_output_ptr;
			**p_output_ptr = (ztd_char8_t)(((__c32 >> 0) & 0b00111111) | (0b10000000));
			++*p_output_ptr;
		}
	}
	else if (__c32 < __first_4byte_unicode_code_point) {
		// 3 bytes
		if (!_IsCounting) {
			if (!_IsUnbounded) {
				ptrdiff_t output_size = output_ptr_last - *p_output_ptr;
				if (output_size < 3) {
					return CNC_MCERROR_INSUFFICIENT_OUTPUT;
				}
			}
			**p_output_ptr = (ztd_char8_t)(((__c32 >> 0xC) & 0b00001111u) | 0b11100000u);
			++*p_output_ptr;
			**p_output_ptr = (ztd_char8_t)(((__c32 >> 0x6) & 0b00111111u) | 0b10000000u);
			++*p_output_ptr;
			**p_output_ptr = (ztd_char8_t)(((__c32 >> 0x0) & 0b00111111u) | 0b10000000u);
			++*p_output_ptr;
		}
	}
	else {
		// 4 bytes
		if (!_IsCounting) {
			if (!_IsUnbounded) {
				ptrdiff_t output_size = output_ptr_last - *p_output_ptr;
				if (output_size < 4) {
					return CNC_MCERROR_INSUFFICIENT_OUTPUT;
				}
			}
			**p_output_ptr = (ztd_char8_t)(((__c32 >> 0x12) & 0b00000111u) | 0b11110000u);
			++*p_output_ptr;
			**p_output_ptr = (ztd_char8_t)(((__c32 >> 0x0C) & 0b00111111u) | 0b10000000u);
			++*p_output_ptr;
			**p_output_ptr = (ztd_char8_t)(((__c32 >> 0x06) & 0b00111111u) | 0b10000000u);
			++*p_output_ptr;
			**p_output_ptr = (ztd_char8_t)(((__c32 >> 0x00) & 0b00111111u) | 0b10000000u);
			++*p_output_ptr;
		}
	}
	*p_input_ptr += 1;
	return CNC_MCERROR_OK;
}

ZTD_EXTERN_C_I_ ZTD_API_LINKAGE_I_ error_size_size size_struct_ptr_size(
     ztd_char8_t* output_ptr, size_t output_size, const ztd_char32_t* input_ptr, size_t input_size) {
	const bool _IsCounting = output_ptr == nullptr;
	if (input_ptr == nullptr || input_size < 1) {
		return error_size_size { CNC_MCERROR_OK, input_size, output_size };
	}
	ztd_char32_t __c32 = *input_ptr;
	if (__c32 > __ztd_idk_detail_last_unicode_code_point) {
		return error_size_size { CNC_MCERROR_INVALID_SEQUENCE, 0, 0 };
	}
	if (__c32 < __first_2byte_unicode_code_point) {
		// 1 byte
		if (!_IsCounting) {
			if (output_size < 1) {
				return error_size_size { CNC_MCERROR_INSUFFICIENT_OUTPUT, input_size, output_size };
			}
			output_ptr[0] = (ztd_char8_t)(((__c32 >> 0) & 0b01111111) | (0b00000000));
		}
		return error_size_size { CNC_MCERROR_OK, 1, 1 };
	}
	else if (__c32 < __first_3byte_unicode_code_point) {
		// 2 bytes
		if (!_IsCounting) {
			if (output_size < 2) {
				return error_size_size { CNC_MCERROR_INSUFFICIENT_OUTPUT, input_size, output_size };
			}
			output_ptr[0] = (ztd_char8_t)(((__c32 >> 6) & 0b00011111) | (0b11000000));
			output_ptr[1] = (ztd_char8_t)(((__c32 >> 0) & 0b00111111) | (0b10000000));
		}
		return error_size_size { CNC_MCERROR_OK, 1, 2 };
	}
	else if (__c32 < __first_4byte_unicode_code_point) {
		// 3 bytes
		if (!_IsCounting) {
			if (output_size < 3) {
				return error_size_size { CNC_MCERROR_INSUFFICIENT_OUTPUT, input_size, output_size };
			}
			output_ptr[0] = (ztd_char8_t)(((__c32 >> 0xC) & 0b00001111u) | 0b11100000u);
			output_ptr[1] = (ztd_char8_t)(((__c32 >> 0x6) & 0b00111111u) | 0b10000000u);
			output_ptr[2] = (ztd_char8_t)(((__c32 >> 0x0) & 0b00111111u) | 0b10000000u);
		}
		return error_size_size { CNC_MCERROR_OK, 1, 3 };
	}
	else {
		// 4 bytes
		if (!_IsCounting) {
			if (output_size < 4) {
				return error_size_size { CNC_MCERROR_INSUFFICIENT_OUTPUT, input_size, output_size };
			}
			output_ptr[0] = (ztd_char8_t)(((__c32 >> 0x12) & 0b00000111u) | 0b11110000u);
			output_ptr[1] = (ztd_char8_t)(((__c32 >> 0x0C) & 0b00111111u) | 0b10000000u);
			output_ptr[2] = (ztd_char8_t)(((__c32 >> 0x06) & 0b00111111u) | 0b10000000u);
			output_ptr[3] = (ztd_char8_t)(((__c32 >> 0x00) & 0b00111111u) | 0b10000000u);
		}
		return error_size_size { CNC_MCERROR_OK, 1, 4 };
	}
}

ZTD_EXTERN_C_I_ ZTD_API_LINKAGE_I_ error_ptr_ptr ptr_struct_ptr_size(
     ztd_char8_t* output_ptr, size_t output_size, const ztd_char32_t* input_ptr, size_t input_size) {
	const bool _IsCounting = output_ptr == nullptr;
	if (input_ptr == nullptr || input_size == 0) {
		return error_ptr_ptr { CNC_MCERROR_OK, input_ptr, output_ptr };
	}
	ztd_char32_t __c32 = *input_ptr;
	if (__c32 > __ztd_idk_detail_last_unicode_code_point) {
		return error_ptr_ptr { CNC_MCERROR_INVALID_SEQUENCE, input_ptr, output_ptr };
	}
	++input_ptr;
	if (__c32 < __first_2byte_unicode_code_point) {
		// 1 byte
		if (!_IsCounting) {
			if (output_size < 1) {
				return error_ptr_ptr { CNC_MCERROR_INSUFFICIENT_OUTPUT, --input_ptr, output_ptr };
			}
			*output_ptr = (ztd_char8_t)(((__c32 >> 0) & 0b01111111) | (0b00000000));
			++output_ptr;
		}
		return error_ptr_ptr { CNC_MCERROR_OK, input_ptr, output_ptr };
	}
	else if (__c32 < __first_3byte_unicode_code_point) {
		// 2 bytes
		if (!_IsCounting) {
			if (output_size < 2) {
				return error_ptr_ptr { CNC_MCERROR_INSUFFICIENT_OUTPUT, --input_ptr, output_ptr };
			}
			*output_ptr = (ztd_char8_t)(((__c32 >> 6) & 0b00011111) | (0b11000000));
			++output_ptr;
			*output_ptr = (ztd_char8_t)(((__c32 >> 0) & 0b00111111) | (0b10000000));
			++output_ptr;
		}
		return error_ptr_ptr { CNC_MCERROR_OK, input_ptr, output_ptr };
	}
	else if (__c32 < __first_4byte_unicode_code_point) {
		// 3 bytes
		if (!_IsCounting) {
			if (output_size < 3) {
				return error_ptr_ptr { CNC_MCERROR_INSUFFICIENT_OUTPUT, --input_ptr, output_ptr };
			}
			*output_ptr = (ztd_char8_t)(((__c32 >> 0xC) & 0b00001111u) | 0b11100000u);
			++output_ptr;
			*output_ptr = (ztd_char8_t)(((__c32 >> 0x6) & 0b00111111u) | 0b10000000u);
			++output_ptr;
			*output_ptr = (ztd_char8_t)(((__c32 >> 0x0) & 0b00111111u) | 0b10000000u);
			++output_ptr;
		}
		return error_ptr_ptr { CNC_MCERROR_OK, input_ptr, output_ptr };
	}
	else {
		// 4 bytes
		if (!_IsCounting) {
			if (output_size < 4) {
				return error_ptr_ptr { CNC_MCERROR_INSUFFICIENT_OUTPUT, --input_ptr, output_ptr };
			}
			*output_ptr = (ztd_char8_t)(((__c32 >> 0x12) & 0b00000111u) | 0b11110000u);
			++output_ptr;
			*output_ptr = (ztd_char8_t)(((__c32 >> 0x0C) & 0b00111111u) | 0b10000000u);
			++output_ptr;
			*output_ptr = (ztd_char8_t)(((__c32 >> 0x06) & 0b00111111u) | 0b10000000u);
			++output_ptr;
			*output_ptr = (ztd_char8_t)(((__c32 >> 0x00) & 0b00111111u) | 0b10000000u);
			++output_ptr;
		}
		return error_ptr_ptr { CNC_MCERROR_OK, input_ptr, output_ptr };
	}
}

ZTD_EXTERN_C_I_ ZTD_API_LINKAGE_I_ error_size_size size_struct_ptr_ptr(ztd_char8_t* output_ptr,
     ztd_char8_t* output_ptr_last, const ztd_char32_t* input_ptr, const ztd_char32_t* input_ptr_last) {
	const bool _IsCounting = output_ptr == nullptr;
	if (input_ptr == nullptr || input_ptr == input_ptr_last) {
		return error_size_size { CNC_MCERROR_OK, 0, (size_t)(_IsCounting ? 0 : output_ptr_last - output_ptr) };
	}
	ztd_char32_t __c32 = *input_ptr;
	++input_ptr;
	const size_t input_size = (size_t)(input_ptr_last - input_ptr);
	if (__c32 > __ztd_idk_detail_last_unicode_code_point) {
		return error_size_size { CNC_MCERROR_INVALID_SEQUENCE, 0, 0 };
	}
	const size_t output_size = _IsCounting ? 4 : (size_t)(output_ptr_last - output_ptr);
	if (__c32 < __first_2byte_unicode_code_point) {
		// 1 byte
		if (!_IsCounting) {
			if (output_size < 1) {
				return error_size_size { CNC_MCERROR_INSUFFICIENT_OUTPUT, input_size, output_size };
			}
			output_ptr[0] = (ztd_char8_t)(((__c32 >> 0) & 0b01111111) | (0b00000000));
		}
		return error_size_size { CNC_MCERROR_OK, 1, 1 };
	}
	else if (__c32 < __first_3byte_unicode_code_point) {
		// 2 bytes
		if (!_IsCounting) {
			if (output_size < 2) {
				return error_size_size { CNC_MCERROR_INSUFFICIENT_OUTPUT, input_size, output_size };
			}
			output_ptr[0] = (ztd_char8_t)(((__c32 >> 6) & 0b00011111) | (0b11000000));
			output_ptr[1] = (ztd_char8_t)(((__c32 >> 0) & 0b00111111) | (0b10000000));
		}
		return error_size_size { CNC_MCERROR_OK, 1, 2 };
	}
	else if (__c32 < __first_4byte_unicode_code_point) {
		// 3 bytes
		if (!_IsCounting) {
			if (output_size < 3) {
				return error_size_size { CNC_MCERROR_INSUFFICIENT_OUTPUT, input_size, output_size };
			}
			output_ptr[0] = (ztd_char8_t)(((__c32 >> 0xC) & 0b00001111u) | 0b11100000u);
			output_ptr[1] = (ztd_char8_t)(((__c32 >> 0x6) & 0b00111111u) | 0b10000000u);
			output_ptr[2] = (ztd_char8_t)(((__c32 >> 0x0) & 0b00111111u) | 0b10000000u);
		}
		return error_size_size { CNC_MCERROR_OK, 1, 3 };
	}
	else {
		// 4 bytes
		if (!_IsCounting) {
			if (output_size < 4) {
				return error_size_size { CNC_MCERROR_INSUFFICIENT_OUTPUT, input_size, output_size };
			}
			output_ptr[0] = (ztd_char8_t)(((__c32 >> 0x12) & 0b00000111u) | 0b11110000u);
			output_ptr[1] = (ztd_char8_t)(((__c32 >> 0x0C) & 0b00111111u) | 0b10000000u);
			output_ptr[2] = (ztd_char8_t)(((__c32 >> 0x06) & 0b00111111u) | 0b10000000u);
			output_ptr[3] = (ztd_char8_t)(((__c32 >> 0x00) & 0b00111111u) | 0b10000000u);
		}
		return error_size_size { CNC_MCERROR_OK, 1, 4 };
	}
}

ZTD_EXTERN_C_I_ ZTD_API_LINKAGE_I_ error_ptr_ptr ptr_struct_ptr_ptr(ztd_char8_t* output_ptr,
     ztd_char8_t* output_ptr_last, const ztd_char32_t* input_ptr, const ztd_char32_t* input_ptr_last) {
	const bool _IsCounting = output_ptr == nullptr;
	if (input_ptr == nullptr || input_ptr == input_ptr_last) {
		return error_ptr_ptr { CNC_MCERROR_OK, input_ptr, output_ptr };
	}
	ztd_char32_t __c32       = *input_ptr;
	const size_t output_size = _IsCounting ? 4 : (size_t)(output_ptr_last - output_ptr);
	if (__c32 > __ztd_idk_detail_last_unicode_code_point) {
		return error_ptr_ptr { CNC_MCERROR_INVALID_SEQUENCE, input_ptr, output_ptr };
	}
	++input_ptr;
	if (__c32 < __first_2byte_unicode_code_point) {
		// 1 byte
		if (!_IsCounting) {
			if (output_size < 1) {
				return error_ptr_ptr { CNC_MCERROR_INSUFFICIENT_OUTPUT, --input_ptr, output_ptr };
			}
			*output_ptr = (ztd_char8_t)(((__c32 >> 0) & 0b01111111) | (0b00000000));
			++output_ptr;
		}
		return error_ptr_ptr { CNC_MCERROR_OK, input_ptr, output_ptr };
	}
	else if (__c32 < __first_3byte_unicode_code_point) {
		// 2 bytes
		if (!_IsCounting) {
			if (output_size < 2) {
				return error_ptr_ptr { CNC_MCERROR_INSUFFICIENT_OUTPUT, --input_ptr, output_ptr };
			}
			*output_ptr = (ztd_char8_t)(((__c32 >> 6) & 0b00011111) | (0b11000000));
			++output_ptr;
			*output_ptr = (ztd_char8_t)(((__c32 >> 0) & 0b00111111) | (0b10000000));
			++output_ptr;
		}
		return error_ptr_ptr { CNC_MCERROR_OK, input_ptr, output_ptr };
	}
	else if (__c32 < __first_4byte_unicode_code_point) {
		// 3 bytes
		if (!_IsCounting) {
			if (output_size < 3) {
				return error_ptr_ptr { CNC_MCERROR_INSUFFICIENT_OUTPUT, --input_ptr, output_ptr };
			}
			*output_ptr = (ztd_char8_t)(((__c32 >> 0xC) & 0b00001111u) | 0b11100000u);
			++output_ptr;
			*output_ptr = (ztd_char8_t)(((__c32 >> 0x6) & 0b00111111u) | 0b10000000u);
			++output_ptr;
			*output_ptr = (ztd_char8_t)(((__c32 >> 0x0) & 0b00111111u) | 0b10000000u);
			++output_ptr;
		}
		return error_ptr_ptr { CNC_MCERROR_OK, input_ptr, output_ptr };
	}
	else {
		// 4 bytes
		if (!_IsCounting) {
			if (output_size < 4) {
				return error_ptr_ptr { CNC_MCERROR_INSUFFICIENT_OUTPUT, --input_ptr, output_ptr };
			}
			*output_ptr = (ztd_char8_t)(((__c32 >> 0x12) & 0b00000111u) | 0b11110000u);
			++output_ptr;
			*output_ptr = (ztd_char8_t)(((__c32 >> 0x0C) & 0b00111111u) | 0b10000000u);
			++output_ptr;
			*output_ptr = (ztd_char8_t)(((__c32 >> 0x06) & 0b00111111u) | 0b10000000u);
			++output_ptr;
			*output_ptr = (ztd_char8_t)(((__c32 >> 0x00) & 0b00111111u) | 0b10000000u);
			++output_ptr;
		}
		return error_ptr_ptr { CNC_MCERROR_OK, input_ptr, output_ptr };
	}
}
