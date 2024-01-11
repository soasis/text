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

#ifndef ZTD_TEXT_PROPAGATE_RECODE_ERROR_HPP
#define ZTD_TEXT_PROPAGATE_RECODE_ERROR_HPP

#include <ztd/text/version.hpp>

#include <ztd/text/encode_result.hpp>
#include <ztd/text/decode_result.hpp>
#include <ztd/text/recode_result.hpp>

#include <ztd/idk/type_traits.hpp>

#include <ztd/prologue.hpp>

namespace ztd { namespace text {
	ZTD_TEXT_INLINE_ABI_NAMESPACE_OPEN_I_

	//////
	/// @addtogroup ztd_text_propagate_errors ztd::text::propagate_(re|trans)code_errors
	///
	/// @{

	//////
	/// @brief Transcoding helper. Takes the given `__from_encoding`, `__to_encoding`, `__encode_error_handler`, and
	/// `__decode_error_handler` and launders the ztd::text::encode_result through the `__encode_error_handler`. The
	/// transformed encode result is then transformed to a ztd::text::decode_result before transforming that into the
	/// desired ztd::text::recode_result type.
	///
	/// @tparam _Result The exact recode result type to use.
	///
	/// @param[in] __output The output view to be writing into.
	/// @param[in] __from_encoding The desired encoding that performs the decode portion of the transcoding step.
	/// @param[in] __to_encoding The desired encoding that performs the decode portion of the transcoding step.
	/// @param[in] __encode_result The result value that has an error in it.
	/// @param[in] __encode_error_handler The error handler to mill the `__result` and
	/// other relevant information through.
	/// @param[in] __decode_error_handler The error handler to mill the `__encode_error_handler`'s  invoked result and
	/// other relevant information through.
	/// @param[in] __to_state The current state of the encoding step of the recode operation.
	/// @param[in] __from_input_progress Any unread input characters in any intermediate between the (failed) encode
	/// and decode operations.
	/// @param[in] __from_output_progress Any unread intermediate output characters in any intermediates between the
	/// (failed) encode and decode operations.
	/// @param[in] __to_input_progress Any unread intermediate input characters in any intermediate between the
	/// (failed) encode and decode operations.
	/// @param[in] __to_output_progress Any unread output characters in any intermediates between the (failed) encode
	/// and decode operations.
	///
	/// @remarks This function is a helper whose sole purpose is to ensure that the other half of error handling is
	/// called by recode-style functions written by the end user (e.g., writing overriding hooks for
	/// ztd::text::recode). This function attempts to take care of any unread/unwritten characters and other minor
	/// points in its pursuit of properly making sure the error manifests on the other side.
	template <typename _Result, typename _Output, typename _FromEncoding, typename _ToEncoding,
		typename _EncodeErrorHandler, typename _DecodeErrorHandler, typename _ToState, typename _FromInputProgress,
		typename _FromOutputProgress, typename _ToInputProgress, typename _ToOutputProgress, typename _ResultInput,
		typename _Intermediate, typename _FromState>
	constexpr auto propagate_recode_encode_error(_Output&& __output, _FromEncoding&& __from_encoding,
		_ToEncoding&& __to_encoding, encode_result<_ResultInput, _Intermediate, _FromState>&& __encode_result,
		_EncodeErrorHandler&& __encode_error_handler, _DecodeErrorHandler&& __decode_error_handler,
		_ToState& __to_state, _FromInputProgress&& __from_input_progress,
		_FromOutputProgress&& __from_output_progress, _ToInputProgress&& __to_input_progress,
		_ToOutputProgress&& __to_output_progress) noexcept {
		// first, run the encode error handler
		auto __encode_error_result = ::std::forward<_EncodeErrorHandler>(__encode_error_handler)(
			::std::forward<_FromEncoding>(__from_encoding), ::std::move(__encode_result),
			::std::forward<_FromInputProgress>(__from_input_progress),
			::std::forward<_FromOutputProgress>(__from_output_progress));
		// then, run the decode error handler
		decode_result<_Intermediate, ::ztd::remove_cvref_t<_Output>, _ToState> __decode_result(
			::std::move(__encode_error_result.output), ::std::forward<_Output>(__output), __to_state,
			__encode_error_result.error_code, __encode_error_result.error_count);
		auto __decode_error_result
			= ::std::forward<_DecodeErrorHandler>(__decode_error_handler)(::std::forward<_ToEncoding>(__to_encoding),
			     ::std::move(__decode_result), ::std::forward<_ToInputProgress>(__to_input_progress),
			     ::std::forward<_ToOutputProgress>(__to_output_progress));
		// compose into the final result type
		return _Result(::std::move(__encode_error_result.input), ::std::move(__decode_error_result.output),
			__encode_error_result.state, __decode_error_result.state, __decode_error_result.error_code,
			__decode_error_result.error_count, ::std::move(__encode_error_result.output),
			__encode_error_result.error_code, __encode_error_result.error_count);
	}

