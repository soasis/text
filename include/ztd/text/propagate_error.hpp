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

#ifndef ZTD_TEXT_PROPAGATE_ERROR_HPP
#define ZTD_TEXT_PROPAGATE_ERROR_HPP

#include <ztd/text/version.hpp>

#include <ztd/text/decode_result.hpp>
#include <ztd/text/encode_result.hpp>
#include <ztd/text/transcode_result.hpp>
#include <ztd/text/recode_result.hpp>

#include <ztd/idk/type_traits.hpp>

#include <ztd/prologue.hpp>

namespace ztd { namespace text {
	ZTD_TEXT_INLINE_ABI_NAMESPACE_OPEN_I_

	//////
	/// @addtogroup ztd_text_propagate_errors ztd::text::propagate_(cis|trans)code_errors
	///
	/// @{

	//////
	/// @brief Transcoding helper. Takes the given `__to_encoding` and `__encode_error_handler` and launders the failed
	/// ztd::text::decode_result through it, producing a ztd::text::encode_result and transforming that into the
	/// desired ztd::text::transcode_result type.
	///
	/// @tparam _Result The exact transcode result type to use.
	///
	/// @param[in] __output The output view to be writing into.
	/// @param[in] __to_encoding The desired encoding that performs the encode portion of the transcoding step.
	/// @param[in] __result The result value that has an error on it.
	/// @param[in] __encode_error_handler The error handler to mill the `__result` and other relevant information
	/// through.
	/// @param[in] __to_state The current state of the encoding step of the transcode operation.
	/// @param[in] __to_input_progress Any unread output characters in any intermediate between the (failed) decode and
	/// encode operations.
	/// @param[in] __to_output_progress Any unread output characters in any intermediates between the (failed) decode
	/// and encode operations.
	///
	/// @remarks This function is a helper whose sole purpose is to ensure that the other half of error handling is
	/// called by transcode-style functions written by the end user (e.g., writing overriding hooks for
	/// ztd::text::transcode). This function attempts to take care of any unread/unwritten characters and other minor
	/// points in its pursuit of properly making sure the error manifests on the other side.
	template <typename _Result, typename _Output, typename _ToEncoding, typename _EncodeErrorHandler,
		typename _ToState, typename _ToInputProgress, typename _ToOutputProgress, typename _Input,
		typename _Intermediate, typename _FromState>
	constexpr auto propagate_transcode_error(_Output&& __output, _ToEncoding&& __to_encoding,
		decode_result<_Input, _Intermediate, _FromState>&& __result, _EncodeErrorHandler&& __encode_error_handler,
		_ToState& __to_state, _ToInputProgress&& __to_input_progress,
		_ToOutputProgress&& __to_output_progress) noexcept {
		// just run the encode error handler only
		encode_result<_Intermediate, ::std::remove_reference_t<_Output>, _ToState> __encode_result(
			::std::move(__result.output), ::std::forward<_Output>(__output), __to_state, __result.error_code,
			__result.error_count);
		auto __encode_error_result
			= ::std::forward<_EncodeErrorHandler>(__encode_error_handler)(::std::forward<_ToEncoding>(__to_encoding),
			     ::std::move(__encode_result), ::std::forward<_ToInputProgress>(__to_input_progress),
			     ::std::forward<_ToOutputProgress>(__to_output_progress));
		return _Result(::std::move(__result.input), ::std::move(__encode_error_result.output), __result.state,
			__encode_error_result.state, __encode_error_result.error_code, __encode_error_result.error_count);
	}

	//////
	/// @brief Takes the given `__to_encoding` and `__encode_error_handler` and launders the failed
	/// ztd::text::decode_result through it, producing a ztd::text::encode_result and transforming that into the
	/// desired ztd::text::transcode_result type.
	///
	/// @param[in] __output The output view to be writing into.
	/// @param[in] __to_encoding The desired encoding that performs the encode portion of the transcoding step.
	/// @param[in] __result The result value that has an error on it.
	/// @param[in] __encode_error_handler The error handler to mill the `__result` and other relevant information
	/// through.
	/// @param[in] __to_state The current state of the encoding step of the transcode operation.
	/// @param[in] __to_input_progress Any unread output characters in any intermediate between the (failed) decode and
	/// encode operations.
	/// @param[in] __to_output_progress Any unread output characters in any intermediates between the (failed) decode
	/// and encode operations.
	///
	/// @remarks This function is a helper whose sole purpose is to ensure that the other half of error handling is
	/// called by transcode-style functions written by the end user (e.g., writing overriding hooks for
	/// ztd::text::transcode). This function attempts to take care of any unread/unwritten characters and other minor
	/// points in its pursuit of properly making sure the error manifests on the other side. Unlike it's counterpart,
	/// this function does not take an `_Result` template parameter and instead deduces the returned transcode result
	/// type from inputs.
	template <typename _Output, typename _ToEncoding, typename _EncodeErrorHandler, typename _ToState,
		typename _ToInputProgress, typename _ToOutputProgress, typename _Input, typename _Intermediate,
		typename _FromState>
	constexpr auto propagate_transcode_error(_Output&& __output, _ToEncoding&& __to_encoding,
		decode_result<_Input, _Intermediate, _FromState>&& __result, _EncodeErrorHandler&& __encode_error_handler,
		_ToState& __to_state, _ToInputProgress&& __to_input_progress,
		_ToOutputProgress&& __to_output_progress) noexcept {
		using _Result = transcode_result<_Input, ::ztd::ranges::subrange_for_t<::std::remove_reference_t<_Output>>,
			_FromState, _ToState>;
		return propagate_transcode_error<_Result>(::std::forward<_Output>(__output),
			::std::forward<_Output>(__to_encoding), ::std::move(__result),
			::std::forward<_EncodeErrorHandler>(__encode_error_handler), __to_state,
			::std::forward<_ToInputProgress>(__to_input_progress),
			::std::forward<_ToOutputProgress>(__to_output_progress));
	}

