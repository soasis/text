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

#ifndef ZTD_TEXT_DEFAULT_HANDLER_HPP
#define ZTD_TEXT_DEFAULT_HANDLER_HPP

#include <ztd/text/version.hpp>

#include <ztd/text/assume_valid_handler.hpp>
#include <ztd/text/throw_handler.hpp>
#include <ztd/text/replacement_handler.hpp>

#include <ztd/idk/type_traits.hpp>

#include <climits>

#include <ztd/prologue.hpp>

namespace ztd { namespace text {
	ZTD_TEXT_INLINE_ABI_NAMESPACE_OPEN_I_

	//////
	/// @brief The default error handler for the entire library. Can be configured to use different strategies at build
	/// time. Without configuration, it defaults to the ztd::text::replacement_handler_t.
	class default_handler_t
#if ZTD_IS_ON(ZTD_TEXT_DEFAULT_HANDLER_THROWS)
	: private throw_handler_t {
	private:
		using __error_handler_base_t = throw_handler_t;
#else
	: private replacement_handler_t {
	private:
		using __error_handler_base_t = replacement_handler_t;
#endif

	public:
		//////
		///@brief The underlying error handler type.
		using error_handler = __error_handler_base_t;

		using __error_handler_base_t::__error_handler_base_t;

		using __error_handler_base_t::operator();
	};

	//////
	/// @brief An instance of the default_handler_t type for ease of use.
	inline constexpr default_handler_t default_handler = {};

	namespace __txt_detail {
		template <typename _ErrorHandler>
		constexpr auto __duplicate_or_be_careless(_ErrorHandler& __original) {
			using _UErrorHandler = remove_cvref_t<_ErrorHandler>;
			if constexpr (!::std::is_function_v<_UErrorHandler>) {
				if constexpr (::std::is_copy_constructible_v<_UErrorHandler>) {
					return __original;
				}
				else {
					return default_handler_t {};
				}
			}
			else {
				return default_handler_t {};
			}
		}
	} // namespace __txt_detail

	ZTD_TEXT_INLINE_ABI_NAMESPACE_CLOSE_I_
}} // namespace ztd::text


#include <ztd/epilogue.hpp>

#endif
