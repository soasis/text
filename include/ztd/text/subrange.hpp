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

#ifndef ZTD_TEXT_SUBRANGE_HPP
#define ZTD_TEXT_SUBRANGE_HPP

#include <ztd/text/detail/range.hpp>

#include <ztd/text/detail/type_traits.hpp>

#include <iterator>
#include <utility>

namespace ztd { namespace text {
	ZTD_TEXT_INLINE_ABI_NAMESPACE_OPEN_I_

	//////
	/// @addtogroup ztd_text_support Support Classes
	/// @{
	//////

	//////
	/// @brief An enumeration that helps determine whether a subrange has size information or not.
	///
	//////
	enum class subrange_kind : bool {
		//////
		/// @brief Does not have a size (or does not have a size that can be computed in O(1)).
		///
		//////
		unsized,
		//////
		/// @brief Has a size that can be computed in O(1).
		///
		//////
		sized
	};

	//////
	/// @brief A utility class to aid in trafficking iterator pairs (or, possibly, and iterator and sentinel pair)
	/// through the API to provide a generic, basic "range" type. Attempts to mimic @c std::ranges::subrange on
	/// platforms where it is not available.
	///
	/// @tparam _It The iterator type.
	/// @tparam _Sen The sentinel type, defaulted to @p _It.
	/// @tparam _Kind Whether or not this is a "Sized Subrange": that is, that a calculation for the size of the
	/// subrange can be done in O(1) time and is available.
	//////
	template <typename _It, typename _Sen = _It,
		subrange_kind _Kind = __detail::__is_iterator_concept_or_better_v<::std::random_access_iterator_tag,
		                           _It>&& ::std::is_same_v<_It, _Sen>
		     ? subrange_kind::sized
		     : subrange_kind::unsized>
	class subrange {
	public:
		//////
		/// @brief The @c iterator type for this subrange, dictated by the template parameter @p _It.
		///
		//////
		using iterator = _It;
		//////
		/// @brief The @c const_iterator type for this subrange, dictated by the template parameter @p _It.
		///
		//////
		using const_iterator = iterator;
		//////
		/// @brief The @c sentinel type for this subrange, dictated by the template parameter @p _Sen.
		///
		//////
		using sentinel = _Sen;
		//////
		/// @brief The @c const_sentinel type for this subrange, dictated by the template parameter @p _Sen.
		///
		//////
		using const_sentinel = sentinel;
		//////
		/// @brief The iterator category. Same as the iterator category for @p _It.
		///
		//////
		using iterator_category = __detail::__iterator_category_t<iterator>;
		//////
		/// @brief The iterator concept. Same as the iterator concept for @p _It.
		///
		//////
		using iterator_concept = __detail::__iterator_concept_t<iterator>;
		//////
		/// @brief The @c pointer type. Same as the @c pointer type for @p _It.
		///
		//////
		using pointer = __detail::__iterator_pointer_t<iterator>;
		//////
		/// @brief The @c const_pointer type. Same as the @c const_pointer type for @p _It.
		///
		//////
		using const_pointer = pointer;
		//////
		/// @brief The @c reference type. Same as the @c reference type for @p _It.
		///
		//////
		using reference = __detail::__iterator_reference_t<iterator>;
		//////
		/// @brief The @c const_reference type. Same as the @c const_reference type for @p _It.
		///
		//////
		using const_reference = reference;
		//////
		/// @brief The @c value_type. Same as the @c value_type for @p _It.
		///
		//////
		using value_type = __detail::__iterator_value_type_t<iterator>;
		//////
		/// @brief The @c difference_type. Same as the @c difference_type for @p _It.
		///
		//////
		using difference_type = ::std::conditional_t<::std::is_same_v<iterator_concept, ::std::output_iterator_tag>,
			ptrdiff_t, __detail::__iterator_difference_type_t<iterator>>;
		//////
		/// @brief The @c size_type. Same as the @c size_type for @p _It.
		///
		//////
		using size_type = ::std::make_unsigned_t<difference_type>;

