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

#ifndef ZTD_TEXT_DETAIL_WORD_ITERATOR_HPP
#define ZTD_TEXT_DETAIL_WORD_ITERATOR_HPP

#include <ztd/text/version.hpp>

#include <ztd/text/endian.hpp>

#include <ztd/text/detail/range.hpp>
#include <ztd/text/detail/type_traits.hpp>
#include <ztd/text/detail/memory.hpp>
#include <ztd/text/detail/to_underlying.hpp>

#include <cstddef>
#include <limits>
#include <climits>
#include <cstring>
#include <memory>

namespace ztd { namespace text {
	ZTD_TEXT_INLINE_ABI_NAMESPACE_OPEN_I_
	namespace __detail {

		template <typename _It, typename _Word, endian _Endian>
		class __word_reference {
		public:
			using value_type = _Word;

		private:
		private:
			using __base_iterator              = _It;
			using __maybe_void_base_value_type = __detail::__iterator_value_type_t<__base_iterator>;
			using __base_value_type            = ::std::conditional_t<::std::is_void_v<__maybe_void_base_value_type>,
                    ::std::byte, __maybe_void_base_value_type>;
			using __underlying_base_value_type = decltype(__any_to_underlying(__base_value_type {}));
			using __underlying_value_type      = decltype(__any_to_underlying(value_type {}));
			inline static constexpr ::std::size_t __base_values_per_word
				= sizeof(value_type) / sizeof(__base_value_type);

		public:
			constexpr __word_reference(__base_iterator __it) : _M_base_it(::std::move(__it)) {
			}

			constexpr __word_reference& operator=(value_type __val) noexcept {
#if ZTD_TEXT_IS_ON(ZTD_TEXT_STD_LIBRARY_IS_CONSTANT_EVALUATED_I_)
				if (::std::is_constant_evaluated()) {
					// God's given, handwritten, bit-splittin'
					// one-way """memcpy""". 😵
					__underlying_value_type __bit_value = __any_to_underlying(__val);
					auto __base_it                      = this->_M_base_it;
					for (::std::size_t __index = 0; __index < __base_values_per_word; ++__index) {
						__underlying_value_type __bit_position = static_cast<__underlying_value_type>(
							__index * (sizeof(__underlying_base_value_type) * CHAR_BIT));
						__underlying_base_value_type __shifted_bit_value
							= static_cast<__underlying_base_value_type>(__bit_value >> __bit_position);
						*__base_it = static_cast<__base_value_type>(__shifted_bit_value);
						++__base_it;
					}
				}
				else
#endif
				{
					if constexpr (_Endian == endian::native) {
						__base_value_type __storage[__base_values_per_word];
						::std::memcpy(__storage, ::std::addressof(__val), sizeof(value_type));
						::std::copy_n(__storage, __adl::__adl_size(__storage), this->_M_base_it);
					}
					else if (_Endian == endian::little) {
						__base_value_type __storage[__base_values_per_word];
						::std::memcpy(__storage, ::std::addressof(__val), sizeof(value_type));
						::std::copy_n(__storage, __adl::__adl_size(__storage), this->_M_base_it);
					}
					else {
						// copy... as-is, I guess?
						__base_value_type __storage[__base_values_per_word];
						auto __first_it = __storage + 0;
						auto __last_it  = __storage + __base_values_per_word;
						::std::memcpy(__storage, ::std::addressof(__val), sizeof(value_type));
						::std::copy_backward(__first_it, __last_it, this->_M_base_it);
					}
				}
				return *this;
			}

			constexpr value_type value() const noexcept {
				if constexpr (_Endian == endian::native
					&& (endian::native != endian::big && endian::native != endian::little)) {
					static_assert(__always_false_constant_v<endian, _Endian>,
						"read value from byte stream to native endianness that is neither little nor big (byte "
						"order is impossible to infer from the standard)");
				}
#if ZTD_TEXT_IS_ON(ZTD_TEXT_STD_LIBRARY_IS_CONSTANT_EVALUATED_I_)
				if (::std::is_constant_evaluated()) {
					__base_value_type __storage[__base_values_per_word] {};
					__underlying_value_type __val = __any_to_underlying(value_type {});
					if constexpr (_Endian == endian::little) {
						::std::copy_n(this->_M_base_it, __adl::__adl_size(__storage), __storage);
					}
					else {
						static_assert(_Endian == endian::big);
						auto __last_it = ::std::next(this->_M_base_it, __adl::__adl_size(__storage));
						::std::copy_backward(this->_M_base_it, __last_it, __storage);
					}
					// God's given, handwritten, bit-fusin'
					// one-way """memcpy""". 😵
					for (::std::size_t __index = 0; __index < __base_values_per_word; ++__index) {
						__underlying_value_type __bit_value
							= static_cast<__underlying_value_type>(__any_to_underlying(__storage[__index]));
						__underlying_value_type __bit_position = static_cast<__underlying_value_type>(
							__index * (sizeof(__underlying_base_value_type) * CHAR_BIT));
						__underlying_value_type __shifted_bit_value = (__bit_value << __bit_position);
						__val |= __shifted_bit_value;
					}
					return static_cast<value_type>(__val);
				}
				else
#endif
				{
					__base_value_type __storage[__base_values_per_word];
					value_type __val;
					if constexpr (_Endian == endian::big) {
						auto __last_it = ::std::next(this->_M_base_it, __adl::__adl_size(__storage));
						::std::copy_backward(this->_M_base_it, __last_it, __storage);
					}
					else {
						static_assert(_Endian == endian::little);
						::std::copy_n(this->_M_base_it, __adl::__adl_size(__storage), __storage);
					}
					::std::memcpy(
						::std::addressof(__val), ::std::addressof(__storage), __adl::__adl_size(__storage));
					return __val;
				}
			}

