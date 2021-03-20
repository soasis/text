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

#ifndef ZTD_TEXT_DETAIL_BLACKHOLE_ITERATOR_HPP
#define ZTD_TEXT_DETAIL_BLACKHOLE_ITERATOR_HPP

#include <ztd/text/version.hpp>

#include <ztd/text/detail/type_traits.hpp>

#include <tuple>
#include <iterator>
#include <cstddef>

#include <ztd/text/detail/prologue.hpp>

namespace ztd { namespace text {
	ZTD_TEXT_INLINE_ABI_NAMESPACE_OPEN_I_
	namespace __txt_detail {

	// A more aggressive form of std::ignore,
	// this form includes a constructor which takes everything
	// and does nothing, making it truly the devourer of all things.
	struct __blackhole {
		constexpr __blackhole()                   = default;
		constexpr __blackhole(const __blackhole&) = default;
		constexpr __blackhole(__blackhole&&)      = default;
		template <typename _Right,
			::std::enable_if_t<!::std::is_same_v<__remove_cvref_t<_Right>, __blackhole>>* = nullptr>
		constexpr __blackhole(_Right&&) noexcept {
		}

		constexpr __blackhole& operator=(const __blackhole&) = default;
		constexpr __blackhole& operator=(__blackhole&&) = default;
		template <typename _Right,
			::std::enable_if_t<!::std::is_same_v<__remove_cvref_t<_Right>, __blackhole>>* = nullptr>
		constexpr __blackhole& operator=(_Right&&) noexcept {
			return *this;
		}
	};

	class __blackhole_iterator {
	public:
		using iterator_category = ::std::output_iterator_tag;
		using difference_type   = ::std::ptrdiff_t;
		using pointer           = __blackhole*;
		using value_type        = __blackhole;
		using reference         = __blackhole;

		constexpr __blackhole_iterator operator++(int) const {
			auto __copy = *this;
			++__copy;
			return __copy;
		}

		constexpr __blackhole_iterator& operator+=(difference_type) {
			return *this;
		}

		constexpr __blackhole_iterator& operator-=(difference_type) {
			return *this;
		}

		constexpr __blackhole_iterator operator+(difference_type) {
			return *this;
		}

		constexpr __blackhole_iterator operator-(difference_type) {
			return *this;
		}

		constexpr difference_type operator-(__blackhole_iterator) {
			return 0;
		}

		constexpr __blackhole_iterator& operator++() {
			return *this;
		}

		constexpr __blackhole_iterator operator--(int) const {
			return *this;
		}

		constexpr __blackhole_iterator& operator--() {
			return *this;
		}

		constexpr reference operator*() const {
			return reference {};
		}
	};

}ZTD_TEXT_INLINE_ABI_NAMESPACE_CLOSE_I_
}} // namespace ztd::text::__txt_detail

#include <ztd/text/detail/epilogue.hpp>

#endif // ZTD_TEXT_DETAIL_BLACKHOLE_ITERATOR_HPP
