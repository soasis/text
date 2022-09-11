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

#ifndef ZTD_TEXT_BASIC_TEXT_HPP
#define ZTD_TEXT_BASIC_TEXT_HPP

#include <ztd/text/version.hpp>

#include <ztd/text/error_handler.hpp>
#include <ztd/text/normalization.hpp>
#include <ztd/text/code_unit.hpp>
#include <ztd/text/code_point.hpp>
#include <ztd/text/is_compatible_code_points.hpp>
#include <ztd/text/encoding.hpp>
#include <ztd/text/validate_decodable_as.hpp>
#include <ztd/text/encode.hpp>
#include <ztd/text/transcode.hpp>
#include <ztd/text/decode_view.hpp>
#include <ztd/text/normalized_view.hpp>
#include <ztd/text/basic_text_view_iterator.hpp>
#include <ztd/text/assert.hpp>
#include <ztd/text/detail/default_char_range.hpp>

#include <ztd/idk/unwrap.hpp>
#include <ztd/idk/basic_c_string_view.hpp>
#include <ztd/ranges/from_range.hpp>
#include <ztd/ranges/unbounded.hpp>
#include <ztd/ranges/counted_iterator.hpp>
#include <ztd/ranges/adl.hpp>

#include <string>
#include <iterator>

#include <ztd/prologue.hpp>

namespace ztd { namespace text {
	ZTD_TEXT_INLINE_ABI_NAMESPACE_OPEN_I_

	namespace __txt_detail {
		class __range_constructor_t { };
		class __count_constructor_t { };
	} // namespace __txt_detail

	//////
	/// @brief A wrapper (container adapter) that takes the given `_Encoding` type and `_NormalizationForm` type and
	/// imposes it over the given chosen `_Range` storage for the purposes of allowing users to examine the text.
	///
	/// @tparam _Encoding The encoding to store any input and presented text as.
	/// @tparam _NormalizationForm The normalization form to impose on the stored text's sequences.
	/// @tparam _Range The container type that will be stored within this ztd::text::basic_text using the code
	/// units from the `_Encoding` type.
	/// @tparam _ErrorHandler The default error handler to use for any and all operations on text. Generally, most
	/// operations will provide room to override this.
	//////
	template <typename _Encoding, typename _NormalizationForm = nfkc,
		typename _Range = __txt_detail::__default_char_range_t<code_unit_t<_Encoding>>>
	class basic_text {
	private:
		using _URange             = unwrap_remove_cvref_t<_Range>;
		using _CVRange            = unwrap_remove_reference_t<_Range>;
		using _UEncoding          = unwrap_remove_cvref_t<_Encoding>;
		using _UNormalizationForm = unwrap_remove_cvref_t<_NormalizationForm>;

		template <typename _RangeOrCount, typename... _Args>
		using __allow_single_argument_with_variadic_constructor = ::std::integral_constant<bool,
			(::std::is_same_v<::ztd::remove_cvref_t<_RangeOrCount>, basic_text>)
			     ? (sizeof...(_Args) > 0)
			     : !(::ztd::is_character_pointer_v<_RangeOrCount>)>;

	public:
		//////
		/// @brief The type that this view is wrapping.
		using range_type = _Range;
		//////
		/// @brief The encoding type that this view is using to interpret the underlying sequence of code units.
		using encoding_type = _Encoding;
		//////
		/// @brief The normalization form type this view is imposing on top of the encoded sequence.
		using normalization_type = _NormalizationForm;
		//////
		/// @brief The code point type when the underlying storage is decoded.
		using code_point = code_point_t<_UEncoding>;
		//////
		/// @brief The code unit type expected by the underlying storage.
		using code_unit = code_unit_t<_UEncoding>;

		//////
		/// @brief FIXME.
		using iterator
			= basic_text_view_iterator<_UEncoding, _UNormalizationForm, ::ztd::ranges::reconstruct_t<_CVRange>>;

		//////
		/// @brief FIXME.
		using const_iterator = basic_text_view_iterator<_UEncoding, _UNormalizationForm,
			::ztd::ranges::reconstruct_t<const _CVRange>>;

		//////
		/// @brief FIXME.
		using sentinel = text_view_sentinel_t;

		//////
		/// @brief FIXME.
		using const_sentinel = text_view_sentinel_t;

		//////
		/// @brief The type for the basic iterators.
		using value_type = code_point;

		//////
		/// @brief The type for the basic iterators.
		using reference = code_point;

		//////
		/// @brief The type for the basic iterators.
		using difference_type = ranges::iterator_difference_type_t<iterator>;