	//////
	/// @brief Transcoding helper. Takes the given `__from_encoding`, `__to_encoding`, `__encode_error_handler`, and
	/// `__decode_error_handler` and launders the ztd::text::encode_result through the `__encode_error_handler`. The
	/// transformed encode result is then transformed to a ztd::text::decode_result before transforming that into the
	/// desired ztd::text::recode_result type.
	///
	/// @tparam _Result The exact recode result type to use.
	///
	/// @param[in] __output The output view to be writing into.
	/// @param[in] __from_encoding The desired encoding that performs the decode portion of the transcoding step.
	/// @param[in] __to_encoding The desired encoding that performs the decode portion of the transcoding step.
	/// @param[in] __encode_result The result value that has an error in it.
	/// @param[in] __encode_error_handler The error handler to mill the `__result` and
	/// other relevant information through.
	/// @param[in] __decode_error_handler The error handler to mill the `__encode_error_handler`'s  invoked result and
	/// other relevant information through.
	/// @param[in] __to_state The current state of the encoding step of the recode operation.
	/// @param[in] __from_input_progress Any unread input characters in any intermediate between the (failed) encode
	/// and decode operations.
	/// @param[in] __from_output_progress Any unread intermediate output characters in any intermediates between the
	/// (failed) encode and decode operations.
	/// @param[in] __to_input_progress Any unread intermediate input characters in any intermediate between the
	/// (failed) encode and decode operations.
	/// @param[in] __to_output_progress Any unread output characters in any intermediates between the (failed) encode
	/// and decode operations.
	///
	/// @remarks This function is a helper whose sole purpose is to ensure that the other half of error handling is
	/// called by recode-style functions written by the end user (e.g., writing overriding hooks for
	/// ztd::text::recode). This function attempts to take care of any unread/unwritten characters and other minor
	/// points in its pursuit of properly making sure the error manifests on the other side.
	template <typename _Output, typename _FromEncoding, typename _ToEncoding, typename _EncodeErrorHandler,
		typename _DecodeErrorHandler, typename _ToState, typename _FromInputProgress, typename _FromOutputProgress,
		typename _ToInputProgress, typename _ToOutputProgress, typename _ResultInput, typename _ResultIntermediate,
		typename _FromState>
	constexpr auto propagate_recode_encode_error(_Output&& __output, _FromEncoding&& __from_encoding,
		_ToEncoding&& __to_encoding, encode_result<_ResultInput, _ResultIntermediate, _FromState>&& __encode_result,
		_EncodeErrorHandler&& __encode_error_handler, _DecodeErrorHandler&& __decode_error_handler,
		_ToState& __to_state, _FromInputProgress&& __from_input_progress,
		_FromOutputProgress&& __from_output_progress, _ToInputProgress&& __to_input_progress,
		_ToOutputProgress&& __to_output_progress) noexcept {
		using _Result
			= recode_result<_ResultInput, ::ztd::remove_cvref_t<_Output>, _FromState, _ToState, _ResultIntermediate>;
		return ::ztd::text::propagate_recode_encode_error<_Result>(::std::forward<_Output>(__output),
			::std::move(__encode_result), ::std::forward<_FromEncoding>(__from_encoding),
			::std::forward<_ToEncoding>(__to_encoding), ::std::forward<_EncodeErrorHandler>(__encode_error_handler),
			::std::forward<_DecodeErrorHandler>(__decode_error_handler), __to_state,
			::std::forward<_ToInputProgress>(__from_input_progress),
			::std::forward<_ToOutputProgress>(__from_output_progress),
			::std::forward<_ToInputProgress>(__to_input_progress),
			::std::forward<_ToOutputProgress>(__to_output_progress));
	}

