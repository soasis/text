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
// =============================================================================

#pragma once

#ifndef ZTD_TEXT_DETAIL_ENCODING_ITERATOR_HPP
#define ZTD_TEXT_DETAIL_ENCODING_ITERATOR_HPP

#include <ztd/text/version.hpp>

#include <ztd/text/transcode_result.hpp>
#include <ztd/text/error_handler.hpp>
#include <ztd/text/state.hpp>
#include <ztd/text/unbounded.hpp>
#include <ztd/text/subrange.hpp>

#include <ztd/text/detail/ebco.hpp>
#include <ztd/text/detail/blackhole_iterator.hpp>
#include <ztd/text/detail/encoding_iterator_storage.hpp>
#include <ztd/text/detail/adl.hpp>
#include <ztd/text/detail/encoding_range.hpp>
#include <ztd/text/detail/transcode_one.hpp>
#include <ztd/text/detail/span.hpp>

#include <array>
#include <cassert>

namespace ztd { namespace text {
	ZTD_TEXT_INLINE_ABI_NAMESPACE_OPEN_I_
	namespace __detail {

		class __encoding_sentinel { };

		template <__transaction _EncodeOrDecode, typename _Derived, typename _Encoding, typename _Range,
			typename _ErrorHandler, typename _State>
		class __encoding_iterator
		: private __detail::__ebco<__detail::__remove_cvref_t<_Encoding>, 0>,
		  private __detail::__ebco<__detail::__remove_cvref_t<_ErrorHandler>, 1>,
		  private __detail::__state_storage<__detail::__remove_cvref_t<__detail::__unwrap_t<_Encoding>>,
			  __detail::__remove_cvref_t<_State>>,
		  private __detail::__cache_cursor<
			  max_code_units_v<__detail::__remove_cvref_t<__detail::__unwrap_t<_Encoding>>>>,
		  private __detail::__ebco<_Range, 2> {
		private:
			using _URange        = __detail::__remove_cvref_t<__detail::__unwrap_t<_Range>>;
			using _UEncoding     = __detail::__remove_cvref_t<__detail::__unwrap_t<_Encoding>>;
			using _UErrorHandler = __detail::__remove_cvref_t<__detail::__unwrap_t<_ErrorHandler>>;
			using _UState        = __detail::__remove_cvref_t<__detail::__unwrap_t<_State>>;
			using _BaseIterator  = __detail::__range_iterator_t<_URange>;
			static constexpr ::std::size_t _MaxValues = max_code_units_v<_UEncoding>;
			static constexpr bool _IsSingleValueType  = _MaxValues == 1;
			using __base_cursor_t                     = __detail::__cache_cursor<_MaxValues>;
			using __base_encoding_t                   = __detail::__ebco<__detail::__remove_cvref_t<_Encoding>, 0>;
			using __base_error_handler_t = __detail::__ebco<__detail::__remove_cvref_t<_ErrorHandler>, 1>;
			using __base_range_t         = __detail::__ebco<_Range, 2>;
			using __base_state_t         = __detail::__state_storage<__detail::__remove_cvref_t<_Encoding>,
                    __detail::__remove_cvref_t<_State>>;

		public:
			//////
			/// @brief The underlying range type.
			///
			//////
			using range_type = _Range;
			//////
			/// @brief The base iterator type.
			///
			//////
			using iterator_type = _BaseIterator;
			//////
			/// @brief The encoding type used for transformations.
			///
			//////
			using encoding_type = _Encoding;
			//////
			/// @brief The error handler when an encode operation fails.
			///
			//////
			using error_handler_type = _ErrorHandler;
			//////
			/// @brief The state type used for encode operations.
			///
			//////
			using encoding_state_type = __detail::__remove_cvref_t<_State>;
			//////
			/// @brief The strength of the iterator category, as defined in relation to the base.
			///
			//////
			using iterator_category = ::std::conditional_t<
				__detail::__is_iterator_concept_or_better_v<::std::bidirectional_iterator_tag, _BaseIterator>,
				::std::bidirectional_iterator_tag, __detail::__iterator_category_t<_BaseIterator>>;
			//////
			/// @brief The strength of the iterator concept, as defined in relation to the base.
			///
			//////
			using iterator_concept = ::std::conditional_t<
				__detail::__is_iterator_concept_or_better_v<::std::bidirectional_iterator_tag, _BaseIterator>,
				::std::bidirectional_iterator_tag, __detail::__iterator_concept_t<_BaseIterator>>;
			//////
			/// @brief The object type that gets output on every dereference.
			///
			//////
			using value_type = ::std::conditional_t<_EncodeOrDecode == __transaction::__encode,
				encoding_code_unit_t<_Encoding>, encoding_code_point_t<_Encoding>>;
			//////
			/// @brief A pointer type to the value_type.
			///
			//////
			using pointer = value_type*;
			//////
			/// @brief The value returned from derefencing the iterator.
			///
			/// @remarks This is a proxy iterator, so the @c reference is a non-reference @c value_type.
			//////
			using reference = value_type;
			//////
			/// @brief The type returned when two of these pointers are subtracted from one another.
			///
			/// @remarks It's not a very useful type...
			//////
			using difference_type = __detail::__iterator_difference_type_t<_BaseIterator>;

			constexpr __encoding_iterator() = default;

			constexpr __encoding_iterator(const __encoding_iterator&) = default;
			constexpr __encoding_iterator(__encoding_iterator&&)      = default;

			constexpr __encoding_iterator(range_type __range)
			: __encoding_iterator(::std::move(__range), encoding_type {}, error_handler_type {}) {
			}

			constexpr __encoding_iterator(range_type __range, encoding_type __encoding)
			: __encoding_iterator(::std::move(__range), ::std::move(__encoding), error_handler_type {}) {
			}

			constexpr __encoding_iterator(range_type __range, error_handler_type __error_handler)
			: __encoding_iterator(::std::move(__range), encoding_type {}, ::std::move(__error_handler)) {
			}

			constexpr __encoding_iterator(
				range_type __range, encoding_type __encoding, error_handler_type __error_handler)
			: __base_encoding_t(::std::move(__encoding))
			, __base_error_handler_t(::std::move(__error_handler))
			, __base_state_t(this->encoding())
			, __base_cursor_t()
			, __base_range_t(::std::move(__range))
			, _M_cache() {
				this->_M_read_one();
			}

			constexpr __encoding_iterator(range_type __range, encoding_type __encoding,
				error_handler_type __error_handler, encoding_state_type __state)
			: __base_encoding_t(::std::move(__encoding))
			, __base_error_handler_t(::std::move(__error_handler))
			, __base_state_t(this->encoding(), ::std::move(__state))
			, __base_cursor_t()
			, __base_range_t(::std::move(__range))
			, _M_cache() {
				this->_M_read_one();
			}

			// assignment
			constexpr __encoding_iterator& operator=(const __encoding_iterator&) = default;
			constexpr __encoding_iterator& operator=(__encoding_iterator&&) = default;

			//////
			/// @brief The encoding object.
			///
			/// @returns A const l-value reference to the encoding object used to construct this iterator.
			//////
			constexpr const encoding_type& encoding() const {
				return this->__base_encoding_t::get_value();
			}

			//////
			/// @brief The encoding object.
			///
			/// @returns An l-value reference to the encoding object used to construct this iterator.
			//////
			constexpr encoding_type& encoding() {
				return this->__base_encoding_t::get_value();
			}

			//////
			/// @brief The state object.
			///
			/// @returns A const l-value reference to the state object used to construct this iterator.
			//////
			constexpr const encoding_state_type& state() const {
				return this->__base_state_t::_M_get_state();
			}

			//////
			/// @brief The state object.
			///
			/// @returns An l-value reference to the state object used to construct this iterator.
			//////
			constexpr encoding_state_type& state() {
				return this->__base_state_t::_M_get_state();
			}

			//////
			/// @brief The error handler object.
			///
			/// @returns A const l-value reference to the error handler used to construct this iterator.
			//////
			constexpr const error_handler_type& handler() const {
				return this->__base_error_handler_t::get_value();
			}

			//////
			/// @brief The error handler object.
			///
			/// @returns An l-value reference to the error handler used to construct this iterator.
			//////
			constexpr error_handler_type& handler() {
				return this->__base_error_handler_t::get_value();
			}

			//////
			/// @brief The input range used to construct this object.
			///
			/// @returns A const l-value reference to the input range used to construct this iterator.
			//////
			constexpr const range_type& base() const& {
				return this->__base_range_t::get_value();
			}

			//////
			/// @brief The input range used to construct this object.
			///
			/// @returns An l-value reference to the input range used to construct this iterator.
			//////
			constexpr range_type& base() & {
				return this->__base_range_t::get_value();
			}

			//////
			/// @brief The input range used to construct this object.
			///
			/// @returns An r-value reference to the input range used to construct this iterator.
			//////
			constexpr range_type&& base() && {
				return ::std::move(this->__base_range_t::get_value());
			}

			//////
			/// @brief Whether or not the underlying range for this iterator is empty or not.
			///
			/// @returns True if the range is empty, false otherwise.
			//////
			constexpr bool empty() const noexcept {
				if constexpr (__is_detected_v<__detect_adl_empty, _Range>) {
					return __adl::__adl_empty(this->__base_range_t::get_value());
				}
				else {
					return __adl::__adl_cbegin(this->__base_range_t::get_value())
						== __adl::__adl_cend(this->__base_range_t::get_value());
				}
			}

			//////
			/// @brief Increment a copy of the iterator.
			///
			/// @returns A copy to the incremented iterator.
			//////
			constexpr _Derived operator++(int) {
				_Derived __copy = this->_M_derived();
				++__copy;
				return __copy;
			}

			//////
			/// @brief Increment the iterator.
			///
			/// @returns A reference to *this, after incrementing the iterator.
			//////
			constexpr _Derived& operator++() {
				if constexpr (_IsSingleValueType) {
					this->_M_next_one();
				}
				else {
					++this->_M_position;
					if (this->_M_position == this->_M_size) {
						this->_M_next_one();
						this->_M_position = 0;
					}
				}
				return this->_M_derived();
			}

			//////
			/// @brief Dereference the iterator.
			///
			/// @returns A value_type (NOT a reference) of the iterator.
			///
			/// @remarks This is a proxy iterator, and therefore only returns a value_type object and not a reference
			/// object. Encoding iterators are only readable, not writable.
			//////
			constexpr value_type operator*() const {
				if constexpr (_IsSingleValueType) {
					return this->_M_cache[0];
				}
				else {
					return this->_M_cache[this->_M_position];
				}
			}

			// observers: comparison

			//////
			/// @brief Compares whether or not this iterator has truly reached the end.
			///
			//////
			friend constexpr bool operator==(const _Derived& __it, const __encoding_sentinel&) {
				return __it.empty();
			}

			//////
			/// @brief Compares whether or not this iterator has truly reached the end.
			///
			//////
			friend constexpr bool operator==(const __encoding_sentinel& __sen, const _Derived& __it) {
				return __it == __sen;
			}

			//////
			/// @brief Compares whether or not this iterator has truly reached the end.
			///
			//////
			friend constexpr bool operator!=(const _Derived& __it, const __encoding_sentinel& __sen) {
				return !(__it == __sen);
			}

			//////
			/// @brief Compares whether or not this iterator has truly reached the end.
			///
			//////
			friend constexpr bool operator!=(const __encoding_sentinel& __sen, const _Derived& __it) {
				return !(__sen == __it);
			}

		private:
			template <__consume _Consume>
			constexpr void _M_consume_one() noexcept {
				auto __result = __basic_encode_or_decode_one<_Consume, _EncodeOrDecode>(
					this->_M_range(), this->encoding(), this->_M_cache, this->handler(), this->state());
				assert(__result.error_code == encoding_error::ok);
				if constexpr (_Consume == __consume::__embrace_the_void) {
					this->__base_range_t::get_value() = ::std::move(__result.input);
				}
				if constexpr (!_IsSingleValueType && _Consume != __consume::__embrace_the_void) {
					this->_M_size     = __detail::__adl::__adl_begin(__result.output) - this->_M_cache.begin();
					this->_M_position = 0;
				}
			}

			constexpr void _M_read_one() noexcept {
				this->_M_consume_one<__consume::__no>();
			}

			constexpr void _M_next_one() noexcept {
				this->_M_consume_one<__consume::__embrace_the_void>();
				this->_M_read_one();
			}

			constexpr _Derived& _M_derived() {
				return static_cast<_Derived&>(*this);
			}

			constexpr const _Derived& _M_derived() const {
				return static_cast<const _Derived&>(*this);
			}

			constexpr _URange& _M_range() {
				return this->__base_range_t::get_value();
			}

			constexpr const _URange& _M_range() const {
				return this->__base_range_t::get_value();
			}

			::std::array<value_type, _MaxValues> _M_cache;
		};

	} // namespace __detail
	ZTD_TEXT_INLINE_ABI_NAMESPACE_CLOSE_I_
}} // namespace ztd::text

#endif // ZTD_TEXT_DETAIL_ENCODING_ITERATOR_HPP
