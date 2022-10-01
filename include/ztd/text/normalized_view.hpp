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

#ifndef ZTD_TEXT_NORMALIZE_VIEW_HPP
#define ZTD_TEXT_NORMALIZE_VIEW_HPP

#include <ztd/text/version.hpp>

#include <ztd/text/normalized_iterator.hpp>

#include <ztd/idk/unwrap.hpp>

#include <vector>

#include <ztd/prologue.hpp>

namespace ztd { namespace text {
	ZTD_TEXT_INLINE_ABI_NAMESPACE_OPEN_I_

	//////
	/// @addtogroup ztd_text_ranges Ranges, Views, and Iterators
	///
	/// @{

	template <typename _NormalizationForm, typename _Range,
		typename _Storage = ::std::vector<ranges::range_value_type_t<unwrap_remove_cvref_t<_Range>>>>
	class normalized_view {
	private:
		using _UNormalizationForm = unwrap_remove_cvref_t<_NormalizationForm>;
		using _URange             = unwrap_remove_cvref_t<_Range>;

	public:
		//////
		/// @brief The iterator type for this view.
		using iterator = normalized_iterator<_NormalizationForm, _Range, _Storage>;
		//////
		/// @brief The sentinel type for this view.
		using sentinel = normalized_sentinel_t;
		//////
		/// @brief The underlying range type.
		using range_type = _Range;
		//////
		/// @brief The encoding type used for transformations.
		using normalization_form = _NormalizationForm;

		//////
		/// @brief Constructs with the given `__normalization_form`.
		///
		/// @param[in] __normalization_form The normalization form object to use for this iterator.
		constexpr normalized_view(normalization_form __normalization_form) noexcept(
			::std::is_nothrow_constructible_v<iterator, normalization_form>)
		: _M_it(::std::move(__normalization_form)) {
		}

		//////
		/// @brief Constructs with the given `__normalization_form`.
		///
		/// @param[in] __normalization_form The normalization form object to use for this iterator.
		/// @param[in] __range The range this normalization iterator will walk over.
		constexpr normalized_view(normalization_form __normalization_form, range_type __range) noexcept(
			::std::is_nothrow_constructible_v<iterator, normalization_form, range_type>)
		: _M_it(::std::move(__normalization_form), ::std::move(__range)) {
		}

		//////
		/// @brief Constructs from one of its iterators, reconstituting the range.
		///
		/// @param[in] __it A previously-made normalized_view iterator.
		constexpr normalized_view(iterator __it) noexcept(::std::is_nothrow_move_constructible_v<iterator>)
		: _M_it(::std::move(__it)) {
		}

		//////
		/// @brief Default constructor. Defaulted.
		constexpr normalized_view() = default;

		//////
		/// @brief Copy constructor. Defaulted.
		constexpr normalized_view(const normalized_view&) = default;

		//////
		/// @brief Move constructor. Defaulted.
		constexpr normalized_view(normalized_view&&) = default;

		//////
		/// @brief Copy assignment operator. Defaulted.
		constexpr normalized_view& operator=(const normalized_view&) = default;
		//////
		/// @brief Move assignment operator. Defaulted.
		constexpr normalized_view& operator=(normalized_view&&) = default;

		//////
		/// @brief The beginning of the range. Uses a sentinel type and not a special iterator.
		constexpr iterator begin() & noexcept {
			if constexpr (::std::is_copy_constructible_v<iterator>) {
				return this->_M_it;
			}
			else {
				return ::std::move(this->_M_it);
			}
		}

		//////
		/// @brief The beginning of the range. Uses a sentinel type and not a special iterator.
		constexpr iterator begin() const& noexcept {
			return this->_M_it;
		}

		//////
		/// @brief The beginning of the range. Uses a sentinel type and not a special iterator.
		constexpr iterator begin() && noexcept {
			return ::std::move(this->_M_it);
		}

		//////
		/// @brief The end of the range. Uses a sentinel type and not a special iterator.
		constexpr sentinel end() const noexcept {
			return sentinel();
		}

	private:
		iterator _M_it;
	};

	//////
	/// @}

	ZTD_TEXT_INLINE_ABI_NAMESPACE_CLOSE_I_
}} // namespace ztd::text

#include <ztd/epilogue.hpp>

#endif // ZTD_TEXT_NORMALIZE_VIEW_HPP
