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

#ifndef ZTD_TEXT_DETAIL_RECONSTRUCT_CPO_HPP
#define ZTD_TEXT_DETAIL_RECONSTRUCT_CPO_HPP

#include <ztd/text/version.hpp>

#include <ztd/text/subrange.hpp>

#include <ztd/text/detail/range.hpp>
#include <ztd/text/detail/adl.hpp>
#include <ztd/text/detail/type_traits.hpp>
#include <ztd/text/detail/reconstruct.types.hpp>

#include <iterator>
#include <utility>
#include <type_traits>

#include <ztd/text/detail/prologue.hpp>

namespace ztd { namespace text {
	ZTD_TEXT_INLINE_ABI_NAMESPACE_OPEN_I_

	namespace __txt_detail {

		template <typename _It, typename _Sen>
		constexpr auto __adl_iterator_reconstruct(_It&& __iterator, _Sen&& __sentinel) noexcept(
			noexcept(reconstruct(::std::declval<_It>(), ::std::declval<_Sen>())))
			-> decltype(reconstruct(::std::declval<_It>(), ::std::declval<_Sen>())) {
			return reconstruct(::std::forward<_It>(__iterator), ::std::forward<_Sen>(__sentinel));
		}

		template <typename _Tag, typename _It, typename _Sen>
		constexpr auto __adl_reconstruct(::std::in_place_type_t<_Tag> __ty, _It&& __iterator,
			_Sen&& __sentinel) noexcept(noexcept(reconstruct(__ty, ::std::declval<_It>(), ::std::declval<_Sen>())))
			-> decltype(reconstruct(__ty, ::std::declval<_It>(), ::std::declval<_Sen>())) {
			return reconstruct(__ty, ::std::forward<_It>(__iterator), ::std::forward<_Sen>(__sentinel));
		}

		template <typename _Tag, typename _InRange, typename _It, typename _Sen>
		constexpr auto __adl_range_iterator_reconstruct(::std::in_place_type_t<_Tag> __ty, _InRange&& __in_range,
			_It&& __iterator,
			_Sen&& __sentinel) noexcept(noexcept(reconstruct(__ty, ::std::forward<_InRange>(__in_range))))
			-> decltype(reconstruct(__ty, ::std::forward<_InRange>(__in_range), ::std::forward<_It>(__iterator),
			     ::std::forward<_Sen>(__sentinel))) {
			return reconstruct(__ty, ::std::forward<_InRange>(__in_range), ::std::forward<_It>(__iterator),
				::std::forward<_Sen>(__sentinel));
		}

		template <typename _Tag, typename _Range>
		constexpr auto __adl_range_reconstruct(::std::in_place_type_t<_Tag> __ty, _Range&& __in_range) noexcept(
			noexcept(reconstruct(__ty, ::std::forward<_Range>(__in_range))))
			-> decltype(reconstruct(__ty, ::std::forward<_Range>(__in_range))) {
			return reconstruct(__ty, ::std::forward<_Range>(__in_range));
		}

		template <typename _It, typename _Sen>
		using __detect_is_iterator_reconstructible_range
			= decltype(__adl_iterator_reconstruct(::std::declval<_It>(), ::std::declval<_Sen>()));

		template <typename _Tag, typename _It, typename _Sen>
		using __detect_is_reconstructible_range = decltype(__adl_reconstruct(
			::std::in_place_type<__remove_cvref_t<_Tag>>, ::std::declval<_It>(), ::std::declval<_Sen>()));

		template <typename _Tag, typename _Range, typename _It, typename _Sen>
		using __detect_is_range_iterator_reconstructible_range
			= decltype(__adl_range_iterator_reconstruct(::std::in_place_type<__remove_cvref_t<_Tag>>,
			     ::std::declval<_Range>(), ::std::declval<_It>(), ::std::declval<_Sen>()));

		template <typename _Tag, typename _Range>
		using __detect_is_range_reconstructible_range = decltype(__adl_range_reconstruct(
			::std::in_place_type<__remove_cvref_t<_Tag>>, ::std::declval<_Range>()));

		template <typename _It, typename _Sen>
		using __is_iterator_reconstructible_range = ::std::integral_constant<bool,
			__is_detected<__detect_is_iterator_reconstructible_range, _It, _Sen>::value
			     && (::std::is_class_v<__remove_cvref_t<
			               _It>> || std::is_enum_v<__remove_cvref_t<_It>> || ::std::is_class_v<__remove_cvref_t<_Sen>> || std::is_enum_v<__remove_cvref_t<_Sen>>)>;

		template <typename _Tag, typename _It, typename _Sen>
		using __is_reconstructible_range = __is_detected<__detect_is_reconstructible_range, _Tag, _It, _Sen>;

		template <typename _Tag, typename _Range, typename _It, typename _Sen>
		using __is_range_iterator_reconstructible_range
			= __is_detected<__detect_is_range_iterator_reconstructible_range, _Tag, _Range, _It, _Sen>;

		template <typename _Tag, typename _Range>
		using __is_range_reconstructible_range = __is_detected<__detect_is_range_reconstructible_range, _Tag, _Range>;

