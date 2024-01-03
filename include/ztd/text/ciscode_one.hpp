// =============================================================================
//
// ztd.text
// Copyright © JeanHeyd "ThePhD" Meneide and Shepherd's Oasis, LLC
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

#ifndef ZTD_TEXT_CISCODE_ONE_HPP
#define ZTD_TEXT_CISCODE_ONE_HPP

#include <ztd/text/version.hpp>

#include <ztd/text/recode_one.hpp>

#include <ztd/prologue.hpp>

namespace ztd { namespace text {
	ZTD_TEXT_INLINE_ABI_NAMESPACE_OPEN_I_

	//////
	/// @addtogroup ztd_text_ciscode_one ztd::text::ciscode_one[_into/_to]
	///
	/// @brief These functions convert from a view of input code units into a view of output code units
	/// (typically, through an intermediary code point type that is similar between the two) using either the
	/// inferred or specified encodings. If no error handler is provided, the equivalent of the
	/// ztd::text::default_handler_t is used by default, but it is marked as careless. If no associated state is
	/// provided for either the "to" or "from" encodings, one will be created with automatic storage duration (as
	/// a "stack" variable) for the provided encoding.
	/// @{

	//////
	/// @copydoc ztd::text::basic_recode_one_into_raw
	template <typename... _Args>
	constexpr auto basic_ciscode_one_into_raw(_Args&&... __args) noexcept(
		noexcept(::ztd::text::basic_recode_one_into_raw(::std::forward<_Args>(__args)...)))
		-> decltype(::ztd::text::basic_recode_one_into_raw(::std::forward<_Args>(__args)...)) {
		return ::ztd::text::basic_recode_one_into_raw(::std::forward<_Args>(__args)...);
	}

	//////
	/// @copydoc ztd::text::basic_recode_one_into
	template <typename... _Args>
	constexpr auto ciscode_one_into(_Args&&... __args) noexcept(
		noexcept(::ztd::text::basic_recode_one_into(::std::forward<_Args>(__args)...)))
		-> decltype(::ztd::text::basic_recode_one_into(::std::forward<_Args>(__args)...)) {
		return ::ztd::text::basic_recode_one_into(::std::forward<_Args>(__args)...);
	}

	//////
	/// @copydoc ztd::text::recode_one_to
	template <typename _Arg0 = void, typename... _Args>
	constexpr auto ciscode_one_to(_Args&&... __args) noexcept(
		noexcept(::ztd::text::recode_one_to<_Arg0>(::std::forward<_Args>(__args)...)))
		-> decltype(::ztd::text::recode_one_to<_Arg0>(::std::forward<_Args>(__args)...)) {
		return ::ztd::text::recode_one_to<_Arg0>(::std::forward<_Args>(__args)...);
	}

	//////
	/// @copydoc ztd::text::recode_one
	template <typename _Arg0 = void, typename... _Args>
	constexpr auto ciscode_one(_Args&&... __args) noexcept(
		noexcept(::ztd::text::recode_one<_Arg0>(::std::forward<_Args>(__args)...)))
		-> decltype(::ztd::text::recode_one<_Arg0>(::std::forward<_Args>(__args)...)) {
		return ::ztd::text::recode_one<_Arg0>(::std::forward<_Args>(__args)...);
	}


	//////
	/// @}

	ZTD_TEXT_INLINE_ABI_NAMESPACE_CLOSE_I_
}} // namespace ztd::text

#include <ztd/epilogue.hpp>

#endif