	//////
	/// @brief Transcoding helper. Takes the given `__to_encoding` and `__decode_error_handler` and launders the failed
	/// ztd::text::encode_result through it, producing a ztd::text::decode_result and transforming that into the
	/// desired ztd::text::recode_result type.
	///
	/// @tparam _Result The exact recode result type to use.
	///
	/// @param[in] __to_encoding The desired encoding that performs the decode portion of the transcoding step.
	/// @param[in] __encode_result The encode result value that has an error in it.
	/// @param[in] __decode_result The decode result value that has not yet been processed by the decode error handler.
	/// @param[in] __decode_error_handler The error handler to mill the `__result` and other relevant information
	/// through.
	/// @param[in] __to_input_progress Any unread output characters in any intermediate between the (failed) encode and
	/// decode operations.
	/// @param[in] __to_output_progress Any unread output characters in any intermediates between the (failed) encode
	/// and decode operations.
	///
	/// @remarks This function is a helper whose sole purpose is to ensure that the other half of error handling is
	/// called by recode-style functions written by the end user (e.g., writing overriding hooks for
	/// ztd::text::recode). This function attempts to take care of any unread/unwritten characters and other minor
	/// points in its pursuit of properly making sure the error manifests on the other side.
	template <typename _Result, typename _ToEncoding, typename _DecodeErrorHandler, typename _ToInputProgress,
		typename _ToOutputProgress, typename _ResultInput, typename _ResultPivot, typename _FromState,
		typename _ResultIntermediate, typename _ResultOutput, typename _ToState>
	constexpr auto propagate_recode_decode_error(_ToEncoding&& __to_encoding,
		encode_result<_ResultInput, _ResultPivot, _FromState>&& __encode_result,
		decode_result<_ResultIntermediate, _ResultOutput, _ToState>&& __decode_result,
		_DecodeErrorHandler&& __decode_error_handler, _ToInputProgress&& __to_input_progress,
		_ToOutputProgress&& __to_output_progress) noexcept {
		// just run the decode error handler only
		auto __decode_error_result
			= ::std::forward<_DecodeErrorHandler>(__decode_error_handler)(::std::forward<_ToEncoding>(__to_encoding),
			     ::std::move(__decode_result), ::std::forward<_ToInputProgress>(__to_input_progress),
			     ::std::forward<_ToOutputProgress>(__to_output_progress));
		return _Result(::std::move(__encode_result.input), ::std::move(__decode_error_result.output),
			__encode_result.state, __decode_error_result.state, __decode_error_result.error_code,
			__decode_error_result.error_count, ::std::move(__encode_result.output), __encode_result.error_code,
			__encode_result.error_count);
	}