		//////
		/// @brief The type for the basic iterators.
		using size_type = ranges::iterator_size_type_t<iterator>;

		//////
		/// @brief FIXME.
		using iterator_category = ranges::iterator_concept_t<iterator>;

		//////
		/// @brief FIXME.
		using iterator_concept = ranges::iterator_concept_t<iterator>;


	private:
		template <typename _Input>
		static constexpr bool _S_constructor_from_range_noexcept() noexcept {
			// TODO: test for encode noexcept-ness and container insertion noexcept-ness
			return false
				&& (::std::is_nothrow_default_constructible_v<encoding_type>          // cf
				     && ::std::is_nothrow_default_constructible_v<normalization_type> // cf
				     && ::std::is_nothrow_default_constructible_v<range_type>);
		}

		template <typename _Input, typename _FromEncoding>
		static constexpr bool _S_constructor_from_range_noexcept() noexcept {
			// TODO: test for encode noexcept-ness and container insertion noexcept-ness
			return false
				&& (::std::is_nothrow_default_constructible_v<encoding_type>          // cf
				     && ::std::is_nothrow_default_constructible_v<normalization_type> // cf
				     && ::std::is_nothrow_default_constructible_v<range_type>);
		}

		template <typename _Input, typename _FromEncoding, typename _ErrorHandler>
		static constexpr bool _S_constructor_from_range_noexcept() noexcept {
			// TODO: test for encode noexcept-ness and container insertion noexcept-ness
			return false
				&& (::std::is_nothrow_default_constructible_v<encoding_type>          // cf
				     && ::std::is_nothrow_default_constructible_v<normalization_type> // cf
				     && ::std::is_nothrow_default_constructible_v<range_type>);
		}

		static constexpr bool _S_constructor_in_place() noexcept {
			return ::std::is_nothrow_default_constructible_v<encoding_type> // cf
				&& ::std::is_nothrow_default_constructible_v<range_type>   // cf
				&& ::std::is_nothrow_default_constructible_v<normalization_type>;
		}

		template <typename _InPlaceOrRange>
		static constexpr bool _S_constructor_range_noexcept() noexcept {
			if constexpr (::std::is_same_v<::ztd::remove_cvref_t<_InPlaceOrRange>, ::std::in_place_t>) {
				return _S_constructor_in_place();
			}
			else {
				return _S_constructor_from_range_noexcept<_InPlaceOrRange>();
			}
		}

		template <typename _RangeOrCount>
		static constexpr bool _S_constructor_range_or_count_noexcept() noexcept {
			return false;
		}

		template <typename _Ptr, typename... _Args>
		static constexpr bool _S_constructor_pointer_noexcept() noexcept {
			using _CStringView = ::ztd::basic_c_string_view<::std::remove_pointer_t<_Ptr>>;
			return ::std::is_nothrow_constructible_v<_CStringView, _Ptr> // cf
				&& _S_constructor_from_range_noexcept<_CStringView, _Args...>();
		}

	public:
		// constructors and assignment operators

		//////
		/// @brief Copy constructor. Defaulted.
		constexpr basic_text(const basic_text&) = default;
		//////
		/// @brief Move constructor. Defaulted.
		constexpr basic_text(basic_text&&) = default;
		//////
		/// @brief Copy assignment operator. Defaulted.
		constexpr basic_text& operator=(const basic_text&) = default;
		//////
		/// @brief Move assignment operator. Defaulted.
		constexpr basic_text& operator=(basic_text&&) = default;

		//////
		/// @brief Constructs a basic text, using the default constructor for all held components.
		constexpr basic_text() noexcept(::std::is_nothrow_constructible_v<basic_text, ::std::in_place_t>)
		: basic_text(::std::in_place) {
		}

		template <typename _Ptr, typename... _Args,
			::std::enable_if_t<::ztd::is_character_pointer_v<_Ptr>>* = nullptr>
		constexpr basic_text(_Ptr __ptr, _Args&&... __args) noexcept(
			_S_constructor_pointer_noexcept<_Ptr, _Args...>())
		: basic_text(::ztd::ranges::from_range,
			::ztd::basic_c_string_view<::std::remove_pointer_t<_Ptr>>(::std::forward<_Ptr>(__ptr)),
			::std::forward<_Args>(__args)...) {
		}

