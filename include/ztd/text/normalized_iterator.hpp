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

#ifndef ZTD_TEXT_NORMALIZED_ITERATOR_HPP
#define ZTD_TEXT_NORMALIZED_ITERATOR_HPP

#include <ztd/text/version.hpp>

#include <ztd/text/normalization_result.hpp>
#include <ztd/text/assert.hpp>

#include <ztd/idk/ebco.hpp>
#include <ztd/idk/unwrap.hpp>
#include <ztd/idk/type_traits.hpp>
#include <ztd/ranges/default_sentinel.hpp>
#include <ztd/ranges/range.hpp>
#include <ztd/static_containers.hpp>

#include <vector>

#include <ztd/prologue.hpp>

namespace ztd { namespace text {
	ZTD_TEXT_INLINE_ABI_NAMESPACE_OPEN_I_

	namespace __txt_detail {
		template <typename _Range, typename _NormalizationForm, typename = void>
		struct __default_normal_storage {
			using type = ::std::vector<ranges::range_value_type_t<unwrap_remove_cvref_t<_Range>>>;
		};

		template <typename _Range, typename _NormalizationForm>
		struct __default_normal_storage<_Range, _NormalizationForm,
			::std::void_t<decltype(_NormalizationForm::max_output)>> {
			using type = ::ztd::static_vector<ranges::range_value_type_t<unwrap_remove_cvref_t<_Range>>, 1>;
		};


		template <typename _Range, typename _NormalizationForm>
		using __default_normal_storage_t = typename __default_normal_storage<_Range, _NormalizationForm>::type;
	} // namespace __txt_detail

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
	/// @tparam _Storage The storage medium to use to hold output from the normalization algorithm.
	//////
	template <typename _NormalizationForm, typename _Range,
		typename _Storage = __txt_detail::__default_normal_storage_t<_Range, _NormalizationForm>>
	class normalized_iterator : private ebco<_NormalizationForm, 0>, private ebco<_Range, 1> {
	private:
		using __base_normalization_form_t = ebco<_NormalizationForm, 0>;
		using __base_range_t              = ebco<_Range, 1>;
		using _UNormalizationForm         = unwrap_remove_cvref_t<_NormalizationForm>;
		using _URange                     = unwrap_remove_cvref_t<_Range>;
		using __code_point                = ranges::range_value_type_t<_URange>;

	public:
		//////
		/// @brief The code point type.
		using value_type = __code_point;

		//////
		/// @brief The code point type.
		using range_type = _Range;

		//////
		/// @brief The code point type.
		using normalization_type = _NormalizationForm;

		//////
		/// @brief Default constructor. Defaulted.
		constexpr normalized_iterator() = default;

		//////
		/// @brief Constructs with the given `__range`.
		///
		/// @param[in] __range The range this normalization iterator will walk over.
		//////
		constexpr normalized_iterator(_Range __range) noexcept(::std::is_nothrow_move_constructible_v<_Range> // cf
			     && ::std::is_nothrow_default_constructible_v<_NormalizationForm>)
		: __base_normalization_form_t(), __base_range_t(::std::move(__range)) {
			this->_M_get_more();
		}

		//////
		/// @brief Constructs with the given `__range` and `__normalization_form`.
		///
		/// @param[in] __range The range this normalization iterator will walk over.
		/// @param[in] __normalization_form The normalization form object to use for this iterator.
		//////
		constexpr normalized_iterator(_Range __range, _NormalizationForm __normalization_form) noexcept(
			::std::is_nothrow_move_constructible_v<_NormalizationForm> // cf
			     && ::std::is_nothrow_move_constructible_v<_Range>)
		: __base_normalization_form_t(::std::move(__normalization_form)), __base_range_t(::std::move(__range)) {
			this->_M_get_more();
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
			++this->_M_cursor;
			if (this->_M_cursor == ::ztd::ranges::ranges_adl::adl_size(this->_M_normalized)) {
				this->_M_get_more();
				this->_M_cursor = 0;
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
			return __it._M_base_is_empty()
				&& __it._M_cursor == ::ztd::ranges::ranges_adl::adl_size(__it._M_normalized);
		}

		//////
		/// @brief Compares an iterator to its sentinel, which tests for whether the iteration is complete.
		///
		/// @param[in] __it The iterator to check against the sentinel.
		/// @param[in] __sen The sentinel that triggers this comparison.
		//////
		friend constexpr bool operator==(
			const normalized_sentinel_t& __sen, const normalized_iterator& __it) noexcept {
			return __it == __sen;
		}

		//////
		/// @brief Compares an iterator to its sentinel, which tests for whether the iteration is complete.
		///
		/// @param[in] __it The iterator to check against the sentinel.
		//////
		friend constexpr bool operator!=(const normalized_iterator& __it, const normalized_sentinel_t&) noexcept {
			return !__it._M_base_is_empty()
				|| __it._M_cursor != ::ztd::ranges::ranges_adl::adl_size(__it._M_normalized);
		}

		//////
		/// @brief Compares an iterator to its sentinel, which tests for whether the iteration is complete.
		///
		/// @param[in] __it The iterator to check against the sentinel.
		/// @param[in] __sen The sentinel that triggers this comparison.
		//////
		friend constexpr bool operator!=(
			const normalized_sentinel_t& __sen, const normalized_iterator& __it) noexcept {
			return __it != __sen;
		}

	private:
		constexpr bool _M_base_is_empty() const noexcept {
			const _URange& __range = this->__base_range_t::get_value();
			if constexpr (is_detected_v<ranges::detect_adl_empty, _Range>) {
				return ranges::ranges_adl::adl_empty(__range);
			}
			else {
				return ranges::ranges_adl::adl_begin(__range) == ranges::ranges_adl::adl_end(__range);
			}
		}

		constexpr void _M_get_more() noexcept {
			using _Unbounded = ::ztd::ranges::unbounded_view<::std::back_insert_iterator<_Storage>>;
			auto& __norm     = this->__base_normalization_form_t::get_value();
			auto& __range    = this->__base_range_t::get_value();
			this->_M_normalized.clear();
			_Unbounded __output(::std::back_inserter(this->_M_normalized));
			auto __result = __norm(__range, __output);
			__range       = ::std::move(__result.input);
			ZTD_TEXT_ASSERT(__result.error_code == normalization_error::ok);
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

#endif // ZTD_TEXT_NORMALIZED_ITERATOR_HPP
