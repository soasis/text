// =============================================================================
//
// ztd.text
// Copyright © 2021 JeanHeyd "ThePhD" Meneide and Shepherd's Oasis, LLC
// Contact: opensource@soasis.org
//
// Commercial License Usage
// Licensees holding valid commercial ztd.text licenses may use this file in
// accordance with the commercial license agreement provided with the
// Software or, alternatively, in accordance with the terms contained in
// a written agreement between you and Shepherd's Oasis, LLC.
// For licensing terms and conditions see your agreement. For
// further information contact opensource@soasis.org.
//
// Apache License Version 2 Usage
// Alternatively, this file may be used under the terms of Apache License
// Version 2.0 (the "License") for non-commercial use; you may not use this
// file except in compliance with the License. You may obtain a copy of the
// License at
//
//		http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
//
// ============================================================================>

#pragma once

#ifndef ZTD_TEXT_DETAIL_UNICODE_DETAIL_HPP
#define ZTD_TEXT_DETAIL_UNICODE_DETAIL_HPP

#include <ztd/text/char8_t.hpp>

#include <cstddef>

namespace ztd { namespace text {
	ZTD_TEXT_INLINE_ABI_NAMESPACE_OPEN_I_
	namespace __txt_detail {

		// codepoint related
		inline constexpr char32_t __last_code_point = 0x10FFFF;

		inline constexpr char32_t __first_lead_surrogate = 0xD800;
		inline constexpr char32_t __last_lead_surrogate  = 0xDBFF;

		inline constexpr char32_t __first_trail_surrogate = 0xDC00;
		inline constexpr char32_t __last_trail_surrogate  = 0xDFFF;

		inline constexpr char32_t __first_surrogate = __first_lead_surrogate;
		inline constexpr char32_t __last_surrogate  = __last_trail_surrogate;

		inline constexpr bool __is_lead_surrogate(char32_t __value) noexcept {
			return __value >= __first_lead_surrogate && __value <= __last_lead_surrogate;
		}
		inline constexpr bool __is_trail_surrogate(char32_t __value) noexcept {
			return __value >= __first_trail_surrogate && __value <= __last_trail_surrogate;
		}
		inline constexpr bool __is_surrogate(char32_t __value) noexcept {
			return __value >= __first_surrogate && __value <= __last_surrogate;
		}

		// utf8 related
		inline constexpr char32_t __last_1byte_value = 0x7F;
		inline constexpr char32_t __last_2byte_value = 0x7FF;
		inline constexpr char32_t __last_3byte_value = 0xFFFF;
		inline constexpr char32_t __last_4byte_value = 0x1FFFFF;
		inline constexpr char32_t __last_5byte_value = 0x3FFFFFF;
		inline constexpr char32_t __last_6byte_value = 0x7FFFFFFF;

		inline constexpr uchar8_t __start_1byte_mask         = 0x80u;
		inline constexpr uchar8_t __start_1byte_continuation = 0x00u;
		inline constexpr uchar8_t __start_1byte_shift        = 7u;
		inline constexpr uchar8_t __start_2byte_mask         = 0xC0u;
		inline constexpr uchar8_t __start_2byte_continuation = __start_2byte_mask;
		inline constexpr uchar8_t __start_2byte_shift        = 5u;
		inline constexpr uchar8_t __start_3byte_mask         = 0xE0u;
		inline constexpr uchar8_t __start_3byte_continuation = __start_3byte_mask;
		inline constexpr uchar8_t __start_3byte_shift        = 4u;
		inline constexpr uchar8_t __start_4byte_mask         = 0xF0u;
		inline constexpr uchar8_t __start_4byte_continuation = __start_4byte_mask;
		inline constexpr uchar8_t __start_4byte_shift        = 3u;
		inline constexpr uchar8_t __start_5byte_mask         = 0xF8u;
		inline constexpr uchar8_t __start_5byte_continuation = __start_5byte_mask;
		inline constexpr uchar8_t __start_5byte_shift        = 2u;
		inline constexpr uchar8_t __start_6byte_mask         = 0xFCu;
		inline constexpr uchar8_t __start_6byte_continuation = __start_6byte_mask;
		inline constexpr uchar8_t __start_6byte_shift        = 1u;

		inline constexpr uchar8_t __continuation_mask       = 0xC0u;
		inline constexpr uchar8_t __continuation_signature  = 0x80u;
		inline constexpr uchar8_t __continuation_mask_value = 0x3Fu;
		inline constexpr uchar8_t __single_mask_value       = 0x7Fu;

