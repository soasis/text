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

#ifndef ZTD_TEXT_SKIP_HANDLER_HPP
#define ZTD_TEXT_SKIP_HANDLER_HPP

#include <ztd/text/version.hpp>

#include <ztd/text/decode_result.hpp>
#include <ztd/text/encode_result.hpp>
#include <ztd/text/skip_input_error.hpp>

#include <ztd/idk/unwrap.hpp>

#include <ztd/prologue.hpp>

namespace ztd { namespace text {
	ZTD_TEXT_INLINE_ABI_NAMESPACE_OPEN_I_

	//////
	/// @brief An error handler that simply skips bad input on error, and performs no replacement.
	///
	/// @remarks If a sequence of text depends on the state of the previous text, and there is an illegal sequence in
	/// the middle of such a sequence, using their error handler can cause a cascade of failures as the `state` may not
	/// be prepared for the new input; therefore, it may skip more than a user of this error handler might expect. This
	/// is something that may happen due to shift state issues (e.g., when an encoding is not self-synchronizing
	/// (ztd::text::is_self_syncrhonizing_code)).
	class skip_handler_t {
	public:
		//////
		/// @brief Skips over any input that may produce an error.
		///
		/// @param[in] __encoding The Encoding that experienced the error.
		/// @param[in] __result The current state of the encode operation.
		/// @param[in] __input_progress How much input was (potentially irreversibly) read from the input range before
		/// undergoing the attempted encode operation.
		/// @param[in] __output_progress How much output was (potentially irreversibly) written to the output range
		/// before undergoing the attempted encode operation.
		template <typename _Encoding, typename _Result, typename _InputProgress, typename _OutputProgress>
		constexpr auto operator()(const _Encoding& __encoding, _Result&& __result,
			const _InputProgress& __input_progress, const _OutputProgress& __output_progress) const
			noexcept(::ztd::text::is_input_error_skippable_v<const _Encoding&, _Result, const _InputProgress&,
			     const _OutputProgress&>) {
			if (__result.error_code != ztd::text::encoding_error::insufficient_output_space) {
				__result.error_code = ztd::text::encoding_error::ok;
			}
			return ::ztd::text::skip_input_error(
				__encoding, ::std::forward<_Result>(__result), __input_progress, __output_progress);
		}
	};

	//////
	/// @brief An instance of skip_handler_t for ease of use.
	inline constexpr skip_handler_t skip_handler = {};

	ZTD_TEXT_INLINE_ABI_NAMESPACE_CLOSE_I_
}} // namespace ztd::text


#include <ztd/epilogue.hpp>

#endif
