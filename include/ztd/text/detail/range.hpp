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
// =============================================================================

#pragma once

#ifndef ZTD_TEXT_DETAIL_RANGE_HPP
#define ZTD_TEXT_DETAIL_RANGE_HPP

#include <ztd/text/version.hpp>

#include <ztd/text/detail/adl.hpp>
#include <ztd/text/detail/type_traits.hpp>

#include <iterator>
#include <type_traits>
#include <utility>
#include <array>

namespace ztd { namespace text {
	ZTD_TEXT_INLINE_ABI_NAMESPACE_OPEN_I_

#ifdef __cpp_lib_concepts
	using contiguous_iterator_tag = ::std::contiguous_iterator_tag;
#else
	class contiguous_iterator_tag : public ::std::random_access_iterator_tag { };
#endif
	namespace __detail {

		template <typename _It>
		constexpr auto __dereference(_It&& __it) noexcept(noexcept(*::std::forward<_It>(__it)))
			-> decltype(*::std::forward<_It>(__it)) {
			return *::std::forward<_It>(__it);
		}

		template <typename _It>
		constexpr auto __next(_It __it) noexcept(noexcept(++__it)) {
			++__it;
			return __it;
		}

		template <typename _It, typename _Diff>
		constexpr _It __next(_It __it, _Diff __diff) noexcept(noexcept(++__it)) {
			for (; __diff > 0; --__diff) {
				++__it;
			}
			return __it;
		}

		template <typename _It>
		constexpr auto __prev(_It __it) noexcept(noexcept(--__it)) {
			--__it;
			return __it;
		}

		template <typename _It>
		using __iterator_value_type_t = typename ::std::iterator_traits<::std::remove_reference_t<_It>>::value_type;

		template <typename _It>
		using __iterator_pointer_t = typename ::std::iterator_traits<::std::remove_reference_t<_It>>::pointer;

		template <typename _It>
		using __iterator_reference_t = typename ::std::iterator_traits<::std::remove_reference_t<_It>>::reference;

		template <typename _It>
		using __iterator_difference_type_t =
			typename ::std::iterator_traits<::std::remove_reference_t<_It>>::difference_type;

		template <typename _It>
		using __iterator_category_t =
			typename ::std::iterator_traits<::std::remove_reference_t<_It>>::iterator_category;

		template <typename _It, typename = void>
		struct __iterator_concept_or_fallback {
			using type = ::std::conditional_t<::std::is_pointer_v<__remove_cvref_t<_It>>, contiguous_iterator_tag,
				__iterator_category_t<_It>>;
		};

		template <typename _It>
		struct __iterator_concept_or_fallback<_It,
			::std::void_t<typename ::std::iterator_traits<::std::remove_reference_t<_It>>::iterator_concept>> {
			using type = typename ::std::iterator_traits<::std::remove_reference_t<_It>>::iterator_concept;
		};

		template <typename _It>
		using __iterator_concept_or_fallback_t =
			typename __iterator_concept_or_fallback<::std::remove_reference_t<_It>>::type;

		template <typename _It>
		using __iterator_concept_t = __iterator_concept_or_fallback_t<_It>;

		template <typename _Tag, typename _It>
		inline constexpr bool __is_iterator_concept_or_better_v
			= ::std::is_base_of_v<_Tag, __iterator_concept_t<_It>>;

		template <typename _Range>
		using __range_iterator_t
			= decltype(__adl::__adl_begin(::std::declval<::std::add_lvalue_reference_t<_Range>>()));

		template <typename _Range>
		using __range_sentinel_t
			= decltype(__adl::__adl_end(::std::declval<::std::add_lvalue_reference_t<_Range>>()));

		template <typename _Range>
		using __range_const_iterator_t
			= decltype(__adl::__adl_begin(::std::declval<::std::add_lvalue_reference_t<_Range>>()));

		template <typename _Range>
		using __range_const_sentinel_t
			= decltype(__adl::__adl_end(::std::declval<::std::add_lvalue_reference_t<_Range>>()));

		template <typename _Range>
		using __range_value_type_t = __iterator_value_type_t<__range_iterator_t<_Range>>;

		template <typename _Range>
		using __range_pointer_t = __iterator_pointer_t<__range_iterator_t<_Range>>;

		template <typename _Range>
		using __range_reference_t = __iterator_reference_t<__range_iterator_t<_Range>>;

		template <typename _Range>
		using __range_difference_type_t = __iterator_difference_type_t<__range_iterator_t<_Range>>;

		template <typename _Range>
		using __range_iterator_category_t = __iterator_category_t<__range_iterator_t<_Range>>;

		template <typename _Range>
		using __range_iterator_concept_t = __iterator_concept_t<__range_iterator_t<_Range>>;

		template <typename _Tag, typename _Range>
		inline constexpr bool __is_range_iterator_concept_or_better_v
			= ::std::is_base_of_v<_Tag, __range_iterator_concept_t<_Range>>;

	} // namespace __detail

	ZTD_TEXT_INLINE_ABI_NAMESPACE_CLOSE_I_
}} // namespace ztd::text

#endif // ZTD_TEXT_DETAIL_RANGE_HPP
