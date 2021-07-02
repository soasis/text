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

#ifndef ZTD_TEXT_DETAIL_MEMORY_HPP
#define ZTD_TEXT_DETAIL_MEMORY_HPP

#include <ztd/text/version.hpp>

#include <ztd/text/detail/type_traits.hpp>
#include <ztd/text/detail/mark_contiguous.hpp>

#include <memory>

#include <ztd/text/detail/prologue.hpp>

namespace ztd { namespace text {
	ZTD_TEXT_INLINE_ABI_NAMESPACE_OPEN_I_
	namespace __txt_detail {

		template <typename _Type, typename = void>
		struct __operator_arrow { };

		template <typename _Type>
		struct __operator_arrow<_Type, ::std::void_t<decltype(::std::declval<_Type&>().operator->())>> {
			using type = typename __operator_arrow<decltype(::std::declval<_Type&>().operator->())>::type;
		};

		template <typename _Type>
		using __operator_arrow_t = typename __operator_arrow<_Type>::type;

		template <typename _Type, typename = void>
		struct __is_operator_arrowable : public ::std::integral_constant<bool, ::std::is_pointer_v<_Type>> { };

		template <typename _Type>
		struct __is_operator_arrowable<_Type, ::std::void_t<decltype(::std::declval<_Type&>().operator->())>>
		: public ::std::integral_constant<bool,
			  __is_operator_arrowable<decltype(::std::declval<_Type&>().operator->())>::value> { };

		template <typename _Type>
		using __detect_std_pointer_traits_to_address
			= decltype(::std::pointer_traits<_Type>::to_address(::std::declval<_Type&>()));

		template <typename _Type>
		inline constexpr bool __is_operator_arrowable_v = __is_operator_arrowable<_Type>::value;

		template <typename _Type, typename = void>
		struct __is_to_addressable {
			inline static constexpr bool value
				= (::std::is_pointer_v<
				        _Type> && !::std::is_function_v<::std::remove_reference_t<::std::remove_pointer_t<_Type>>>)
				|| __is_operator_arrowable_v<::std::remove_reference_t<_Type>>;
		};

		template <typename _Type>
		struct __is_to_addressable<_Type, ::std::void_t<typename __remove_cvref_t<_Type>::element_type>>
		: public ::std::integral_constant<bool,
			  __is_detected_v<__detect_std_pointer_traits_to_address,
			       _Type> || (!::std::is_function_v<::std::remove_reference_t<_Type>> && __is_operator_arrowable_v<::std::remove_reference_t<_Type>>)> {
		};

		template <typename _Type>
		inline constexpr bool __is_to_addressable_v = __is_to_addressable<_Type>::value;

		namespace __adl {

#if ZTD_TEXT_IS_ON(ZTD_TEXT_STD_LIBRARY_TO_ADDRESS_I_)
			template <typename _Type>
			constexpr auto __adl_to_address(_Type&& __ptr_like) noexcept(
				noexcept(::std::to_address(::std::forward<_Type>(__ptr_like))))
				-> decltype(::std::to_address(::std::forward<_Type>(__ptr_like))) {
				return ::std::to_address(::std::forward<_Type>(__ptr_like));
			}
#else
			template <typename _Type>
			constexpr _Type* __adl_to_address(_Type* __ptr) noexcept {
				static_assert(!::std::is_function_v<_Type>, "the pointer shall not be function pointer type");
				return __ptr;
			}

			template <typename _Pointer, ::std::enable_if_t<!::std::is_pointer_v<_Pointer>>* = nullptr>
			auto __adl_to_address(_Pointer& p) noexcept {
				if constexpr (__mark_contiguous_v<_Pointer>) {
#if ZTD_TEXT_IS_ON(ZTD_TEXT_LIBVCXX_I_)
					return __adl_to_address(p._Unwrapped());
#else
					return __adl_to_address(p.operator->());
#endif
				}
				else {
					if constexpr (__is_detected_v<__detect_std_pointer_traits_to_address, _Pointer>) {
						return ::std::pointer_traits<_Pointer>::to_address(p);
					}
					else {
						return __adl_to_address(p.operator->());
					}
				}
			}
#endif
		} // namespace __adl
	}      // namespace __txt_detail
	ZTD_TEXT_INLINE_ABI_NAMESPACE_CLOSE_I_
}} // namespace ztd::text

#include <ztd/text/detail/epilogue.hpp>

#endif // ZTD_TEXT_DETAIL_MEMORY_HPP
