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

#ifndef ZTD_TEXT_DETAIL_ITERATOR_HPP
#define ZTD_TEXT_DETAIL_ITERATOR_HPP

#include <ztd/text/version.hpp>

#include <ztd/text/detail/adl.hpp>
#include <ztd/text/detail/type_traits.hpp>
#include <ztd/text/detail/memory.hpp>

#include <iterator>
#include <type_traits>
#include <utility>

#if ZTD_TEXT_IS_ON(ZTD_TEXT_STD_LIBRARY_RANGES_I_)
#include <ranges>
#endif

#include <ztd/text/detail/prologue.hpp>

namespace ztd { namespace text {
	ZTD_TEXT_INLINE_ABI_NAMESPACE_OPEN_I_

#if ZTD_TEXT_IS_ON(ZTD_TEXT_STD_LIBRARY_CONTIGUOUS_ITERATOR_TAG_I_)
	using contiguous_iterator_tag = ::std::contiguous_iterator_tag;
#else
	class contiguous_iterator_tag : public ::std::random_access_iterator_tag { };
#endif

	namespace __txt_detail {

		template <typename _Type, typename... _Args>
		using __detect_next = decltype(::std::declval<_Type>().next(::std::declval<_Args>()...));

		template <typename _Type, typename... _Args>
		using __detect_prev = decltype(::std::declval<_Type>().next(::std::declval<_Args>()...));

		template <typename _Type, typename... _Args>
		using __detect_advance = decltype(::std::declval<_Type>().advance(::std::declval<_Args>()...));

		template <typename _Type, typename... _Args>
		using __detect_recede = decltype(::std::declval<_Type>().recede(::std::declval<_Args>()...));

		template <typename _Type>
		using __detect_lvalue_increment = decltype(++::std::declval<_Type&>());

		template <typename _Type>
		using __detect_lvalue_decrement = decltype(--::std::declval<_Type&>());

		template <typename _It, typename _Sen, typename = void>
		struct __is_distance_operable : ::std::false_type { };

		template <typename _It, typename _Sen>
		struct __is_distance_operable<_It, _Sen,
			::std::void_t<decltype(::std::declval<_Sen>() - ::std::declval<_It>())>>
		: ::std::integral_constant<bool,
			  ::std::is_convertible_v<decltype(::std::declval<_Sen>() - ::std::declval<_It>()),
			       __iterator_difference_type_t<_It>>> { };

		template <typename _It, typename _Sen>
		inline constexpr bool __is_distance_operable_v = __is_distance_operable<_It, _Sen>::value;

		template <typename _It, typename = void>
		struct __iterator_category_failure {
			using type = ::std::conditional_t<::std::is_pointer_v<__remove_cvref_t<_It>>, contiguous_iterator_tag,
				::std::output_iterator_tag>;
		};

		template <typename _It>
		struct __iterator_category_failure<_It,
			::std::void_t<typename ::std::remove_reference_t<_It>::iterator_category>> {
			using type = typename ::std::remove_reference_t<_It>::iterator_category;
		};

		template <typename _It, typename = void>
		struct __iterator_category_or_fallback {
			using type = typename __iterator_category_failure<_It>::type;
		};

		template <typename _It>
		struct __iterator_category_or_fallback<_It,
			::std::void_t<typename ::std::iterator_traits<::std::remove_reference_t<_It>>::iterator_category>> {
			using type = typename ::std::iterator_traits<::std::remove_reference_t<_It>>::iterator_category;
		};

		template <typename _It, typename = void>
		struct __iterator_concept_failure {
			using type = ::std::conditional_t<::std::is_pointer_v<__remove_cvref_t<_It>>, contiguous_iterator_tag,
				::std::output_iterator_tag>;
		};

		template <typename _It>
		struct __iterator_concept_failure<_It,
			::std::void_t<typename ::std::remove_reference_t<_It>::iterator_concept>> {
			using type = typename ::std::remove_reference_t<_It>::iterator_concept;
		};

