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

#ifndef ZTD_TEXT_DETAIL_REFERENCE_WRAPPER_HPP
#define ZTD_TEXT_DETAIL_REFERENCE_WRAPPER_HPP

#include <ztd/text/version.hpp>

#include <functional>
#include <utility>
#include <type_traits>
#include <memory>

#include <ztd/text/detail/prologue.hpp>

namespace ztd { namespace text {
	ZTD_TEXT_INLINE_ABI_NAMESPACE_OPEN_I_

	namespace __txt_detail {

		template <typename _Ty>
		class __reference_wrapper {
		private:
			using _Ref = ::std::add_lvalue_reference_t<_Ty>;
			using _Ptr = ::std::add_pointer_t<_Ty>;
			_Ptr __ptr;

		public:
			constexpr __reference_wrapper(_Ref __ref) noexcept : __ptr(::std::addressof(__ref)) {
			}

			constexpr operator _Ref() const noexcept {
				return *__ptr;
			}

			constexpr _Ref get() const noexcept {
				return *__ptr;
			}
		};

		template <typename _Ty>
		__reference_wrapper(_Ty&) -> __reference_wrapper<_Ty>;

	} // namespace __txt_detail

	//////
	/// @brief A subsitute for C++20's reference wrapper if the current @c std::reference_wrapper provided by the
	/// standard library is not @c constexpr since it was only done then.
	///
	//////
	template <typename _Ty>
	using reference_wrapper =
#if ZTD_TEXT_IS_ON(ZTD_TEXT_STD_LIBRARY_REFERENCE_WRAPPER_CONSTEXPR_I_)
		::std::reference_wrapper<_Ty>
#else
		::ztd::text::__txt_detail::__reference_wrapper<_Ty>
#endif
		;

	ZTD_TEXT_INLINE_ABI_NAMESPACE_CLOSE_I_
}} // namespace ztd::text

#endif // ZTD_TEXT_DETAIL_REFERENCE_WRAPPER_HPP
