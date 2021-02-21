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

#ifndef ZTD_TEXT_DETAIL_RECONSTRUCT_HPP
#define ZTD_TEXT_DETAIL_RECONSTRUCT_HPP

#include <ztd/text/version.hpp>

#include <ztd/text/subrange.hpp>

#include <ztd/text/detail/range.hpp>
#include <ztd/text/detail/adl.hpp>
#include <ztd/text/detail/span.hpp>
#include <ztd/text/detail/memory.hpp>
#include <ztd/text/detail/type_traits.hpp>
#include <ztd/text/detail/empty_string.hpp>

#include <iterator>
#include <utility>
#include <type_traits>
#include <string_view>

namespace ztd { namespace text {
	ZTD_TEXT_INLINE_ABI_NAMESPACE_OPEN_I_

	namespace __detail {

		template <typename _Ty, decltype(::ztd::text::dynamic_extent) _Extent, typename _It, typename _Sen>
		constexpr ::ztd::text::span<_Ty> reconstruct(
			::std::in_place_type_t<::ztd::text::span<_Ty, _Extent>>, _It __iterator, _Sen __sentinel) noexcept {
			if constexpr (!::std::is_integral_v<_Sen>) {
				return ::ztd::text::span<_Ty>(__adl::__adl_to_address(__iterator), __sentinel - __iterator);
			}
			else {
				return ::ztd::text::span<_Ty>(__adl::__adl_to_address(__iterator), __sentinel);
			}
		}

		template <typename _Ty, typename _Traits, typename _It, typename _Sen>
		constexpr ::std::basic_string_view<_Ty, _Traits> reconstruct(
			::std::in_place_type_t<::std::basic_string_view<_Ty, _Traits>>, _It __iterator,
			_Sen __sentinel) noexcept {
			using _SizeType = typename ::std::basic_string_view<_Ty, _Traits>::size_type;
			if constexpr (!::std::is_integral_v<_Sen>) {
				_SizeType __ptr_size = static_cast<_SizeType>(__sentinel - __iterator);
#if defined(_ITERATOR_DEBUG_LEVEL) && _ITERATOR_DEBUG_LEVEL >= 1
				if (__ptr_size == static_cast<_SizeType>(0)) {
					const auto& __empty_str = __empty_string<_Ty>();
					return ::std::basic_string_view<_Ty, _Traits>(__empty_str + 0, 0);
				}
#endif
				return ::std::basic_string_view<_Ty, _Traits>(::std::addressof(*__iterator), __ptr_size);
			}
			else {
#if defined(_ITERATOR_DEBUG_LEVEL) && _ITERATOR_DEBUG_LEVEL >= 1
				if (static_cast<_SizeType>(__sentinel) == static_cast<_SizeType>(0)) {
					const auto& __empty_str = __empty_string<_Ty>();
					return ::std::basic_string_view<_Ty, _Traits>(__empty_str + 0, 0);
				}
#endif
				return ::std::basic_string_view<_Ty, _Traits>(
					::std::addressof(*__iterator), static_cast<_SizeType>(__sentinel));
			}
		}

		template <typename _Ty, typename _Traits, typename _ArrayTy, ::std::size_t _ArraySize>
		constexpr ::std::basic_string_view<_Ty, _Traits> reconstruct(
			::std::in_place_type_t<::std::basic_string_view<_Ty, _Traits>>,
			const _ArrayTy (&__arr)[_ArraySize]) noexcept {
			return ::std::basic_string_view<_Ty, _Traits>(__arr);
		}

		template <typename _Range, typename _It, typename _Sen>
		constexpr auto __adl_pair_reconstruct(::std::in_place_type_t<_Range> __ty, _It __iterator,
			_Sen __sentinel) noexcept(noexcept(reconstruct(__ty, ::std::move(__iterator), ::std::move(__sentinel))))
			-> decltype(reconstruct(__ty, ::std::move(__iterator), ::std::move(__sentinel))) {
			return reconstruct(__ty, ::std::move(__iterator), ::std::move(__sentinel));
		}

		template <typename _Range, typename _It, typename _Sen>
		constexpr auto __adl_reconstruct(
			::std::in_place_type_t<_Range> __ty, _It __iterator, _Sen __sentinel) noexcept(noexcept(reconstruct(__ty,
			subrange<__remove_cvref_t<_It>, __remove_cvref_t<_Sen>>(::std::declval<_It>(), ::std::declval<_Sen>()))))
			-> decltype(reconstruct(__ty,
			     subrange<__remove_cvref_t<_It>, __remove_cvref_t<_Sen>>(
			          ::std::declval<_It>(), ::std::declval<_Sen>()))) {
			return reconstruct(__ty,
				subrange<__remove_cvref_t<_It>, __remove_cvref_t<_Sen>>(
				     ::std::move(__iterator), ::std::move(__sentinel)));
		}

