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

#ifndef ZTD_TEXT_DETAIL_ENCODING_ITERATOR_STORAGE_HPP
#define ZTD_TEXT_DETAIL_ENCODING_ITERATOR_STORAGE_HPP

#include <ztd/text/version.hpp>

#include <ztd/text/state.hpp>
#include <ztd/text/encoding_error.hpp>

#include <ztd/idk/unwrap.hpp>
#include <ztd/idk/ebco.hpp>
#include <ztd/idk/to_underlying.hpp>
#include <ztd/idk/detail/math.hpp>
#include <ztd/ranges/range.hpp>

#include <cstddef>
#include <climits>

#include <ztd/prologue.hpp>

namespace ztd { namespace text {
	ZTD_TEXT_INLINE_ABI_NAMESPACE_OPEN_I_
	namespace __txt_detail {

		inline constexpr ::std::size_t _CursorlessSizeSentinel = 1;

		template <typename _Encoding, typename _EncodingState, ::std::size_t _Id = 0>
		class __state_storage : private ebco<unwrap_remove_cvref_t<_EncodingState>, _Id> {
		private:
			using _UEncoding      = unwrap_remove_cvref_t<_Encoding>;
			using _UEncodingState = unwrap_remove_cvref_t<_EncodingState>;
			using __state_base_t  = ebco<unwrap_remove_cvref_t<_EncodingState>, _Id>;

		public:
			template <typename _ArgEncoding = _UEncoding,
				::std::enable_if_t<
				     !is_state_independent_v<remove_cvref_t<_ArgEncoding>,
				          _UEncodingState> && !::std::is_same_v<remove_cvref_t<_ArgEncoding>, __state_storage>>* = nullptr>
			constexpr __state_storage(_ArgEncoding& __encoding) noexcept(
				::std::is_nothrow_constructible_v<__state_base_t, _Encoding&>)
			: __state_base_t(::std::forward<_ArgEncoding>(__encoding)) {
			}
			template <typename _ArgEncoding = _UEncoding,
				::std::enable_if_t<
				     is_state_independent_v<remove_cvref_t<_ArgEncoding>,
				          _UEncodingState> && !::std::is_same_v<remove_cvref_t<_ArgEncoding>, __state_storage>>* = nullptr>
			constexpr __state_storage(_ArgEncoding&) noexcept(
				::std::is_nothrow_default_constructible_v<__state_base_t>)
			: __state_base_t() {
			}
			constexpr __state_storage(_Encoding&, const _UEncodingState& __state) noexcept(
				::std::is_nothrow_constructible_v<__state_base_t, const _UEncodingState&>)
			: __state_base_t(__state) {
			}
			constexpr __state_storage(_Encoding&, _UEncodingState&& __state) noexcept(
				::std::is_nothrow_constructible_v<__state_base_t, _UEncodingState&&>)
			: __state_base_t(::std::move(__state)) {
			}
			constexpr __state_storage(const __state_storage&)            = default;
			constexpr __state_storage(__state_storage&&)                 = default;
			constexpr __state_storage& operator=(const __state_storage&) = default;
			constexpr __state_storage& operator=(__state_storage&&)      = default;

			constexpr ::std::add_lvalue_reference_t<_UEncodingState> _M_get_state() noexcept {
				return this->__state_base_t::get_value();
			}

			constexpr ::std::add_const_t<::std::add_lvalue_reference_t<_UEncodingState>>
			_M_get_state() const noexcept {
				return this->__state_base_t::get_value();
			}
		};

