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

#ifndef ZTD_TEXT_SKIP_INPUT_ERROR_HPP
#define ZTD_TEXT_SKIP_INPUT_ERROR_HPP

#include <ztd/text/version.hpp>

#include <ztd/text/decode_result.hpp>
#include <ztd/text/encode_result.hpp>
#include <ztd/text/is_input_error_skippable.hpp>

#include <ztd/idk/unwrap.hpp>

#include <ztd/prologue.hpp>

namespace ztd { namespace text {
	ZTD_TEXT_INLINE_ABI_NAMESPACE_OPEN_I_

	namespace __txt_detail {
		template <typename _Encoding, typename _Result>
		constexpr bool __skip_handler_noexcept() noexcept {
			if constexpr (is_input_error_skippable_v<_Encoding, _Result>) {
				return noexcept(::std::declval<_Encoding>().skip_input_error(::std::declval<_Result>()));
			}
			else {
				return true;
			}
		}
	} // namespace __txt_detail

	//////
	/// @brief Attempts to skip over an input error in the text.
	///
	/// @param[in] __encoding The Encoding that experienced the error.
	/// @param[in] __result The current state of the encode operation.
	///
	/// @remarks If there exists a well-formed function call of the form `__encoding.skip_input_error(__result)`, it
	/// will call that function. Otherwise, it will attempt to grab the input iterator and pre-increment it exactly
	/// once. The goal for this is to provide functionality which can smartly skip over a collection of ill-formed code
	/// units or bytes in an input sequence, rather than generated e.g. 3 different replacement characters for a
	/// mal-formed UTF-8 sequence. For example, given this malformed wineglass code point as an input UTF-8 sequence:
	///
	/// `"\xC0\x9F\x8D\xB7meow"`
	///
	/// when used in conjunction with ztd::text::utf8 (and similar), a proper decode/transcode call will error on
	/// `'\\xC0'` before this function skips until the `'m'` input code unit, resulting in a leftover sequence of
	///
	/// "meow".
	template <typename _Encoding, typename _Result>
	constexpr auto skip_input_error(const _Encoding& __encoding, _Result&& __result) noexcept(
		__txt_detail::__skip_handler_noexcept<const _Encoding&, _Result>()) {
		if constexpr (is_input_error_skippable_v<const _Encoding&, _Result>) {
			return __encoding.skip_input_error(::std::forward<_Result>(__result));
		}
		else {
			// we can only advance one character at a time.
			auto __it   = ztd::ranges::begin(::std::forward<_Result>(__result).input);
			auto __last = ztd::ranges::end(__result.input);
			if (__it != __last)
				++__it;
			using _Input  = decltype(__result.input);
			using _UInput = ::ztd::remove_cvref_t<_Input>;
			using _Output = decltype(__result.output);
			using _State
				= ::std::remove_reference_t<::ztd::unwrap_t<::ztd::remove_cvref_t<decltype(__result.state)>>>;
			using _ReconstructedInput = ::ztd::ranges::reconstruct_t<_UInput, decltype(__it)&&, decltype(__last)&&>;
			using _ResultType         = ::std::conditional_t<::ztd::is_specialization_of_v<_Result, decode_result>,
                    decode_result<_ReconstructedInput, _Output, _State>,
                    encode_result<_ReconstructedInput, _Output, _State>>;
			return _ResultType(
				::ztd::ranges::reconstruct(::std::in_place_type<_UInput>, ::std::move(__it), ::std::move(__last)),
				::std::move(__result.output), __result.state, __result.error_code, __result.error_count);
		}
	}

	ZTD_TEXT_INLINE_ABI_NAMESPACE_CLOSE_I_
}} // namespace ztd::text


#include <ztd/epilogue.hpp>

#endif
