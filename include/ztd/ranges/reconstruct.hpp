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

#ifndef ZTD_RANGES_RECONSTRUCT_HPP
#define ZTD_RANGES_RECONSTRUCT_HPP

#include <ztd/ranges/version.hpp>

#include <ztd/ranges/subrange.hpp>
#include <ztd/ranges/adl.hpp>
#include <ztd/ranges/range.hpp>

#include <ztd/tag_invoke.hpp>
#include <ztd/idk/type_traits.hpp>
#include <ztd/idk/empty_string.hpp>
#include <ztd/idk/hijack.hpp>
#include <ztd/idk/to_address.hpp>
#include <ztd/ranges/span.hpp>

#include <utility>
#include <type_traits>
#include <string_view>

#include <ztd/prologue.hpp>

namespace ztd { namespace ranges {
	ZTD_RANGES_INLINE_ABI_NAMESPACE_OPEN_I_

	namespace __rng_detail {

		class __reconstruct_fn : public ::ztd::hijack::token {
		private:
			template <typename _TagOrIt, typename _RangeOrSen>
			static constexpr bool __range_reconstruct_or_iterator_reconstruct_noexcept() noexcept {
				if constexpr (is_tag_invocable_v<__reconstruct_fn, _TagOrIt, _RangeOrSen>) {
					return is_nothrow_tag_invocable_v<__reconstruct_fn, _TagOrIt, _RangeOrSen>;
				}
				else if constexpr (is_specialization_of_v<remove_cvref_t<_TagOrIt>, ::std::in_place_type_t>) {
					return __range_iterator_reconstruct_noexcept<_TagOrIt, _RangeOrSen,
						decltype(ranges_adl::adl_begin(::std::declval<_RangeOrSen>())),
						decltype(ranges_adl::adl_end(::std::declval<_RangeOrSen>()))>();
				}
				else {
					return ::std::is_nothrow_constructible_v<
						subrange<remove_cvref_t<_TagOrIt>, remove_cvref_t<_RangeOrSen>>, _TagOrIt, _RangeOrSen>;
				}
			}

			template <typename _Tag, typename _It, typename _Sen>
			static constexpr bool __reconstruct_noexcept() noexcept {
				return is_tag_invocable_v<__reconstruct_fn, ::std::in_place_type_t<_Tag>, _It, _Sen>
					? is_nothrow_tag_invocable_v<__reconstruct_fn, ::std::in_place_type_t<_Tag>, _It, _Sen>
					: __range_reconstruct_or_iterator_reconstruct_noexcept<_It, _Sen>();
			}

			template <typename _Tag, typename _Range, typename _It, typename _Sen>
			static constexpr bool __range_iterator_reconstruct_noexcept() noexcept {
				return is_tag_invocable_v<__reconstruct_fn, ::std::in_place_type_t<_Tag>, _Range, _It, _Sen>
					? is_nothrow_tag_invocable_v<__reconstruct_fn, ::std::in_place_type_t<_Tag>, _Range, _It, _Sen>
					: __reconstruct_noexcept<_Tag, _It, _Sen>();
			}

		public:
			template <typename _Tag, typename _It, typename _Sen>
			constexpr auto operator()(::std::in_place_type_t<_Tag> __ty, _It&& __iterator, _Sen&& __sentinel) const
				noexcept(__reconstruct_noexcept<_Tag, _It, _Sen>()) {
				if constexpr (is_tag_invocable_v<__reconstruct_fn, ::std::in_place_type_t<_Tag>, _It, _Sen>) {
					return ::ztd::tag_invoke(
						*this, __ty, ::std::forward<_It>(__iterator), ::std::forward<_Sen>(__sentinel));
				}
				else {
					return (*this)(::std::forward<_It>(__iterator), ::std::forward<_Sen>(__sentinel));
				}
			}

			template <typename _Tag, typename _Range, typename _It, typename _Sen>
			constexpr decltype(auto) operator()(::std::in_place_type_t<_Tag> __tag, _Range&& __range, _It&& __it,
				_Sen&& __sen) const noexcept(__range_iterator_reconstruct_noexcept<_Tag, _Range, _It, _Sen>()) {
				if constexpr (is_tag_invocable_v<__reconstruct_fn, ::std::in_place_type_t<_Tag>, _Range, _It,
					              _Sen>) {
					return ::ztd::tag_invoke(*this, __tag, ::std::forward<_Range>(__range),
						::std::forward<_It>(__it), ::std::forward<_Sen>(__sen));
				}
				else {
					return (*this)(__tag, ::std::forward<_It>(__it), ::std::forward<_Sen>(__sen));
				}
			}

			template <typename _TagOrIt, typename _RangeOrSen>
			constexpr decltype(auto) operator()(
				_TagOrIt&& __tag_or_iterator, _RangeOrSen&& __range_or_sentinel) const
				noexcept(__range_reconstruct_or_iterator_reconstruct_noexcept<_TagOrIt, _RangeOrSen>()) {
				if constexpr (is_specialization_of_v<remove_cvref_t<_TagOrIt>, ::std::in_place_type_t>) {
					if constexpr (is_tag_invocable_v<__reconstruct_fn, _TagOrIt, _RangeOrSen>) {
						return ::ztd::tag_invoke(*this, ::std::forward<_TagOrIt>(__tag_or_iterator),
							::std::forward<_RangeOrSen>(__range_or_sentinel));
					}
					else {
						return (*this)(::std::forward<_TagOrIt>(__tag_or_iterator),
							::std::forward<_RangeOrSen>(__range_or_sentinel),
							ranges_adl::adl_begin(__range_or_sentinel),
							ranges_adl::adl_end(__range_or_sentinel));
					}
				}
				else if constexpr (is_tag_invocable_v<__reconstruct_fn, _TagOrIt, _RangeOrSen>) {
					return ::ztd::tag_invoke(*this, ::std::forward<_TagOrIt>(__tag_or_iterator),
						::std::forward<_RangeOrSen>(__range_or_sentinel));
				}
				else {
					return subrange<remove_cvref_t<_TagOrIt>, remove_cvref_t<_RangeOrSen>>(
						::std::forward<_TagOrIt>(__tag_or_iterator),
						::std::forward<_RangeOrSen>(__range_or_sentinel));
				}
			}
		};

	} // namespace __rng_detail

