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

#ifndef ZTD_TEXT_DETAIL_RANGE_HPP
#define ZTD_TEXT_DETAIL_RANGE_HPP

#include <ztd/text/version.hpp>

#include <ztd/text/detail/adl.hpp>
#include <ztd/text/detail/type_traits.hpp>
#include <ztd/text/detail/iterator.hpp>

#include <iterator>
#include <type_traits>
#include <utility>

#if ZTD_TEXT_IS_ON(ZTD_TEXT_STD_LIBRARY_RANGES_I_)
#include <ranges>
#endif

#include <ztd/text/detail/prologue.hpp>

namespace ztd { namespace text {
	ZTD_TEXT_INLINE_ABI_NAMESPACE_OPEN_I_

	namespace __txt_detail {
#if ZTD_TEXT_IS_ON(ZTD_TEXT_STD_LIBRARY_RANGES_I_)
		template <typename _Range>
		using __range_iterator_t = ::std::ranges::iterator_t<_Range>;

		template <typename _Range>
		using __range_sentinel_t = ::std::ranges::sentinel_t<_Range>;

		template <typename _Range>
		using __range_value_type_t = ::std::ranges::range_value_t<_Range>;

		template <typename _Range>
		using __range_reference_t = ::std::ranges::range_reference_t<_Range>;

		template <typename _Range>
		using __range_rvalue_reference_t = ::std::ranges::range_rvalue_reference_t<_Range>;

		template <typename _Range>
		using __range_difference_type_t = ::std::ranges::range_difference_t<_Range>;

		template <typename _Range>
		using __range_size_type_t = ::std::ranges::range_size_t<_Range>;
#else
		template <typename _Range>
		using __range_iterator_t = ::std::remove_reference_t<decltype(
			__adl::__adl_begin(::std::declval<::std::add_lvalue_reference_t<_Range>>()))>;

		template <typename _Range>
		using __range_sentinel_t = ::std::remove_reference_t<decltype(
			__adl::__adl_end(::std::declval<::std::add_lvalue_reference_t<_Range>>()))>;

		template <typename _Range>
		using __range_value_type_t = __iterator_value_type_t<__range_iterator_t<_Range>>;

		template <typename _Range>
		using __range_reference_t = __iterator_reference_t<__range_iterator_t<_Range>>;

		template <typename _Range>
		using __range_rvalue_reference_t = __iterator_rvalue_reference_t<__range_iterator_t<_Range>>;

		template <typename _Range>
		using __range_difference_type_t = __iterator_difference_type_t<__range_iterator_t<_Range>>;

		template <typename _Range>
		using __range_size_type_t = __iterator_size_type_t<__range_iterator_t<_Range>>;
#endif

		template <typename _Range>
		using __range_const_iterator_t
			= decltype(__adl::__adl_cbegin(::std::declval<::std::add_lvalue_reference_t<_Range>>()));

		template <typename _Range>
		using __range_const_sentinel_t
			= decltype(__adl::__adl_cend(::std::declval<::std::add_lvalue_reference_t<_Range>>()));

		template <typename _Range>
		using __range_pointer_t = __iterator_pointer_t<__range_iterator_t<_Range>>;

		template <typename _Range>
		using __range_iterator_category_t = __iterator_category_t<__range_iterator_t<_Range>>;

		template <typename _Range>
		using __range_iterator_concept_t = __iterator_concept_t<__range_iterator_t<_Range>>;

		template <typename _Tag, typename _Range>
		inline constexpr bool __is_range_iterator_concept_or_better_v
			= ::std::is_base_of_v<_Tag, __range_iterator_concept_t<_Range>>;

		template <typename _Range>
		inline constexpr bool __is_range_input_or_output_range_v
			= __is_iterator_input_or_output_iterator_v<__range_iterator_t<_Range>>;

		template <typename _Range>
		inline constexpr bool __is_range_contiguous_range_v
			= __is_iterator_contiguous_iterator_v<__range_iterator_t<_Range>>;

		template <typename _Range>
		inline constexpr bool __is_sized_range_v
			= __is_sized_sentinel_for_v<__range_iterator_t<_Range>, __range_sentinel_t<_Range>>;

		template <typename _Range, typename _Element>
		using __detect_push_back = decltype(::std::declval<_Range>().push_back(::std::declval<_Element>()));

		template <typename _Range, typename _IterFirst, typename _IterLast = _IterFirst>
		using __detect_insert_bulk
			= decltype(::std::declval<_Range>().insert(__adl::__adl_begin(::std::declval<_Range>()),
			     ::std::declval<_IterFirst>(), ::std::declval<_IterLast>()));

	} // namespace __txt_detail

	ZTD_TEXT_INLINE_ABI_NAMESPACE_CLOSE_I_
}} // namespace ztd::text

#include <ztd/text/detail/epilogue.hpp>

#endif // ZTD_TEXT_DETAIL_RANGE_HPP