		template <typename _Range, typename _InRange>
		constexpr auto __adl_range_reconstruct(::std::in_place_type_t<_Range> __ty, _InRange&& __in_range) noexcept(
			noexcept(reconstruct(__ty, ::std::forward<_InRange>(__in_range))))
			-> decltype(reconstruct(__ty, ::std::forward<_InRange>(__in_range))) {
			return reconstruct(__ty, ::std::forward<_InRange>(__in_range));
		}

		template <typename _Range, typename _It = __range_iterator_t<_Range>,
			typename _Sen = __range_sentinel_t<_Range>>
		using __detect_is_pair_reconstructible_range = decltype(__adl_pair_reconstruct(
			::std::in_place_type<__remove_cvref_t<_Range>>, ::std::declval<_It>(), ::std::declval<_Sen>()));

		template <typename _Range, typename _It = __range_iterator_t<__remove_cvref_t<_Range>>,
			typename _Sen = __range_sentinel_t<__remove_cvref_t<_Range>>>
		using __is_pair_reconstructible_range
			= __is_detected<__detect_is_pair_reconstructible_range, _Range, _It, _Sen>;

		template <typename _Range, typename _It = __range_iterator_t<__remove_cvref_t<_Range>>,
			typename _Sen = __range_sentinel_t<__remove_cvref_t<_Range>>>
		inline constexpr bool __is_pair_reconstructible_range_v
			= __is_pair_reconstructible_range<_Range, _It, _Sen>::value;

		template <typename _Range, typename _It = __range_iterator_t<__remove_cvref_t<_Range>>,
			typename _Sen = __range_sentinel_t<__remove_cvref_t<_Range>>, typename = void>
		class __is_nothrow_pair_reconstructible_range : public ::std::false_type { };

		template <typename _Range, typename _It, typename _Sen>
		class __is_nothrow_pair_reconstructible_range<_Range, _It, _Sen,
			::std::enable_if_t<__is_pair_reconstructible_range_v<_Range, _It, _Sen>>>
		: public ::std::integral_constant<bool,
			  noexcept(__adl_pair_reconstruct(::std::in_place_type<__remove_cvref_t<_Range>>, ::std::declval<_It>(),
			       ::std::declval<_Sen>()))> { };

		template <typename _Range, typename _It = __range_iterator_t<_Range>,
			typename _Sen = __range_sentinel_t<_Range>>
		inline constexpr bool __is_nothrow_pair_reconstructible_range_v
			= __is_nothrow_pair_reconstructible_range<_Range, _It, _Sen>::value;

		template <typename _Range, typename _It, typename _Sen>
		using __detect_is_reconstructible_range = decltype(__adl_reconstruct(
			::std::in_place_type<__remove_cvref_t<_Range>>, ::std::declval<_It>(), ::std::declval<_Sen>()));

		template <typename _Range, typename _It = __range_iterator_t<__remove_cvref_t<_Range>>,
			typename _Sen = __range_sentinel_t<__remove_cvref_t<_Range>>>
		using __is_reconstructible_range = __is_detected<__detect_is_reconstructible_range, _Range, _It, _Sen>;

		template <typename _Range, typename _It = __range_iterator_t<__remove_cvref_t<_Range>>,
			typename _Sen = __range_sentinel_t<__remove_cvref_t<_Range>>>
		inline constexpr bool __is_reconstructible_range_v = __is_reconstructible_range<_Range, _It, _Sen>::value;

		template <typename _Range, typename _It = __range_iterator_t<__remove_cvref_t<_Range>>,
			typename _Sen = __range_sentinel_t<__remove_cvref_t<_Range>>, typename = void>
		class __is_nothrow_reconstructible_range : public ::std::false_type { };

		template <typename _Range, typename _It, typename _Sen>
		class __is_nothrow_reconstructible_range<_Range, _It, _Sen,
			::std::enable_if_t<__is_reconstructible_range_v<_Range, _It, _Sen>>>
		: public ::std::integral_constant<bool,
			  noexcept(__adl_reconstruct(::std::in_place_type<__remove_cvref_t<_Range>>, ::std::declval<_It>(),
			       ::std::declval<_Sen>()))> { };

