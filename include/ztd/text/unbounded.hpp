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

#ifndef ZTD_TEXT_UNBOUNDED_HPP
#define ZTD_TEXT_UNBOUNDED_HPP

#include <ztd/text/detail/range.hpp>

#include <iterator>
#include <utility>

namespace ztd { namespace text {
	ZTD_TEXT_INLINE_ABI_NAMESPACE_OPEN_I_

	//////
	/// @addtogroup ztd_text_support Support Classes
	/// @{
	//////

	//////
	/// @brief A sentinel that cannot compare equal to any other iterator and thus results in infinitely long ranges.
	///
	//////
	struct infinity_sentinel_t {
		//////
		/// @brief Equality comparison. Always false.
		///
		//////
		template <typename _Left>
		friend constexpr bool operator==(const _Left&, const infinity_sentinel_t&) {
			return false;
		}

		//////
		/// @brief Equality comparison. Always false.
		///
		//////
		template <typename _Right>
		friend constexpr bool operator==(const infinity_sentinel_t&, const _Right&) {
			return false;
		}

		//////
		/// @brief Inequality comparison. Always true.
		///
		//////
		template <typename _Left>
		friend constexpr bool operator!=(const _Left&, const infinity_sentinel_t&) {
			return true;
		}

		//////
		/// @brief Inequality comparison. Always true.
		///
		//////
		template <typename _Right>
		friend constexpr bool operator!=(const infinity_sentinel_t&, const _Right&) {
			return true;
		}
	};

	//////
	/// @brief An available and usable ztd::text::infinity_sentinel_t for ease of use.
	///
	//////
	inline constexpr infinity_sentinel_t infinity_sentinel = {};

	//////
	/// @brief A class whose iterator and sentinel denote an infinity-range that, if iterated with a traditional for
	/// range loop, will never cease.
	//////
	template <typename _It>
	class unbounded_view {
	private:
		_It _M_it;

	public:
		//////
		/// @brief The iterator type.
		///
		//////
		using iterator = _It;
		//////
		/// @brief The iterator type that can iterate indefinitely (or some approximation thereof).
		///
		//////
		using const_iterator = iterator;
		//////
		/// @brief The sentinel type, an infinity type that compares equal to nothing.
		///
		//////
		using sentinel = infinity_sentinel_t;
		//////
		/// @brief The const sentinel type.
		///
		/// @remarks It's just the sentinal type.
		//////
		using const_sentinel = sentinel;
		//////
		/// @brief The pointer type related to the iterator.
		///
		//////
		using pointer = __txt_detail::__iterator_pointer_t<iterator>;
		//////
		/// @brief The const pointer type related to the iterator.
		///
		/// @remarks It's just the pointer type.
		//////
		using const_pointer = pointer;
		//////
		/// @brief The reference type for this range.
		///
		//////
		using reference = __txt_detail::__iterator_reference_t<iterator>;
		//////
		/// @brief The const reference type for this range.
		///
		/// @remarks It's just the reference type.
		//////
		using const_reference = reference;
		//////
		/// @brief The value type for this range.
		///
		//////
		using value_type = __txt_detail::__iterator_value_type_t<iterator>;
		//////
		/// @brief The difference type that results from iterator subtraction (not practical for this range).
		///
		//////
		using difference_type = __txt_detail::__iterator_pointer_t<iterator>;

		//////
		/// @brief Constructs a default-constructed iterator and an infinity sentinel as the range.
		///
		/// @remarks Not very useful for anything other than generic programming shenanigans.
		//////
		constexpr unbounded_view() noexcept = default;

		//////
		/// @brief Constructs an unbounded_view using the specified iterator value iterator and an infinity sentinel.
		///
		//////
		constexpr unbounded_view(iterator __it) noexcept : _M_it(::std::move(__it)) {
		}

		//////
		/// @brief The iterator the unbounded_view was constructed with.
		///
		//////
		constexpr iterator begin() const noexcept {
			return this->_M_it;
		}

		//////
		/// @brief The ending sentinel.
		///
		/// @remarks The sentinel is an infinity sentinel that never compares equal to any other thing: in short, any
		/// range composed of [iterator, infinity_sentinel) will never cease.
		//////
		constexpr sentinel end() const noexcept {
			return sentinel {};
		}

		//////
		/// @brief The reconstruct extension point for re-creating this type from its iterator and sentinel.
		///
		///
		//////
		constexpr friend unbounded_view reconstruct(
			::std::in_place_type_t<unbounded_view>, iterator __iterator, sentinel) {
			return unbounded_view<_It>(::std::move(__iterator));
		}
	};

	//////
	/// @}
	//////

	ZTD_TEXT_INLINE_ABI_NAMESPACE_CLOSE_I_
}} // namespace ztd::text

#endif // ZTD_TEXT_UNBOUNDED_HPP
