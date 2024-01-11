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
		template <typename _Encoding, typename _Result, typename _InputProgress, typename _OutputProgress>
		constexpr bool __skip_input_error_noexcept() noexcept {
			if constexpr (is_input_error_skippable_v<_Encoding, _Result, _InputProgress, _OutputProgress>) {
				return noexcept(::std::declval<_Encoding>().skip_input_error(::std::declval<_Result>(),
					::std::declval<_InputProgress>(), ::std::declval<_OutputProgress>()));
			}
			else {
				return true;
			}
		}

	} // namespace __txt_detail

	//////
	/// @addtogroup ztd_text_properties Property and Trait Helpers
	///
	/// @{

	//////
	/// @brief Checks whether calling ztd::text::skip_input_error is an exceptionless operation.
	///
	/// @tparam _Encoding The encoding type.
	/// @tparam _Result  The result type; either a ztd::text::decode_result or an ztd::text::encode_result-typed
	/// object.
	/// @tparam _InputProgress A type representing the input progress type.
	/// @tparam _OutputProgress  A type representing the output progress type.
	template <typename _Encoding, typename _Result, typename _InputProgress, typename _OutputProgress>
	inline constexpr bool is_nothrow_skip_input_error_v = __txt_detail::__skip_input_error_noexcept<const _Encoding&,
		_Result, const _InputProgress&, const _OutputProgress&>();

	//////
	/// @brief Attempts to skip over an input error in the text.
	///
	/// @param[in] __result The current result state of the encode or decode operation.
	/// @param[in] __input_progress A contiguous range containing all of the (potentially) irreversibly read input from
	/// an encoding operation.
	/// @param[in] __output_progress A contiguous range containing all of the (potentially) irreversibly written output
	/// from an encoding operation.
	///
	/// @remarks This function is specifically for UTF-32 input, where e.g. multiple surrogates may be part of the
	/// incoming text and the target encoding does not support that. Therefore, it will skip over every too-large
	/// codepoint, and every surrogate pair codepoint, before stopping.
	template <typename _Result, typename _InputProgress, typename _OutputProgress>
	constexpr auto skip_utf32_input_error(_Result&& __result, const _InputProgress& __input_progress,
		const _OutputProgress& __output_progress) noexcept {
		// we can only advance one character at a time.
		auto __it     = ztd::ranges::begin(::std::forward<_Result>(__result).input);
		auto __last   = ztd::ranges::end(__result.input);
		using _Input  = decltype(__result.input);
		using _UInput = ::ztd::remove_cvref_t<_Input>;
		using _Output = decltype(__result.output);
		using _State  = ::std::remove_reference_t<::ztd::unwrap_t<::ztd::remove_cvref_t<decltype(__result.state)>>>;
		using _ReconstructedInput = ::ztd::ranges::reconstruct_t<_UInput, decltype(__it)&&, decltype(__last)&&>;
		using _ResultType         = ::std::conditional_t<::ztd::is_specialization_of_v<_Result, decode_result>,
               decode_result<_ReconstructedInput, _Output, _State>,
               encode_result<_ReconstructedInput, _Output, _State>>;
		if (__it != __last) {
			// if there is already some items in the input progress (things irreversibly read), then
			// we are not obligated to do "at least" one skip; barrier it behind an empty check for
			// progress.
			if (::ztd::ranges::empty(__input_progress) && ::ztd::ranges::empty(__output_progress)) {
				++__it;
			}
			for (; __it != __last; ++__it) {
				ztd_char32_t __c32 = static_cast<ztd_char32_t>(*__it);
				if (__c32 < __ztd_idk_detail_last_unicode_code_point && !__ztd_idk_detail_is_surrogate(__c32)) {
					break;
				}
			}
		}
		return _ResultType(
			::ztd::ranges::reconstruct(::std::in_place_type<_UInput>, ::std::move(__it), ::std::move(__last)),
			::std::move(__result.output), __result.state, __result.error_code, __result.error_count);
	}

	//////
	/// @brief Attempts to skip over an input error in the text.
	///
	/// @param[in] __result The current result state of the encode or decode operation.
	/// @param[in] __input_progress A contiguous range containing all of the (potentially) irreversibly read input from
	/// an encoding operation.
	/// @param[in] __output_progress A contiguous range containing all of the (potentially) irreversibly written output
	/// from an encoding operation.
	///
	/// @remarks This function is specifically for UTF-32 input that also includes surrogate values as a valid option.
	/// Therefore, it will skip over every too-large codepoint.
	template <typename _Result, typename _InputProgress, typename _OutputProgress>
	constexpr auto skip_utf32_with_surrogates_input_error(_Result&& __result, const _InputProgress& __input_progress,
		const _OutputProgress& __output_progress) noexcept {
		// we can only advance one character at a time.
		auto __it     = ztd::ranges::begin(::std::forward<_Result>(__result).input);
		auto __last   = ztd::ranges::end(__result.input);
		using _Input  = decltype(__result.input);
		using _UInput = ::ztd::remove_cvref_t<_Input>;
		using _Output = decltype(__result.output);
		using _State  = ::std::remove_reference_t<::ztd::unwrap_t<::ztd::remove_cvref_t<decltype(__result.state)>>>;
		using _ReconstructedInput = ::ztd::ranges::reconstruct_t<_UInput, decltype(__it)&&, decltype(__last)&&>;
		using _ResultType         = ::std::conditional_t<::ztd::is_specialization_of_v<_Result, decode_result>,
               decode_result<_ReconstructedInput, _Output, _State>,
               encode_result<_ReconstructedInput, _Output, _State>>;
		if (__it != __last) {
			// if there is already some items in the input progress (things irreversibly read), then
			// we are not obligated to do "at least" one skip; barrier it behind an empty check for
			// progress.
			if (::ztd::ranges::empty(__input_progress) && ::ztd::ranges::empty(__output_progress)) {
				++__it;
			}
			for (; __it != __last; ++__it) {
				if (static_cast<ztd_char32_t>(*__it) < __ztd_idk_detail_last_unicode_code_point) {
					break;
				}
			}
		}
		return _ResultType(
			::ztd::ranges::reconstruct(::std::in_place_type<_UInput>, ::std::move(__it), ::std::move(__last)),
			::std::move(__result.output), __result.state, __result.error_code, __result.error_count);
	}

	//////
	/// @brief Attempts to skip over an input error in the text.
	///
	/// @param[in] __encoding The Encoding that experienced the error.
	/// @param[in] __result The current result state of the encode or decode operation.
	/// @param[in] __input_progress A contiguous range containing all of the (potentially) irreversibly read input from
	/// an encoding operation.
	/// @param[in] __output_progress A contiguous range containing all of the (potentially) irreversibly written output
	/// from an encoding operation.
	///
	/// @remarks If there exists a well-formed function call of the form `__encoding.skip_input_error(__result)`,
	/// it will call that function. Otherwise, it will attempt to grab the input iterator and pre-increment it
	/// exactly once. The goal for this is to provide functionality which can smartly skip over a collection of
	/// ill-formed code units or bytes in an input sequence, rather than generated e.g. 3 different replacement
	/// characters for a mal-formed UTF-8 sequence. For example, given this malformed wineglass code point as an
	/// input UTF-8 sequence:
	///
	/// `"\xC0\x9F\x8D\xB7meow"`
	///
	/// when used in conjunction with ztd::text::utf8 (and similar), a proper decode/transcode call will error on
	/// `'\\xC0'`. Then, this function skips until the `'m'` input code unit, resulting in a leftover sequence of
	///
	/// "meow".
	template <typename _Encoding, typename _Result, typename _InputProgress, typename _OutputProgress>
	constexpr auto skip_input_error(const _Encoding& __encoding, _Result&& __result,
		const _InputProgress& __input_progress,
		const _OutputProgress&
		     __output_progress) noexcept(::ztd::text::is_nothrow_skip_input_error_v<const _Encoding&, _Result,
		const _InputProgress&, const _OutputProgress&>) {
		if constexpr (::ztd::text::is_input_error_skippable_v<const _Encoding&, _Result, const _InputProgress&,
			              const _OutputProgress&>) {
			return __encoding.skip_input_error(
				::std::forward<_Result>(__result), __input_progress, __output_progress);
		}
		else {
			// we can only advance one input unit after a failure occurs...
			auto __it     = ztd::ranges::begin(::std::forward<_Result>(__result).input);
			auto __last   = ztd::ranges::end(__result.input);
			using _Input  = decltype(__result.input);
			using _UInput = ::ztd::remove_cvref_t<_Input>;
			using _Output = decltype(__result.output);
			using _State
				= ::std::remove_reference_t<::ztd::unwrap_t<::ztd::remove_cvref_t<decltype(__result.state)>>>;
			using _ReconstructedInput = ::ztd::ranges::reconstruct_t<_UInput, decltype(__it)&&, decltype(__last)&&>;
			using _ResultType         = ::std::conditional_t<::ztd::is_specialization_of_v<_Result, decode_result>,
                    decode_result<_ReconstructedInput, _Output, _State>,
                    encode_result<_ReconstructedInput, _Output, _State>>;
			if (__it != __last) {
				// if there is already some items in the input progress (things irreversibly read), then
				// we are not obligated to do "at least" one skip; barrier it behind an empty check before making
				// progress.
				if (::ztd::ranges::empty(__input_progress) && ::ztd::ranges::empty(__output_progress))
					++__it;
			}
			return _ResultType(
				::ztd::ranges::reconstruct(::std::in_place_type<_UInput>, ::std::move(__it), ::std::move(__last)),
				::std::move(__result.output), __result.state, __result.error_code, __result.error_count);
		}
	}

	//////
	/// @}

	ZTD_TEXT_INLINE_ABI_NAMESPACE_CLOSE_I_
}} // namespace ztd::text


#include <ztd/epilogue.hpp>

#endif