	//////
	/// @brief Takes the given `__to_encoding` and `__decode_error_handler` and launders the failed
	/// ztd::text::encode_result through it, producing a ztd::text::decode_result and transforming that into the
	/// desired ztd::text::recode_result type.
	///
	/// @param[in] __to_encoding The desired encoding that performs the decode portion of the transcoding step.
	/// @param[in] __encode_result The result value that has an error on it.
	/// @param[in] __decode_result The result value that has an error on it.
	/// @param[in] __decode_error_handler The error handler to mill the `__result` and other relevant information
	/// through.
	/// @param[in] __to_input_progress Any unread output characters in any intermediate between the (failed) encode and
	/// decode operations.
	/// @param[in] __to_output_progress Any unread output characters in any intermediates between the (failed) encode
	/// and decode operations.
	///
	/// @remarks This function is a helper whose sole purpose is to ensure that the other half of error handling is
	/// called by recode-style functions written by the end user (e.g., writing overriding hooks for
	/// ztd::text::recode). This function attempts to take care of any unread/unwritten characters and other minor
	/// points in its pursuit of properly making sure the error manifests on the other side. Unlike it's counterpart,
	/// this function does not take an `_Result` template parameter and instead deduces the returned recode result
	/// type from inputs.
	template <typename _ToEncoding, typename _DecodeErrorHandler, typename _ToInputProgress,
		typename _ToOutputProgress, typename _ResultInput, typename _ResultPivot, typename _FromState,
		typename _ResultIntermediate, typename _ResultOutput, typename _ToState>
	constexpr auto propagate_recode_decode_error(_ToEncoding&& __to_encoding,
		encode_result<_ResultInput, _ResultPivot, _FromState>&& __encode_result,
		decode_result<_ResultIntermediate, _ResultOutput, _ToState>&& __decode_result,
		_DecodeErrorHandler&& __decode_error_handler, _ToInputProgress&& __to_input_progress,
		_ToOutputProgress&& __to_output_progress) noexcept {
		using _Result = recode_result<_ResultInput, _ResultOutput, _FromState, _ToState, _ResultPivot>;
		return ::ztd::text::propagate_recode_decode_error<_Result>(::std::forward<_ToEncoding>(__to_encoding),
			::std::move(__encode_result), ::std::move(__decode_result),
			::std::forward<_DecodeErrorHandler>(__decode_error_handler),
			::std::forward<_ToInputProgress>(__to_input_progress),
			::std::forward<_ToOutputProgress>(__to_output_progress));
	}