	//////
	/// @brief Transcoding helper. Takes the given `__to_encoding` and `__decode_error_handler` and launders the failed
	/// ztd::text::decode_result through it, producing a ztd::text::encode_result and cisforming that into the
	/// desired ztd::text::recode_result type.
	///
	/// @tparam _Result The exact recode result type to use.
	///
	/// @param[in] __output The output view to be writing into.
	/// @param[in] __to_encoding The desired encoding that performs the encode portion of the ciscoding step.
	/// @param[in] __result The result value that has an error on it.
	/// @param[in] __decode_error_handler The error handler to mill the `__result` and other relevant information
	/// through.
	/// @param[in] __to_state The current state of the encoding step of the recode operation.
	/// @param[in] __to_input_progress Any unread output characters in any intermediate between the (failed) decode and
	/// encode operations.
	/// @param[in] __to_output_progress Any unread output characters in any intermediates between the (failed) decode
	/// and encode operations.
	///
	/// @remarks This function is a helper whose sole purpose is to ensure that the other half of error handling is
	/// called by recode-style functions written by the end user (e.g., writing overriding hooks for
	/// ztd::text::recode). This function attempts to take care of any unread/unwritten characters and other minor
	/// points in its pursuit of properly making sure the error manifests on the other side.
	template <typename _Result, typename _Output, typename _ToEncoding, typename _DecodeErrorHandler,
		typename _ToState, typename _ToInputProgress, typename _ToOutputProgress, typename _Input,
		typename _Intermediate, typename _FromState>
	constexpr auto propagate_recode_error(_Output&& __output, _ToEncoding&& __to_encoding,
		encode_result<_Input, _Intermediate, _FromState>&& __result, _DecodeErrorHandler&& __decode_error_handler,
		_ToState& __to_state, _ToInputProgress&& __to_input_progress,
		_ToOutputProgress&& __to_output_progress) noexcept {
		// just run the encode error handler only
		decode_result<_Intermediate, ::std::remove_reference_t<_Output>, _ToState> __decode_result(
			::std::move(__result.output), ::std::forward<_Output>(__output), __to_state, __result.error_code,
			__result.error_count);
		auto __decode_error_result
			= ::std::forward<_DecodeErrorHandler>(__decode_error_handler)(::std::forward<_ToEncoding>(__to_encoding),
			     ::std::move(__decode_result), ::std::forward<_ToInputProgress>(__to_input_progress),
			     ::std::forward<_ToOutputProgress>(__to_output_progress));
		return _Result(::std::move(__result.input), ::std::move(__decode_error_result.output), __result.state,
			__decode_error_result.state, __decode_error_result.error_code, __decode_error_result.error_count);
	}

	//////
	/// @brief Takes the given `__to_encoding` and `__decode_error_handler` and launders the failed
	/// ztd::text::decode_result through it, producing a ztd::text::encode_result and transforming that into the
	/// desired ztd::text::recode_result type.
	///
	/// @param[in] __output The output view to be writing into.
	/// @param[in] __to_encoding The desired encoding that performs the encode portion of the ciscoding step.
	/// @param[in] __result The result value that has an error on it.
	/// @param[in] __decode_error_handler The error handler to mill the `__result` and other relevant information
	/// through.
	/// @param[in] __to_state The current state of the encoding step of the recode operation.
	/// @param[in] __to_input_progress Any unread output characters in any intermediate between the (failed) decode and
	/// encode operations.
	/// @param[in] __to_output_progress Any unread output characters in any intermediates between the (failed) decode
	/// and encode operations.
	///
	/// @remarks This function is a helper whose sole purpose is to ensure that the other half of error handling is
	/// called by recode-style functions written by the end user (e.g., writing overriding hooks for
	/// ztd::text::recode). This function attempts to take care of any unread/unwritten characters and other minor
	/// points in its pursuit of properly making sure the error manifests on the other side. Unlike it's counterpart,
	/// this function does not take an `_Result` template parameter and instead deduces the returned recode result
	/// type from inputs.
	template <typename _Output, typename _ToEncoding, typename _DecodeErrorHandler, typename _ToState,
		typename _ToInputProgress, typename _ToOutputProgress, typename _Input, typename _Intermediate,
		typename _FromState>
	constexpr auto propagate_recode_error(_Output&& __output, _ToEncoding&& __to_encoding,
		encode_result<_Input, _Intermediate, _FromState>&& __result, _DecodeErrorHandler&& __decode_error_handler,
		_ToState& __to_state, _ToInputProgress&& __to_input_progress,
		_ToOutputProgress&& __to_output_progress) noexcept {
		using _Result = recode_result<_Input, ::ztd::ranges::subrange_for_t<::std::remove_reference_t<_Output>>,
			_FromState, _ToState>;
		return propagate_recode_error<_Result>(::std::forward<_Output>(__output),
			::std::forward<_Output>(__to_encoding), ::std::move(__result),
			::std::forward<_DecodeErrorHandler>(__decode_error_handler), __to_state,
			::std::forward<_ToInputProgress>(__to_input_progress),
			::std::forward<_ToOutputProgress>(__to_output_progress));
	}

	//////
	///
	/// @}


	ZTD_TEXT_INLINE_ABI_NAMESPACE_CLOSE_I_
}} // namespace ztd::text

#include <ztd/epilogue.hpp>

#endif
