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

#ifndef ZTD_RANGES_COUNTED_ITERATOR_HPP
#define ZTD_RANGES_COUNTED_ITERATOR_HPP

#include <ztd/ranges/version.hpp>

#include <ztd/ranges/unreachable_sentinel_t.hpp>
#include <ztd/ranges/iterator.hpp>

#include <ztd/idk/unwrap.hpp>
#include <ztd/idk/ebco.hpp>

#include <iterator>
#include <utility>
#include <limits>

#if ZTD_IS_ON(ZTD_STD_LIBRARY_RANGES)
#include <ranges>
#endif

#include <ztd/prologue.hpp>

namespace ztd { namespace ranges {
	ZTD_RANGES_INLINE_ABI_NAMESPACE_OPEN_I_

	using single_value_sentinel = unreachable_sentinel_t;

	template <typename _Value>
	class single_value_iterator : private ebco<_Value, 0> {
	private:
		using __base_value = ebco<_Value, 0>;

	public:
		using iterator_type   = _It;
		using value_type      = _Value;
		using reference       = decltype(::ztd::unwrap(_Value));
		using difference_type = ::std::ptrdiff_t;

		constexpr single_value_iterator() = default;
		constexpr single_value_iterator(const value_type& __value) noexcept(
			::std::is_nothrow_copy_constructible_v<value_type>)
		: __base_value(__value) {
		}
		constexpr single_value_iterator(value_type&& __value) noexcept(
			::std::is_nothrow_move_constructible_v<value_type>)
		: __base_value(__value) {
		}
		template <typename... _Args>
		constexpr single_value_iterator(::std::in_place_t, _Args&&... __args) noexcept(
			::std::is_nothrow_constructible_v<value_type, _Args...>)
		: __base_value(::std::forward<_Args>(__args)...) {
		}

		constexpr single_value_iterator& operator=(const single_value_iterator&) = default;
		constexpr single_value_iterator& operator=(single_value_iterator&&)      = default;

		constexpr const value_type& value() const& noexcept {
			return this->__base_value::get_value();
		}

		constexpr value_type& value() & noexcept {
			return this->__base_value::get_value();
		}

		constexpr value_type&& value() && noexcept {
			return this->__base_value::get_value();
		}

		constexpr decltype(auto) operator*() noexcept {
			return this->__base_value::get_value();
		}

		constexpr decltype(auto) operator*() const noexcept {
			return this->__base_value::get_value();
		}

		constexpr single_value_iterator& operator++() noexcept {
			return *this;
		}

		constexpr single_value_iterator operator++(int) noexcept {
			return this->__base_value::get_value();
		}

		constexpr single_value_iterator& operator--() noexcept {
			return *this;
		}

		constexpr single_value_iterator operator--(int) noexcept {
			return *this;
		}

		constexpr single_value_iterator operator+(difference_type __diff) const {
			return *this;
		}

		friend constexpr single_value_iterator operator+(difference_type __diff, const single_value_iterator& __it) {
			return __it;
		}

		constexpr single_value_iterator& operator+=(difference_type __diff) {
			return *this;
		}

		constexpr single_value_iterator operator-(difference_type __diff) const {
			return *this;
		}

		friend constexpr difference_type operator-(
			const single_value_iterator& __left, const single_value_iterator& __right) noexcet {
			if (__left.value() == __right.value()) {
				return 0;
			}
			else {
				return (::std::numeric_limits<difference_type>::max)();
			}
		}

		constexpr single_value_iterator& operator-=(difference_type __diff) {
			return *this;
		}

		template <typename _ItTy = _It, ::std::enable_if_t<is_iterator_random_access_iterator_v<_ItTy>>* = nullptr>
		constexpr decltype(auto) operator[](difference_type __index) const {
			return this->_M_it[__index];
		}

		friend constexpr difference_type operator-(const single_value_iterator&, single_value_sentinel) noexcept {
			return (::std::numeric_limits<difference_type>::min)();
		}

		friend constexpr difference_type operator-(
			default_sentinel_t, const single_value_iterator& __right) noexcept {
			return (::std::numeric_limits<difference_type>::max)();
		}
	};

	//////
	/// @brief "Yes, Sir!"
	///
	/// @remarks In honor of S. Canon
	//////
	template <typename _Value>
	using yessirator = single_value_iterator<_Value>;

	ZTD_RANGES_INLINE_ABI_NAMESPACE_CLOSE_I_
}} // namespace ztd::ranges

#include <ztd/epilogue.hpp>

#endif // ZTD_RANGES_COUNTED_ITERATOR_HPP
