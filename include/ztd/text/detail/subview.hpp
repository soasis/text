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

#ifndef ZTD_TEXT_DETAIL_SUBVIEW_HPP
#define ZTD_TEXT_DETAIL_SUBVIEW_HPP

#include <ztd/text/version.hpp>

#include <ztd/text/subrange.hpp>
#include <ztd/text/detail/empty_string.hpp>

#include <ztd/text/detail/reconstruct.hpp>
#include <ztd/text/detail/range.hpp>
#include <ztd/text/detail/adl.hpp>
#include <ztd/text/detail/span.hpp>

#include <iterator>
#include <utility>
#include <type_traits>
#include <string_view>

namespace ztd { namespace text {
	ZTD_TEXT_INLINE_ABI_NAMESPACE_OPEN_I_
	namespace __detail {

#if !defined(__cpp_lib_subviewible_ranges)

	template <typename _Ty, ::std::ptrdiff_t _Extent, typename _It, typename _Sen>
	constexpr ::std::span<_Ty> subview(::std::span<_Ty, _Extent>, _It __iterator, _Sen __sentinel) {
		return __subview(
			::std::in_place_type<::std::span<_Ty, _Extent>>, ::std::move(__iterator), ::std::move(__sentinel));
	}

	template <typename _Ty, typename _Traits, typename _It, typename _Sen>
	constexpr ::std::basic_string_view<_Ty, _Traits> subview(
		::std::basic_string_view<_Ty, _Traits>, _It __iterator, _Sen __sentinel) {
		return __subview(::std::in_place_type<::std::basic_string_view<_Ty, _Traits>>, ::std::move(__iterator),
			::std::move(__sentinel));
	}
#endif // subviewible ranges P1664

	template <typename _Range, typename _It, typename _Sen>
	constexpr auto __adl_pair_subview(::std::in_place_type_t<_Range> __ty, _It __iterator, _Sen __sentinel) noexcept(
		noexcept(subview(__ty, ::std::move(__iterator), ::std::move(__sentinel))))
		-> decltype(subview(__ty, ::std::move(__iterator), ::std::move(__sentinel))) {
		return subview(__ty, ::std::move(__iterator), ::std::move(__sentinel));
	}

	template <typename _Range, typename _It, typename _Sen>
	constexpr auto __adl_subview(::std::in_place_type_t<_Range> __ty, _It __iterator, _Sen __sentinel) noexcept(
		noexcept(subview(__ty,
			subrange<__remove_cvref_t<_It>, __remove_cvref_t<_Sen>>(
				::std::declval<_It>(), ::std::declval<_Sen>())))) -> decltype(subview(__ty,
		subrange<__remove_cvref_t<_It>, __remove_cvref_t<_Sen>>(::std::declval<_It>(), ::std::declval<_Sen>()))) {
		return subview(__ty,
			subrange<__remove_cvref_t<_It>, __remove_cvref_t<_Sen>>(
				::std::move(__iterator), ::std::move(__sentinel)));
	}

	template <typename _Range, typename _It = __range_iterator_t<_Range>, typename _Sen = __range_sentinel_t<_Range>>
	using __detect_is_pair_subviewible_range
		= decltype(__adl_pair_subview(_Range >, ::std::declval<_It>(), ::std::declval<_Sen>()));

	template <typename _Range, typename _It = __range_iterator_t<__remove_cvref_t<_Range>>,
		typename _Sen = __range_sentinel_t<__remove_cvref_t<_Range>>>
	using __is_pair_subviewible_range = __is_detected<__detect_is_pair_subviewible_range, _Range, _It, _Sen>;

	template <typename _Range, typename _It = __range_iterator_t<__remove_cvref_t<_Range>>,
		typename _Sen = __range_sentinel_t<__remove_cvref_t<_Range>>>
	inline constexpr bool __is_pair_subviewible_range_v = __is_pair_subviewible_range<_Range, _It, _Sen>::value;

	template <typename _Range, typename _It = __range_iterator_t<__remove_cvref_t<_Range>>,
		typename _Sen = __range_sentinel_t<__remove_cvref_t<_Range>>, typename = void>
	class __is_nothrow_pair_subviewible_range : public ::std::false_type {};

	template <typename _Range, typename _It, typename _Sen>
	class __is_nothrow_pair_subviewible_range<_Range, _It, _Sen,
		::std::enable_if_t<__is_pair_subviewible_range_v<_Range, _It, _Sen>>>
	: public ::std::integral_constant<bool,
		  noexcept(__adl_pair_subview(::std::declval<_Range>(), ::std::declval<_It>(), ::std::declval<_Sen>()))> {};

