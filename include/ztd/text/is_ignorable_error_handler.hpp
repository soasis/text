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

#ifndef ZTD_TEXT_IS_IGNORABLE_ERROR_HANDLER_HPP
#define ZTD_TEXT_IS_IGNORABLE_ERROR_HANDLER_HPP

#include <ztd/text/version.hpp>

#include <ztd/text/forward.hpp>

#include <ztd/idk/type_traits.hpp>

#include <ztd/prologue.hpp>

namespace ztd { namespace text {
	ZTD_TEXT_INLINE_ABI_NAMESPACE_OPEN_I_

	namespace __txt_detail {
		template <typename _Type>
		using __detect_assume_valid = decltype(_Type::assume_valid::value);

		template <typename, typename = void>
		struct __is_ignorable_error_handler_sfinae : public ::std::false_type { };

		template <typename _Type>
		struct __is_ignorable_error_handler_sfinae<_Type,
			::std::enable_if_t<is_detected_v<__detect_assume_valid, _Type>>>
		: public ::std::integral_constant<bool, _Type::assume_valid::value> { };
	} // namespace __txt_detail

	//////
	/// @addtogroup ztd_text_properties Property and Trait Helpers
	///
	/// @{

	//////
	/// @brief Whether or not the given `_Type` is an error handler that can be ignored.
	///
	/// @tparam _Type the Error Handling type to chec.
	///
	/// @remarks An error handler type can mark itself as ignorable by using a `using` `assume_valid` @c =
	/// `std::integral_constant`<bool, `value`> where `value` determines if the type's error handling callback can
	/// be ignored. This is what ztd::text::assume_valid does. Being configurable means templated error handlers can
	/// select whether or not they should be ignorable based on compile time, safe conditions that you can make up
	/// (including checking Macros or other environment data as a means of determining whether or not validity should
	/// be ignored.) If this results in a type derived from `std::true_type` and the encoder object using it
	/// encounters an error, then it is Undefined Behavior what occurs afterwards.
	template <typename _Type>
	class is_ignorable_error_handler
	: public __txt_detail::__is_ignorable_error_handler_sfinae<::ztd::remove_cvref_t<_Type>> { };

	//////
	/// @brief An alias of the inner `value` for ztd::text::is_ignorable_error_handler.
	template <typename _Type>
	inline constexpr bool is_ignorable_error_handler_v
		= is_ignorable_error_handler<::ztd::remove_cvref_t<_Type>>::value;

	//////
	/// @}

	ZTD_TEXT_INLINE_ABI_NAMESPACE_CLOSE_I_
}} // namespace ztd::text

#include <ztd/epilogue.hpp>

#endif