			constexpr operator value_type() const {
				return this->value();
			}

		private:
			__base_iterator _M_base_it;
		};

		template <typename _It>
		class __word_sentinel {
		public:
			using sentinel_type = _It;

			__word_sentinel() = default;
			constexpr __word_sentinel(sentinel_type __sen) : _M_base_sen(::std::move(__sen)) {
			}

			constexpr sentinel_type& base() & {
				return this->_M_base_sen;
			}

			constexpr const sentinel_type& base() const& {
				return this->_M_base_sen;
			}

			constexpr sentinel_type&& base() && {
				return ::std::move(this->_M_base_sen);
			}

		private:
			sentinel_type _M_base_sen;
		};

		template <typename _Derived, typename _Word, typename _It, endian _Endian, typename = void>
		class __category_word_iterator {
		private:
			using __base_iterator   = _It;
			using __sentinel        = __word_sentinel<_It>;
			using __base_reference  = __detail::__iterator_reference_t<__base_iterator>;
			using __base_value_type = __detail::__iterator_value_type_t<__base_iterator>;
			using __difference_type = __detail::__iterator_difference_type_t<__base_iterator>;
			using __size_type       = ::std::make_unsigned_t<__difference_type>;
			using __value_type      = _Word;

			static_assert(sizeof(__value_type) >= sizeof(__base_value_type),
				"the 'byte' type selected for the word_iterator must not be larger than the value_type of the "
				"iterator that it is meant to view");
			static_assert((sizeof(__value_type) % sizeof(__base_value_type)) == 0,
				"the 'byte' type selected for the word_iterator must be evenly divisible by the "
				"iterator that it is meant to view");

			static inline constexpr __size_type __base_values_per_word
				= sizeof(__value_type) / sizeof(__base_value_type);

		public:
			using iterator_type     = __base_iterator;
			using iterator_category = __iterator_category_t<__base_iterator>;
			using difference_type   = __difference_type;
			using pointer           = _Word*;
			using value_type        = __value_type;
			using reference         = __word_reference<__base_iterator, _Word, _Endian>;

			__category_word_iterator() = default;
			constexpr __category_word_iterator(iterator_type __it) : _M_base_it(::std::move(__it)) {
			}

			constexpr iterator_type& base() & {
				return this->_M_base_it;
			}

			constexpr const iterator_type& base() const& {
				return this->_M_base_it;
			}
			constexpr iterator_type&& base() && {
				return ::std::move(this->_M_base_it);
			}

			constexpr _Derived operator++(int) const {
				auto __copy = this->_M_this();
				++__copy;
				return __copy;
			}

			constexpr _Derived& operator++() {
				this->_M_base_it += __base_values_per_word;
				return this->_M_this();
			}

			constexpr _Derived operator--(int) const {
				auto __copy = this->_M_this();
				--__copy;
				return __copy;
			}

			constexpr _Derived& operator--() {
				this->_M_base_it -= __base_values_per_word;
				return this->_M_this();
			}

			constexpr _Derived operator+(difference_type __by) const {
				auto __copy = this->_M_this();
				__copy += __by;
				return __copy;
			}

			constexpr _Derived& operator+=(difference_type __by) {
				if (__by < static_cast<difference_type>(0)) {
					return this->operator+=(-__by);
				}
				this->_M_base_it += __base_values_per_word * __by;
				return this->_M_this();
			}

			constexpr difference_type operator-(const __category_word_iterator& __right) const {
				difference_type __dist = this->_M_base_it - __right._M_base_it;
				return static_cast<difference_type>(__dist * __base_values_per_word);
			}