		template <typename _RangeOrCount, typename... _Args,
			::std::enable_if_t<
			     __allow_single_argument_with_variadic_constructor<_RangeOrCount, _Args...>::value // cf
			     >* = nullptr>
		constexpr basic_text(_RangeOrCount&& __range_or_count, _Args&&... __args) // cf
			noexcept(_S_constructor_range_or_count_noexcept<_RangeOrCount>())
		: basic_text(
			::std::conditional_t<::std::is_same_v<::ztd::remove_cvref_t<_RangeOrCount>, ::std::in_place_t>, // cf
			     ::std::in_place_t,                                                                         // cf
			     ::ztd::ranges::from_range_t>(),
			::std::forward<_RangeOrCount>(__range_or_count), ::std::forward<_Args>(__args)...) {
		}

		//////
		/// @brief Constructs from a given range, performing a conversion from code points if necessary.
		template <typename _Input>
		constexpr basic_text(::ztd::ranges::from_range_t, _Input&& __input) noexcept(
			_S_constructor_from_range_noexcept<_Input>())
		: basic_text() {
			using _InputValueType = ranges::range_value_type_t<_Input>;
			if constexpr (is_compatible_code_points_v<code_point, _InputValueType>) {
				// must transcode from whatever is in __input to our internal container...
				using _BackInserterIterator
					= decltype(::std::back_inserter(ztd::unwrap(::std::declval<range_type&>())));
				using _Unbounded = ranges::unbounded_view<_BackInserterIterator>;
				_Unbounded __insert_view(::std::back_inserter(ztd::unwrap(this->_M_range)));
				::ztd::text::encode_into(
					::std::forward<_Input>(__input), this->_M_encoding, ::std::move(__insert_view));
			}
			else {
				// must transcode from whatever is in __input to our internal container...
				using _BackInserterIterator
					= decltype(::std::back_inserter(ztd::unwrap(::std::declval<range_type&>())));
				using _Unbounded = ranges::unbounded_view<_BackInserterIterator>;
				_Unbounded __insert_view(::std::back_inserter(ztd::unwrap(this->_M_range)));
				::ztd::text::transcode_into(
					::std::forward<_Input>(__input), this->_M_encoding, ::std::move(__insert_view));
			}
		}

		//////
		/// @brief Constructs from a given range, performing a conversion from code points if necessary.
		template <typename _Input, typename _FromEncoding>
		constexpr basic_text(::ztd::ranges::from_range_t, _Input&& __input, _FromEncoding&& __from_encoding) noexcept(
			_S_constructor_from_range_noexcept<_Input, _FromEncoding>())
		: basic_text() {
			using _InputValueType = ranges::range_value_type_t<_Input>;
			if constexpr (is_compatible_code_points_v<code_point, _InputValueType>) {
				// must transcode from whatever is in __input to our internal container...
				using _BackInserterIterator
					= decltype(::std::back_inserter(ztd::unwrap(::std::declval<range_type&>())));
				using _Unbounded = ranges::unbounded_view<_BackInserterIterator>;
				_Unbounded __insert_view(::std::back_inserter(ztd::unwrap(this->_M_range)));
				::ztd::text::encode_into(::std::forward<_Input>(__input), this->_M_encoding,
					::std::move(__insert_view), ::std::forward<_FromEncoding>(__from_encoding));
			}
			else {
				// must transcode from whatever is in __input to our internal container...
				using _BackInserterIterator
					= decltype(::std::back_inserter(ztd::unwrap(::std::declval<range_type&>())));
				using _Unbounded = ranges::unbounded_view<_BackInserterIterator>;
				_Unbounded __insert_view(::std::back_inserter(ztd::unwrap(this->_M_range)));
				::ztd::text::transcode_into(::std::forward<_Input>(__input), this->_M_encoding,
					::std::move(__insert_view), ::std::forward<_FromEncoding>(__from_encoding));
			}
		}

		//////
		/// @brief Constructs from a given range, performing a conversion from code points if necessary.
		template <typename _Input, typename _FromEncoding, typename _ErrorHandler>
		constexpr basic_text(::ztd::ranges::from_range_t, _Input&& __input, _FromEncoding&& __from_encoding,
			_ErrorHandler&& __error_handler) noexcept(_S_constructor_from_range_noexcept<_Input, _FromEncoding,
			_ErrorHandler>())
		: basic_text() {
			using _InputValueType = ranges::range_value_type_t<_Input>;
			if constexpr (is_compatible_code_points_v<code_point, _InputValueType>) {
				// must transcode from whatever is in __input to our internal container...
				using _BackInserterIterator
					= decltype(::std::back_inserter(ztd::unwrap(::std::declval<range_type&>())));
				using _Unbounded = ranges::unbounded_view<_BackInserterIterator>;
				_Unbounded __insert_view(::std::back_inserter(ztd::unwrap(this->_M_range)));
				::ztd::text::encode_into(::std::forward<_Input>(__input), this->_M_encoding,
					::std::move(__insert_view), ::std::forward<_FromEncoding>(__from_encoding),
					::std::forward<_ErrorHandler>(__error_handler));
			}
			else {
				// must transcode from whatever is in __input to our internal container...
				using _BackInserterIterator
					= decltype(::std::back_inserter(ztd::unwrap(::std::declval<range_type&>())));
				using _Unbounded = ranges::unbounded_view<_BackInserterIterator>;
				_Unbounded __insert_view(::std::back_inserter(ztd::unwrap(this->_M_range)));
				::ztd::text::transcode_into(::std::forward<_Input>(__input), this->_M_encoding,
					::std::move(__insert_view), ::std::forward<_FromEncoding>(__from_encoding),
					::std::forward<_ErrorHandler>(__error_handler));
			}
		}

