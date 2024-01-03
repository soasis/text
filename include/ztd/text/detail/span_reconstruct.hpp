// =============================================================================
//
// ztd.text
// Copyright © JeanHeyd "ThePhD" Meneide and Shepherd's Oasis, LLC
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
// https://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
//
// ============================================================================ //

#pragma once

#ifndef ZTD_TEXT_DETAIL_SPAN_RECONSTRUCT_HPP
#define ZTD_TEXT_DETAIL_SPAN_RECONSTRUCT_HPP

#include <ztd/text/version.hpp>

#include <ztd/idk/span.hpp>
#include <ztd/idk/char_traits.hpp>
#include <ztd/idk/size.hpp>
#include <ztd/ranges/adl.hpp>
#include <ztd/ranges/reconstruct.hpp>

#include <string_view>

#include <ztd/prologue.hpp>

namespace ztd { namespace text {
	ZTD_TEXT_INLINE_ABI_NAMESPACE_OPEN_I_

	namespace __txt_detail {
		template <typename _InputTag, bool _Mutable, typename _Input>
		constexpr bool __span_reconstruct_as_noexcept() noexcept {
			return true;
		}

		template <typename _InputTag, bool _Mutable, typename _Input>
		constexpr auto __span_reconstruct_as(_Input&& __input) noexcept(
			__span_reconstruct_as_noexcept<_InputTag, _Mutable, _Input>()) {
			using _CVInput   = ::std::remove_reference_t<_Input>;
			using _UInput    = ::ztd::remove_cvref_t<_Input>;
			using _UInputTag = ::ztd::remove_cvref_t<_InputTag>;
			// try to catch string literal_ts / arrays
			if constexpr (::ztd::ranges::is_reconstructible_v<::std::in_place_type_t<_UInputTag>, _Input>) {
				return ::ztd::ranges::reconstruct(
					::std::in_place_type<_UInputTag>, ::std::forward<_Input>(__input));
			}
			else if constexpr (::ztd::is_span_v<_UInput>) {
				using _Ty = ::std::conditional_t<_Mutable, typename _UInput::element_type,
					const typename _UInput::value_type>;
				return ::ztd::span<_Ty, _UInput::extent>(__input);
			}
			else if constexpr (::std::is_array_v<_UInput>               // cf
				&& ::std::is_const_v<::std::remove_extent_t<_CVInput>> // cf
				&& ::std::is_lvalue_reference_v<_Input>) {
				using _CharTy = ::std::remove_extent_t<_UInput>;
				if constexpr (is_char_traitable_v<_CharTy>) {
					return ::ztd::ranges::reconstruct(::std::in_place_type<::ztd::span<const _CharTy>>,
						::ztd::ranges::cbegin(__input),
						::ztd::ranges::cbegin(__input) + ::ztd::c_string_ptr_size(__input));
				}
				else {
					using _Ty = ::std::remove_extent_t<_CVInput>;
					return ::ztd::ranges::reconstruct(
						::std::in_place_type<::ztd::span<const _Ty>>, ::std::forward<_Input>(__input));
				}
			}
			else {
				if constexpr (ranges::is_range_contiguous_range_v<_CVInput> && ranges::is_sized_range_v<_CVInput>) {
					using _Ty = ::ztd::ranges::range_element_type_t<_CVInput>;
					return ::ztd::ranges::reconstruct(
						::std::in_place_type<::ztd::span<_Ty>>, ::std::forward<_Input>(__input));
				}
				else {
					return ::ztd::ranges::reconstruct(
						::std::in_place_type<_UInput>, ::std::forward<_Input>(__input));
				}
			}
		}

		template <typename _InputTag, typename _Input>
		constexpr auto __span_reconstruct(_Input&& __input) noexcept(
			__span_reconstruct_as_noexcept<_InputTag, false, _Input>()) {
			return __span_reconstruct_as<_InputTag, false>(::std::forward<_Input>(__input));
		}

		template <typename _InputTag, typename _Input>
		constexpr auto __span_reconstruct_mutable(_Input&& __input) noexcept(
			__span_reconstruct_as_noexcept<_InputTag, true, _Input>()) {
			return __span_reconstruct_as<_InputTag, true>(::std::forward<_Input>(__input));
		}

		template <typename _InputTag, typename _Input>
		using __span_reconstruct_t = decltype(__span_reconstruct<_InputTag>(::std::declval<_Input>()));

		template <typename _InputTag, typename _Input>
		using __span_reconstruct_mutable_t
			= decltype(__span_reconstruct_mutable<_InputTag>(::std::declval<_Input>()));
	} // namespace __txt_detail

	ZTD_TEXT_INLINE_ABI_NAMESPACE_CLOSE_I_
}} // namespace ztd::text

#include <ztd/epilogue.hpp>

#endif