	//////
	/// @brief Transcoding helper. Takes the given `__from_encoding`, `__to_encoding`, `__encode_error_handler`, and
	/// `__decode_error_handler` and launders the ztd::text::encode_result through the `__encode_error_handler`. The
	/// transformed encode result is then transformed to a ztd::text::decode_result before transforming that into the
	/// desired ztd::text::recode_result type.
	///
	/// @tparam _Result The exact recode result type to use.
	///
	/// @param[in] __input The input view to be reading from.
	/// @param[in] __output The output view to be writing into.
	/// @param[in] __from_encoding The desired encoding that performs the decode portion of the transcoding step.
	/// @param[in] __to_encoding The desired encoding that performs the decode portion of the transcoding step.
	/// @param[in] __result The result value that has an error in it.
	/// @param[in] __encode_error_handler The error handler to mill the `__result` and
	/// other relevant information through.
	/// @param[in] __decode_error_handler The error handler to mill the `__encode_error_handler`'s  invoked result and
	/// other relevant information through.
	/// @param[in] __to_state The current state of the encoding step of the recode operation.
	/// @param[in] __from_input_progress Any unread input characters in any intermediate between the (failed) encode
	/// and decode operations.
	/// @param[in] __from_output_progress Any unread intermediate output characters in any intermediates between the
	/// (failed) encode and decode operations.
	/// @param[in] __to_input_progress Any unread intermediate input characters in any intermediate between the
	/// (failed) encode and decode operations.
	/// @param[in] __to_output_progress Any unread output characters in any intermediates between the (failed) encode
	/// and decode operations.
	///
	/// @remarks This function is a helper whose sole purpose is to ensure that the other half of error handling is
	/// called by recode-style functions written by the end user (e.g., writing overriding hooks for
	/// ztd::text::recode). This function attempts to take care of any unread/unwritten characters and other minor
	/// points in its pursuit of properly making sure the error manifests on the other side.
	template <typename _Result, typename _Input, typename _Output, typename _FromEncoding, typename _ToEncoding,
		typename _EncodeErrorHandler, typename _DecodeErrorHandler, typename _ToState, typename _FromInputProgress,
		typename _FromOutputProgress, typename _ToInputProgress, typename _ToOutputProgress, typename _ResultInput,
		typename _Intermediate, typename _FromState>
	constexpr auto propagate_recode_encode_error_with(_Input&& __input, _Output&& __output,
		_FromEncoding&& __from_encoding, _ToEncoding&& __to_encoding,
		encode_result<_ResultInput, _Intermediate, _FromState>&& __encode_result,
		_EncodeErrorHandler&& __encode_error_handler, _DecodeErrorHandler&& __decode_error_handler,
		_ToState& __to_state, _FromInputProgress&& __from_input_progress,
		_FromOutputProgress&& __from_output_progress, _ToInputProgress&& __to_input_progress,
		_ToOutputProgress&& __to_output_progress) noexcept {
		// first, run the encode error handler
		auto __encode_error_result = ::std::forward<_EncodeErrorHandler>(__encode_error_handler)(
			::std::forward<_FromEncoding>(__from_encoding), ::std::move(__encode_result),
			::std::forward<_FromInputProgress>(__from_input_progress),
			::std::forward<_FromOutputProgress>(__from_output_progress));
		// then, run the decode error handler
		decode_result<_Intermediate, ::ztd::remove_cvref_t<_Output>, _ToState> __decode_result(
			::std::move(__encode_error_result.output), ::std::forward<_Output>(__output), __to_state,
			__encode_error_result.error_code, __encode_error_result.error_count);
		auto __decode_error_result
			= ::std::forward<_DecodeErrorHandler>(__decode_error_handler)(::std::forward<_ToEncoding>(__to_encoding),
			     ::std::move(__decode_result), ::std::forward<_ToInputProgress>(__to_input_progress),
			     ::std::forward<_ToOutputProgress>(__to_output_progress));
		// compose into the final result type
		return _Result(::std::forward<_Input>(__input), ::std::move(__decode_error_result.output),
			__encode_error_result.state, __decode_error_result.state, __decode_error_result.error_code,
			__decode_error_result.error_count, ::std::move(__encode_error_result.output),
			__encode_error_result.error_code, __encode_error_result.error_count);
	}

