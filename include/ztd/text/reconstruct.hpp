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

#ifndef ZTD_TEXT_RECONSTRUCT_HPP
#define ZTD_TEXT_RECONSTRUCT_HPP

#include <ztd/text/version.hpp>

#include <ztd/text/detail/reconstruct.types.hpp>
#include <ztd/text/detail/reconstruct.cpo.hpp>
#include <ztd/text/detail/span.hpp>

#include <string_view>

#include <ztd/text/detail/prologue.hpp>

namespace ztd {
	namespace text {
		ZTD_TEXT_INLINE_ABI_NAMESPACE_OPEN_I_

		namespace __txt_detail {
			template <typename _Input>
			constexpr bool __span_or_reconstruct_noexcept() {
				using _UInput = __remove_cvref_t<_Input>;
				if constexpr (::std::is_array_v<_UInput> && ::std::is_lvalue_reference_v<_Input>) {
					return true;
				}
				else if constexpr (__is_iterator_contiguous_iterator_v<__range_iterator_t<_Input>>) {
					return true;
				}
				else {
					return noexcept(__reconstruct(::std::in_place_type<_UInput>, ::std::declval<_Input>()));
				}
			}

			template <typename _Input>
			constexpr bool __string_view_or_span_or_reconstruct_noexcept() {
				using _VInput = ::std::remove_reference_t<_Input>;
				using _UInput = __remove_cvref_t<_Input>;
				if constexpr (
				     ::std::is_array_v<
				          _UInput> && ::std::is_const_v<::std::remove_extent_t<_VInput>> && ::std::is_lvalue_reference_v<_Input>) {
					return true;
				}
				else {
					return noexcept(__reconstruct(::std::in_place_type<_UInput>, ::std::declval<_Input>()));
				}
			}

			template <typename _Input>
			constexpr auto __span_or_reconstruct(_Input&& __input) noexcept(
			     __span_or_reconstruct_noexcept<_Input>()) {
				using _VInput = ::std::remove_reference_t<_Input>;
				using _UInput = __remove_cvref_t<_Input>;
				if constexpr (
				     __is_iterator_contiguous_iterator_v<__range_iterator_t<
				          _Input>> && !__is_specialization_of_v<_UInput, ::std::basic_string_view> && !__is_std_span_v<_UInput>) {
					using _Ty = ::std::remove_extent_t<_VInput>;
					return __reconstruct(::std::in_place_type<::ztd::text::span<_Ty>>, __adl::__adl_begin(__input),
					     __adl::__adl_end(__input));
				}
				else {
					return __reconstruct(::std::in_place_type<_UInput>, ::std::forward<_Input>(__input));
				}
			}

			template <typename _Input>
			constexpr auto __string_view_or_span_or_reconstruct(_Input&& __input) noexcept(
			     __string_view_or_span_or_reconstruct_noexcept<_Input>()) {
				using _VInput = ::std::remove_reference_t<_Input>;
				using _UInput = __remove_cvref_t<_Input>;
				if constexpr (
				     ::std::is_array_v<
				          _UInput> && ::std::is_const_v<::std::remove_extent_t<_VInput>> && ::std::is_lvalue_reference_v<_Input>) {
					using _CharTy = ::std::remove_extent_t<_UInput>;
					if constexpr (__is_char_traitable_v<_CharTy>) {
						return ::std::basic_string_view<_CharTy>(::std::forward<_Input>(__input));
					}
					else {
						using _Ty = ::std::remove_extent_t<_VInput>;
						return __reconstruct(::std::in_place_type<::ztd::text::span<_Ty>>,
						     __adl::__adl_begin(__input), __adl::__adl_end(__input));
					}
				}
				else {
					if constexpr (
					     __is_iterator_contiguous_iterator_v<__range_iterator_t<
					          _Input>> && !__is_specialization_of_v<_UInput, ::std::basic_string_view> && !__is_std_span_v<_UInput>) {
						using _CharTy = __range_value_type_t<_Input>;
						if constexpr (__is_char_traitable_v<_CharTy>) {
							return __reconstruct(::std::in_place_type<::std::basic_string_view<_CharTy>>,
							     __adl::__adl_begin(__input), __adl::__adl_end(__input));
						}
						else {
							return __reconstruct(::std::in_place_type<::ztd::text::span<_CharTy>>,
							     __adl::__adl_begin(__input), __adl::__adl_end(__input));
						}
					}
					else {
						return __reconstruct(::std::in_place_type<_UInput>, ::std::forward<_Input>(__input));
					}
				}
			}

			template <typename _Input>
			using __span_or_reconstruct_t = decltype(__span_or_reconstruct(::std::declval<_Input>()));

			template <typename _Input>
			using __string_view_or_span_or_reconstruct_t
			     = decltype(__string_view_or_span_or_reconstruct(::std::declval<_Input>()));
		} // namespace __txt_detail

		ZTD_TEXT_INLINE_ABI_NAMESPACE_CLOSE_I_
	} // namespace text

	namespace ranges { inline namespace __cpo {
		//////
		/// @brief The reconstruct object is a Customization Point Object (CPO) as defined by the Standard Library in
		/// [customization.point.object] (http://eel.is/c++draft/customization.point.object). It is the route for
		/// being able to put a range back from various bits and pieces of itself. For more in-depth information, see
		/// P1664 (https://thephd.dev/_vendor/future_cxx/papers/d1664.html).
		//////
		inline constexpr auto& reconstruct = ::ztd::text::__txt_detail::__reconstruct;
	}} // namespace ranges::__cpo

} // namespace ztd

#include <ztd/text/detail/epilogue.hpp>

#endif // ZTD_TEXT_RECONSTRUCT_HPP