			constexpr _Derived operator-(difference_type __by) const {
				auto __copy = this->_M_this();
				__copy -= __by;
				return __copy;
			}

			constexpr _Derived& operator-=(difference_type __by) {
				if (__by < static_cast<difference_type>(0)) {
					return this->operator+=(-__by);
				}
				this->_M_base_it -= __base_values_per_word * __by;
				return this->_M_this();
			}

			constexpr reference operator[](difference_type __index) {
				auto __copy = this->_M_this();
				__copy += __index;
				return *__copy;
			}

			constexpr reference operator*() const {
				return reference(this->_M_base_it);
			}

		private:
			constexpr _Derived& _M_this() & {
				return static_cast<_Derived&>(*this);
			}

			constexpr const _Derived& _M_this() const& {
				return static_cast<const _Derived&>(*this);
			}

			constexpr _Derived&& _M_this() && {
				return static_cast<_Derived&&>(*this);
			}

			iterator_type _M_base_it;
		};

		template <typename _Derived, typename _Word, typename _It, endian _Endian>
		class __category_word_iterator<_Derived, _Word, _It, _Endian,
			::std::enable_if_t<::std::is_same_v<__iterator_category_t<_It>, ::std::output_iterator_tag>>> {
		private:
			using __base_iterator   = _It;
			using __base_reference  = ::std::byte;
			using __base_value_type = ::std::byte;
			using __difference_type = ::std::ptrdiff_t;
			using __size_type       = ::std::make_unsigned_t<__difference_type>;
			using __value_type      = _Word;

			static_assert(sizeof(__value_type) >= sizeof(__base_value_type),
				"The 'byte' type selected for the word_iterator shall not be larger than the value_type of the "
				"iterator that it is meant to view.");

			static_assert((sizeof(__value_type) % sizeof(__base_value_type)) == 0,
				"The 'byte' type selected for the word_iterator shall equally divide the value_type of the "
				"iterator that it is meant to view.");

			static inline constexpr __size_type __base_values_per_word
				= sizeof(__value_type) / sizeof(__base_value_type);

		public:
			using iterator_type     = __base_iterator;
			using iterator_category = __iterator_category_t<__base_iterator>;
			using difference_type   = __difference_type;
			using pointer           = _Word*;
			using value_type        = __value_type;
			using reference         = __word_reference<__base_iterator, _Word, _Endian>;

			__category_word_iterator() = default;
			__category_word_iterator(iterator_type __it) : _M_base_it(::std::move(__it)) {
			}

			iterator_type& base() & {
				return this->_M_base_it;
			}

			const iterator_type& base() const& {
				return this->_M_base_it;
			}

			iterator_type&& base() && {
				return ::std::move(this->_M_base_it);
			}

			_Derived operator++(int) const {
				auto __copy = this->_M_this();
				++__copy;
				return __copy;
			}

			_Derived& operator++() {
				__detail::__next(this->_M_base_it, __base_values_per_word);
				return this->_M_this();
			}

			reference operator*() const {
				return reference(this->_M_base_it);
			}

		private:
			iterator_type _M_base_it;

			_Derived& _M_this() {
				return static_cast<_Derived&>(*this);
			}

			const _Derived& _M_this() const {
				return static_cast<const _Derived&>(*this);
			}
		};

		template <typename _LeftDerived, typename _LeftWord, typename _LeftIt, endian _LeftEndian, typename _RightIt>
		bool operator==(const __category_word_iterator<_LeftDerived, _LeftWord, _LeftIt, _LeftEndian>& __left,
			const __word_sentinel<_RightIt>& __right) {
			return __left.base() == __right.base();
		}

		template <typename _LeftDerived, typename _LeftWord, typename _LeftIt, endian _LeftEndian, typename _RightIt>
		bool operator!=(const __category_word_iterator<_LeftDerived, _LeftWord, _LeftIt, _LeftEndian>& __left,
			const __word_sentinel<_RightIt>& __right) {
			return __left.base() != __right.base();
		}

		template <typename _Word, typename _It, endian _Endian = endian::native>
		class __word_iterator
		: public __detail::__category_word_iterator<__word_iterator<_Word, _It, _Endian>, _Word, _It, _Endian> {
		private:
			using __base_t
				= __detail::__category_word_iterator<__word_iterator<_Word, _It, _Endian>, _Word, _It, _Endian>;

		public:
			using __base_t::__base_t;
		};

	} // namespace __detail
	ZTD_TEXT_INLINE_ABI_NAMESPACE_CLOSE_I_
}} // namespace ztd::text

#endif // ZTD_TEXT_DETAIL_WORD_ITERATOR_HPP