		template <typename _Encoding, typename _Range, typename _ErrorHandler, typename _State>
		class __iterator_storage
		: private ebco<remove_cvref_t<_Encoding>, 0>,
		  private ebco<remove_cvref_t<_ErrorHandler>, 1>,
		  private __state_storage<unwrap_remove_cvref_t<_Encoding>, remove_cvref_t<_State>, 2>,
		  private ebco<remove_cvref_t<_Range>, 3> {
		private:
			using __base_encoding_t      = ebco<remove_cvref_t<_Encoding>, 0>;
			using __base_error_handler_t = ebco<remove_cvref_t<_ErrorHandler>, 1>;
			using __base_state_t = __state_storage<unwrap_remove_cvref_t<_Encoding>, remove_cvref_t<_State>, 2>;
			using __base_range_t = ebco<_Range, 3>;
			using _URange        = unwrap_remove_cvref_t<_Range>;
			using _UEncoding     = unwrap_remove_cvref_t<_Encoding>;
			using _UErrorHandler = unwrap_remove_cvref_t<_ErrorHandler>;
			using _UState        = unwrap_remove_cvref_t<_State>;

		public:
			using range_type         = _Range;
			using encoding_type      = _Encoding;
			using state_type         = _State;
			using error_handler_type = _ErrorHandler;

			constexpr __iterator_storage()                                     = default;
			constexpr __iterator_storage(const __iterator_storage&)            = default;
			constexpr __iterator_storage(__iterator_storage&&)                 = default;
			constexpr __iterator_storage& operator=(const __iterator_storage&) = default;
			constexpr __iterator_storage& operator=(__iterator_storage&&)      = default;

			template <typename _ArgRange,
				::std::enable_if_t<!::std::is_same_v<remove_cvref_t<_ArgRange>, __iterator_storage>>* = nullptr>
			constexpr __iterator_storage(_ArgRange&& __range) noexcept(
				noexcept(__iterator_storage(::std::declval<range_type>(), ::std::declval<encoding_type>(),
				     ::std::declval<error_handler_type>())))
			: __iterator_storage(::std::forward<_ArgRange>(__range), encoding_type {}, error_handler_type {}) {
			}

			constexpr __iterator_storage(range_type __range, encoding_type __encoding) noexcept(
				noexcept(__iterator_storage(::std::declval<range_type>(), ::std::declval<encoding_type>(),
				     ::std::declval<error_handler_type>())))
			: __iterator_storage(::std::move(__range), ::std::move(__encoding), error_handler_type {}) {
			}

			constexpr __iterator_storage(range_type __range, error_handler_type __error_handler) noexcept(
				noexcept(__iterator_storage(::std::declval<range_type>(), ::std::declval<encoding_type>(),
				     ::std::declval<error_handler_type>())))
			: __iterator_storage(::std::move(__range), encoding_type {}, ::std::move(__error_handler)) {
			}

			constexpr __iterator_storage(range_type __range, encoding_type __encoding,
				error_handler_type __error_handler)                                     // cf
				noexcept(::std::is_nothrow_move_constructible_v<encoding_type>          // cf
				          && ::std::is_nothrow_move_constructible_v<error_handler_type> // cf
				               && ::std::is_nothrow_move_constructible_v<range_type>    // cf
				                    && noexcept(__base_state_t(::std::declval<encoding_type&>())))
			: __base_encoding_t(::std::move(__encoding))
			, __base_error_handler_t(::std::move(__error_handler))
			, __base_state_t(this->_M_get_encoding())
			, __base_range_t(::std::move(__range)) {
			}

			constexpr __iterator_storage(range_type __range, encoding_type __encoding,
				error_handler_type __error_handler, state_type __state) noexcept(  // cf
				::std::is_nothrow_move_constructible_v<encoding_type>              // cf
				     && ::std::is_nothrow_move_constructible_v<error_handler_type> // cf
				          && ::std::is_nothrow_move_constructible_v<range_type>    // cf
				               && noexcept(
				                    __base_state_t(::std::declval<encoding_type&>(), ::std::declval<state_type>())))
			: __base_encoding_t(::std::move(__encoding))
			, __base_error_handler_t(::std::move(__error_handler))
			, __base_state_t(this->_M_get_encoding(), ::std::move(__state))
			, __base_range_t(::std::move(__range)) {
			}

			using __base_state_t::_M_get_state;

			const encoding_type& _M_get_encoding() const& noexcept {
				return this->__base_encoding_t::get_value();
			}

			encoding_type& _M_get_encoding() & noexcept {
				return this->__base_encoding_t::get_value();
			}

			encoding_type&& _M_get_encoding() && noexcept {
				return ::std::move(this->__base_encoding_t::get_value());
			}

			const _UErrorHandler& _M_get_error_handler() const& noexcept {
				return this->__base_error_handler_t::get_value();
			}

			_UErrorHandler& _M_get_error_handler() & noexcept {
				return this->__base_error_handler_t::get_value();
			}

			_UErrorHandler&& _M_get_error_handler() && noexcept {
				return ::std::move(this->__base_error_handler_t::get_value());
			}

			const _URange& _M_get_range() const& noexcept {
				return this->__base_range_t::get_value();
			}

			_URange& _M_get_range() & noexcept {
				return this->__base_range_t::get_value();
			}

			_URange&& _M_get_range() && noexcept {
				return ::std::move(this->__base_range_t::get_value());
			}
		};

