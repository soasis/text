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

#ifndef ZTD_TEXT_IS_COMPATIBLE_CODE_POINTS_HPP
#define ZTD_TEXT_IS_COMPATIBLE_CODE_POINTS_HPP

#include <ztd/text/version.hpp>

#include <ztd/text/is_unicode_code_point.hpp>

#include <ztd/idk/type_traits.hpp>

#include <ztd/prologue.hpp>

namespace ztd { namespace text {
	ZTD_TEXT_INLINE_ABI_NAMESPACE_OPEN_I_

	//////
	/// @brief Checks whether the two code point types are compatible with one another.
	///
	/// @tparam _LeftCodePoint A code point type.
	/// @tparam _RightCodePoint Another code point type.
	///
	/// @remarks The `value` boolean is true if the given code point types are:
	/// - both the same type
	/// - both considered unicode code points by ztd::text::is_unicode_code_point_v
	//////
	template <typename _LeftCodePoint, typename _RightCodePoint>
	class is_compatible_code_points
	: public ::std::integral_constant<bool,
		  ::std::is_same_v<_LeftCodePoint, _RightCodePoint> // cf
		       || (is_unicode_code_point_v<_LeftCodePoint> && is_unicode_code_point_v<_RightCodePoint>)> { };

	//////
	/// @brief A @c \::value alias for ztd::text::is_compatible_code_points
	template <typename _LeftCodePoint, typename _RightCodePoint>
	inline constexpr bool is_compatible_code_points_v
		= is_compatible_code_points<_LeftCodePoint, _RightCodePoint>::value;

	ZTD_TEXT_INLINE_ABI_NAMESPACE_CLOSE_I_
}} // namespace ztd::text

#include <ztd/epilogue.hpp>

#endif // ZTD_TEXT_IS_COMPATIBLE_CODE_POINTS_HPP
