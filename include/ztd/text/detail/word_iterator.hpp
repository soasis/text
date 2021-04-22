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
#include <ztd/text/detail/algorithm.hpp>
#include <ztd/text/detail/math.hpp>
#include <ztd/text/detail/ebco.hpp>
#include <ztd/text/detail/reconstruct.hpp>

#include <cstddef>
#include <limits>
#include <climits>
#include <cstring>
#include <memory>
#include <optional>
#include <iostream>

#include <ztd/text/detail/prologue.hpp>

namespace ztd { namespace text {
	ZTD_TEXT_INLINE_ABI_NAMESPACE_OPEN_I_
	namespace __txt_detail {

		template <typename _Word, typename _Range, bool>
		class __word_iterator_storage : private __ebco<_Range> {
		private:
			using __base_t = __ebco<_Range>;

		public:
			constexpr __word_iterator_storage() noexcept(::std::is_nothrow_default_constructible_v<_Range>)
			: __base_t() {
			}
			constexpr __word_iterator_storage(_Range&& __range) noexcept(
				::std::is_nothrow_move_constructible_v<_Range>)
			: __base_t(::std::move(__range)) {
			}
			constexpr __word_iterator_storage(const _Range& __range) noexcept(
				::std::is_nothrow_copy_constructible_v<_Range>)
			: __base_t(__range) {
			}

			using __base_t::__get_value;
		};

		template <typename _Word, typename _Range>
		class __word_iterator_storage<_Word, _Range, true> : private __ebco<_Range> {
		private:
			using __base_t = __ebco<_Range>;

		public:
			::std::optional<_Word> _M_val;

			constexpr __word_iterator_storage() noexcept(::std::is_nothrow_default_constructible_v<_Range>)
			: __base_t() {
			}
			constexpr __word_iterator_storage(_Range&& __range) noexcept(
				::std::is_nothrow_move_constructible_v<_Range>&& ::std::is_nothrow_default_constructible_v<_Word>)
			: __base_t(::std::move(__range)), _M_val(::std::nullopt) {
			}
			constexpr __word_iterator_storage(const _Range& __range) noexcept(
				::std::is_nothrow_copy_constructible_v<_Range>&& ::std::is_nothrow_default_constructible_v<_Word>)
			: __base_t(__range), _M_val(::std::nullopt) {
			}

			using __base_t::__get_value;
		};

		using __word_sentinel = default_sentinel_t;