		template <typename _It, typename _Sen>
		inline constexpr bool __is_iterator_reconstructible_range_v
			= __is_iterator_reconstructible_range<_It, _Sen>::value;

		template <typename _Tag, typename _It, typename _Sen>
		inline constexpr bool __is_reconstructible_range_v = __is_reconstructible_range<_Tag, _It, _Sen>::value;

		template <typename _Tag, typename _Range, typename _It, typename _Sen>
		inline constexpr bool __is_range_iterator_reconstructible_range_v
			= __is_range_iterator_reconstructible_range<_Tag, _Range, _It, _Sen>::value;

		template <typename _Tag, typename _Range>
		inline constexpr bool __is_range_reconstructible_range_v
			= __is_range_reconstructible_range<_Tag, _Range>::value;

		template <typename _It, typename _Sen, typename = void>
		class __is_nothrow_iterator_reconstructible_range : public ::std::false_type { };

		template <typename _It, typename _Sen>
		class __is_nothrow_iterator_reconstructible_range<_It, _Sen,
			::std::enable_if_t<__is_iterator_reconstructible_range_v<_It, _Sen>>>
		: public ::std::integral_constant<bool,
			  noexcept(__adl_iterator_reconstruct(::std::declval<_It>(), ::std::declval<_Sen>()))> { };

		template <typename _It, typename _Sen>
		inline constexpr bool __is_nothrow_iterator_reconstructible_range_v
			= __is_nothrow_iterator_reconstructible_range<_It, _Sen>::value;

		template <typename _Tag, typename _It, typename _Sen, typename = void>
		class __is_nothrow_reconstructible_range : public ::std::false_type { };

		template <typename _Tag, typename _It, typename _Sen>
		class __is_nothrow_reconstructible_range<_Tag, _It, _Sen,
			::std::enable_if_t<__is_reconstructible_range_v<_Tag, _It, _Sen>>>
		: public ::std::integral_constant<bool,
			  noexcept(__adl_reconstruct(::std::in_place_type<__remove_cvref_t<_Tag>>, ::std::declval<_It>(),
			       ::std::declval<_Sen>()))> { };

		template <typename _Tag, typename _It, typename _Sen>
		inline constexpr bool __is_nothrow_reconstructible_range_v
			= __is_nothrow_reconstructible_range<_Tag, _It, _Sen>::value;

		template <typename _Tag, typename _Range, typename _It, typename _Sen, typename = void>
		class __is_nothrow_range_iterator_reconstructible_range : public ::std::false_type { };

		template <typename _Tag, typename _Range, typename _It, typename _Sen>
		class __is_nothrow_range_iterator_reconstructible_range<_Tag, _Range, _It, _Sen,
			::std::enable_if_t<__is_range_iterator_reconstructible_range_v<_Tag, _Range, _It, _Sen>>>
		: public ::std::integral_constant<bool,
			  noexcept(__adl_range_iterator_reconstruct(::std::in_place_type<__remove_cvref_t<_Tag>>,
			       ::std::declval<_Range>(), ::std::declval<_It>(), ::std::declval<_Sen>()))> { };

		template <typename _Tag, typename _Range, typename _It, typename _Sen>
		inline constexpr bool __is_nothrow_range_iterator_reconstructible_range_v
			= __is_nothrow_range_iterator_reconstructible_range<_Tag, _Range, _It, _Sen>::value;

		template <typename _Tag, typename _Range, typename = void>
		class __is_nothrow_range_reconstructible_range : public ::std::false_type { };

		template <typename _Tag, typename _Range>
		class __is_nothrow_range_reconstructible_range<_Tag, _Range,
			::std::enable_if_t<__is_range_reconstructible_range_v<_Tag, _Range>>>
		: public ::std::integral_constant<bool,
			  noexcept(__adl_range_reconstruct(
			       ::std::in_place_type<__remove_cvref_t<_Tag>>, ::std::declval<_Range>()))> { };

		template <typename _Tag, typename _Range>
		inline constexpr bool __is_nothrow_range_reconstructible_range_v
			= __is_nothrow_range_reconstructible_range<_Tag, _Range>::value;

		struct __reconstruct_cpo {
		private:
			template <typename _Tag, typename _It, typename _Sen>
			static constexpr bool __reconstruct_noexcept() noexcept {
				if constexpr (__is_reconstructible_range_v<_Tag, _It, _Sen>) {
					return __is_nothrow_reconstructible_range_v<_Tag, _It, _Sen>;
				}
				else {
					return __range_reconstruct_or_iterator_reconstruct_noexcept<_It, _Sen>();
				}
			}

			template <typename _Tag, typename _Range, typename _It, typename _Sen>
			static constexpr bool __range_iterator_reconstruct_noexcept() noexcept {
				if constexpr (__is_range_iterator_reconstructible_range_v<_Tag, _Range, _It, _Sen>) {
					return __is_nothrow_range_iterator_reconstructible_range_v<_Tag, _Range, _It, _Sen>;
				}
				else {
					return __reconstruct_noexcept<_Tag, _It, _Sen>();
				}
			}