	//////
	/// @brief Transcoding helper. Takes the given `__from_encoding`, `__to_encoding`, `__encode_error_handler`, and
	/// `__decode_error_handler` and launders the ztd::text::encode_result through the `__encode_error_handler`. The
	/// transformed encode result is then transformed to a ztd::text::decode_result before transforming that into the
	/// desired ztd::text::recode_result type.
	///
	/// @tparam _Result The exact recode result type to use.
	///
	/// @param[in] __input The input view to be reading from.
	/// @param[in] __output The output view to be writing into.
	/// @param[in] __from_encoding The desired encoding that performs the decode portion of the transcoding step.
	/// @param[in] __to_encoding The desired encoding that performs the decode portion of the transcoding step.
	/// @param[in] __result The result value that has an error in it.
	/// @param[in] __encode_error_handler The error handler to mill the `__result` and
	/// other relevant information through.
	/// @param[in] __decode_error_handler The error handler to mill the `__encode_error_handler`'s  invoked result and
	/// other relevant information through.
	/// @param[in] __to_state The current state of the encoding step of the recode operation.
	/// @param[in] __from_input_progress Any unread input characters in any intermediate between the (failed) encode
	/// and decode operations.
	/// @param[in] __from_output_progress Any unread intermediate output characters in any intermediates between the
	/// (failed) encode and decode operations.
	/// @param[in] __to_input_progress Any unread intermediate input characters in any intermediate between the
	/// (failed) encode and decode operations.
	/// @param[in] __to_output_progress Any unread output characters in any intermediates between the (failed) encode
	/// and decode operations.
	///
	/// @remarks This function is a helper whose sole purpose is to ensure that the other half of error handling is
	/// called by recode-style functions written by the end user (e.g., writing overriding hooks for
	/// ztd::text::recode). This function attempts to take care of any unread/unwritten characters and other minor
	/// points in its pursuit of properly making sure the error manifests on the other side.
	template <typename _Input, typename _Output, typename _FromEncoding, typename _ToEncoding,
		typename _EncodeErrorHandler, typename _DecodeErrorHandler, typename _ToState, typename _FromInputProgress,
		typename _FromOutputProgress, typename _ToInputProgress, typename _ToOutputProgress, typename _ResultInput,
		typename _ResultIntermediate, typename _FromState>
	constexpr auto propagate_recode_encode_error_with(_Input&& __input, _Output&& __output,
		_FromEncoding&& __from_encoding, _ToEncoding&& __to_encoding,
		encode_result<_ResultInput, _ResultIntermediate, _FromState>&& __encode_result,
		_EncodeErrorHandler&& __encode_error_handler, _DecodeErrorHandler&& __decode_error_handler,
		_ToState& __to_state, _FromInputProgress&& __from_input_progress,
		_FromOutputProgress&& __from_output_progress, _ToInputProgress&& __to_input_progress,
		_ToOutputProgress&& __to_output_progress) noexcept {
		using _Result
			= recode_result<_ResultInput, ::ztd::remove_cvref_t<_Output>, _FromState, _ToState, _ResultIntermediate>;
		return ::ztd::text::propagate_recode_encode_error_with<_Result>(::std::forward<_Input>(__input),
			::std::forward<_Output>(__output), ::std::move(__encode_result),
			::std::forward<_FromEncoding>(__from_encoding), ::std::forward<_ToEncoding>(__to_encoding),
			::std::forward<_EncodeErrorHandler>(__encode_error_handler),
			::std::forward<_DecodeErrorHandler>(__decode_error_handler), __to_state,
			::std::forward<_ToInputProgress>(__from_input_progress),
			::std::forward<_ToOutputProgress>(__from_output_progress),
			::std::forward<_ToInputProgress>(__to_input_progress),
			::std::forward<_ToOutputProgress>(__to_output_progress));
	}

	//////
	/// @brief Transcoding helper. Takes the given `__to_encoding` and `__decode_error_handler` and launders the failed
	/// ztd::text::encode_result through it, producing a ztd::text::decode_result and transforming that into the
	/// desired ztd::text::recode_result type.
	///
	/// @tparam _Result The exact recode result type to use.
	///
	/// @param[in] __input The input view to be read from.
	/// @param[in] __to_encoding The desired encoding that performs the decode portion of the transcoding step.
	/// @param[in] __encode_result The result value that has an error on it.
	/// @param[in] __decode_result The result value that has an error on it.
	/// @param[in] __decode_error_handler The error handler to mill the `__result` and other relevant information
	/// through.
	/// @param[in] __to_state The current state of the encoding step of the recode operation.
	/// @param[in] __to_input_progress Any unread output characters in any intermediate between the (failed) encode and
	/// decode operations.
	/// @param[in] __to_output_progress Any unread output characters in any intermediates between the (failed) encode
	/// and decode operations.
	///
	/// @remarks This function is a helper whose sole purpose is to ensure that the other half of error handling is
	/// called by recode-style functions written by the end user (e.g., writing overriding hooks for
	/// ztd::text::recode). This function attempts to take care of any unread/unwritten characters and other minor
	/// points in its pursuit of properly making sure the error manifests on the other side.
	template <typename _Result, typename _Input, typename _ToEncoding, typename _DecodeErrorHandler,
		typename _ToInputProgress, typename _ToOutputProgress, typename _ResultInput, typename _ResultPivot,
		typename _FromState, typename _ResultIntermediate, typename _ResultOutput, typename _ToState>
	constexpr auto propagate_recode_decode_error_with(_Input&& __input, _ToEncoding&& __to_encoding,
		encode_result<_ResultInput, _ResultPivot, _FromState>&& __encode_result,
		decode_result<_ResultIntermediate, _ResultOutput, _ToState>&& __decode_result,
		_DecodeErrorHandler&& __decode_error_handler, _ToInputProgress&& __to_input_progress,
		_ToOutputProgress&& __to_output_progress) noexcept {
		// just run the decode error handler only
		auto __decode_error_result
			= ::std::forward<_DecodeErrorHandler>(__decode_error_handler)(::std::forward<_ToEncoding>(__to_encoding),
			     ::std::move(__decode_result), ::std::forward<_ToInputProgress>(__to_input_progress),
			     ::std::forward<_ToOutputProgress>(__to_output_progress));
		return _Result(::std::forward<_Input>(__input), ::std::move(__decode_error_result.output),
			__encode_result.state, __decode_error_result.state, __decode_error_result.error_code,
			__decode_error_result.error_count, ::std::move(__encode_result.output), __encode_result.error_code,
			__encode_result.error_count);
	}