		explicit constexpr basic_text(::std::in_place_t) // cf
			noexcept(_S_constructor_in_place())
		: _M_encoding(), _M_normalization(), _M_range() {
			this->_M_verify_integrity();
		}

		constexpr basic_text(::std::in_place_t, range_type __range)            // cf
			noexcept(::std::is_nothrow_default_constructible_v<encoding_type> // cf
			          && ::std::is_nothrow_move_constructible_v<range_type>   // cf
			               && ::std::is_nothrow_default_constructible_v<normalization_type>)
		: _M_encoding(), _M_normalization(), _M_range(::std::move(__range)) {
			this->_M_verify_integrity();
		}

		constexpr basic_text(::std::in_place_t, range_type __range, encoding_type __encoding) // cf
			noexcept(::std::is_nothrow_default_constructible_v<encoding_type>                // cf
			          && ::std::is_nothrow_move_constructible_v<range_type>                  // cf
			               && ::std::is_nothrow_default_constructible_v<normalization_type>)
		: _M_encoding(::std::move(__encoding)), _M_normalization(), _M_range(::std::move(__range)) {
			this->_M_verify_integrity();
		}

		constexpr basic_text(::std::in_place_t, range_type __range, encoding_type __encoding,
			normalization_type __normalization_form)                          // cf
			noexcept(::std::is_nothrow_default_constructible_v<encoding_type> // cf
			          && ::std::is_nothrow_move_constructible_v<range_type>   // cf
			               && ::std::is_nothrow_default_constructible_v<normalization_type>)
		: _M_encoding(::std::move(__encoding))
		, _M_normalization(::std::move(__normalization_form))
		, _M_range(::std::move(__range)) {
			this->_M_verify_integrity();
		}

		// observers
		// observers: iteration

		constexpr auto begin() const noexcept {
			using _OuterRangeType = typename const_iterator::range_type;
			return const_iterator(_OuterRangeType(::ztd::ranges::reconstruct(::std::in_place_type<_URange>,
				                                      ::ztd::ranges::ranges_adl::adl_begin(this->_M_range),
				                                      ::ztd::ranges::ranges_adl::adl_end(this->_M_range)),
				                      this->_M_encoding),
				this->_M_normalization);
		}

		constexpr auto end() const noexcept {
			return sentinel();
		}

		// observers: storage
		constexpr range_type& base() & noexcept {
			return this->_M_range;
		}

		constexpr const range_type& base() const& noexcept {
			return this->_M_range;
		}

		constexpr range_type&& base() && noexcept {
			return ::std::move(this->_M_range);
		}

		// observers: underlying range
		constexpr range_type& range() & noexcept {
			return this->_M_range;
		}

		constexpr const range_type& range() const& noexcept {
			return this->_M_range;
		}

		constexpr range_type&& range() && noexcept {
			return ::std::move(this->_M_range);
		}

		// observers: encoding
		constexpr encoding_type& encoding() & noexcept {
			return this->_M_range;
		}

		constexpr const encoding_type& encoding() const& noexcept {
			return this->_M_range;
		}

		constexpr encoding_type&& encoding() && noexcept {
			return ::std::move(this->_M_encoding);
		}

		// modifiers:
		// modifiers: insertion


	private:
		constexpr void _M_verify_integrity() const noexcept {
			const bool __success = ::ztd::text::validate_decodable_as(this->_M_range, this->_M_encoding).valid;
			ZTD_TEXT_ASSERT_MESSAGE("given data has violated its encoding constraints", __success);
		}

		encoding_type _M_encoding;
		normalization_type _M_normalization;
		range_type _M_range;
	};

	ZTD_TEXT_INLINE_ABI_NAMESPACE_CLOSE_I_
}} // namespace ztd::text

#include <ztd/epilogue.hpp>

#endif // ZTD_TEXT_BASIC_TEXT_HPP