		template <typename _It, typename = void>
		struct __iterator_concept_or_fallback {
			using type = typename __iterator_concept_failure<_It>::type;
		};

		template <typename _It>
		struct __iterator_concept_or_fallback<_It,
			::std::void_t<typename ::std::iterator_traits<::std::remove_reference_t<_It>>::iterator_concept>> {
			using type = typename ::std::iterator_traits<::std::remove_reference_t<_It>>::iterator_concept;
		};

		template <typename _It, typename = void>
		struct __iterator_category_or_concept_or_fallback {
		private:
			using _MaybeType = typename __iterator_category_or_fallback<_It>::type;

		public:
			using type = ::std::conditional_t<::std::is_same_v<_MaybeType, ::std::output_iterator_tag>,
				typename __iterator_concept_or_fallback<__remove_cvref_t<_It>>::type, _MaybeType>;
		};

		template <typename _It>
		struct __iterator_category_or_concept_or_fallback<_It,
			::std::void_t<typename ::std::iterator_traits<::std::remove_reference_t<_It>>::iterator_category>> {
			using type = typename ::std::iterator_traits<::std::remove_reference_t<_It>>::iterator_category;
		};

		template <typename _It, typename = void>
		struct __iterator_concept_or_category_or_fallback {
		private:
			using _MaybeType = typename __iterator_concept_or_fallback<_It>::type;

		public:
			using type = ::std::conditional_t<::std::is_same_v<_MaybeType, ::std::output_iterator_tag>,
				typename __iterator_category_or_fallback<__remove_cvref_t<_It>>::type, _MaybeType>;
		};

		template <typename _It>
		struct __iterator_concept_or_category_or_fallback<_It,
			::std::void_t<typename ::std::iterator_traits<::std::remove_reference_t<_It>>::iterator_concept>> {
			using type = typename ::std::iterator_traits<::std::remove_reference_t<_It>>::iterator_concept;
		};

		template <typename _It, typename = void>
		struct __iterator_pointer_or_fallback {
		private:
			using _Reference = __iterator_reference_t<::std::remove_reference_t<_It>>;

		public:
			using type = ::std::conditional_t<::std::is_reference_v<_Reference>,
				::std::add_pointer_t<::std::remove_reference_t<_Reference>>, void>;
		};

		template <typename _It>
		struct __iterator_pointer_or_fallback<_It,
			::std::void_t<typename ::std::iterator_traits<::std::remove_reference_t<_It>>::pointer>> {
			using type = typename ::std::iterator_traits<::std::remove_reference_t<_It>>::pointer;
		};

		template <typename _It>
		using __iterator_rvalue_reference_t = decltype(__adl::__adl_iter_move(::std::declval<_It&>()));

		template <typename _It>
		using __iterator_pointer_t = typename __iterator_pointer_or_fallback<::std::remove_reference_t<_It>>::type;

		template <typename _It>
		using __iterator_category_t =
			typename __iterator_category_or_concept_or_fallback<::std::remove_reference_t<_It>>::type;

		template <typename _It>
		using __iterator_concept_or_fallback_t =
			typename __iterator_concept_or_category_or_fallback<::std::remove_reference_t<_It>>::type;

		template <typename _It>
		using __iterator_concept_t = __iterator_concept_or_fallback_t<_It>;

		template <typename _Tag, typename _It>
		inline constexpr bool __is_iterator_concept_or_better_v
			= ::std::is_base_of_v<_Tag, __iterator_concept_t<_It>>;

		template <typename _It>
		inline constexpr bool __is_iterator_contiguous_iterator_v = (
#if ZTD_TEXT_IS_ON(ZTD_TEXT_STD_LIBRARY_CONTIGUOUS_ITERATOR_TAG_I_)
			__is_iterator_concept_or_better_v<contiguous_iterator_tag, _It> &&
#else
			::std::is_pointer_v<_It> &&
#endif
			__is_to_addressable_v<
			     _It> && ::std::is_lvalue_reference_v<__iterator_reference_t<::std::remove_reference_t<_It>>>);