	template <typename _Range, typename _It = __range_iterator_t<_Range>, typename _Sen = __range_sentinel_t<_Range>>
	inline constexpr bool __is_nothrow_pair_subviewible_range_v
		= __is_nothrow_pair_subviewible_range<_Range, _It, _Sen>::value;

	template <typename _Range, typename _It, typename _Sen>
	using __detect_is_subviewible_range
		= decltype(__adl_subview(::std::declval<_Range>(), ::std::declval<_It>(), ::std::declval<_Sen>()));

	template <typename _Range, typename _It = __range_iterator_t<__remove_cvref_t<_Range>>,
		typename _Sen = __range_sentinel_t<__remove_cvref_t<_Range>>>
	using __is_subviewible_range = __is_detected<__detect_is_subviewible_range, _Range, _It, _Sen>;

	template <typename _Range, typename _It = __range_iterator_t<__remove_cvref_t<_Range>>,
		typename _Sen = __range_sentinel_t<__remove_cvref_t<_Range>>>
	inline constexpr bool __is_subviewible_range_v = __is_subviewible_range<_Range, _It, _Sen>::value;

	template <typename _Range, typename _It = __range_iterator_t<__remove_cvref_t<_Range>>,
		typename _Sen = __range_sentinel_t<__remove_cvref_t<_Range>>, typename = void>
	class __is_nothrow_subviewible_range : public ::std::false_type {};

	template <typename _Range, typename _It, typename _Sen>
	class __is_nothrow_subviewible_range<_Range, _It, _Sen,
		::std::enable_if_t<__is_subviewible_range_v<_Range, _It, _Sen>>>
	: public ::std::integral_constant<bool,
		  noexcept(__adl_subview(::std::declval<_Range>(), ::std::declval<_It>(), ::std::declval<_Sen>()))> {};

	template <typename _Range, typename _It = __range_iterator_t<_Range>, typename _Sen = __range_sentinel_t<_Range>>
	inline constexpr bool __is_nothrow_subviewible_range_v = __is_nothrow_subviewible_range<_Range, _It, _Sen>::value;

	template <typename _Range, typename _It, typename _Sen>
	constexpr auto __subview(_Range __original_range, _It __iterator, _Sen __sentinel) noexcept(
		__is_nothrow_pair_subviewible_range_v<_Range, _It,
			_Sen> || __is_nothrow_subviewible_range_v<_Range, _It, _Sen> || ::std::is_nothrow_constructible_v<subrange<__remove_cvref_t<_It>, __remove_cvref_t<_Sen>>, _It, _Sen>) {
		if constexpr (__is_pair_subviewible_range_v<_Range, _It, _Sen>) {
			return __adl_pair_subview(
				::std::move(__original_range), ::std::move(__iterator), ::std::move(__sentinel));
		}
		else if constexpr (__is_subviewible_range_v<_Range, _It, _Sen>) {
			return __adl_subview(::std::move(__original_range), ::std::move(__iterator), ::std::move(__sentinel));
		}
		else {
			return __subview(::std::in_place_type<_Range>, ::std::move(__iterator), ::std::move(__sentinel));
		}
	}

	template <typename _Range, typename _ArgRange>
	constexpr decltype(auto) __subview(_Range&& __original_range, _ArgRange&& __range) noexcept(noexcept(
		__subview(::std::forward<_Range>(__original_range), __adl::__adl_begin(::std::forward<_ArgRange>(__range)),
			__adl::__adl_end(::std::forward<_ArgRange>(__range))))) {
		return __subview(::std::forward<_Range>(__original_range),
			__adl::__adl_begin(::std::forward<_ArgRange>(__range)),
			__adl::__adl_end(::std::forward<_ArgRange>(__range)));
	}

	template <typename _Range, typename _It = __range_iterator_t<_Range>, typename _Sen = __range_sentinel_t<_Range>>
	using __subview_t = decltype(__subview(
		::std::declval<_Range>(), ::std::declval<__remove_cvref_t<_It>>(), ::std::declval<__remove_cvref_t<_Sen>>()));

}ZTD_TEXT_INLINE_ABI_NAMESPACE_CLOSE_I_
}} // namespace ztd::text::__detail

#endif // ZTD_TEXT_DETAIL_SUBVIEW_HPP