		inline constexpr bool __utf8_is_invalid(uchar8_t __b) noexcept {
			return __b == 0xC0 || __b == 0xC1 || __b > 0xF4;
		}

		inline constexpr bool __utf8_is_continuation(uchar8_t __value) noexcept {
			return (__value & __continuation_mask) == __continuation_signature;
		}

		inline constexpr bool __utf8_is_overlong(char32_t __value, ::std::size_t __bytes) noexcept {
			return __value <= __last_1byte_value || (__value <= __last_2byte_value && __bytes > 2)
				|| (__value <= __last_3byte_value && __bytes > 3);
		}

		inline constexpr bool __utf8_is_overlong_extended(char32_t __value, ::std::size_t __bytes) noexcept {
			return __value <= __last_1byte_value || (__value <= __last_2byte_value && __bytes > 2)
				|| (__value <= __last_3byte_value && __bytes > 3) || (__value <= __last_4byte_value && __bytes > 4)
				|| (__value <= __last_5byte_value && __bytes > 5);
		}

		template <bool __overlong_allowed = false>
		inline constexpr int __decode_length(char32_t __value) noexcept {
			if (__value <= __txt_detail::__last_1byte_value) {
				return 1;
			}
			if (__value <= __txt_detail::__last_2byte_value) {
				return 2;
			}
			if (__value <= __txt_detail::__last_3byte_value) {
				return 3;
			}
			if (__value <= __txt_detail::__last_4byte_value) {
				return 4;
			}
			if constexpr (__overlong_allowed) {
				if (__value <= __txt_detail::__last_5byte_value) {
					return 5;
				}
				if (__value <= __txt_detail::__last_6byte_value) {
					return 6;
				}
			}
			return 8;
		}

		inline constexpr int __sequence_length(uchar8_t __value) noexcept {
			return (__value & __start_1byte_mask) == __start_1byte_continuation ? 1
				: (__value & __start_3byte_mask) != __start_3byte_continuation ? 2
				: (__value & __start_4byte_mask) != __start_4byte_continuation ? 3
				                                                               : 4;
		}

		inline constexpr int __sequence_length_extended(uchar8_t __value) noexcept {
			return (__value & __start_1byte_mask) == __start_1byte_continuation ? 1
				: (__value & __start_3byte_mask) != __start_3byte_continuation ? 2
				: (__value & __start_4byte_mask) != __start_4byte_continuation ? 3
				: (__value & __start_5byte_mask) != __start_5byte_continuation ? 4
				: (__value & __start_6byte_mask) != __start_6byte_continuation ? 5
				                                                               : 6;
		}

		inline constexpr char32_t __decode(uchar8_t __value0, uchar8_t __value1) noexcept {
			return static_cast<char32_t>(((__value0 & 0x1F) << 6) | (__value1 & 0x3F));
		}

		inline constexpr char32_t __decode(uchar8_t __value0, uchar8_t __value1, uchar8_t __value2) noexcept {
			return static_cast<char32_t>(((__value0 & 0x0F) << 12) | ((__value1 & 0x3F) << 6) | (__value2 & 0x3F));
		}

		inline constexpr char32_t __decode(
			uchar8_t __value0, uchar8_t __value1, uchar8_t __value2, uchar8_t __value3) noexcept {
			return static_cast<char32_t>(((__value0 & 0x07) << 18) | ((__value1 & 0x3F) << 12)
				| ((__value2 & 0x3F) << 6) | (__value3 & 0x3F));
		}

		// utf16 related
		inline constexpr char32_t __last_ascii_value   = 0x7F;
		inline constexpr char32_t __last_bmp_value     = 0xFFFF;
		inline constexpr char32_t __normalizing_value  = 0x10000;
		inline constexpr int __lead_surrogate_bitmask  = 0xFFC00;
		inline constexpr int __trail_surrogate_bitmask = 0x3FF;
		inline constexpr int __lead_shifted_bits       = 10;
		inline constexpr char32_t __replacement        = 0xFFFD;
		inline constexpr char32_t __ascii_replacement  = 0x003F;

		inline constexpr char32_t __utf16_combine_surrogates(char16_t __lead, char16_t __trail) noexcept {
			auto __hibits = __lead - __first_lead_surrogate;
			auto __lobits = __trail - __first_trail_surrogate;
			return __normalizing_value + ((__hibits << __lead_shifted_bits) | __lobits);
		}

	} // namespace __txt_detail
	ZTD_TEXT_INLINE_ABI_NAMESPACE_CLOSE_I_
}} // namespace ztd::text

#endif // ZTD_TEXT_DETAIL_UNICODE_DETAIL_HPP
