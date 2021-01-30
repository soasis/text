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

#ifndef ZTD_TEXT_DETAIL_MEMORY_HPP
#define ZTD_TEXT_DETAIL_MEMORY_HPP

#include <ztd/text/version.hpp>

#include <ztd/text/detail/type_traits.hpp>

#include <memory>

namespace ztd { namespace text {
	ZTD_TEXT_INLINE_ABI_NAMESPACE_OPEN_I_
	namespace __detail { namespace __adl {

		template <typename _Type>
		using __detect_to_address = decltype(::std::pointer_traits<_Type>::to_address(::std::declval<_Type&>()));

		template <typename _Type>
		constexpr _Type* __adl_to_address(_Type* __ptr) noexcept {
			static_assert(!::std::is_function_v<_Type>, "the pointer shall not be function pointer type");
			return __ptr;
		}

		template <typename _Pointer, ::std::enable_if_t<!::std::is_pointer_v<_Pointer>>* = nullptr>
		auto __adl_to_address(const _Pointer& p) noexcept {
			if constexpr (__is_detected_v<__detect_to_address, _Pointer>) {
				return ::std::pointer_traits<_Pointer>::to_address(p);
			}
			else {
				return __adl_to_address(p.operator->());
			}
		}
	}} // namespace __detail::__adl
	ZTD_TEXT_INLINE_ABI_NAMESPACE_CLOSE_I_
}} // namespace ztd::text

#endif // ZTD_TEXT_DETAIL_MEMORY_HPP