		template <typename _Word, typename _Range, endian _Endian>
		class __word_iterator
		: private __word_iterator_storage<_Word, __reconstruct_t<__remove_cvref_t<_Range>>,
			  __is_iterator_input_iterator_v<__range_iterator_t<__reconstruct_t<__remove_cvref_t<_Range>>>>> {
		private:
			using _URange                      = __reconstruct_t<__remove_cvref_t<_Range>>;
			using __base_iterator              = __range_iterator_t<_URange>;
			using __base_sentinel              = __range_sentinel_t<_URange>;
			using __base_reference             = __iterator_reference_t<__base_iterator>;
			using __maybe_void_base_value_type = __iterator_value_type_t<__base_iterator>;
			using __base_value_type            = ::std::conditional_t<
                    ::std::is_void_v<
                         __maybe_void_base_value_type> || !::std::is_arithmetic_v<__maybe_void_base_value_type>,
                    ::std::byte, __maybe_void_base_value_type>;
			using __difference_type               = __iterator_difference_type_t<__base_iterator>;
			using __size_type                     = __iterator_size_type_t<__base_iterator>;
			using __value_type                    = _Word;
			inline constexpr static bool _IsInput = __is_iterator_input_iterator_v<__base_iterator>;
			using __base_storage_t                = __word_iterator_storage<_Word, _URange, _IsInput>;

			static_assert(sizeof(__value_type) >= sizeof(__base_value_type),
				"the 'byte' type selected for the word_iterator must not be larger than the value_type of the "
				"iterator that it is meant to view");
			static_assert((sizeof(__value_type) % sizeof(__base_value_type)) == 0,
				"the 'byte' type selected for the word_iterator must be evenly divisible by the "
				"iterator that it is meant to view");

			static inline constexpr __size_type __base_values_per_word
				= sizeof(__value_type) / sizeof(__base_value_type);

			class __word_reference {
			public:
				using value_type = _Word;

			private:
				using __underlying_base_value_type = decltype(__any_to_underlying(__base_value_type {}));
				using __underlying_value_type      = decltype(__any_to_underlying(value_type {}));
				inline static constexpr __underlying_value_type __base_bits_per_element
					= static_cast<__underlying_value_type>(sizeof(__underlying_base_value_type) * CHAR_BIT);
				inline static constexpr __underlying_value_type __base_lowest_bit_mask
					= static_cast<__underlying_value_type>(__ce_ipow(2, __base_bits_per_element) - 1);

			public:
				constexpr __word_reference(_URange& __range) noexcept : _M_base_range_ref(__range) {
				}

				template <typename _Value,
					::std::enable_if_t<::std::is_convertible_v<_Value,
					                        value_type> && !::std::is_const_v<__base_iterator>>* = nullptr>
				constexpr __word_reference& operator=(_Value __maybe_val) noexcept {
					if constexpr (_Endian == endian::native
						&& (endian::native != endian::big && endian::native != endian::little)) {
						static_assert(__always_false_constant_v<endian, _Endian>,
							"read value from byte stream to native endianness that is neither little nor big "
							"(byte order is impossible to infer from the standard)");
					}
					static_assert(sizeof(value_type) <= (sizeof(__base_value_type) * __base_values_per_word),
						"the size of the value type must be less than or equal to the array size");
					value_type __val = static_cast<value_type>(__maybe_val);
					__base_value_type __write_storage[__base_values_per_word] {};
					auto __write_storage_first = __write_storage + 0;
					auto __write_storage_last  = __write_storage + __base_values_per_word;
#if ZTD_TEXT_IS_ON(ZTD_TEXT_STD_LIBRARY_IS_CONSTANT_EVALUATED_I_)
					if (!::std::is_constant_evaluated()) {
						// just memcpy the data
						::std::memcpy(__write_storage, ::std::addressof(__val), sizeof(value_type));
					}
					else
#endif
					{
						// God's given, handwritten, bit-splittin'
						// one-way """memcpy""". 😵
						__underlying_value_type __bit_value = __any_to_underlying(static_cast<value_type>(__val));
						auto __write_storage_it             = __write_storage + 0;
						for (::std::size_t __index = 0; __index < __base_values_per_word; ++__index) {
							__underlying_value_type __bit_position
								= static_cast<__underlying_value_type>(__index * __base_bits_per_element);
							__underlying_base_value_type __shifted_bit_value
								= static_cast<__underlying_base_value_type>(__bit_value >> __bit_position);
							*__write_storage_it
								= static_cast<__base_value_type>(__shifted_bit_value & __base_lowest_bit_mask);
							++__write_storage_it;
						}
					}
					if constexpr (_Endian != endian::native) {
						if constexpr (_Endian == endian::big) {
							__reverse(__write_storage_first, __write_storage_last);
						}
						else {
							// TODO: what about middle endian or some such??
						}
					}
					auto& __base_range = this->_M_base_range();
					if constexpr (_IsInput) {
						auto __result = __copy(__write_storage_first, __write_storage_last,
							__adl::__adl_begin(::std::move(__base_range)),
							__adl::__adl_end(::std::move(__base_range)));
						this->_M_base_range()
							= __reconstruct(::std::in_place_type<_URange>, ::std::move(__result.out));
					}
					else {
						__copy(__write_storage_first, __write_storage_last, __adl::__adl_begin(__base_range),
							__adl::__adl_end(__base_range));
					}
					return *this;
				}

				constexpr value_type value() const noexcept {
					if constexpr (_Endian == endian::native
						&& (endian::native != endian::big && endian::native != endian::little)) {
						static_assert(__always_false_constant_v<endian, _Endian>,
							"read value from byte stream to native endianness that is neither little nor big "
							"(byte order is impossible to infer from the standard)");
					}
					__base_value_type __read_storage[__base_values_per_word] {};
					__base_value_type* __read_storage_first = __read_storage + 0;
					::std::size_t __read_storage_size       = __adl::__adl_size(__read_storage);
					value_type __val {};
					if constexpr (_IsInput) {
						// input iterator here (output iterstors cannot be used)
						// to do this kind of work
						// use iterator directly, re-update it when we are done
						// to prevent failure
						auto& __base_range    = this->_M_base_range();
						auto __result         = __copy_n_unsafe(__adl::__adl_begin(::std::move(__base_range)),
                                   __read_storage_size, __read_storage_first);
						this->_M_base_range() = __reconstruct(::std::in_place_type<_URange>,
							::std::move(__result.in).begin().base(), ::std::move(__base_range).end());
					}
					else {
						// prevent feed-updating iterator through usage here
						// just copy-and-use
						auto __base_it_copy            = this->_M_base_range().begin();
						[[maybe_unused]] auto __result = __copy_n_unsafe(
							::std::move(__base_it_copy), __read_storage_size, __read_storage_first);
					}
					if constexpr (_Endian == endian::big) {
						if constexpr ((sizeof(value_type) * CHAR_BIT) > 8) {
							__base_value_type* __read_storage_last = __read_storage + __base_values_per_word;
							__reverse(__read_storage_first, __read_storage_last);
						}
					}
#if ZTD_TEXT_IS_ON(ZTD_TEXT_STD_LIBRARY_IS_CONSTANT_EVALUATED_I_)
					if (!::std::is_constant_evaluated())
#else
					if (false)
#endif
					{
						::std::size_t __read_memory_storage_size
							= __read_storage_size * sizeof(__base_value_type);
						::std::memcpy(::std::addressof(__val), __read_storage_first, __read_memory_storage_size);
					}
					else {
						// God's given, handwritten, bit-fusin'
						// one-way """memcpy""". 😵
						for (::std::size_t __index = 0; __index < __base_values_per_word; ++__index) {
							__underlying_value_type __bit_value = static_cast<__underlying_value_type>(
								__any_to_underlying(__read_storage[__index]));
							__underlying_value_type __bit_position
								= static_cast<__underlying_value_type>(__index * __base_bits_per_element);
							__underlying_value_type __shifted_bit_value = (__bit_value << __bit_position);
							__val |= __shifted_bit_value;
						}
					}
					return static_cast<value_type>(__val);
				}

				constexpr operator value_type() const noexcept {
					return this->value();
				}

			private:
				_URange& _M_base_range() const noexcept {
					return this->_M_base_range_ref.get();
				}

				::std::reference_wrapper<_URange> _M_base_range_ref;
			};

		public:
			using range_type        = _URange;
			using iterator          = __base_iterator;
			using sentinel          = __base_sentinel;
			using iterator_category = __iterator_category_t<__base_iterator>;
			using iterator_concept  = __iterator_concept_t<__base_iterator>;
			using difference_type   = __difference_type;
			using pointer           = _Word*;
			using value_type        = __value_type;
			using reference         = ::std::conditional_t<_IsInput, value_type&, __word_reference>;
			using const_reference   = ::std::conditional_t<_IsInput, const value_type&, __word_reference>;

		private:
			static constexpr bool _S_deref_noexcept() noexcept {
				if constexpr (_IsInput) {
					return true;
				}
				else {
					return noexcept(reference(::std::declval<range_type&>()));
				}
			}

			static constexpr bool _S_const_deref_noexcept() noexcept {
				if constexpr (_IsInput) {
					return true;
				}
				else {
					return noexcept(const_reference(::std::declval<range_type&>()));
				}
			}

			static constexpr bool _S_copy_noexcept() noexcept {
				return ::std::is_nothrow_copy_constructible_v<iterator>;
			}

			static constexpr bool _S_recede_noexcept() noexcept {
				return noexcept(--::std::declval<iterator&>());
			}

			static constexpr bool _S_advance_noexcept() noexcept {
				return noexcept(++::std::declval<iterator&>());
			}

		public:
			constexpr __word_iterator() = default;
			constexpr __word_iterator(const range_type& __base_range) noexcept(
				::std::is_nothrow_constructible_v<__base_storage_t, const range_type&>)
			: __base_storage_t(__base_range) {
			}
			constexpr __word_iterator(range_type&& __base_range) noexcept(
				::std::is_nothrow_constructible_v<__base_storage_t, range_type&&>)
			: __base_storage_t(::std::move(__base_range)) {
			}

			__word_iterator(const __word_iterator&) = default;
			__word_iterator(__word_iterator&&)      = default;
			__word_iterator& operator=(const __word_iterator&) = default;
			__word_iterator& operator=(__word_iterator&&) = default;

			constexpr range_type range() & noexcept(::std::is_copy_constructible_v<range_type>
				     ? ::std::is_nothrow_copy_constructible_v<range_type>
				     : ::std::is_nothrow_move_constructible_v<range_type>) {
				if constexpr (::std::is_copy_constructible_v<range_type>) {
					return this->__base_storage_t::__get_value();
				}
				else {
					return ::std::move(this->__base_storage_t::__get_value());
				}
			}

			constexpr range_type range() const& noexcept(::std::is_nothrow_copy_constructible_v<range_type>) {
				return this->__base_storage_t::__get_value();
			}

			constexpr range_type range() && noexcept(::std::is_nothrow_move_constructible_v<range_type>) {
				return ::std::move(this->__base_storage_t::__get_value());
			}

			constexpr __word_iterator operator++(int) const noexcept(_S_copy_noexcept() && _S_advance_noexcept()) {
				auto __copy = *this;
				++__copy;
				return __copy;
			}

			constexpr __word_iterator& operator++() noexcept(_S_advance_noexcept()) {
				if constexpr (_IsInput) {
					// force read on next dereference
					this->__base_storage_t::_M_val = ::std::nullopt;
				}
				else {
					auto __first_it = __adl::__adl_begin(::std::move(this->__base_storage_t::__get_value()));
					auto __last_it  = __adl::__adl_end(::std::move(this->__base_storage_t::__get_value()));
					__advance(__first_it, __base_values_per_word);
					this->__base_storage_t::__get_value() = __reconstruct(
						::std::in_place_type<_URange>, ::std::move(__first_it), ::std::move(__last_it));
				}
				return *this;
			}

			template <typename _Dummy = range_type>
			constexpr ::std::enable_if_t<
				__is_range_iterator_concept_or_better_v<::std::bidirectional_iterator_tag, _Dummy>, __word_iterator>
			operator--(int) const noexcept(_S_copy_noexcept() && _S_recede_noexcept()) {
				auto __copy = *this;
				--__copy;
				return __copy;
			}

			template <typename _Dummy = range_type>
			constexpr ::std::enable_if_t<
				__is_range_iterator_concept_or_better_v<::std::bidirectional_iterator_tag, _Dummy>,
				__word_iterator&>
			operator--() noexcept {
				__recede(this->__base_storage_t::__get_value(), __base_values_per_word);
				return *this;
			}

			template <typename _Dummy = range_type>
			constexpr ::std::enable_if_t<
				__is_range_iterator_concept_or_better_v<::std::random_access_iterator_tag, _Dummy>, __word_iterator>
			operator+(difference_type __by) const noexcept(_S_copy_noexcept() && _S_advance_noexcept()) {
				auto __copy = *this;
				__copy += __by;
				return __copy;
			}

			template <typename _Dummy = range_type>
			constexpr ::std::enable_if_t<
				__is_range_iterator_concept_or_better_v<::std::random_access_iterator_tag, _Dummy>,
				__word_iterator&>
			operator+=(difference_type __by) noexcept(_S_advance_noexcept()) {
				if (__by < static_cast<difference_type>(0)) {
					return this->operator+=(-__by);
				}
				auto __first_it = __adl::__adl_begin(::std::move(this->__base_storage_t::__get_value()));
				auto __last_it  = __adl::__adl_end(::std::move(this->__base_storage_t::__get_value()));
				__advance(__first_it, __base_values_per_word * __by);
				this->__base_storage_t::__get_value() = __reconstruct(
					::std::in_place_type<_URange>, ::std::move(__first_it), ::std::move(__last_it));
				return *this;
			}

			template <typename _Dummy = range_type>
			constexpr ::std::enable_if_t<
				__is_range_iterator_concept_or_better_v<::std::random_access_iterator_tag, _Dummy>, difference_type>
			operator-(const __word_iterator& __right) const noexcept {
				difference_type __dist
					= this->__base_storage_t::__get_value() - __right.__base_storage_t::__get_value();
				return static_cast<difference_type>(__dist * __base_values_per_word);
			}

			template <typename _Dummy = range_type>
			constexpr ::std::enable_if_t<
				__is_range_iterator_concept_or_better_v<::std::random_access_iterator_tag, _Dummy>, __word_iterator>
			operator-(difference_type __by) const noexcept(_S_copy_noexcept() && _S_recede_noexcept()) {
				auto __copy = *this;
				__copy -= __by;
				return __copy;
			}

			template <typename _Dummy = range_type>
			constexpr ::std::enable_if_t<
				__is_range_iterator_concept_or_better_v<::std::random_access_iterator_tag, _Dummy>,
				__word_iterator&>
			operator-=(difference_type __by) noexcept(_S_recede_noexcept()) {
				if (__by < static_cast<difference_type>(0)) {
					return this->operator+=(-__by);
				}
				auto __first_it = __adl::__adl_begin(::std::move(this->__base_storage_t::__get_value()));
				auto __last_it  = __adl::__adl_end(::std::move(this->__base_storage_t::__get_value()));
				__recede(__first_it, __base_values_per_word * __by);
				this->__base_storage_t::__get_value() = __reconstruct(
					::std::in_place_type<_URange>, ::std::move(__first_it), ::std::move(__last_it));
				return *this;
			}

			template <typename _Dummy = range_type>
			constexpr ::std::enable_if_t<
				__is_range_iterator_concept_or_better_v<::std::random_access_iterator_tag, _Dummy>, reference>
			operator[](difference_type __index) noexcept(_S_copy_noexcept() && _S_advance_noexcept()) {
				auto __copy = *this;
				__copy += __index;
				return *__copy;
			}

			template <typename _Dummy = range_type>
			constexpr ::std::enable_if_t<
				__is_range_iterator_concept_or_better_v<::std::random_access_iterator_tag, _Dummy>, const_reference>
			operator[](difference_type __index) const noexcept(_S_copy_noexcept() && _S_advance_noexcept()) {
				auto __copy = *this;
				__copy += __index;
				return *__copy;
			}

			constexpr reference operator*() noexcept(_S_deref_noexcept()) {
				if constexpr (_IsInput) {
					if (this->__base_storage_t::_M_val == ::std::nullopt) {
						this->_M_read_one();
					}
					return *this->__base_storage_t::_M_val;
				}
				else {
					return reference(this->__base_storage_t::__get_value());
				}
			}

			constexpr const_reference operator*() const noexcept(_S_const_deref_noexcept()) {
				if constexpr (_IsInput) {
					if (this->__base_storage_t::_M_val == ::std::nullopt) {
						const_cast<__word_iterator*>(this)->_M_read_one();
					}
					return *this->__base_storage_t::_M_val;
				}
				else {
					return const_reference(this->__base_storage_t::__get_value());
				}
			}

			friend constexpr bool operator==(const __word_iterator& __left, const __word_sentinel&) noexcept(
				noexcept(__left._M_base_is_empty())) {
				return __left._M_base_is_empty();
			}

			friend constexpr bool operator!=(const __word_iterator& __left, const __word_sentinel&) noexcept(
				noexcept(!__left._M_base_is_empty())) {
				return !__left._M_base_is_empty();
			}

			friend constexpr bool operator==(const __word_sentinel& __sen, const __word_iterator& __left) noexcept(
				noexcept(__left == __sen)) {
				return __left == __sen;
			}

			friend constexpr bool operator!=(const __word_sentinel& __sen, const __word_iterator& __left) noexcept(
				noexcept(__left != __sen)) {
				return __left != __sen;
			}

		private:
			constexpr void _M_read_one() noexcept(_S_deref_noexcept()) {
				if constexpr (_IsInput) {
					_Word __read_word              = __word_reference(this->__base_storage_t::__get_value());
					this->__base_storage_t::_M_val = ::std::optional<_Word>(__read_word);
				}
			}

			constexpr bool _M_base_is_empty() const noexcept {
				if constexpr (__is_detected_v<__detect_adl_empty, range_type>) {
					return __adl::__adl_empty(this->__base_storage_t::__get_value());
				}
				else {
					return __adl::__adl_begin(this->__base_storage_t::__get_value())
						== __adl::__adl_end(this->__base_storage_t::__get_value());
				}
			}
		};

	} // namespace __txt_detail
	ZTD_TEXT_INLINE_ABI_NAMESPACE_CLOSE_I_
}} // namespace ztd::text

#include <ztd/text/detail/epilogue.hpp>

#endif // ZTD_TEXT_DETAIL_WORD_ITERATOR_HPP
