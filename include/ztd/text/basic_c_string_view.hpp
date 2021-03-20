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

#ifndef ZTD_TEXT_BASIC_C_STRING_VIEW_HPP
#define ZTD_TEXT_BASIC_C_STRING_VIEW_HPP

#include <ztd/text/version.hpp>

#include <ztd/text/char8_t.hpp>

#include <ztd/text/detail/type_traits.hpp>
#include <ztd/text/detail/empty_string.hpp>
#include <ztd/text/detail/assert.hpp>

#include <string_view>
#include <utility>

#include <ztd/text/detail/prologue.hpp>

namespace ztd { namespace text {
	ZTD_TEXT_INLINE_ABI_NAMESPACE_OPEN_I_

	//////
	/// @brief A class that is identical to std::string_view, except that it attempts to verify and guarantee that
	/// ``.data() + .size()``, when dereferenced, is valid and gives a nullptr. The ``.size()`` does not include the
	/// null terminator in its count.
	//////
	template <typename _CharType, typename _Traits = ::std::char_traits<_CharType>>
	class basic_c_string_view : private ::std::basic_string_view<_CharType, _Traits> {
	private:
		using __base_t = ::std::basic_string_view<_CharType, _Traits>;

		constexpr bool _M_last_element_check() const noexcept {
			const _CharType& __last_element = *(this->data() + this->size());
			return __last_element == static_cast<_CharType>('\0');
		}

	public:
		using const_iterator         = typename __base_t::const_iterator;
		using const_pointer          = typename __base_t::const_pointer;
		using const_reverse_iterator = typename __base_t::const_reverse_iterator;
		using difference_type        = typename __base_t::difference_type;
		using iterator               = typename __base_t::iterator;
		using pointer                = typename __base_t::pointer;
		using reference              = typename __base_t::reference;
		using const_reference        = typename __base_t::reference;
		using reverse_iterator       = typename __base_t::reverse_iterator;
		using size_type              = typename __base_t::size_type;
		using traits_type            = typename __base_t::traits_type;
		using value_type             = typename __base_t::value_type;

		using __base_t::npos;

		constexpr basic_c_string_view() noexcept
		: basic_c_string_view(
			static_cast<const_pointer>(__txt_detail::__empty_string<_CharType>()), static_cast<size_type>(1)) {
		}

