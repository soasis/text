// =============================================================================
//
// ztd.text
// Copyright © 2022-2023 JeanHeyd "ThePhD" Meneide and Shepherd's Oasis, LLC
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
// https://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
//
// ============================================================================ //

#pragma once

#ifndef ZTD_TEXT_MAX_UNITS_HPP
#define ZTD_TEXT_MAX_UNITS_HPP

#include <ztd/text/version.hpp>

#include <ztd/text/code_point.hpp>
#include <ztd/text/code_unit.hpp>

#include <ztd/idk/type_traits.hpp>

#include <ztd/prologue.hpp>

namespace ztd { namespace text {
	ZTD_TEXT_INLINE_ABI_NAMESPACE_OPEN_I_

	//////
	/// @addtogroup ztd_text_properties Property and Trait Helpers
	///
	/// @{
	/////

	//////
	/// @brief Gets the maximum number of code units needed to represent a full ztd::text::decode_one operation
	/// without running out of output space.
	template <typename _From>
	inline static constexpr ::std::size_t max_decode_code_points_v = max_code_points_v<_From> * 2;

	//////
	/// @brief Gets the maximum number of code units needed to represent a full ztd::text::encode_one operation without
	/// running out of space.
	template <typename _From>
	inline static constexpr ::std::size_t max_encode_code_units_v = max_code_units_v<_From> * 2;

	//////
	/// @brief Gets the maximum number of code units needed to represent a full ztd::text::transcode_one operation
	/// without running out of output space.
	template <typename _From, typename _To>
	inline static constexpr ::std::size_t max_recode_code_points_v
		= max_code_units_v<_From> * max_code_points_v<_To> * 2;

	//////
	/// @brief Gets the maximum number of code units needed to represent a full ztd::text::transcode_one operation
	/// without running out of output space.
	template <typename _From, typename _To>
	inline static constexpr ::std::size_t max_transcode_code_units_v
		= max_code_points_v<_From> * max_code_units_v<_To> * 2;

	//////
	/// @}
	/////

	ZTD_TEXT_INLINE_ABI_NAMESPACE_CLOSE_I_
}} // namespace ztd::text

#include <ztd/epilogue.hpp>

#endif
