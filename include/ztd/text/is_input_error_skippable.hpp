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

#ifndef ZTD_TEXT_IS_INPUT_ERROR_SKIPPABLE_HPP
#define ZTD_TEXT_IS_INPUT_ERROR_SKIPPABLE_HPP

#include <ztd/text/version.hpp>

#include <ztd/text/forward.hpp>

#include <ztd/idk/type_traits.hpp>

#include <ztd/prologue.hpp>

namespace ztd { namespace text {
	ZTD_TEXT_INLINE_ABI_NAMESPACE_OPEN_I_

	namespace __txt_detail {
		template <typename _Encoding, typename _Result>
		using __detect_skip_input_error
			= decltype(::std::declval<_Encoding>().skip_input_error(::std::declval<_Result>()));
	} // namespace __txt_detail

	//////
	/// @addtogroup ztd_text_properties Property and Trait Helpers
	///
	/// @{

	//////
	/// @brief Whether or not the given `_Encoding` has a function called `skip_input_error` that takes the given
	/// `_Result` type.
	///
	/// @tparam _Encoding The encoding that may contain the skip_input_error function.
	/// @tparam _Result The result type to check if the input is callable.
	///
	/// @remarks This is used by ztd::text::replacement_handler and ztd::text::skip_handler to pass over malformed
	/// input when it happens.
	template <typename _Encoding, typename _Result>
	class is_input_error_skippable : public ::std::integral_constant<bool,
		                                 is_detected_v<__txt_detail::__detect_skip_input_error, _Encoding, _Result>> {
	};

	//////
	/// @brief An alias of the inner `value` for ztd::text::is_input_error_skippable.
	template <typename _Encoding, typename _Result>
	inline constexpr bool is_input_error_skippable_v = is_input_error_skippable<_Encoding, _Result>::value;

	//////
	/// @}

	ZTD_TEXT_INLINE_ABI_NAMESPACE_CLOSE_I_
}} // namespace ztd::text

#include <ztd/epilogue.hpp>

#endif