			template <typename _TagOrIt, typename _RangeOrSen>
			static constexpr bool __range_reconstruct_or_iterator_reconstruct_noexcept() noexcept {
				if constexpr (__is_specialization_of_v<__remove_cvref_t<_TagOrIt>, ::std::in_place_type_t>) {
					if constexpr (__is_range_reconstructible_range_v<_TagOrIt, _RangeOrSen>) {
						return __is_nothrow_range_reconstructible_range_v<_TagOrIt, _RangeOrSen>;
					}
					else {
						using _It  = __range_iterator_t<_RangeOrSen>;
						using _Sen = __range_sentinel_t<_RangeOrSen>;
						return __range_iterator_reconstruct_noexcept<_TagOrIt, _RangeOrSen, _It, _Sen>();
					}
				}
				else {
					if constexpr (__is_iterator_reconstructible_range_v<_TagOrIt, _RangeOrSen>) {
						return __is_nothrow_iterator_reconstructible_range_v<_TagOrIt, _RangeOrSen>;
					}
					else if (::std::is_constructible_v<
						         ::ztd::text::subrange<__remove_cvref_t<_TagOrIt>, __remove_cvref_t<_RangeOrSen>>,
						         _TagOrIt, _RangeOrSen>) {
						return ::std::is_nothrow_constructible_v<
							::ztd::text::subrange<__remove_cvref_t<_TagOrIt>, __remove_cvref_t<_RangeOrSen>>,
							_TagOrIt, _RangeOrSen>;
					}
					else {
						return false;
					}
				}
			}

		public:
			template <typename _Tag, typename _It, typename _Sen>
			constexpr auto operator()(::std::in_place_type_t<_Tag> __ty, _It&& __iterator, _Sen&& __sentinel) const
				noexcept(__reconstruct_noexcept<_Tag, _It, _Sen>()) {
				if constexpr (__is_reconstructible_range_v<_Tag, _It, _Sen>) {
					return __adl_reconstruct(
						__ty, ::std::forward<_It>(__iterator), ::std::forward<_Sen>(__sentinel));
				}
				else {
					return (*this)(::std::forward<_It>(__iterator), ::std::forward<_Sen>(__sentinel));
				}
			}

			template <typename _Tag, typename _Range, typename _It, typename _Sen>
			constexpr decltype(auto) operator()(::std::in_place_type_t<_Tag> __tag, _Range&& __range, _It&& __it,
				_Sen&& __sen) const noexcept(__range_iterator_reconstruct_noexcept<_Tag, _Range, _It, _Sen>()) {
				if constexpr (__is_range_iterator_reconstructible_range_v<_Tag, _Range, _It, _Sen>) {
					return __adl_range_iterator_reconstruct(__tag, ::std::forward<_Range>(__range),
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
				if constexpr (__is_specialization_of_v<__remove_cvref_t<_TagOrIt>, ::std::in_place_type_t>) {
					if constexpr (__is_range_reconstructible_range_v<_TagOrIt, _RangeOrSen>) {
						return __adl_range_reconstruct(
							__tag_or_iterator, ::std::forward<_RangeOrSen>(__range_or_sentinel));
					}
					else {
						return (*this)(__tag_or_iterator, ::std::forward<_RangeOrSen>(__range_or_sentinel),
							__adl::__adl_begin(__range_or_sentinel), __adl::__adl_end(__range_or_sentinel));
					}
				}
				else if constexpr (__is_iterator_reconstructible_range_v<_TagOrIt, _RangeOrSen>) {
					return __adl_iterator_reconstruct(::std::forward<_TagOrIt>(__tag_or_iterator),
						::std::forward<_RangeOrSen>(__range_or_sentinel));
				}
				else {
					return subrange<__remove_cvref_t<_TagOrIt>, __remove_cvref_t<_RangeOrSen>>(
						::std::forward<_TagOrIt>(__tag_or_iterator),
						::std::forward<_RangeOrSen>(__range_or_sentinel));
				}
			}
		} inline constexpr __reconstruct = {};

		template <typename _Tag, typename _It = __range_iterator_t<__remove_cvref_t<_Tag>>,
			typename _Sen = __range_sentinel_t<__remove_cvref_t<_Tag>>>
		using __reconstruct_t = decltype(__reconstruct(::std::in_place_type<__remove_cvref_t<_Tag>>,
			::std::declval<__remove_cvref_t<_It>>(), ::std::declval<__remove_cvref_t<_Sen>>()));

		template <typename _Range, typename _Tag = __remove_cvref_t<_Range>>
		using __range_reconstruct_t = decltype(__reconstruct(::std::in_place_type<_Tag>, ::std::declval<_Range>()));

	} // namespace __txt_detail
	ZTD_TEXT_INLINE_ABI_NAMESPACE_CLOSE_I_
}} // namespace ztd::text

#include <ztd/text/detail/epilogue.hpp>

#endif // ZTD_TEXT_DETAIL_RECONSTRUCT_CPO_HPP
