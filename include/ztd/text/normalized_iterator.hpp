// =============================================================================
//
// ztd.text
// Copyright © 2022 JeanHeyd "ThePhD" Meneide and Shepherd's Oasis, LLC
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
// ============================================================================ //

#pragma once

#ifndef ZTD_TEXT_NORMALIZE_ITERATOR_HPP
#define ZTD_TEXT_NORMALIZE_ITERATOR_HPP

#include <ztd/text/version.hpp>

#include <ztd/idk/ebco.hpp>
#include <ztd/idk/unwrap.hpp>
#include <ztd/idk/type_traits.hpp>
#include <ztd/ranges/default_sentinel.hpp>

#include <vector>

#include <ztd/prologue.hpp>

namespace ztd { namespace text {
	ZTD_TEXT_INLINE_ABI_NAMESPACE_OPEN_I_

	//////
	/// @addtogroup ztd_text_ranges Ranges, Views, and Iterators
	/// @{
	//////

	using normalized_sentinel_t = ranges::default_sentinel_t;

	//////
	/// @brief An iterator that walks over the code points of a sequence as-if they were normalized.
	///
	/// @tparam _NormalizationForm The normalization form to apply to the sequence of code points.
	/// @tparam _Range The sequence of code points to iterate over.
	//////
	template <typename _NormalizationForm, typename _Range,
		typename _Storage = ::std::vector<ranges::range_value_type_t<remove_cvref_t<unwrap_t<_Range>>>>>
	class normalized_iterator : private ebco<_NormalizationForm, 0>, private ebco<_Range, 1> {
	private:
		using __base_normalization_form = ebco<_NormalizationForm, 0>;
		using __base_range              = ebco<_Range, 1>;
		using _UNormalizationForm       = remove_cvref_t<unwrap_t<_NormalizationForm>>;
		using _URange                   = remove_cvref_t<unwrap_t<_Range>>;
		using __code_point              = ranges::range_value_type_t<_URange>;

	public:
		//////
		/// @brief The code point type.
		using value_type = __code_point;

		//////
		/// @brief Default constructor. Defaulted.
		constexpr normalized_iterator() = default;

		//////
		/// @brief Constructs with the given `__normalization_form`.
		///
		/// @param[in] __normalization_form The normalization form object to use for this iterator.
		//////
		constexpr normalized_iterator(_NormalizationForm __normalization_form) noexcept(
			::std::is_nothrow_move_constructible_v<_NormalizationForm> // cf
			     && ::std::is_nothrow_default_constructible_v<_Range>)
		: __base_normalization_form(::std::move(__normalization_form)), __base_range() {
		}

		//////
		/// @brief Constructs with the given `__normalization_form`.
		///
		/// @param[in] __normalization_form The normalization form object to use for this iterator.
		/// @param[in] __range The range this normalization iterator will walk over.
		//////
		constexpr normalized_iterator(_NormalizationForm __normalization_form, _Range __range) noexcept(
			::std::is_nothrow_move_constructible_v<_NormalizationForm> // cf
			     && ::std::is_nothrow_move_constructible_v<_Range>)
		: __base_normalization_form(::std::move(__normalization_form)), __base_range(::std::move(__range)) {
		}

		//////
		/// @brief Copy constructor. Defaulted.
		constexpr normalized_iterator(const normalized_iterator&) = default;

		//////
		/// @brief Move constructor. Defaulted.
		constexpr normalized_iterator(normalized_iterator&&) = default;

		//////
		/// @brief Copy assignment operator. Defaulted.
		constexpr normalized_iterator& operator=(const normalized_iterator&) = default;
		//////
		/// @brief Move assignment operator. Defaulted.
		constexpr normalized_iterator& operator=(normalized_iterator&&) = default;

		//////
		/// @brief Retrieves the next code point in the normalized view of the underlying range.
		constexpr normalized_iterator& operator++() noexcept {
			if (this->_M_cursor == this->_M_normalized.size()) {
				this->_M_get_more();
			}
			return *this;
		}

		//////
		/// @brief The current code point in the normalized range.
		constexpr value_type operator*() const noexcept {
			return this->_M_normalized[static_cast<::std::size_t>(this->_M_cursor)];
		}

		//////
		/// @brief Compares an iterator to its sentinel, which tests for whether the iteration is complete.
		///
		/// @param[in] __it The iterator to check against the sentinel.
		//////
		friend constexpr bool operator==(const normalized_iterator& __it, const normalized_sentinel_t&) noexcept {
			const _URange& __range = __it.__base_range::get_value();
			return ::ztd::ranges::ranges_adl::adl_empty(__range);
		}

		//////
		/// @brief Compares an iterator to its sentinel, which tests for whether the iteration is complete.
		///
		/// @param[in] __it The iterator to check against the sentinel.
		//////
		friend constexpr bool operator==(const normalized_sentinel_t&, const normalized_iterator& __it) noexcept {
			const _URange& __range = __it.__base_range::get_value();
			return ::ztd::ranges::ranges_adl::adl_empty(__range);
		}

		//////
		/// @brief Compares an iterator to its sentinel, which tests for whether the iteration is complete.
		///
		/// @param[in] __it The iterator to check against the sentinel.
		//////
		friend constexpr bool operator!=(const normalized_iterator& __it, const normalized_sentinel_t&) noexcept {
			const _URange& __range = __it.__base_range::get_value();
			return !::ztd::ranges::ranges_adl::adl_empty(__range);
		}

		//////
		/// @brief Compares an iterator to its sentinel, which tests for whether the iteration is complete.
		///
		/// @param[in] __it The iterator to check against the sentinel.
		//////
		friend constexpr bool operator!=(const normalized_sentinel_t&, const normalized_iterator& __it) noexcept {
			const _URange& __range = __it.__base_range::get_value();
			return !::ztd::ranges::ranges_adl::adl_empty(__range);
		}

	private:
		constexpr void _M_get_more() noexcept {
		}

		_Storage _M_normalized;
		unsigned char _M_cursor = 0;
	};

	//////
	/// @}
	//////

	ZTD_TEXT_INLINE_ABI_NAMESPACE_CLOSE_I_
}} // namespace ztd::text

#include <ztd/epilogue.hpp>

#endif // ZTD_TEXT_NORMALIZE_ITERATOR_HPP
