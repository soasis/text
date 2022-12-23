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

#ifndef ZTD_TEXT_DETAIL_UPDATE_INPUT_HPP
#define ZTD_TEXT_DETAIL_UPDATE_INPUT_HPP

#include <ztd/text/version.hpp>

#include <ztd/ranges/reconstruct.hpp>

#include <ztd/prologue.hpp>

namespace ztd { namespace text {
	ZTD_TEXT_INLINE_ABI_NAMESPACE_OPEN_I_

	namespace __txt_detail {
		template <typename _Expected, typename _Input>
		constexpr bool __update_input_noexcept() noexcept {
			using _UInput    = remove_cvref_t<_Input>;
			using _UExpected = remove_cvref_t<_Expected>;
			if constexpr (::std::is_same_v<_UInput, _UExpected>) {
				return true;
			}
			else {
				return ::ztd::ranges::is_nothrow_range_reconstructible_v<_UExpected, _Input>;
			}
		}

		template <typename _Expected, typename _Input>
		constexpr decltype(auto) __update_input(_Input&& __input) noexcept(
			__update_input_noexcept<_Expected, _Input>()) {
			using _UInput    = remove_cvref_t<_Input>;
			using _UExpected = remove_cvref_t<_Expected>;
			if constexpr (::std::is_same_v<_UInput, _UExpected>) {
				return ::std::forward<_Input>(__input);
			}
			else {
				return ::ztd::ranges::reconstruct(
					::std::in_place_type<_UExpected>, ::std::forward<_Input>(__input));
			}
		}
	} // namespace __txt_detail

	ZTD_TEXT_INLINE_ABI_NAMESPACE_CLOSE_I_
}} // namespace ztd::text

#include <ztd/epilogue.hpp>

#endif
