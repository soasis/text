// =============================================================================
//
// ztd.text
// Copyright © 2022 JeanHeyd "ThePhD" Meneide and Shepherd's Oasis, LLC
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
// ============================================================================ //

#pragma once

#ifndef ZTD_TEXT_DETAIL_CHAR_PREDICATES_HPP
#define ZTD_TEXT_DETAIL_CHAR_PREDICATES_HPP

#include <ztd/text/version.hpp>

#include <ztd/idk/type_traits.hpp>

#include <utility>

#include <ztd/prologue.hpp>

namespace ztd { namespace text {
	ZTD_TEXT_INLINE_ABI_NAMESPACE_OPEN_I_

	namespace __txt_detail {

		struct __equal_char_fn {
			template <typename _Left, typename _Right>
			constexpr auto operator()(_Left&& __left, _Right&& __right) const noexcept {
				using _ULeft  = ::ztd::remove_cvref_t<_Left>;
				using _URight = ::ztd::remove_cvref_t<_Right>;
				if constexpr (::std::is_same_v<_ULeft, _URight>) {
					return ::std::forward<_Left>(__left) == ::std::forward<_Right>(__right);
				}
				else if constexpr (::std::is_same_v<_ULeft, char> || ::std::is_same_v<_URight, char>) {
					if constexpr (::std::is_same_v<_ULeft, unsigned char>
						|| ::std::is_same_v<_URight, unsigned char>) {
						return static_cast<unsigned char>(__left) == static_cast<unsigned char>(__right);
					}
					else if constexpr (::std::is_same_v<_ULeft, signed char>
						|| ::std::is_same_v<_URight, signed char>) {
						return static_cast<signed char>(__left) == static_cast<signed char>(__right);
					}
					else {
						return ::std::forward<_Left>(__left) == ::std::forward<_Right>(__right);
					}
				}
				else {
					return ::std::forward<_Left>(__left) == ::std::forward<_Right>(__right);
				}
			}
		};

		inline constexpr const __equal_char_fn __equal_char = {};

	} // namespace __txt_detail

	ZTD_TEXT_INLINE_ABI_NAMESPACE_CLOSE_I_
}} // namespace ztd::text

#include <ztd/epilogue.hpp>

#endif