	//////
	/// @brief Takes the given `__to_encoding` and `__decode_error_handler` and launders the failed
	/// ztd::text::encode_result through it, producing a ztd::text::decode_result and transforming that into the
	/// desired ztd::text::recode_result type.
	///
	/// @param[in] __input The input view to be read from.
	/// @param[in] __to_encoding The desired encoding that performs the decode portion of the transcoding step.
	/// @param[in] __encode_result The result value that has an error on it.
	/// @param[in] __decode_result The result value that has an error on it.
	/// @param[in] __decode_error_handler The error handler to mill the `__result` and other relevant information
	/// through.
	/// @param[in] __to_state The current state of the encoding step of the recode operation.
	/// @param[in] __to_input_progress Any unread output characters in any intermediate between the (failed) encode and
	/// decode operations.
	/// @param[in] __to_output_progress Any unread output characters in any intermediates between the (failed) encode
	/// and decode operations.
	///
	/// @remarks This function is a helper whose sole purpose is to ensure that the other half of error handling is
	/// called by recode-style functions written by the end user (e.g., writing overriding hooks for
	/// ztd::text::recode). This function attempts to take care of any unread/unwritten characters and other minor
	/// points in its pursuit of properly making sure the error manifests on the other side. Unlike it's counterpart,
	/// this function does not take an `_Result` template parameter and instead deduces the returned recode result
	/// type from inputs.
	template <typename _Input, typename _ToEncoding, typename _DecodeErrorHandler, typename _ToInputProgress,
		typename _ToOutputProgress, typename _ResultInput, typename _ResultPivot, typename _FromState,
		typename _ResultIntermediate, typename _ResultOutput, typename _ToState>
	constexpr auto propagate_recode_decode_error_with(_Input&& __input, _ToEncoding&& __to_encoding,
		encode_result<_ResultInput, _ResultPivot, _FromState>&& __encode_result,
		decode_result<_ResultIntermediate, _ResultOutput, _ToState>&& __decode_result,
		_DecodeErrorHandler&& __decode_error_handler, _ToInputProgress&& __to_input_progress,
		_ToOutputProgress&& __to_output_progress) noexcept {
		using _Result = recode_result<_ResultInput, _ResultOutput, _FromState, _ToState, _ResultPivot>;
		return ::ztd::text::propagate_recode_decode_error_with<_Result>(::std::forward<_Input>(__input),
			::std::forward<_ToEncoding>(__to_encoding), ::std::move(__encode_result), ::std::move(__decode_result),
			::std::forward<_DecodeErrorHandler>(__decode_error_handler),
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
