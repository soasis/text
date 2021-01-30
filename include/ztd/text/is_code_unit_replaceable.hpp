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

#ifndef ZTD_TEXT_IS_CODE_UNIT_REPLACEABLE_HPP
#define ZTD_TEXT_IS_CODE_UNIT_REPLACEABLE_HPP

#include <ztd/text/version.hpp>

#include <ztd/text/code_unit.hpp>

#include <ztd/text/detail/type_traits.hpp>

#include <type_traits>

namespace ztd { namespace text {
	ZTD_TEXT_INLINE_ABI_NAMESPACE_OPEN_I_

	namespace __detail {
		template <typename _Type>
		using __detect_is_code_units_replaceable = decltype(::std::declval<const _Type&>().replacement_code_units());

		template <typename _Type>
		struct __is_code_units_replaceable
		: __detail::__is_detected<__detail::__detect_is_code_units_replaceable, _Type> { };

		template <typename _Type>
		inline constexpr bool __is_code_units_replaceable_v = __is_code_units_replaceable<_Type>::value;
	} // namespace __detail
	ZTD_TEXT_INLINE_ABI_NAMESPACE_CLOSE_I_
}} // namespace ztd::text

#endif // ZTD_TEXT_IS_CODE_UNIT_REPLACEABLE_HPP