		constexpr basic_c_string_view(const_iterator __arg0, const_iterator __arg1) noexcept
#if defined(_ITERATOR_DEBUG_LEVEL) && _ITERATOR_DEBUG_LEVEL >= 1
		: __base_t(__arg0 == __arg1 ? __txt_detail::__empty_string<_CharType>() : ::std::addressof(*__arg0),
			::std::distance(__arg0, __arg1)) {
#else
		: __base_t(::std::addressof(*__arg0), ::std::distance(__arg0, __arg1)) {
#endif
			ZTD_TEXT_ASSERT_MESSAGE_I_("c_string_view must be null-terminated!", this->_M_last_element_check());
		}

		constexpr basic_c_string_view(const_iterator __arg0, size_type __arg1) noexcept : __base_t(__arg0, __arg1) {
			ZTD_TEXT_ASSERT_MESSAGE_I_("c_string_view must be null-terminated!", this->_M_last_element_check());
		}

#if defined(_ITERATOR_DEBUG_LEVEL) && _ITERATOR_DEBUG_LEVEL >= 0
		constexpr basic_c_string_view(const_pointer __arg0, const_pointer __arg1) noexcept
#if defined(_ITERATOR_DEBUG_LEVEL) && _ITERATOR_DEBUG_LEVEL >= 1
		: __base_t(__arg0 == __arg1 ? __txt_detail::__empty_string<_CharType>() : ::std::addressof(*__arg0),
			::std::distance(__arg0, __arg1)) {
#else
		: __base_t(::std::addressof(*__arg0), ::std::distance(__arg0, __arg1)) {
#endif
			ZTD_TEXT_ASSERT_MESSAGE_I_("c_string_view must be null-terminated!", this->_M_last_element_check());
		}

		constexpr basic_c_string_view(const_pointer __arg0, size_type __arg1) : __base_t(__arg0, __arg1) {
			ZTD_TEXT_ASSERT_MESSAGE_I_("c_string_view must be null-terminated!", this->_M_last_element_check());
		}
#endif // not needed if iterator is implicitly constructible from the pointer

		constexpr basic_c_string_view(const_pointer __arg0) noexcept : __base_t(__arg0, _Traits::length(__arg0)) {
			ZTD_TEXT_ASSERT_MESSAGE_I_("c_string_view must be null-terminated!", this->_M_last_element_check());
		}

		template <typename _Arg,
			::std::enable_if_t<
			     !::std::is_same_v<__txt_detail::__remove_cvref_t<_Arg>,
			          basic_c_string_view> && !::std::is_same_v<__txt_detail::__remove_cvref_t<_Arg>, const_pointer> && !::std::is_array_v<__txt_detail::__remove_cvref_t<_Arg>>>* = nullptr>
		constexpr basic_c_string_view(_Arg&& __arg) noexcept : __base_t(::std::data(__arg), ::std::size(__arg)) {
			ZTD_TEXT_ASSERT_MESSAGE_I_("c_string_view must be null-terminated!", this->_M_last_element_check());
		}

		constexpr basic_c_string_view(basic_c_string_view&&)      = default;
		constexpr basic_c_string_view(const basic_c_string_view&) = default;
		constexpr basic_c_string_view& operator=(basic_c_string_view&&) = default;
		constexpr basic_c_string_view& operator=(const basic_c_string_view&) = default;

		constexpr size_type size() const noexcept {
			return this->__base_t::size();
		}

		constexpr size_type length() const noexcept {
			return this->size();
		}

		using __base_t::front;
		using __base_t::max_size;
		using __base_t::operator[];

		constexpr bool empty() const noexcept {
			return this->cbegin() == this->cend();
		}

		constexpr reference back() noexcept {
			return *(this->data() + this->size());
		}

		constexpr const_reference back() const noexcept {
			return *(this->data() + this->size());
		}

		constexpr pointer data() noexcept {
			return this->__base_t::data();
		}

		constexpr const_pointer data() const noexcept {
			return this->__base_t::data();
		}

		constexpr const_pointer cdata() const noexcept {
			return this->__base_t::data();
		}

		constexpr const_pointer c_str() const noexcept {
			return this->data();
		}

		constexpr const_iterator begin() const noexcept {
			return this->__base_t::begin();
		}

		constexpr iterator begin() noexcept {
			return this->__base_t::begin();
		}

		constexpr const_iterator cbegin() const noexcept {
			return this->__base_t::cbegin();
		}

		constexpr const_iterator end() const noexcept {
			const_iterator __it = this->__base_t::end();
			return __it;
		}

		constexpr iterator end() noexcept {
			iterator __it = this->__base_t::end();
			return __it;
		}

		constexpr const_iterator cend() const noexcept {
			const_iterator __it = this->__base_t::cend();
			return __it;
		}

		constexpr const_reverse_iterator rbegin() const noexcept {
			return this->__base_t::rbegin();
		}

		constexpr reverse_iterator rbegin() noexcept {
			return this->__base_t::rbegin();
		}

		constexpr const_reverse_iterator crbegin() const noexcept {
			return this->__base_t::crbegin();
		}

		constexpr const_reverse_iterator rend() const noexcept {
			const_reverse_iterator __it = this->__base_t::rend();
			return __it;
		}

		constexpr reverse_iterator rend() noexcept {
			reverse_iterator __it = this->__base_t::rend();
			return __it;
		}

		constexpr const_reverse_iterator crend() const noexcept {
			const_reverse_iterator __it = this->__base_t::crend();
			return __it;
		}

		// TODO: properly re-implement all of these methods to exclude the null terminator!!
		using __base_t::compare;

		using __base_t::copy;
#if ZTD_TEXT_IS_ON(ZTD_TEXT_STD_LIBRARY_STARTS_ENDS_WITH_I_)
		using __base_t::ends_with;
		using __base_t::starts_with;
#endif
#if ZTD_TEXT_IS_ON(ZTD_TEXT_STD_LIBRARY_STRING_CONTAINS_I_)
		using __base_t::contains;
#endif

		using __base_t::find;
		using __base_t::remove_prefix;
		using __base_t::rfind;

		using __base_t::find_first_not_of;
		using __base_t::find_first_of;
		using __base_t::find_last_not_of;
		using __base_t::find_last_of;
	};


	template <typename _CharType, typename _Traits>
	constexpr bool operator==(
		basic_c_string_view<_CharType, _Traits> __left, basic_c_string_view<_CharType, _Traits> __right) noexcept {
		return __left.size() == __right.size() && __left.compare(__right) == 0;
	}

	template <typename _CharType, typename _Traits>
	constexpr bool operator==(basic_c_string_view<_CharType, _Traits> __left,
		__txt_detail::__type_identity_t<basic_c_string_view<_CharType, _Traits>> __right) noexcept {
		return __left.size() == __right.size() && __left.compare(__right) == 0;
	}

	template <typename _CharType, typename _Traits>
	constexpr bool operator==(__txt_detail::__type_identity_t<basic_c_string_view<_CharType, _Traits>> __left,
		basic_c_string_view<_CharType, _Traits> __right) noexcept {
		return __left.size() == __right.size() && __left.compare(__right) == 0;
	}

	template <typename _CharType, typename _Traits>
	constexpr bool operator!=(
		basic_c_string_view<_CharType, _Traits> __left, basic_c_string_view<_CharType, _Traits> __right) noexcept {
		return !(__left == __right);
	}

	template <typename _CharType, typename _Traits>
	constexpr bool operator!=(basic_c_string_view<_CharType, _Traits> __left,
		__txt_detail::__type_identity_t<basic_c_string_view<_CharType, _Traits>> __right) noexcept {
		return !(__left == __right);
	}

	template <typename _CharType, typename _Traits>
	constexpr bool operator!=(__txt_detail::__type_identity_t<basic_c_string_view<_CharType, _Traits>> __left,
		basic_c_string_view<_CharType, _Traits> __right) noexcept {
		return !(__left == __right);
	}

	template <typename _CharType, typename _Traits>
	constexpr bool operator<(
		basic_c_string_view<_CharType, _Traits> __left, basic_c_string_view<_CharType, _Traits> __right) noexcept {
		return __left.compare(__right) < 0;
	}

	template <typename _CharType, typename _Traits>
	constexpr bool operator<(basic_c_string_view<_CharType, _Traits> __left,
		__txt_detail::__type_identity_t<basic_c_string_view<_CharType, _Traits>> __right) noexcept {
		return __left.compare(__right) < 0;
	}

	template <typename _CharType, typename _Traits>
	constexpr bool operator<(__txt_detail::__type_identity_t<basic_c_string_view<_CharType, _Traits>> __left,
		basic_c_string_view<_CharType, _Traits> __right) noexcept {
		return __left.compare(__right) < 0;
	}

	template <typename _CharType, typename _Traits>
	constexpr bool operator>(
		basic_c_string_view<_CharType, _Traits> __left, basic_c_string_view<_CharType, _Traits> __right) noexcept {
		return __left.compare(__right) > 0;
	}

	template <typename _CharType, typename _Traits>
	constexpr bool operator>(basic_c_string_view<_CharType, _Traits> __left,
		__txt_detail::__type_identity_t<basic_c_string_view<_CharType, _Traits>> __right) noexcept {
		return __left.compare(__right) > 0;
	}

	template <typename _CharType, typename _Traits>
	constexpr bool operator>(__txt_detail::__type_identity_t<basic_c_string_view<_CharType, _Traits>> __left,
		basic_c_string_view<_CharType, _Traits> __right) noexcept {
		return __left.compare(__right) > 0;
	}

	template <typename _CharType, typename _Traits>
	constexpr bool operator<=(
		basic_c_string_view<_CharType, _Traits> __left, basic_c_string_view<_CharType, _Traits> __right) noexcept {
		return __left.compare(__right) <= 0;
	}

	template <typename _CharType, typename _Traits>
	constexpr bool operator<=(basic_c_string_view<_CharType, _Traits> __left,
		__txt_detail::__type_identity_t<basic_c_string_view<_CharType, _Traits>> __right) noexcept {
		return __left.compare(__right) <= 0;
	}

	template <typename _CharType, typename _Traits>
	constexpr bool operator<=(__txt_detail::__type_identity_t<basic_c_string_view<_CharType, _Traits>> __left,
		basic_c_string_view<_CharType, _Traits> __right) noexcept {
		return __left.compare(__right) <= 0;
	}

	template <typename _CharType, typename _Traits>
	constexpr bool operator>=(
		basic_c_string_view<_CharType, _Traits> __left, basic_c_string_view<_CharType, _Traits> __right) noexcept {
		return __left.compare(__right) >= 0;
	}

	template <typename _CharType, typename _Traits>
	constexpr bool operator>=(basic_c_string_view<_CharType, _Traits> __left,
		__txt_detail::__type_identity_t<basic_c_string_view<_CharType, _Traits>> __right) noexcept {
		return __left.compare(__right) >= 0;
	}

	template <typename _CharType, typename _Traits>
	constexpr bool operator>=(__txt_detail::__type_identity_t<basic_c_string_view<_CharType, _Traits>> __left,
		basic_c_string_view<_CharType, _Traits> __right) noexcept {
		return __left.compare(__right) >= 0;
	}


	template <typename _Ty, typename _Traits, typename _It, typename _Sen>
	constexpr ::std::basic_string_view<_Ty, _Traits> reconstruct(
		::std::in_place_type_t<basic_c_string_view<_Ty, _Traits>>, _It __iterator, _Sen __sentinel) {
		using _SizeType = typename ::std::basic_string_view<_Ty, _Traits>::size_type;
		if constexpr (!::std::is_integral_v<_Sen>) {
#if defined(_ITERATOR_DEBUG_LEVEL) && _ITERATOR_DEBUG_LEVEL >= 1
			if (__iterator == __sentinel) {
				const auto& __empty_str = __txt_detail::__empty_string<_Ty>();
				return ::std::basic_string_view<_Ty, _Traits>(__empty_str + 0, 0);
			}
#endif
			return ::std::basic_string_view<_Ty, _Traits>(
				::std::addressof(*__iterator), static_cast<_SizeType>(__sentinel - __iterator));
		}
		else {
#if defined(_ITERATOR_DEBUG_LEVEL) && _ITERATOR_DEBUG_LEVEL >= 1
			if (static_cast<_SizeType>(__sentinel) == static_cast<_SizeType>(0)) {
				const auto& __empty_str = __txt_detail::__empty_string<_Ty>();
				return ::std::basic_string_view<_Ty, _Traits>(__empty_str + 0, 0);
			}
#endif
			return ::std::basic_string_view<_Ty, _Traits>(
				::std::addressof(*__iterator), static_cast<_SizeType>(__sentinel));
		}
	}
	ZTD_TEXT_INLINE_ABI_NAMESPACE_CLOSE_I_
}} // namespace ztd::text

namespace std {

	template <typename _CharType, typename _Traits>
	struct hash<::ztd::text::basic_c_string_view<_CharType, _Traits>> {
		constexpr size_t operator()(const ::ztd::text::basic_c_string_view<_CharType, _Traits>& __c_string) const {
			::std::hash<::std::basic_string_view<_CharType, _Traits>> h;
			return h(__c_string);
		}
	};

} // namespace std

#include <ztd/text/detail/epilogue.hpp>

#endif // ZTD_TEXT_BASIC_C_STRING_VIEW_HPP