		template <typename _Ref>
		class __ref_storage : private ebco<unwrap_t<_Ref>, 0>,
			                 private __state_storage<remove_cvref_t<unwrap_t<typename _Ref::encoding_type>>,
			                      remove_cvref_t<unwrap_t<typename _Ref::state_type>>, 1> {
		private:
			using _Encoding      = typename _Ref::encoding_type;
			using _Range         = typename _Ref::range_type;
			using _ErrorHandler  = typename _Ref::error_handler_type;
			using _State         = typename _Ref::state_type;
			using _UEncoding     = unwrap_remove_cvref_t<_Encoding>;
			using _URange        = unwrap_remove_cvref_t<_Range>;
			using _UErrorHandler = unwrap_remove_cvref_t<_ErrorHandler>;
			using _UState        = unwrap_remove_cvref_t<_State>;

			using __base_ref_t   = ebco<unwrap_t<_Ref>, 0>;
			using __base_state_t = __state_storage<_UEncoding, _UState, 1>;

			unwrap_t<_Ref> _M_get_ref() noexcept {
				return this->__base_ref_t::get_value();
			}

			const unwrap_t<_Ref> _M_get_ref() const noexcept {
				return this->__base_ref_t::get_value();
			}

		public:
			using range_type         = _Range;
			using encoding_type      = _Encoding;
			using state_type         = _State;
			using error_handler_type = _ErrorHandler;

			constexpr __ref_storage(const __ref_storage&)            = default;
			constexpr __ref_storage(__ref_storage&&)                 = default;
			constexpr __ref_storage& operator=(const __ref_storage&) = default;
			constexpr __ref_storage& operator=(__ref_storage&&)      = default;

			template <typename _Arg,
				::std::enable_if_t<!::std::is_same_v<remove_cvref_t<_Arg>, __ref_storage>>* = nullptr>
			constexpr __ref_storage(_Arg&& __arg) noexcept(::std::is_nothrow_constructible_v<__base_ref_t, _Arg>)
			: __base_ref_t(::std::forward<_Arg>(__arg)), __base_state_t(this->_M_get_encoding()) {
			}

			using __base_state_t::_M_get_state;

			const encoding_type& _M_get_encoding() const& noexcept {
				return this->_M_get_ref()->encoding();
			}

			encoding_type& _M_get_encoding() & noexcept {
				return this->_M_get_ref()->encoding();
			}

			encoding_type&& _M_get_encoding() && noexcept {
				return this->_M_get_ref()->encoding();
			}

			const _UErrorHandler& _M_get_error_handler() const& noexcept {
				return this->_M_get_ref()->error_handler();
			}

			_UErrorHandler& _M_get_error_handler() & noexcept {
				return this->_M_get_ref()->error_handler();
			}

			_UErrorHandler&& _M_get_error_handler() && noexcept {
				return this->_M_get_ref()->error_handler();
			}

			const _URange& _M_get_range() const& noexcept {
				return this->_M_get_ref()->range();
			}

			_URange& _M_get_range() & noexcept {
				return this->_M_get_ref()->range();
			}

			_URange&& _M_get_range() && noexcept {
				return this->_M_get_ref()->range();
			}
		};

		template <::std::size_t _MaxN, bool __is_input_or_output_iterator>
		class __cursor_cache {
		public:
			// clang-format off
			using _SizeType = ::std::conditional_t<(_MaxN <= UCHAR_MAX), unsigned char,
				::std::conditional_t<(_MaxN <= USHRT_MAX), unsigned short, 
					::std::conditional_t<(_MaxN <= UINT_MAX), unsigned int,
						::std::conditional_t<(_MaxN <= ULONG_MAX), unsigned long,
							::std::conditional_t<(_MaxN <= ULLONG_MAX), unsigned long long, ::std::size_t>
						>
					>
				>
			>;
			// clang-format on

			_SizeType _M_size     = static_cast<_SizeType>(0);
			_SizeType _M_position = static_cast<_SizeType>(0);
		};

		template <>
		class __cursor_cache<1, true> {
		public:
			using _SizeType = unsigned char;

			_SizeType _M_size = static_cast<_SizeType>(0);
		};

		template <>
		class __cursor_cache<1, false> {
		public:
			using _SizeType = unsigned char;

			_SizeType _M_size = static_cast<_SizeType>(0);
		};

		template <bool>
		class __error_cache {
		public:
			unsigned char _M_error_code = static_cast<unsigned char>(
				ztd::to_underlying(encoding_error::ok) | (ztd::to_underlying(encoding_error::ok) << 2));

			constexpr encoding_error _M_from_error() const noexcept {
				return static_cast<encoding_error>((_M_error_code & (0x02 << 0)) >> 0);
			}

			constexpr encoding_error _M_to_error() const noexcept {
				return static_cast<encoding_error>((_M_error_code & (0x02 << 2)) >> 2);
			}

			constexpr void _M_set_errors(encoding_error __from_error, encoding_error __to_error) noexcept {
				this->_M_error_code = 0;
				this->_M_error_code |= (::ztd::to_underlying(__from_error) << 0);
				this->_M_error_code |= (::ztd::to_underlying(__to_error) << 2);
			}
		};

		template <>
		class __error_cache<true> {
			constexpr encoding_error _M_from_error() const noexcept {
				return encoding_error::ok;
			}

			constexpr encoding_error _M_to_error() const noexcept {
				return encoding_error::ok;
			}
		};

	} // namespace __txt_detail
	ZTD_TEXT_INLINE_ABI_NAMESPACE_CLOSE_I_
}} // namespace ztd::text

#include <ztd/epilogue.hpp>

#endif // ZTD_TEXT_DETAIL_ENCODING_ITERATOR_STORAGE_HPP
