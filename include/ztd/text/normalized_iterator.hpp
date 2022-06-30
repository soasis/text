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

#include <ztd/ranges/default_sentinel.hpp>
#include <ztd/idk/ebco.hpp>
#include <ztd/idk/type_traits.hpp>

#include <array>

#include <ztd/prologue.hpp>

namespace ztd { namespace text {
	ZTD_TEXT_INLINE_ABI_NAMESPACE_OPEN_I_

	//////
	/// @addtogroup ztd_text_ranges Ranges, Views, and Iterators
	/// @{
	//////

	using normalized_sentinel_t = ranges::default_sentinel_t;

	template <typename _NormalizationForm, typename _Range>
	class normalized_iterator : private ebco<_NormalizationForm, 0>, private ebco<_Range, 1> {
	private:
		using __base_normalization_form = ebco<_NormalizationForm, 0>;
		using __base_range              = ebco<_Range, 1>;
		using _UNormalizationForm       = remove_cvref_t<unwrap_t<_NormalizationForm>>;
		using _URange                   = remove_cvref_t<unwrap_t<_Range>>;

	public:
		constexpr normalized_iterator() = default;
		constexpr normalized_iterator(_NormalizationForm __normalization_form) noexcept(
			::std::is_nothrow_move_constructible_v<_NormalizationForm> // cf-hack
			     && ::std::is_nothrow_default_constructible_v<_Range>)
		: __base_normalization_form(::std::move(__normalization_form)), __base_range() {
		}
		constexpr normalized_iterator(_NormalizationForm __normalization_form, _Range __range) noexcept(
			::std::is_nothrow_move_constructible_v<_NormalizationForm> // cf-hack
			     && ::std::is_nothrow_move_constructible_v<_Range>)
		: __base_normalization_form(::std::move(__normalization_form)), __base_range(::std::move(__range)) {
		}

		//////
		/// @brief Default constructor. Defaulted.
		constexpr normalized_iterator() = default;

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
	};

	//////
	/// @}
	//////

	ZTD_TEXT_INLINE_ABI_NAMESPACE_CLOSE_I_
}} // namespace ztd::text

#include <ztd/epilogue.hpp>

#endif // ZTD_TEXT_NORMALIZE_ITERATOR_HPP
