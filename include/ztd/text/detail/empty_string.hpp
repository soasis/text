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

#ifndef ZTD_TEXT_DETAIL_EMPTY_STRING_HPP
#define ZTD_TEXT_DETAIL_EMPTY_STRING_HPP

#include <ztd/text/version.hpp>

#include <ztd/text/char8_t.hpp>

#include <ztd/text/detail/type_traits.hpp>

namespace ztd { namespace text {
	ZTD_TEXT_INLINE_ABI_NAMESPACE_OPEN_I_
	namespace __txt_detail {
		using __uchar_one_t                           = unsigned char[1];
		inline constexpr const __uchar_one_t __u_shim = {};

		using __schar_one_t                           = signed char[1];
		inline constexpr const __schar_one_t __s_shim = {};

		template <typename C>
		inline constexpr decltype(auto) __empty_string() noexcept {
			static_assert(__always_false_v<C>, "unrecognized character type");
			return "";
		}

		template <>
		inline constexpr decltype(auto) __empty_string<char>() noexcept {
			return "";
		}

		template <>
		inline constexpr decltype(auto) __empty_string<unsigned char>() noexcept {
			return (__u_shim);
		}

		template <>
		inline constexpr decltype(auto) __empty_string<signed char>() noexcept {
			return (__s_shim);
		}

		template <>
		inline constexpr decltype(auto) __empty_string<wchar_t>() noexcept {
			return L"";
		}

#if ZTD_TEXT_IS_ON(ZTD_TEXT_NATIVE_CHAR8_T_I_)
		template <>
		inline constexpr decltype(auto) __empty_string<char8_t>() noexcept {
			return u8"";
		}
#endif

		template <>
		inline constexpr decltype(auto) __empty_string<char16_t>() noexcept {
			return u"";
		}

		template <>
		inline constexpr decltype(auto) __empty_string<char32_t>() noexcept {
			return U"";
		}

	} // namespace __txt_detail
	ZTD_TEXT_INLINE_ABI_NAMESPACE_CLOSE_I_
}} // namespace ztd::text

#endif // ZTD_TEXT_DETAIL_EMPTY_STRING_HPP
