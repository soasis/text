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

#ifndef ZTD_TEXT_DETAIL_ENCODING_ITERATOR_STORAGE_HPP
#define ZTD_TEXT_DETAIL_ENCODING_ITERATOR_STORAGE_HPP

#include <ztd/text/version.hpp>

#include <ztd/text/state.hpp>

#include <ztd/text/detail/range.hpp>
#include <ztd/text/detail/ebco.hpp>
#include <ztd/text/detail/math.hpp>

#include <cstddef>
#include <climits>

namespace ztd { namespace text {
	ZTD_TEXT_INLINE_ABI_NAMESPACE_OPEN_I_
	namespace __detail {

		template <typename _Encoding, typename _EncodingState, ::std::size_t _Id = 0>
		class __state_storage : private __ebco<__remove_cvref_t<__unwrap_t<_EncodingState>>, _Id> {
		private:
			using _UEncoding      = __remove_cvref_t<__unwrap_t<_Encoding>>;
			using _UEncodingState = __remove_cvref_t<__unwrap_t<_EncodingState>>;
			using __state_base_t  = __ebco<__remove_cvref_t<__unwrap_t<_EncodingState>>, _Id>;

		public:
			template <typename _ArgEncoding = _UEncodingState,
				::std::enable_if_t<
				     !is_state_independent_v<__remove_cvref_t<_ArgEncoding>,
				          _UEncodingState> && !::std::is_same_v<__remove_cvref_t<_ArgEncoding>, __state_storage>>* = nullptr>
			constexpr __state_storage(_ArgEncoding& __encoding) noexcept(
				::std::is_nothrow_constructible_v<__state_base_t, _Encoding&>)
			: __state_base_t(__encoding) {
			}
			template <typename _ArgEncoding = _UEncodingState,
				::std::enable_if_t<
				     is_state_independent_v<__remove_cvref_t<_ArgEncoding>,
				          _UEncodingState> && !::std::is_same_v<__remove_cvref_t<_ArgEncoding>, __state_storage>>* = nullptr>
			constexpr __state_storage(_ArgEncoding& __encoding) noexcept(
				::std::is_nothrow_default_constructible_v<__state_base_t>)
			: __state_base_t() {
			}
			constexpr __state_storage(_Encoding& __encoding, const _UEncodingState& __state) noexcept(
				::std::is_nothrow_constructible_v<__state_base_t, const _UEncodingState&>)
			: __state_base_t(__state) {
			}
			constexpr __state_storage(_Encoding& __encoding, _UEncodingState&& __state) noexcept(
				::std::is_nothrow_constructible_v<__state_base_t, _UEncodingState&&>)
			: __state_base_t(::std::move(__state)) {
			}
			constexpr __state_storage(const __state_storage&) = default;
			constexpr __state_storage(__state_storage&&)      = default;
			constexpr __state_storage& operator=(const __state_storage&) = default;
			constexpr __state_storage& operator=(__state_storage&&) = default;

			constexpr ::std::add_lvalue_reference_t<_UEncodingState> _M_get_state() noexcept {
				return this->__state_base_t::get_value();
			}

			constexpr ::std::add_const_t<::std::add_lvalue_reference_t<_UEncodingState>>
			_M_get_state() const noexcept {
				return this->__state_base_t::get_value();
			}
		};

		template <::std::size_t _MaxN>
		class __cache_cursor {
		private:
			using _SizeType = ::std::conditional_t<(_MaxN < __ce_ipow(2, sizeof(unsigned char) * CHAR_BIT)),
				unsigned char, ::std::size_t>;

		public:
			_SizeType _M_size;
			_SizeType _M_position;
		};

		template <>
		class __cache_cursor<1> { };

	} // namespace __detail
	ZTD_TEXT_INLINE_ABI_NAMESPACE_CLOSE_I_
}} // namespace ztd::text

#endif // ZTD_TEXT_DETAIL_ENCODING_ITERATOR_STORAGE_HPP
