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

#ifndef ZTD_TEXT_DETAIL_TYPE_TRAITS_HPP
#define ZTD_TEXT_DETAIL_TYPE_TRAITS_HPP

#include <ztd/text/version.hpp>

#include <ztd/text/forward.hpp>
#include <ztd/text/char8_t.hpp>

#include <type_traits>

namespace ztd { namespace text {
	ZTD_TEXT_INLINE_ABI_NAMESPACE_OPEN_I_
	namespace __txt_detail {

		template <typename _Type, typename = ::std::remove_cv_t<::std::remove_reference_t<_Type>>>
		struct __unwrap_impl {
			using type = _Type;
		};

		template <typename _Dummy, typename _Type>
		struct __unwrap_impl<_Dummy, ::std::reference_wrapper<_Type>> {
			using type = ::std::add_lvalue_reference_t<_Type>;
		};

		template <typename _Type>
		using __unwrap_t = typename __unwrap_impl<_Type>::type;

		template <typename>
		class __always_false : public ::std::integral_constant<bool, false> { };

		template <typename _Type, _Type>
		class __always_false_constant : public __always_false<_Type> { };

		template <typename _Type, _Type _Val>
		inline constexpr bool __always_false_constant_v = __always_false_constant<_Type, _Val>::value;

		template <::std::size_t _Val>
		using __always_false_index = __always_false_constant<::std::size_t, _Val>;

		template <::std::size_t _Val>
		inline constexpr bool __always_false_index_v = __always_false_index<_Val>::value;

		template <typename _Type>
		inline constexpr bool __always_false_v = __always_false<_Type>::value;

		template <typename>
		using __always_true = ::std::integral_constant<bool, true>;

		template <typename _Type>
		inline constexpr bool __always_true_v = __always_true<_Type>::value;

		template <typename _Type>
		using __remove_cvref = ::std::remove_cv<::std::remove_reference_t<_Type>>;

		template <typename _Type>
		using __remove_cvref_t = typename __remove_cvref<_Type>::type;

		// clang-format off
		template <typename _Type>
		using __is_character = ::std::integral_constant<bool,
			::std::is_same_v<_Type, char> || ::std::is_same_v<_Type, wchar_t> ||
#if ZTD_TEXT_IS_ON(ZTD_TEXT_NATIVE_CHAR8_T_I_)
			::std::is_same_v<_Type, char8_t> ||
#endif
			::std::is_same_v<_Type, ::ztd::text::uchar8_t> ||
			::std::is_same_v<_Type, unsigned char> ||
			::std::is_same_v<_Type, signed char> ||
			::std::is_same_v<_Type, char16_t> ||
			::std::is_same_v<_Type, char32_t> ||
			::std::is_same_v<_Type, ::ztd::text::unicode_code_point> ||
			::std::is_same_v<_Type, ::ztd::text::unicode_scalar_value>
		>;
		// clang-format on

		template <typename _Type>
		inline constexpr bool __is_character_v = __is_character<_Type>::value;

		template <typename T, template <typename...> class Templ>
		struct __is_specialization_of_impl : ::std::false_type { };
		template <typename... T, template <typename...> class Templ>
		struct __is_specialization_of_impl<Templ<T...>, Templ> : ::std::true_type { };

		template <typename T, template <typename...> class Templ>
		using __is_specialization_of = __is_specialization_of_impl<__remove_cvref_t<T>, Templ>;

		template <typename T, template <typename...> class Templ>
		inline constexpr bool __is_specialization_of_v = __is_specialization_of<T, Templ>::value;

		template <typename _Default, typename _Void, template <typename...> typename _Op, typename... _Args>
		class __detector {
		public:
			using value_t = ::std::false_type;
			using type    = _Default;
		};

		template <typename _Default, template <typename...> typename _Op, typename... _Args>
		class __detector<_Default, ::std::void_t<_Op<_Args...>>, _Op, _Args...> {
		public:
			using value_t = ::std::true_type;
			using type    = _Op<_Args...>;
		};

		class __nonesuch {
		public:
			~__nonesuch()                 = delete;
			__nonesuch(__nonesuch const&) = delete;
			__nonesuch& operator=(__nonesuch const&) = delete;
		};

		template <template <typename...> typename _Op, typename... _Args>
		using __is_detected = typename __detector<__nonesuch, void, _Op, _Args...>::value_t;

		template <template <typename...> typename _Op, typename... _Args>
		inline constexpr bool __is_detected_v = __is_detected<_Op, _Args...>::value;

		template <template <typename...> typename _Op, typename... _Args>
		using __detected_t = typename __detector<__nonesuch, void, _Op, _Args...>::type;

		template <typename _Default, template <typename...> typename _Op, typename... _Args>
		using __detected_or = __detector<_Default, void, _Op, _Args...>;

		template <typename _Type>
		struct __type_identity {
			using type = _Type;
		};

		// useful for suppression
		template <typename _Type>
		using __type_identity_t = typename __type_identity<_Type>::type;

		template <typename _Type, typename _SizeType = ::std::size_t>
		using __detect_reserve_with_size_type
			= decltype(::std::declval<_Type>().reserve(::std::declval<_SizeType>()));

	} // namespace __txt_detail

	ZTD_TEXT_INLINE_ABI_NAMESPACE_CLOSE_I_
}} // namespace ztd::text

#endif // ZTD_TEXT_DETAIL_TYPE_TRAITS_HPP