	inline namespace _c {
		//////
		/// @brief An implementation of the P1664 (https://wg21.link/p1664 |
		/// https://thephd.dev/_vendor/future_cxx/papers/d1664.html) reconstructible ranges extension point. It is
		/// derives from ztd::hijack::token in order for outside implementations to have a common place to put
		/// reconstruction for things outside of their control, without needing to place it in the global namespace or
		/// the immediate ztd::ranges namespace, where there are too many other types that could force asking more
		/// questions about what is in the list for ADL and drive up compile-times.
		inline constexpr __rng_detail::__reconstruct_fn reconstruct = {};
	} // namespace _c

	template <typename _Range, typename _It = ranges::range_iterator_t<remove_cvref_t<_Range>>,
		typename _Sen = ranges::range_sentinel_t<remove_cvref_t<_Range>>>
	using reconstruct_t
		= decltype(ranges::reconstruct(::std::declval<::std::in_place_type_t<remove_cvref_t<_Range>>>(),
		     ::std::declval<_It>(), ::std::declval<_Sen>()));

	template <typename _Range>
	using range_reconstruct_t = decltype(ranges::reconstruct(
		::std::declval<::std::in_place_type_t<remove_cvref_t<_Range>>>(), ::std::declval<_Range>()));

	template <typename _Tag, typename _Range = _Tag>
	using tag_range_reconstruct_t = decltype(ranges::reconstruct(
		::std::declval<::std::in_place_type_t<remove_cvref_t<_Tag>>>(), ::std::declval<_Range>()));

	ZTD_RANGES_INLINE_ABI_NAMESPACE_CLOSE_I_
}} // namespace ztd::ranges

namespace ztd { namespace hijack {

	template <typename _Ty, decltype(::ztd::ranges::dynamic_extent) _Extent, typename _It, typename _Sen,
		::std::enable_if_t<
		     ::ztd::ranges::is_iterator_contiguous_iterator_v<
		          _It> && ((::std::is_const_v<::std::remove_reference_t<::ztd::ranges::iterator_reference_t<_It>>>) ? std::is_const_v<_Ty> : true)>* = nullptr>
	constexpr ::ztd::ranges::span<_Ty> tag_invoke(ztd::tag_t<::ztd::ranges::reconstruct>,
		::std::in_place_type_t<::ztd::ranges::span<_Ty, _Extent>>, _It __iterator, _Sen __sentinel) noexcept {
#if ZTD_IS_ON(ZTD_STD_LIBRARY_SPAN_I_)
		return ::ztd::ranges::span<_Ty>(__iterator, __sentinel);
#else
		if constexpr (!::std::is_integral_v<_Sen>) {
			auto __iterator_address = ::ztd::idk_adl::adl_to_address(__iterator);
			auto __sentinel_address = ::ztd::idk_adl::adl_to_address(__sentinel);
			return ::ztd::ranges::span<_Ty>(__iterator_address, __sentinel_address - __iterator_address);
		}
		else {
			auto __iterator_address = ::ztd::idk_adl::adl_to_address(__iterator);
			return ::ztd::ranges::span<_Ty>(__iterator_address, __sentinel);
		}
#endif
	}

	template <typename _Ty, typename _Traits, typename _It, typename _Sen,
		::std::enable_if_t<::ztd::ranges::is_iterator_contiguous_iterator_v<_It>>* = nullptr>
	constexpr ::std::basic_string_view<_Ty, _Traits> tag_invoke(ztd::tag_t<::ztd::ranges::reconstruct>,
		::std::in_place_type_t<::std::basic_string_view<_Ty, _Traits>>, _It __iterator, _Sen __sentinel) noexcept {
		using _SizeType = typename ::std::basic_string_view<_Ty, _Traits>::size_type;
		if constexpr (!::std::is_integral_v<_Sen>) {
			_SizeType __ptr_size = static_cast<_SizeType>(__sentinel - __iterator);
#if ZTD_IS_ON(ZTD_STD_LIBRARY_DEBUG_ITERATORS_I_)
			if (__ptr_size == static_cast<_SizeType>(0)) {
				const auto& __empty_str = empty_string<_Ty>();
				return ::std::basic_string_view<_Ty, _Traits>(__empty_str + 0, 0);
			}
#endif
			return ::std::basic_string_view<_Ty, _Traits>(idk_adl::adl_to_address(__iterator), __ptr_size);
		}
		else {
#if ZTD_IS_ON(ZTD_STD_LIBRARY_DEBUG_ITERATORS_I_)
			if (static_cast<_SizeType>(__sentinel) == static_cast<_SizeType>(0)) {
				const auto& __empty_str = empty_string<_Ty>();
				return ::std::basic_string_view<_Ty, _Traits>(__empty_str + 0, 0);
			}
#endif
			return ::std::basic_string_view<_Ty, _Traits>(
				idk_adl::adl_to_address(__iterator), static_cast<_SizeType>(__sentinel));
		}
	}

}} // namespace ztd::hijack

#include <ztd/epilogue.hpp>

#endif // ZTD_RANGES_RECONSTRUCT_HPP
