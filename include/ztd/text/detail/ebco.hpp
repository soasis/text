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

#ifndef ZTD_TEXT_DETAIL_EBCO_HPP
#define ZTD_TEXT_DETAIL_EBCO_HPP 1

#include <ztd/text/version.hpp>

#include <utility>
#include <type_traits>

namespace ztd { namespace text {
	ZTD_TEXT_INLINE_ABI_NAMESPACE_OPEN_I_
	namespace __txt_detail {

		template <typename _Type, ::std::size_t = 0, typename = void>
		class __ebco {
		private:
			_Type _M_value;

		public:
			__ebco()              = default;
			__ebco(const __ebco&) = default;
			__ebco(__ebco&&)      = default;
			__ebco& operator=(const __ebco&) = default;
			__ebco& operator=(__ebco&&) = default;
			constexpr __ebco(const _Type& __value) noexcept(::std::is_nothrow_copy_constructible_v<_Type>)
			: _M_value(__value) {};
			constexpr __ebco(_Type&& __value) noexcept(::std::is_nothrow_move_constructible_v<_Type>)
			: _M_value(::std::move(__value)) {};
			constexpr __ebco& operator=(const _Type& __value) noexcept(::std::is_nothrow_copy_assignable_v<_Type>) {
				this->_M_value = __value;
				return *this;
			}
			constexpr __ebco& operator=(_Type&& __value) noexcept(::std::is_nothrow_move_assignable_v<_Type>) {
				this->_M_value = ::std::move(__value);
				return *this;
			};
			template <typename _Arg, typename... _Args,
				typename = ::std::enable_if_t<
				     !::std::is_same_v<::std::remove_reference_t<::std::remove_cv_t<_Arg>>,
				          __ebco> && !::std::is_same_v<::std::remove_reference_t<::std::remove_cv_t<_Arg>>, _Type>>>
			constexpr __ebco(_Arg&& __arg, _Args&&... __args) noexcept(
				::std::is_nothrow_constructible_v<_Type, _Arg, _Args...>)
			: _M_value(::std::forward<_Arg>(__arg), ::std::forward<_Args>(__args)...) {
			}

			constexpr _Type& get_value() & noexcept {
				return static_cast<_Type&>(this->_M_value);
			}

			constexpr _Type const& get_value() const& noexcept {
				return static_cast<_Type const&>(this->_M_value);
			}

			constexpr _Type&& get_value() && noexcept {
				return static_cast<_Type&&>(this->_M_value);
			}
		};

		template <typename _Type, ::std::size_t _Tag>
		class __ebco<_Type, _Tag,
			::std::enable_if_t<
			     ::std::is_class_v<_Type> && !::std::is_final_v<_Type> && !::std::is_reference_v<_Type>>>
		: private _Type {
		public:
			__ebco()              = default;
			__ebco(const __ebco&) = default;
			__ebco(__ebco&&)      = default;
			constexpr __ebco(const _Type& __value) noexcept(::std::is_nothrow_copy_constructible_v<_Type>)
			: _Type(__value) {};
			constexpr __ebco(_Type&& __value) noexcept(::std::is_nothrow_move_constructible_v<_Type>)
			: _Type(::std::move(__value)) {};
			template <typename _Arg, typename... _Args,
				typename = ::std::enable_if_t<
				     !::std::is_same_v<::std::remove_reference_t<::std::remove_cv_t<_Arg>>,
				          __ebco> && !::std::is_same_v<::std::remove_reference_t<::std::remove_cv_t<_Arg>>, _Type>>>
			constexpr __ebco(_Arg&& __arg, _Args&&... __args) noexcept(
				::std::is_nothrow_constructible_v<_Type, _Arg, _Args...>)
			: _Type(::std::forward<_Arg>(__arg), ::std::forward<_Args>(__args)...) {
			}

			__ebco& operator=(const __ebco&) = default;
			__ebco& operator=(__ebco&&) = default;
			constexpr __ebco& operator=(const _Type& __value) noexcept(::std::is_nothrow_copy_assignable_v<_Type>) {
				static_cast<_Type&>(*this) = __value;
				return *this;
			}
			constexpr __ebco& operator=(_Type&& __value) noexcept(::std::is_nothrow_move_assignable_v<_Type>) {
				static_cast<_Type&>(*this) = ::std::move(__value);
				return *this;
			}

			constexpr _Type& get_value() & noexcept {
				return static_cast<_Type&>(*this);
			}

			constexpr _Type const& get_value() const& noexcept {
				return static_cast<_Type const&>(*this);
			}

			constexpr _Type&& get_value() && noexcept {
				return static_cast<_Type&&>(*this);
			}
		};

		template <typename _Type, ::std::size_t _Tag>
		class __ebco<_Type&, _Tag> {
		private:
			_Type* _M_p_value;

		public:
			__ebco()              = default;
			__ebco(const __ebco&) = default;
			__ebco(__ebco&&)      = default;
			__ebco& operator=(const __ebco&) = default;
			__ebco& operator=(__ebco&&) = default;
			constexpr __ebco(_Type& __value) noexcept : _M_p_value(::std::addressof(__value)) {};
			constexpr __ebco& operator=(_Type& __value) noexcept {
				*(this->_M_p_value) = __value;
				return *this;
			}

			constexpr _Type& get_value() & noexcept {
				return *(this->_M_p_value);
			}

			constexpr _Type const& get_value() const& noexcept {
				return *(this->_M_p_value);
			}

			constexpr _Type&& get_value() && noexcept {
				return ::std::move(*(this->_M_p_value));
			}
		};
	} // namespace __txt_detail
	ZTD_TEXT_INLINE_ABI_NAMESPACE_CLOSE_I_
}} // namespace ztd::text


#endif // ZTD_TEXT_DETAIL_EBCO_HPP