		template <typename _It>
		inline constexpr bool __is_iterator_input_iterator_v
			= ::std::is_same_v<::std::input_iterator_tag, __iterator_concept_t<_It>>;

		template <typename _It>
		inline constexpr bool __is_iterator_output_iterator_v
			= ::std::is_same_v<::std::output_iterator_tag, __iterator_concept_t<_It>>;

		template <typename _It>
		inline constexpr bool __is_iterator_input_or_output_iterator_v
			= __is_iterator_input_iterator_v<_It> || __is_iterator_output_iterator_v<_It>;

		template <typename _It, typename _Sen>
		inline constexpr bool __is_sized_sentinel_for_v = __is_distance_operable_v<_It, _Sen>;

		template <typename _It>
		constexpr auto __dereference(_It&& __it) noexcept(noexcept(*::std::forward<_It>(__it)))
			-> decltype(*::std::forward<_It>(__it)) {
			return *::std::forward<_It>(__it);
		}

		template <typename _It, typename... _Args>
		constexpr bool __advance_noexcept() noexcept {
			if constexpr (__is_detected_v<__detect_advance, _It, _Args...>) {
				return noexcept(::std::declval<_It>().advance(::std::declval<_Args>()...));
			}
			else {
				return noexcept(++::std::declval<::std::add_lvalue_reference_t<::std::remove_reference_t<_It>>>());
			}
		}

		template <typename _It, typename... _Args>
		constexpr bool __recede_noexcept() noexcept {
			if constexpr (__is_detected_v<__detect_recede, _It, _Args...>) {
				return noexcept(::std::declval<_It>().recede(::std::declval<_Args>()...));
			}
			else {
				return noexcept(--::std::declval<::std::add_lvalue_reference_t<::std::remove_reference_t<_It>>>());
			}
		}

		template <typename _It>
		constexpr _It&& __advance(_It&& __it) noexcept(__advance_noexcept<_It>()) {
			if constexpr (__is_detected_v<__detect_advance, _It>) {
				::std::forward<_It>(__it).advance();
			}
			else {
				++__it;
			}
			return ::std::forward<_It>(__it);
		}

		template <typename _It, typename _Diff>
		constexpr _It&& __advance(_It&& __it, _Diff __diff) noexcept(__advance_noexcept<_It, _Diff>()) {
			if constexpr (__is_detected_v<__detect_advance, _It, _Diff>) {
				::std::forward<_It>(__it).advance(__diff);
			}
			else {
				if constexpr (__is_iterator_concept_or_better_v<::std::random_access_iterator_tag,
					              __remove_cvref_t<_It>>) {
					__it += __diff;
				}
				else {
					for (; __diff > 0; --__diff) {
						++__it;
					}
				}
			}
			return ::std::forward<_It>(__it);
		}

		template <typename _It>
		constexpr _It&& __recede(_It&& __it) noexcept(__recede_noexcept<_It>()) {
			if constexpr (__is_detected_v<__detect_recede, _It>) {
				::std::forward<_It>(__it).recede();
			}
			else {
				--__it;
			}
			return ::std::forward<_It>(__it);
		}

		template <typename _It, typename _Diff>
		constexpr _It&& __recede(_It&& __it, _Diff __diff) noexcept(__recede_noexcept<_It, _Diff>()) {
			if constexpr (__is_detected_v<__detect_recede, _It, _Diff>) {
				::std::forward<_It>(__it).recede(__diff);
			}
			else {
				if constexpr (__is_iterator_concept_or_better_v<::std::random_access_iterator_tag,
					              __remove_cvref_t<_It>>) {
					__it -= __diff;
				}
				else {
					for (; __diff > 0; --__diff) {
						--__it;
					}
				}
				return ::std::forward<_It>(__it);
			}
		}
	} // namespace __txt_detail

	ZTD_TEXT_INLINE_ABI_NAMESPACE_CLOSE_I_
}} // namespace ztd::text

#include <ztd/text/detail/epilogue.hpp>

#endif // ZTD_TEXT_DETAIL_ITERATOR_HPP