		//////
		/// @brief Constructs a ztd::text::subrange containing a defaulted iterator and a defaulted sentinel.
		///
		//////
		constexpr subrange() noexcept = default;

		//////
		/// @brief Constructs a ztd::text::subrange with its begin and end constructed by @p __range's @c begin() and
		/// @c end() values.
		///
		/// @param[in] __range The Range to get the @c begin() and @c end() out of to initialize the subrange's
		/// iterators.
		//////
		template <typename _Range,
			::std::enable_if_t<!::std::is_same_v<__detail::__remove_cvref_t<_Range>, subrange>>* = nullptr>
		constexpr subrange(_Range&& __range) noexcept
		: subrange(__detail::__adl::__adl_begin(__range), __detail::__adl::__adl_end(__range)) {
		}

		//////
		/// @brief Constructs a ztd::text::subrange with its begin and end constructed by @p __range's @c begin() and
		/// @c end() values.
		///
		/// @param[in] __it An iterator value to @c std::move in.
		/// @param[in] __sen A sentinel value to @c std::move in.
		//////
		constexpr subrange(iterator __it, sentinel __sen) noexcept
		: _M_it(::std::move(__it)), _M_sen(::std::move(__sen)) {
		}

		//////
		/// @brief The stored begin iterator.
		///
		//////
		constexpr iterator begin() const noexcept {
			return this->_M_it;
		}

		//////
		/// @brief The stored end iterator.
		///
		//////
		constexpr sentinel end() const noexcept {
			return this->_M_sen;
		}

		//////
		/// @brief Whether or not this range is empty.
		///
		/// @returns @c begin() == @c end()
		//////
		constexpr bool empty() const noexcept {
			return this->_M_it == this->_M_sen;
		}

		//////
		/// @brief The size of the range.
		///
		/// @returns @c "std::distance(begin(), end())".
		///
		/// @remarks This function call only works if the @p _Kind of this subrange is
		/// ztd::text::subrange_kind::sized.
		//////
		template <subrange_kind _Dummy = _Kind, ::std::enable_if_t<_Dummy == subrange_kind::sized>* = nullptr>
		constexpr size_type size() const noexcept {
			return ::std::distance(this->_M_it, this->_M_sen);
		}

		//////
		/// @brief A @c pointer to the range of elements.
		///
		/// @returns @c std::addressof(*begin()).
		///
		/// @remarks This function call only works if the @c iterator_concept is a @c contiguous_iterator_tag or
		/// better.
		//////
		template <typename _Dummy = _It,
			::std::enable_if_t<
			     __detail::__is_iterator_concept_or_better_v<contiguous_iterator_tag, _Dummy>>* = nullptr>
		constexpr pointer data() const noexcept {
			return ::std::addressof(*this->_M_it);
		}

	private:
		iterator _M_it;
		sentinel _M_sen;
	};

	namespace __detail {
		template <typename _Range>
		using __subrange_for_t = subrange<__range_iterator_t<_Range>, __range_sentinel_t<_Range>>;
	} // namespace __detail

	//////
	/// @brief Decomposes a range into its two iterators and returns it as a ztd::text::subrange.
	///
	//////
	template <typename _Range>
	constexpr __detail::__subrange_for_t<_Range> make_subrange(_Range&& __range) noexcept(
		::std::is_nothrow_constructible_v<_Range, __detail::__subrange_for_t<_Range>>) {
		return { __detail::__adl::__adl_begin(__range), __detail::__adl::__adl_end(__range) };
	}

	//////
	/// @brief Takes two iterators and returns them as a ztd::text::subrange.
	///
	//////
	template <typename _It, typename _Sen>
	constexpr subrange<_It, _Sen> make_subrange(_It&& __it, _Sen&& __sen) noexcept(
		::std::is_nothrow_constructible_v<subrange<_It, _Sen>, _It, _Sen>) {
		return { ::std::forward<_It>(__it), ::std::forward<_Sen>(__sen) };
	}

	//////
	/// @}
	//////

	ZTD_TEXT_INLINE_ABI_NAMESPACE_CLOSE_I_
}} // namespace ztd::text

#endif // ZTD_TEXT_SUBRANGE_HPP