		template <typename _Range, typename _It = __range_iterator_t<_Range>,
			typename _Sen = __range_sentinel_t<_Range>>
		inline constexpr bool __is_nothrow_reconstructible_range_v
			= __is_nothrow_reconstructible_range<_Range, _It, _Sen>::value;

		template <typename _Range, typename _InRange>
		using __detect_is_range_reconstructible_range = decltype(
			__adl_range_reconstruct(::std::in_place_type<__remove_cvref_t<_Range>>, ::std::declval<_InRange>()));

		template <typename _Range, typename _InRange>
		using __is_range_reconstructible_range
			= __is_detected<__detect_is_range_reconstructible_range, _Range, _InRange>;

		template <typename _Range, typename _InRange>
		inline constexpr bool __is_range_reconstructible_range_v
			= __is_range_reconstructible_range<_Range, _InRange>::value;

		template <typename _Range, typename _InRange, typename = void>
		class __is_nothrow_range_reconstructible_range : public ::std::false_type { };

		template <typename _Range, typename _InRange>
		class __is_nothrow_range_reconstructible_range<_Range, _InRange,
			::std::enable_if_t<__is_range_reconstructible_range_v<_Range, _InRange>>>
		: public ::std::integral_constant<bool,
			  noexcept(__adl_range_reconstruct(
			       ::std::in_place_type<__remove_cvref_t<_Range>>, ::std::declval<_InRange>()))> { };

		template <typename _Range, typename _InRange>
		inline constexpr bool __is_nothrow_range_reconstructible_range_v
			= __is_nothrow_range_reconstructible_range<_Range, _InRange>::value;

		template <typename _Range, typename _It, typename _Sen>
		constexpr auto
		__reconstruct(::std::in_place_type_t<_Range> __ty, _It&& __iterator, _Sen&& __sentinel) noexcept(
			__is_nothrow_pair_reconstructible_range_v<_Range, _It,
			     _Sen> || __is_nothrow_reconstructible_range_v<_Range, _It, _Sen> || ::std::is_nothrow_constructible_v<subrange<__remove_cvref_t<_It>, __remove_cvref_t<_Sen>>, _It, _Sen>) {
			if constexpr (__is_pair_reconstructible_range_v<_Range, _It, _Sen>) {
				return __adl_pair_reconstruct(
					__ty, ::std::forward<_It>(__iterator), ::std::forward<_Sen>(__sentinel));
			}
			else if constexpr (__is_reconstructible_range_v<_Range, _It, _Sen>) {
				return __adl_reconstruct(__ty, ::std::forward<_It>(__iterator), ::std::forward<_Sen>(__sentinel));
			}
			else {
				(void)__ty;
				return subrange<__remove_cvref_t<_It>, __remove_cvref_t<_Sen>>(
					::std::forward<_It>(__iterator), ::std::forward<_Sen>(__sentinel));
			}
		}

		template <typename _Range, typename _ArgRange>
		constexpr decltype(auto) __reconstruct(::std::in_place_type_t<_Range> __tag, _ArgRange&& __range) noexcept(
			__is_range_reconstructible_range_v<_Range, _ArgRange>
			     ? __is_nothrow_range_reconstructible_range_v<_Range, _ArgRange>
			     : noexcept(__reconstruct(__tag, __adl::__adl_begin(::std::forward<_ArgRange>(__range)),
			          __adl::__adl_end(::std::forward<_ArgRange>(__range))))) {
			if constexpr (__is_range_reconstructible_range_v<_Range, _ArgRange>) {
				return __adl_range_reconstruct(__tag, ::std::forward<_ArgRange>(__range));
			}
			else {
				return __reconstruct(__tag, __adl::__adl_begin(::std::forward<_ArgRange>(__range)),
					__adl::__adl_end(::std::forward<_ArgRange>(__range)));
			}
		}

		template <typename _Range, typename _It = __range_iterator_t<_Range>,
			typename _Sen = __range_sentinel_t<_Range>>
		using __reconstruct_t = decltype(__reconstruct(::std::in_place_type<_Range>,
			::std::declval<__remove_cvref_t<_It>>(), ::std::declval<__remove_cvref_t<_Sen>>()));

	} // namespace __detail
	ZTD_TEXT_INLINE_ABI_NAMESPACE_CLOSE_I_
}} // namespace ztd::text

#endif // ZTD_TEXT_DETAIL_RECONSTRUCT_HPP
