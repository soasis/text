// =============================================================================
//
// ztd.text
// Copyright © 2021 JeanHeyd "ThePhD" Meneide and Shepherd's Oasis, LLC
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
// ============================================================================>

#pragma once

#ifndef ZTD_TEXT_TRANSCODE_HPP
#define ZTD_TEXT_TRANSCODE_HPP

#include <ztd/text/code_point.hpp>
#include <ztd/text/code_unit.hpp>
#include <ztd/text/error_handler.hpp>
#include <ztd/text/detail/is_lossless.hpp>
#include <ztd/text/state.hpp>
#include <ztd/text/default_encoding.hpp>
#include <ztd/text/transcode_result.hpp>
#include <ztd/text/is_unicode_code_point.hpp>

#include <ztd/text/detail/transcode_one.hpp>
#include <ztd/text/detail/encoding_range.hpp>
#include <ztd/text/unbounded.hpp>
#include <ztd/text/detail/type_traits.hpp>
#include <ztd/text/detail/span.hpp>

#include <string>
#include <vector>
#include <string_view>

namespace ztd { namespace text {
	ZTD_TEXT_INLINE_ABI_NAMESPACE_OPEN_I_

	//////
	/// @addtogroup ztd_text_transcode ztd::text::transcode[_into]
	///
	/// @brief These functions convert from a view of input code units into a view of output code units (typically,
	/// through an intermediary code point type that is similar between the two) using either the inferred or specified
	/// encodings. If no error handler is provided, the equivalent of the ztd::text::default_handler is used by
	/// default, but it is marked as careless. If no associated state is provided for either the "to" or "from"
	/// encodings, one will be created with automatic storage duration (as a "stack" variable) for the provided
	/// encoding.
	/// @{
	//////

	//////
	/// @brief Converts the code units of the given input view through the from encoding to code units of the to
	/// encoding into the output view.
	///
	/// @param[in]     __input An input_view to read code units from and use in the decode operation that will
	/// produce intermediate code points.
	/// @param[in]     __from_encoding The encoding that will be used to decode the input's code units into
	/// intermediate code points.
	/// @param[in]     __output An output_view to write code units to as the result of the encode operation from the
	/// intermediate code points.
	/// @param[in]     __to_encoding The encoding that will be used to encode the intermediate code points into the
	/// final code units.
	/// @param[in]     __from_error_handler The error handlers for the from and to encodings,
	/// respectively.
	/// @param[in]     __to_error_handler The error handlers for the from and to encodings,
	/// respectively.
	/// @param[in,out] __from_state A reference to the associated state for the @p __from_encoding 's decode step.
	/// @param[in,out] __to_state A reference to the associated state for the @p __to_encoding 's encode step.
	///
	/// @result A ztd::text::transcode_result object that contains references to @p __from_state and @p __to_state.
	///
	/// @remark This function detects whether or not the ADL extension point @c text_transcode can be called with the
	/// provided parameters. If so, it will use that ADL extension point over the default implementation. Otherwise, it
	/// will loop over the two encodings and attempt to transcode by first decoding the input code units to code
	/// points, then encoding the intermediate code points to the desired, output code units.
	//////
	template <typename _Input, typename _Output, typename _FromEncoding, typename _ToEncoding,
		typename _FromErrorHandler, typename _ToErrorHandler, typename _FromState, typename _ToState>
	constexpr auto basic_transcode_into(_Input&& __input, _FromEncoding&& __from_encoding, _Output&& __output,
		_ToEncoding&& __to_encoding, _FromErrorHandler&& __from_error_handler, _ToErrorHandler&& __to_error_handler,
		_FromState& __from_state, _ToState& __to_state) {
		using _UInput                = __detail::__remove_cvref_t<_Input>;
		using _UOutput               = __detail::__remove_cvref_t<_Output>;
		using _InputValueType        = __detail::__range_value_type_t<_UInput>;
		using _WorkingInput          = __detail::__reconstruct_t<::std::conditional_t<::std::is_array_v<_UInput>,
               ::std::conditional_t<__detail::__is_character_v<_InputValueType>,
                    ::std::basic_string_view<_InputValueType>, ::ztd::text::span<const _InputValueType>>,
               _UInput>>;
		using _WorkingOutput         = __detail::__reconstruct_t<_UOutput>;
		using _UFromEncoding         = __detail::__remove_cvref_t<_FromEncoding>;
		using _IntermediateCodePoint = code_point_t<_UFromEncoding>;
		using _Result
			= __detail::__reconstruct_transcode_result_t<_WorkingInput, _WorkingOutput, _FromState, _ToState>;

		_WorkingInput __working_input(
			__detail::__reconstruct(::std::in_place_type<_WorkingInput>, ::std::forward<_Input>(__input)));
		_WorkingOutput __working_output(
			__detail::__reconstruct(::std::in_place_type<_WorkingOutput>, ::std::forward<_Output>(__output)));

		if constexpr (__detail::__is_detected_v<__detail::__detect_adl_text_transcode_one, _Input, _FromEncoding,
			              _Output, _ToEncoding, _FromErrorHandler, _ToErrorHandler, _FromState, _ToState>) {
			bool __handled_error = false;
			for (;;) {
				auto __transcode_result = text_transcode_one(::std::forward<_Input>(__input), __from_encoding,
					::std::forward<_Output>(__output), __to_encoding, __from_error_handler, __to_error_handler,
					__from_state, __to_state);
				if (__transcode_result.error_code != encoding_error::ok) {
					return _Result(::std::move(__working_input), ::std::move(__working_output), __from_state,
						__to_state, __transcode_result.error_code, __transcode_result.handled_error);
				}
				__handled_error |= __transcode_result.handled_error;
				__working_input  = ::std::move(__transcode_result.input);
				__working_output = ::std::move(__transcode_result.output);
				if (__detail::__adl::__adl_empty(__working_input)) {
					break;
				}
			}
			return _Result(::std::move(__working_input), ::std::move(__working_output), __from_state, __to_state,
				encoding_error::ok, __handled_error);
		}
		else {
			_IntermediateCodePoint __intermediate[max_code_points_v<_UFromEncoding>];
			bool __handled_error = false;
			for (;;) {
				auto __transcode_result = __detail::__basic_transcode_one<__detail::__consume::__no>(
					::std::move(__working_input), __from_encoding, __intermediate, ::std::move(__working_output),
					__to_encoding, __from_error_handler, __to_error_handler, __from_state, __to_state);
				if (__transcode_result.error_code != encoding_error::ok) {
					return _Result(::std::move(__working_input), ::std::move(__working_output), __from_state,
						__to_state, __transcode_result.error_code, __transcode_result.handled_error);
				}
				__handled_error |= __transcode_result.handled_error;
				__working_input  = ::std::move(__transcode_result.input);
				__working_output = ::std::move(__transcode_result.output);
				if (__detail::__adl::__adl_empty(__working_input)) {
					break;
				}
			}
			return _Result(::std::move(__working_input), ::std::move(__working_output), __from_state, __to_state,
				encoding_error::ok, __handled_error);
		}
	}

	//////
	/// @brief Converts the code units of the given input view through the from encoding to code units of the to
	/// encoding into the output view.
	///
	/// @param[in]     __input An input_view to read code units from and use in the decode operation that will
	/// produce intermediate code points.
	/// @param[in]     __from_encoding The encoding that will be used to decode the input's code units into
	/// intermediate code points.
	/// @param[in]     __output An output_view to write code units to as the result of the encode operation from the
	/// intermediate code points.
	/// @param[in]     __to_encoding The encoding that will be used to encode the intermediate code points into the
	/// final code units.
	/// @param[in]     __from_error_handler The error handlers for the from and to encodings,
	/// respectively.
	/// @param[in]     __to_error_handler The error handlers for the from and to encodings,
	/// respectively.
	/// @param[in,out] __from_state A reference to the associated state for the @p __from_encoding 's decode step.
	/// @param[in,out] __to_state A reference to the associated state for the @p __to_encoding 's encode step.
	///
	/// @result A ztd::text::transcode_result object that contains references to @p __from_state and @p __to_state.
	///
	/// @remark This function detects whether or not the ADL extension point @c text_transcode can be called with the
	/// provided parameters. If so, it will use that ADL extension point over the default implementation. Otherwise, it
	/// will loop over the two encodings and attempt to transcode by first decoding the input code units to code
	/// points, then encoding the intermediate code points to the desired, output code units.
	//////
	template <typename _Input, typename _Output, typename _FromEncoding, typename _ToEncoding,
		typename _FromErrorHandler, typename _ToErrorHandler, typename _FromState, typename _ToState>
	constexpr auto transcode_into(_Input&& __input, _FromEncoding&& __from_encoding, _Output&& __output,
		_ToEncoding&& __to_encoding, _FromErrorHandler&& __from_error_handler, _ToErrorHandler&& __to_error_handler,
		_FromState& __from_state, _ToState& __to_state) {
		if constexpr (__detail::__is_detected_v<__detail::__detect_adl_text_transcode, _Input, _Output, _FromEncoding,
			              _ToEncoding, _FromErrorHandler, _ToErrorHandler, _FromState, _ToState>) {
			return text_transcode(::std::forward<_Input>(__input), ::std::forward<_FromEncoding>(__from_encoding),
				::std::forward<_Output>(__output), ::std::forward<_ToEncoding>(__to_encoding),
				::std::forward<_FromErrorHandler>(__from_error_handler),
				::std::forward<_ToErrorHandler>(__to_error_handler), __from_state, __to_state);
		}
		else if constexpr (__detail::__is_detected_v<__detail::__detect_adl_internal_text_transcode, _Input,
			                   _FromEncoding, _Output, _ToEncoding, _FromErrorHandler, _ToErrorHandler, _FromState,
			                   _ToState>) {
			return __text_transcode(::std::forward<_Input>(__input), ::std::forward<_FromEncoding>(__from_encoding),
				::std::forward<_Output>(__output), ::std::forward<_ToEncoding>(__to_encoding),
				::std::forward<_FromErrorHandler>(__from_error_handler),
				::std::forward<_ToErrorHandler>(__to_error_handler), __from_state, __to_state);
		}
		else {
			return basic_transcode_into(::std::forward<_Input>(__input),
				::std::forward<_FromEncoding>(__from_encoding), ::std::forward<_Output>(__output),
				::std::forward<_ToEncoding>(__to_encoding), ::std::forward<_FromErrorHandler>(__from_error_handler),
				::std::forward<_ToErrorHandler>(__to_error_handler), __from_state, __to_state);
		}
	}

	//////
	/// @brief Converts the code units of the given input view through the from encoding to code units of the to
	/// encoding into the output view.
	///
	/// @param[in]     __input An input_view to read code units from and use in the decode operation that will
	/// produce intermediate code points.
	/// @param[in]     __from_encoding The encoding that will be used to decode the input's code units into
	/// intermediate code points.
	/// @param[in]     __output An output_view to write code units to as the result of the encode operation from the
	/// intermediate code points.
	/// @param[in]     __to_encoding The encoding that will be used to encode the intermediate code points into the
	/// final code units.
	/// @param[in]     __from_error_handler The error handler for the @p __from_encoding 's decode step.
	/// @param[in]     __to_error_handler The error handler for the @p __to_encoding 's encode step.
	/// @param[in,out] __from_state A reference to the associated state for the @p __from_encoding 's decode step.
	///
	/// @result A ztd::text::stateless_transcode_result object.
	///
	/// @remarks This function calls the base reference, the ztd::text::transcode_into after creating a @c to_state
	/// from ztd::text::make_encode_state. The result from this function returns a
	/// ztd::text::stateless_transcode_result as opposed to a ztd::text::transcode_result because the state information
	/// is on the stack, and returning the state in those types by reference will result in references to memory that
	/// has already been cleaned up. If you need access to the state parameters, call the lower-level functionality
	/// with your own created states.
	//////
	template <typename _Input, typename _FromEncoding, typename _Output, typename _ToEncoding,
		typename _FromErrorHandler, typename _ToErrorHandler, typename _FromState>
	constexpr auto transcode_into(_Input&& __input, _FromEncoding&& __from_encoding, _Output&& __output,
		_ToEncoding&& __to_encoding, _FromErrorHandler&& __from_error_handler, _ToErrorHandler&& __to_error_handler,
		_FromState& __from_state) {
		using _UToEncoding = __detail::__remove_cvref_t<_ToEncoding>;
		using _ToState     = encode_state_t<_UToEncoding>;

		_ToState __to_state = make_encode_state(__to_encoding);

		auto __stateful_result
			= transcode_into(::std::forward<_Input>(__input), ::std::forward<_FromEncoding>(__from_encoding),
			     ::std::forward<_ToEncoding>(__to_encoding), ::std::forward<_FromErrorHandler>(__from_error_handler),
			     ::std::forward<_ToErrorHandler>(__to_error_handler), __from_state, __to_state);

		return __detail::__slice_to_stateless(::std::move(__stateful_result));
	}

	//////
	/// @brief Converts the code units of the given input view through the
	///        from encoding to code units of the to encoding into the output
	///        view.
	/// @param[in]     __input An input_view to read code units from and use in the decode operation that will
	/// produce intermediate code points.
	/// @param[in]     __from_encoding The encoding that will be used to decode the input's code units into
	/// intermediate code points.
	/// @param[in]     __output An output_view to write code units to as the result of the encode operation from the
	/// intermediate code points.
	/// @param[in]     __to_encoding The encoding that will be used to encode the intermediate code points into the
	/// final code units.
	/// @param[in]     __from_error_handler The error handler for the @p __from_encoding 's decode step.
	/// @param[in]     __to_error_handler The error handler for the @p __to_encoding 's encode step.
	///
	/// @remarks This function creates a decode state @c from_state by calling ztd::text::make_decode_state. The
	/// result from this function returns a ztd::text::stateless_transcode_result as opposed to a
	/// ztd::text::transcode_result because the state information is on the stack, and returning the state in those
	/// types by reference will result in references to memory that has already been cleaned up. If you need access to
	/// the state parameters, call the lower-level functionality with your own created states.
	//////
	template <typename _Input, typename _FromEncoding, typename _Output, typename _ToEncoding,
		typename _FromErrorHandler, typename _ToErrorHandler>
	constexpr auto transcode_into(_Input&& __input, _FromEncoding&& __from_encoding, _Output&& __output,
		_ToEncoding&& __to_encoding, _FromErrorHandler&& __from_error_handler, _ToErrorHandler&& __to_error_handler) {
		using _UFromEncoding = __detail::__remove_cvref_t<_FromEncoding>;
		using _FromState     = decode_state_t<_UFromEncoding>;

		_FromState __from_state = make_decode_state(__from_encoding);

		return transcode_into(::std::forward<_Input>(__input), ::std::forward<_FromEncoding>(__from_encoding),
			::std::forward<_ToEncoding>(__to_encoding), ::std::forward<_FromErrorHandler>(__from_error_handler),
			::std::forward<_ToErrorHandler>(__to_error_handler), __from_state);
	}

	//////
	/// @brief Converts the code units of the given input view through the
	///        from encoding to code units of the to encoding into the output
	///        view.
	/// @param[in]     __input An input_view to read code units from and use in the decode operation that will
	/// produce intermediate code points.
	/// @param[in]     __from_encoding The encoding that will be used to decode the input's code units into
	/// intermediate code points.
	/// @param[in]     __output An output_view to write code units to as the result of the encode operation from the
	/// intermediate code points.
	/// @param[in]     __to_encoding The encoding that will be used to encode the intermediate code points into the
	/// final code units.
	/// @param[in]     __from_error_handler The error handler for the @p __from_encoding 's decode step.
	///
	/// @remarks This function creates an @c to_error_handler from a class like ztd::text::default_handler, but that is
	/// marked as careless since you did not explicitly provide it. This matters for lossy conversions that are not
	/// injective. The result from this function returns a ztd::text::stateless_transcode_result as opposed to a
	/// ztd::text::transcode_result because the state information is on the stack, and returning the state in those
	/// types by reference will result in references to memory that has already been cleaned up. If you need access to
	/// the state parameters, call the lower-level functionality with your own created states.
	//////
	template <typename _Input, typename _FromEncoding, typename _Output, typename _ToEncoding,
		typename _FromErrorHandler>
	constexpr auto transcode_into(_Input&& __input, _FromEncoding&& __from_encoding, _Output&& __output,
		_ToEncoding&& __to_encoding, _FromErrorHandler&& __from_error_handler) {
		auto __handler = __detail::__duplicate_or_be_careless(__from_error_handler);

		return transcode_into(::std::forward<_Input>(__input), ::std::forward<_FromEncoding>(__from_encoding),
			::std::forward<_Output>(__output), ::std::forward<_ToEncoding>(__to_encoding),
			::std::forward<_FromErrorHandler>(__from_error_handler), __handler);
	}

	//////
	/// @brief Converts the code units of the given input view through the
	///        from encoding to code units of the to encoding into the output
	///        view.
	/// @param[in]     __input An input_view to read code units from and use in the decode operation that will
	/// produce intermediate code points.
	/// @param[in]     __from_encoding The encoding that will be used to decode the input's code units into
	/// intermediate code points.
	/// @param[in]     __output An output_view to write code units to as the result of the encode operation from the
	/// intermediate code points.
	/// @param[in]     __to_encoding The encoding that will be used to encode the intermediate code points into the
	/// final code units.
	///
	/// @remarks This function creates an @c from_error_handler from a class like ztd::text::default_handler, but that
	/// is marked as careless since you did not explicitly provide it. This matters for lossy conversions that are
	/// not injective. The result from this function returns a ztd::text::stateless_transcode_result as opposed to a
	/// ztd::text::transcode_result because the state information is on the stack, and returning the state in those
	/// types by reference will result in references to memory that has already been cleaned up. If you need access to
	/// the state parameters, call the lower-level functionality with your own created states.
	//////
	template <typename _Input, typename _FromEncoding, typename _Output, typename _ToEncoding>
	constexpr auto transcode_into(
		_Input&& __input, _FromEncoding&& __from_encoding, _Output&& __output, _ToEncoding&& __to_encoding) {
		__detail::__careless_handler __handler {};

		return transcode_into(::std::forward<_Input>(__input), ::std::forward<_FromEncoding>(__from_encoding),
			::std::forward<_Output>(__output), ::std::forward<_ToEncoding>(__to_encoding), __handler);
	}

	//////
	/// @brief Converts the code units of the given input view through the
	///        from encoding to code units of the to encoding into the output
	///        view.
	/// @param[in]     __input An input_view to read code units from and use in the decode operation that will
	/// produce intermediate code points.
	/// @param[in]     __to_encoding The encoding that will be used to encode the intermediate code points into the
	/// final code units.
	/// @param[in]     __output An output_view to write code units to as the result of the encode operation from the
	/// intermediate code points.
	///
	/// @remarks This function creates both: a @c from_error_handler using a ztd::text::default_handler that is marked
	/// as careless to pass to the next function overload; and, a @c from_encoding to interpret the @p __input by
	/// checking the @p __input 's @c value_type. This matters for lossy conversions that are not injective. The
	/// result from this function returns a ztd::text::stateless_transcode_result as opposed to a
	/// ztd::text::transcode_result because the state information is on the stack, and returning the state in those
	/// types by reference will result in references to memory that has already been cleaned up. If you need access to
	/// the state parameters, call the lower-level functionality with your own created states.
	//////
	template <typename _Input, typename _ToEncoding, typename _Output>
	constexpr auto transcode_into(_Input&& __input, _ToEncoding&& __to_encoding, _Output&& __output) {
		using _UInput        = __detail::__remove_cvref_t<_Input>;
		using _UFromEncoding = default_code_unit_encoding_t<__detail::__range_value_type_t<_UInput>>;

		_UFromEncoding __from_encoding {};
		__detail::__careless_handler __handler {};

		return transcode_into(::std::forward<_Input>(__input), __from_encoding, ::std::forward<_Output>(__output),
			::std::forward<_ToEncoding>(__to_encoding), __handler);
	}

	//////
	/// @brief Converts the code units of the given input view through the from encoding to code units of the to
	/// encoding for the output, which is then returned in a result structure with additional information about
	/// success.
	///
	/// @tparam _OutputContainer The container to default-construct and serialize data into. Typically, a @c
	/// std::basic_string or a @c std::vector of some sort.
	///
	/// @param[in]     __input An input_view to read code units from and use in the decode operation that will
	/// produce intermediate code points.
	/// @param[in]     __from_encoding The encoding that will be used to decode the input's code units into
	/// intermediate code points.
	/// @param[in]     __to_encoding The encoding that will be used to encode the intermediate code points into the
	/// final code units.
	/// @param[in]     __from_error_handler The error handler for the @p __from_encoding 's decode step.
	/// @param[in]     __to_error_handler The error handler for the @p __to_encoding 's encode step.
	/// @param[in,out] __from_state A reference to the associated state for the @p __from_encoding 's decode step.
	/// @param[in,out] __to_state A reference to the associated state for the @p __to_encoding 's encode step.
	///
	/// @returns A ztd::text::transcode_result object that contains references to @p __from_state and @p __to_state and
	/// an @c ".output" parameter that contains the @p _OutputContainer specified. If the container has a @c ".reserve"
	/// function, it is and some multiple of the input's size is used to pre-size the container, to aid with @c
	/// "push_back"/@c "insert" reallocation pains.
	//////
	template <typename _OutputContainer, typename _Input, typename _FromEncoding, typename _ToEncoding,
		typename _FromErrorHandler, typename _ToErrorHandler, typename _FromState, typename _ToState>
	constexpr auto transcode_to(_Input&& __input, _FromEncoding&& __from_encoding, _ToEncoding&& __to_encoding,
		_FromErrorHandler&& __from_error_handler, _ToErrorHandler&& __to_error_handler, _FromState& __from_state,
		_ToState& __to_state) {

		_OutputContainer __output {};
		if constexpr (__detail::__is_detected_v<__detail::__detect_adl_size, _Input>) {
			using _SizeType = decltype(__detail::__adl::__adl_size(__input));
			if constexpr (__detail::__is_detected_v<__detail::__detect_reserve_with_size_type, _OutputContainer,
				              _SizeType>) {
				// TODO: better estimates
				__output.reserve(__detail::__adl::__adl_size(__input));
			}
		}

		auto __insert_view     = unbounded_view(::std::back_inserter(__output));
		auto __stateful_result = transcode_into(::std::forward<_Input>(__input),
			::std::forward<_FromEncoding>(__from_encoding), ::std::move(__insert_view),
			::std::forward<_ToEncoding>(__to_encoding), ::std::forward<_FromErrorHandler>(__from_error_handler),
			::std::forward<_ToErrorHandler>(__to_error_handler), __from_state, __to_state);
		return __detail::__replace_result_output(::std::move(__stateful_result), ::std::move(__output));
	}

	//////
	/// @brief Converts the code units of the given input view through the from encoding to code units of the to
	/// encoding for the output, which is then returned in a result structure with additional information about
	/// success.
	///
	/// @tparam _OutputContainer The container to default-construct and serialize data into. Typically, a @c
	/// std::basic_string or a @c std::vector of some sort.
	///
	/// @param[in]     __input An input_view to read code units from and use in the decode operation that will
	/// produce intermediate code points.
	/// @param[in]     __from_encoding The encoding that will be used to decode the input's code units into
	/// intermediate code points.
	/// @param[in]     __to_encoding The encoding that will be used to encode the intermediate code points into the
	/// final code units.
	/// @param[in]     __from_error_handler The error handler for the @p __from_encoding 's decode step.
	/// @param[in]     __to_error_handler The error handler for the @p __to_encoding 's encode step.
	/// @param[in,out] __from_state A reference to the associated state for the @p __from_encoding 's decode step.
	///
	/// @returns A ztd::text::stateless_transcode_result object that contains references to an @c ".output" parameter
	/// that contains the @p _OutputContainer specified.
	///
	/// @remarks A default state for the encode step of the operation is create using ztd::text::make_encode_state. The
	/// return type is stateless since both states must be passed in. If you want to have access to the states, create
	/// both of them yourself and pass them into a lower-level function that accepts those parameters.
	//////
	template <typename _OutputContainer, typename _Input, typename _FromEncoding, typename _ToEncoding,
		typename _FromErrorHandler, typename _ToErrorHandler, typename _FromState>
	constexpr auto transcode_to(_Input&& __input, _FromEncoding&& __from_encoding, _ToEncoding&& __to_encoding,
		_FromErrorHandler&& __from_error_handler, _ToErrorHandler&& __to_error_handler, _FromState& __from_state) {
		using _UToEncoding = __detail::__remove_cvref_t<_ToEncoding>;
		using _ToState     = encode_state_t<_UToEncoding>;

		_ToState __to_state = make_encode_state(__to_encoding);

		return transcode_to<_OutputContainer>(::std::forward<_Input>(__input),
			::std::forward<_FromEncoding>(__from_encoding), ::std::forward<_ToEncoding>(__to_encoding),
			::std::forward<_FromErrorHandler>(__from_error_handler),
			::std::forward<_ToErrorHandler>(__to_error_handler), __from_state, __to_state);
	}

	//////
	/// @brief Converts the code units of the given input view through the from encoding to code units of the to
	/// encoding for the output, which is then returned in a result structure with additional information about
	/// success.
	///
	/// @tparam _OutputContainer The container to default-construct and serialize data into. Typically, a @c
	/// std::basic_string or a @c std::vector of some sort.
	///
	/// @param[in]     __input An input_view to read code units from and use in the decode operation that will
	/// produce intermediate code points.
	/// @param[in]     __from_encoding The encoding that will be used to decode the input's code units into
	/// intermediate code points.
	/// @param[in]     __to_encoding The encoding that will be used to encode the intermediate code points into the
	/// final code units.
	/// @param[in]     __from_error_handler The error handler for the @p __from_encoding 's decode step.
	/// @param[in]     __to_error_handler The error handler for the @p __to_encoding 's encode step.
	///
	/// @returns A ztd::text::stateless_transcode_result object that contains references to an @c ".output" parameter
	/// that contains the @p _OutputContainer specified.
	///
	/// @remarks A default state for the decode step of the operation is create using ztd::text::make_decode_state. The
	/// return type is stateless since both states must be passed in. If you want to have access to the states, create
	/// both of them yourself and pass them into a lower-level function that accepts those parameters.
	//////
	template <typename _OutputContainer, typename _Input, typename _FromEncoding, typename _ToEncoding,
		typename _FromErrorHandler, typename _ToErrorHandler>
	constexpr auto transcode_to(_Input&& __input, _FromEncoding&& __from_encoding, _ToEncoding&& __to_encoding,
		_FromErrorHandler&& __from_error_handler, _ToErrorHandler&& __to_error_handler) {
		using _UFromEncoding = __detail::__remove_cvref_t<_FromEncoding>;
		using _FromState     = decode_state_t<_UFromEncoding>;

		_FromState __from_state = make_decode_state(__from_encoding);

		return transcode_to<_OutputContainer>(::std::forward<_Input>(__input),
			::std::forward<_FromEncoding>(__from_encoding), ::std::forward<_ToEncoding>(__to_encoding),
			::std::forward<_FromErrorHandler>(__from_error_handler),
			::std::forward<_ToErrorHandler>(__to_error_handler), __from_state);
	}

	//////
	/// @brief Converts the code units of the given input view through the from encoding to code units of the to
	/// encoding for the output, which is then returned in a result structure with additional information about
	/// success.
	///
	/// @tparam _OutputContainer The container to default-construct and serialize data into. Typically, a @c
	/// std::basic_string or a @c std::vector of some sort.
	///
	/// @param[in]     __input An input_view to read code units from and use in the decode operation that will
	/// produce intermediate code points.
	/// @param[in]     __from_encoding The encoding that will be used to decode the input's code units into
	/// intermediate code points.
	/// @param[in]     __to_encoding The encoding that will be used to encode the intermediate code points into the
	/// final code units.
	/// @param[in]     __from_error_handler The error handler for the @p __from_encoding 's decode step.
	///
	/// @returns A ztd::text::stateless_transcode_result object that contains references to an @c ".output" parameter
	/// that contains the @p _OutputContainer specified.
	///
	/// @remarks A @c to_error_handler for the encode step of the operation is created using default construction of a
	/// ztd::text::default_handler that is marked as careless. The return type is stateless since both states must be
	/// passed in. If you want to have access to the states, create both of them yourself and pass them into a
	/// lower-level function that accepts those parameters.
	//////
	template <typename _OutputContainer, typename _Input, typename _FromEncoding, typename _ToEncoding,
		typename _FromErrorHandler>
	constexpr auto transcode_to(_Input&& __input, _FromEncoding&& __from_encoding, _ToEncoding&& __to_encoding,
		_FromErrorHandler&& __from_error_handler) {
		auto __handler = __detail::__duplicate_or_be_careless(__from_error_handler);

		return transcode_to<_OutputContainer>(::std::forward<_Input>(__input),
			::std::forward<_FromEncoding>(__from_encoding), ::std::forward<_ToEncoding>(__to_encoding),
			::std::forward<_FromErrorHandler>(__from_error_handler), __handler);
	}

	//////
	/// @brief Converts the code units of the given input view through the from encoding to code units of the to
	/// encoding for the output, which is then returned in a result structure with additional information about
	/// success.
	///
	/// @tparam _OutputContainer The container to default-construct and serialize data into. Typically, a @c
	/// std::basic_string or a @c std::vector of some sort.
	///
	/// @param[in]     __input An input_view to read code units from and use in the decode operation that will
	/// produce intermediate code points.
	/// @param[in]     __from_encoding The encoding that will be used to decode the input's code units into
	/// intermediate code points.
	/// @param[in]     __to_encoding The encoding that will be used to encode the intermediate code points into the
	/// final code units.
	///
	/// @returns A ztd::text::stateless_transcode_result object that contains references to an @c ".output" parameter
	/// that contains the @p _OutputContainer specified.
	///
	/// @remarks A @c from_error_handler for the encode step of the operation is created using default construction of
	/// a ztd::text::default_handler that is marked as careless. The return type is stateless since both states must be
	/// passed in. If you want to have access to the states, create both of them yourself and pass them into a
	/// lower-level function that accepts those parameters.
	//////
	template <typename _OutputContainer, typename _Input, typename _FromEncoding, typename _ToEncoding>
	constexpr auto transcode_to(_Input&& __input, _FromEncoding&& __from_encoding, _ToEncoding&& __to_encoding) {
		__detail::__careless_handler __handler {};

		return transcode_to<_OutputContainer>(::std::forward<_Input>(__input),
			::std::forward<_FromEncoding>(__from_encoding), ::std::forward<_ToEncoding>(__to_encoding), __handler);
	}

	//////
	/// @brief Converts the code units of the given input view through the from encoding to code units of the to
	/// encoding for the output, which is then returned in a result structure with additional information about
	/// success.
	///
	/// @tparam _OutputContainer The container to default-construct and serialize data into. Typically, a @c
	/// std::basic_string or a @c std::vector of some sort.
	///
	/// @param[in]     __input An input_view to read code units from and use in the decode operation that will
	/// produce intermediate code points.
	/// @param[in]     __to_encoding The encoding that will be used to encode the intermediate code points into the
	/// final code units.
	///
	/// @returns A ztd::text::stateless_transcode_result object that contains references to an @c ".output" parameter
	/// that contains the @p _OutputContainer specified.
	///
	/// @remarks A @c from_error_handler for the encode step of the operation is created using default construction of
	/// a ztd::text::default_handler that is marked as careless. The return type is stateless since both states must be
	/// passed in. If you want to have access to the states, create both of them yourself and pass them into a
	/// lower-level function that accepts those parameters.
	//////
	template <typename _OutputContainer, typename _Input, typename _ToEncoding>
	constexpr auto transcode_to(_Input&& __input, _ToEncoding&& __to_encoding) {
		using _UInput        = __detail::__remove_cvref_t<_Input>;
		using _UFromEncoding = default_code_unit_encoding_t<__detail::__range_value_type_t<_UInput>>;

		__detail::__careless_handler __handler {};
		_UFromEncoding __from_encoding {};

		return transcode_to<_OutputContainer>(
			::std::forward<_Input>(__input), __from_encoding, ::std::forward<_ToEncoding>(__to_encoding), __handler);
	}

	//////
	/// @brief Converts the code units of the given input view through the from encoding to code units of the to
	/// encoding for the output, which is then returned in a result structure with additional information about
	/// success.
	///
	/// @tparam _OutputContainer The container to default-construct and serialize data into. Typically, a @c
	/// std::basic_string or a @c std::vector of some sort.
	///
	/// @param[in]     __input An input_view to read code units from and use in the decode operation that will
	/// produce intermediate code points.
	/// @param[in]     __from_encoding The encoding that will be used to decode the input's code units into
	/// intermediate code points.
	/// @param[in]     __to_encoding The encoding that will be used to encode the intermediate code points into the
	/// final code units.
	/// @param[in]     __from_error_handler The error handler for the @p __from_encoding 's decode step.
	/// @param[in]     __to_error_handler The error handler for the @p __to_encoding 's encode step.
	/// @param[in,out] __from_state A reference to the associated state for the @p __from_encoding 's decode step.
	/// @param[in,out] __to_state A reference to the associated state for the @p __to_encoding 's encode step.
	///
	/// @returns An @p _OutputContainer with the result, regardless of whether an error occurs or not. If you are
	/// looking for error information and not just a quick one-off conversion function, please use
	/// ztd::text::transcode_to or ztd::text::transcode_into.
	//////
	template <typename _OutputContainer, typename _Input, typename _FromEncoding, typename _ToEncoding,
		typename _FromErrorHandler, typename _ToErrorHandler, typename _FromState, typename _ToState>
	constexpr auto transcode(_Input&& __input, _FromEncoding&& __from_encoding, _ToEncoding&& __to_encoding,
		_FromErrorHandler&& __from_error_handler, _ToErrorHandler&& __to_error_handler, _FromState& __from_state,
		_ToState& __to_state) {

		_OutputContainer __output {};
		if constexpr (__detail::__is_detected_v<__detail::__detect_adl_size, _Input>) {
			using _SizeType = decltype(__detail::__adl::__adl_size(__input));
			if constexpr (__detail::__is_detected_v<__detail::__detect_reserve_with_size_type, _OutputContainer,
				              _SizeType>) {
				// TODO: better estimates
				__output.reserve(__detail::__adl::__adl_size(__input));
			}
		}

		auto __insert_view     = unbounded_view(::std::back_inserter(__output));
		auto __stateful_result = transcode_into(::std::forward<_Input>(__input),
			::std::forward<_FromEncoding>(__from_encoding), ::std::move(__insert_view),
			::std::forward<_ToEncoding>(__to_encoding), ::std::forward<_FromErrorHandler>(__from_error_handler),
			::std::forward<_ToErrorHandler>(__to_error_handler), __from_state, __to_state);
		// We are explicitly discard the stateful result here;
		// use the transcode_to and transcode_into functions for more information
		(void)__stateful_result;
		return __output;
	}

	//////
	/// @brief Converts the code units of the given input view through the from encoding to code units of the to
	/// encoding for the output, which is then returned in a result structure with additional information about
	/// success.
	///
	/// @tparam _OutputContainer The container to default-construct and serialize data into. Typically, a @c
	/// std::basic_string or a @c std::vector of some sort.
	///
	/// @param[in]     __input An input_view to read code units from and use in the decode operation that will
	/// produce intermediate code points.
	/// @param[in]     __from_encoding The encoding that will be used to decode the input's code units into
	/// intermediate code points.
	/// @param[in]     __to_encoding The encoding that will be used to encode the intermediate code points into the
	/// final code units.
	/// @param[in]     __from_error_handler The error handler for the @p __from_encoding 's decode step.
	/// @param[in]     __to_error_handler The error handler for the @p __to_encoding 's encode step.
	/// @param[in,out] __from_state A reference to the associated state for the @p __from_encoding 's decode step.
	///
	/// @returns An @p _OutputContainer with the result, regardless of whether an error occurs or not. If you are
	/// looking for error information and not just a quick one-off conversion function, please use
	/// ztd::text::transcode_to or ztd::text::transcode_into.
	///
	/// @remarks This function creates an @c to_state for the encoding step of the operation using
	/// ztd::text::make_encode_state.
	//////
	template <typename _OutputContainer, typename _Input, typename _FromEncoding, typename _ToEncoding,
		typename _FromErrorHandler, typename _ToErrorHandler, typename _FromState>
	constexpr auto transcode(_Input&& __input, _FromEncoding&& __from_encoding, _ToEncoding&& __to_encoding,
		_FromErrorHandler&& __from_error_handler, _ToErrorHandler&& __to_error_handler, _FromState& __from_state) {
		using _UToEncoding = __detail::__remove_cvref_t<_ToEncoding>;
		using _ToState     = encode_state_t<_UToEncoding>;

		_ToState __to_state = make_encode_state(__to_encoding);

		return transcode<_OutputContainer>(::std::forward<_Input>(__input),
			::std::forward<_FromEncoding>(__from_encoding), ::std::forward<_ToEncoding>(__to_encoding),
			::std::forward<_FromErrorHandler>(__from_error_handler),
			::std::forward<_ToErrorHandler>(__to_error_handler), __from_state, __to_state);
	}

	//////
	/// @brief Converts the code units of the given input view through the from encoding to code units of the to
	/// encoding for the output, which is then returned in a result structure with additional information about
	/// success.
	///
	/// @tparam _OutputContainer The container to default-construct and serialize data into. Typically, a @c
	/// std::basic_string or a @c std::vector of some sort.
	///
	/// @param[in]     __input An input_view to read code units from and use in the decode operation that will
	/// produce intermediate code points.
	/// @param[in]     __from_encoding The encoding that will be used to decode the input's code units into
	/// intermediate code points.
	/// @param[in]     __to_encoding The encoding that will be used to encode the intermediate code points into the
	/// final code units.
	/// @param[in]     __from_error_handler The error handler for the @p __from_encoding 's decode step.
	/// @param[in]     __to_error_handler The error handler for the @p __to_encoding 's encode step.
	///
	/// @returns An @p _OutputContainer with the result, regardless of whether an error occurs or not. If you are
	/// looking for error information and not just a quick one-off conversion function, please use
	/// ztd::text::transcode_to or ztd::text::transcode_into.
	///
	/// @remarks This function creates an @c from_state for the encoding step of the operation using
	/// ztd::text::make_decode_state.
	//////
	template <typename _OutputContainer, typename _Input, typename _FromEncoding, typename _ToEncoding,
		typename _FromErrorHandler, typename _ToErrorHandler>
	constexpr auto transcode(_Input&& __input, _FromEncoding&& __from_encoding, _ToEncoding&& __to_encoding,
		_FromErrorHandler&& __from_error_handler, _ToErrorHandler&& __to_error_handler) {
		using _UFromEncoding = __detail::__remove_cvref_t<_FromEncoding>;
		using _FromState     = decode_state_t<_UFromEncoding>;

		_FromState __from_state = make_decode_state(__from_encoding);

		return transcode<_OutputContainer>(::std::forward<_Input>(__input),
			::std::forward<_FromEncoding>(__from_encoding), ::std::forward<_ToEncoding>(__to_encoding),
			::std::forward<_FromErrorHandler>(__from_error_handler),
			::std::forward<_ToErrorHandler>(__to_error_handler), __from_state);
	}

	//////
	/// @brief Converts the code units of the given input view through the from encoding to code units of the to
	/// encoding for the output, which is then returned in a result structure with additional information about
	/// success.
	///
	/// @tparam _OutputContainer The container to default-construct and serialize data into. Typically, a @c
	/// std::basic_string or a @c std::vector of some sort.
	///
	/// @param[in]     __input An input_view to read code units from and use in the decode operation that will
	/// produce intermediate code points.
	/// @param[in]     __from_encoding The encoding that will be used to decode the input's code units into
	/// intermediate code points.
	/// @param[in]     __to_encoding The encoding that will be used to encode the intermediate code points into the
	/// final code units.
	/// @param[in]     __from_error_handler The error handler for the @p __from_encoding 's decode step.
	///
	/// @returns An @p _OutputContainer with the result, regardless of whether an error occurs or not. If you are
	/// looking for error information and not just a quick one-off conversion function, please use
	/// ztd::text::transcode_to or ztd::text::transcode_into.
	///
	/// @remarks This function creates a @c to_error_handler from a class like ztd::text::default_handler, but that is
	/// marked as careless since you did not explicitly provide it. This matters for lossy conversions that are not
	/// injective.
	//////
	template <typename _OutputContainer, typename _Input, typename _FromEncoding, typename _ToEncoding,
		typename _FromErrorHandler>
	constexpr auto transcode(_Input&& __input, _FromEncoding&& __from_encoding, _ToEncoding&& __to_encoding,
		_FromErrorHandler&& __from_error_handler) {
		auto __handler = __detail::__duplicate_or_be_careless(__from_error_handler);

		return transcode<_OutputContainer>(::std::forward<_Input>(__input),
			::std::forward<_FromEncoding>(__from_encoding), ::std::forward<_ToEncoding>(__to_encoding),
			::std::forward<_FromErrorHandler>(__from_error_handler), __handler);
	}

	//////
	/// @brief Converts the code units of the given input view through the from encoding to code units of the to
	/// encoding for the output, which is then returned in a result structure with additional information about
	/// success.
	///
	/// @tparam _OutputContainer The container to default-construct and serialize data into. Typically, a @c
	/// std::basic_string or a @c std::vector of some sort.
	///
	/// @param[in]     __input An input_view to read code units from and use in the decode operation that will
	/// produce intermediate code points.
	/// @param[in]     __from_encoding The encoding that will be used to decode the input's code units into
	/// intermediate code points.
	/// @param[in]     __to_encoding The encoding that will be used to encode the intermediate code points into the
	/// final code units.
	///
	/// @returns An @p _OutputContainer with the result, regardless of whether an error occurs or not. If you are
	/// looking for error information and not just a quick one-off conversion function, please use
	/// ztd::text::transcode_to or ztd::text::transcode_into.
	///
	/// @remarks This function creates a @c from_error_handler from a class like ztd::text::default_handler, but that
	/// is marked as careless since you did not explicitly provide it. This matters for lossy conversions that are not
	/// injective.
	//////
	template <typename _OutputContainer, typename _Input, typename _FromEncoding, typename _ToEncoding>
	constexpr auto transcode(_Input&& __input, _FromEncoding&& __from_encoding, _ToEncoding&& __to_encoding) {
		__detail::__careless_handler __handler {};

		return transcode(::std::forward<_Input>(__input), ::std::forward<_FromEncoding>(__from_encoding),
			::std::forward<_ToEncoding>(__to_encoding), __handler);
	}

	//////
	/// @brief Converts the code units of the given input view through the from encoding to code units of the to
	/// encoding for the output, which is then returned in a result structure with additional information about
	/// success.
	///
	/// @tparam _OutputContainer The container to default-construct and serialize data into. Typically, a @c
	/// std::basic_string or a @c std::vector of some sort.
	///
	/// @param[in]     __input An input_view to read code units from and use in the decode operation that will
	/// produce intermediate code points.
	/// @param[in]     __to_encoding The encoding that will be used to encode the intermediate code points into the
	/// final code units.
	///
	/// @returns An @p _OutputContainer with the result, regardless of whether an error occurs or not. If you are
	/// looking for error information and not just a quick one-off conversion function, please use
	/// ztd::text::transcode_to or ztd::text::transcode_into.
	///
	/// @remarks This function creates both: a @c from_error_handler from a class like ztd::text::default_handler, but
	/// that is marked as careless since you did not explicitly provide it; and, a @c from_encoding derived from the @p
	/// "__input"'s @c value_type. The careless marking matters for lossy conversions that are not injective.
	//////
	template <typename _OutputContainer, typename _Input, typename _ToEncoding>
	constexpr auto transcode(_Input&& __input, _ToEncoding&& __to_encoding) {
		using _UInput        = __detail::__remove_cvref_t<_Input>;
		using _UFromEncoding = default_code_unit_encoding_t<__detail::__range_value_type_t<_UInput>>;

		__detail::__careless_handler __handler {};
		_UFromEncoding __from_encoding {};

		return transcode<_OutputContainer>(
			::std::forward<_Input>(__input), __from_encoding, ::std::forward<_ToEncoding>(__to_encoding), __handler);
	}

	//////
	/// @brief Converts the code units of the given input view through the from encoding to code units of the to
	/// encoding for the output, which is stored std::basic_string of code units of the @p "__to_encoding".
	///
	/// @param[in]     __input An input_view to read code units from and use in the decode operation that will
	/// produce intermediate code points.
	/// @param[in]     __from_encoding The encoding that will be used to decode the input's code units into
	/// intermediate code points.
	/// @param[in]     __to_encoding The encoding that will be used to encode the intermediate code points into the
	/// final code units.
	/// @param[in]     __from_error_handler The error handler for the @p __from_encoding 's decode step.
	/// @param[in]     __to_error_handler The error handler for the @p __to_encoding 's encode step.
	/// @param[in,out] __from_state A reference to the associated state for the @p __from_encoding 's decode step.
	/// @param[in,out] __to_state A reference to the associated state for the @p __to_encoding 's encode step.
	///
	/// @returns A @c std::basic_string or @c std::vector with an element type of @c
	/// ztd::text::code_unit<_ToEncoding> with the result, regardless of whether an error occurs or not. If
	/// you are looking for error information and not just a quick one-off conversion function, please use
	/// ztd::text::transcode_to or ztd::text::transcode_into.
	//////
	template <typename _Input, typename _FromEncoding, typename _ToEncoding, typename _FromErrorHandler,
		typename _ToErrorHandler, typename _FromState, typename _ToState>
	constexpr auto transcode(_Input&& __input, _FromEncoding&& __from_encoding, _ToEncoding&& __to_encoding,
		_FromErrorHandler&& __from_error_handler, _ToErrorHandler&& __to_error_handler, _FromState& __from_state,
		_ToState& __to_state) {
		using _UToEncoding = __detail::__remove_cvref_t<_ToEncoding>;
		using _CodeUnit    = code_unit_t<_UToEncoding>;
		using _OutputContainer
			= ::std::conditional_t<is_unicode_code_point_v<_CodeUnit> || __detail::__is_character_v<_CodeUnit>,
			     ::std::basic_string<_CodeUnit>, ::std::vector<_CodeUnit>>;

		return transcode<_OutputContainer>(::std::forward<_Input>(__input),
			::std::forward<_FromEncoding>(__from_encoding), ::std::forward<_ToEncoding>(__to_encoding),
			::std::forward<_FromErrorHandler>(__from_error_handler),
			::std::forward<_ToErrorHandler>(__to_error_handler), __from_state, __to_state);
	}

	//////
	/// @brief Converts the code units of the given input view through the from encoding to code units of the to
	/// encoding for the output, which is stored std::basic_string of code units of the @p "__to_encoding".
	///
	/// @param[in]     __input An input_view to read code units from and use in the decode operation that will
	/// produce intermediate code points.
	/// @param[in]     __from_encoding The encoding that will be used to decode the input's code units into
	/// intermediate code points.
	/// @param[in]     __to_encoding The encoding that will be used to encode the intermediate code points into the
	/// final code units.
	/// @param[in]     __from_error_handler The error handler for the @p __from_encoding 's decode step.
	/// @param[in]     __to_error_handler The error handler for the @p __to_encoding 's encode step.
	/// @param[in,out] __from_state A reference to the associated state for the @p __from_encoding 's decode step.
	///
	/// @remarks A default @c to_state is created by calling ztd::text::make_encode_state.
	///
	/// @returns A @c std::basic_string<ztd::text::code_unit_t<_ToEncoding>> with the result, regardless of
	/// whether an error occurs or not. If you are looking for error information and not just a quick one-off
	/// conversion function, please use ztd::text::transcode_to or ztd::text::transcode_into.
	//////
	template <typename _Input, typename _FromEncoding, typename _ToEncoding, typename _FromErrorHandler,
		typename _ToErrorHandler, typename _FromState>
	constexpr auto transcode(_Input&& __input, _FromEncoding&& __from_encoding, _ToEncoding&& __to_encoding,
		_FromErrorHandler&& __from_error_handler, _ToErrorHandler&& __to_error_handler, _FromState& __from_state) {
		using _UToEncoding = __detail::__remove_cvref_t<_ToEncoding>;
		using _ToState     = encode_state_t<_UToEncoding>;

		_ToState __to_state = make_encode_state(__to_encoding);

		return transcode(::std::forward<_Input>(__input), ::std::forward<_FromEncoding>(__from_encoding),
			::std::forward<_ToEncoding>(__to_encoding), ::std::forward<_FromErrorHandler>(__from_error_handler),
			::std::forward<_ToErrorHandler>(__to_error_handler), __from_state, __to_state);
	}

	//////
	/// @brief Converts the code units of the given input view through the from encoding to code units of the to
	/// encoding for the output, which is stored std::basic_string of code units of the @p "__to_encoding".
	///
	/// @param[in]     __input An input_view to read code units from and use in the decode operation that will
	/// produce intermediate code points.
	/// @param[in]     __from_encoding The encoding that will be used to decode the input's code units into
	/// intermediate code points.
	/// @param[in]     __to_encoding The encoding that will be used to encode the intermediate code points into the
	/// final code units.
	/// @param[in]     __from_error_handler The error handler for the @p __from_encoding 's decode step.
	/// @param[in]     __to_error_handler The error handler for the @p __to_encoding 's encode step.
	///
	/// @remarks A default @c from_state is created by calling ztd::text::make_decode_state. It is then passed to
	/// further overloads of this function.
	///
	/// @returns A @c std::basic_string<ztd::text::code_unit_t<_ToEncoding>> with the result, regardless of
	/// whether an error occurs or not. If you are looking for error information and not just a quick one-off
	/// conversion function, please use ztd::text::transcode_to or ztd::text::transcode_into.
	//////
	template <typename _Input, typename _FromEncoding, typename _ToEncoding, typename _FromErrorHandler,
		typename _ToErrorHandler>
	constexpr auto transcode(_Input&& __input, _FromEncoding&& __from_encoding, _ToEncoding&& __to_encoding,
		_FromErrorHandler&& __from_error_handler, _ToErrorHandler&& __to_error_handler) {
		using _UFromEncoding = __detail::__remove_cvref_t<_FromEncoding>;
		using _FromState     = decode_state_t<_UFromEncoding>;

		_FromState __from_state = make_decode_state(__from_encoding);

		return transcode(::std::forward<_Input>(__input), ::std::forward<_FromEncoding>(__from_encoding),
			::std::forward<_ToEncoding>(__to_encoding), ::std::forward<_FromErrorHandler>(__from_error_handler),
			::std::forward<_ToErrorHandler>(__to_error_handler), __from_state);
	}

	//////
	/// @brief Converts the code units of the given input view through the from encoding to code units of the to
	/// encoding for the output, which is stored std::basic_string of code units of the @p "__to_encoding".
	///
	/// @param[in]     __input An input_view to read code units from and use in the decode operation that will
	/// produce intermediate code points.
	/// @param[in]     __from_encoding The encoding that will be used to decode the input's code units into
	/// intermediate code points.
	/// @param[in]     __to_encoding The encoding that will be used to encode the intermediate code points into the
	/// final code units.
	/// @param[in]     __from_error_handler The error handler for the @p __from_encoding 's decode step.
	///
	/// @remarks A @c to_error_handler similar to ztd::text::default_handler is created, but it is marked as careless.
	/// It is then passed to further overloads of this function.
	///
	/// @returns A @c std::basic_string<ztd::text::code_unit_t<_ToEncoding>> with the result, regardless of
	/// whether an error occurs or not. If you are looking for error information and not just a quick one-off
	/// conversion function, please use ztd::text::transcode_to or ztd::text::transcode_into.
	//////
	template <typename _Input, typename _FromEncoding, typename _ToEncoding, typename _FromErrorHandler>
	constexpr auto transcode(_Input&& __input, _FromEncoding&& __from_encoding, _ToEncoding&& __to_encoding,
		_FromErrorHandler&& __from_error_handler) {
		auto __handler = __detail::__duplicate_or_be_careless(__from_error_handler);

		return transcode(::std::forward<_Input>(__input), ::std::forward<_FromEncoding>(__from_encoding),
			::std::forward<_ToEncoding>(__to_encoding), ::std::forward<_FromErrorHandler>(__from_error_handler),
			__handler);
	}

	//////
	/// @brief Converts the code units of the given input view through the from encoding to code units of the to
	/// encoding for the output, which is stored std::basic_string of code units of the @p "__to_encoding".
	///
	/// @param[in]     __input An input_view to read code units from and use in the decode operation that will
	/// produce intermediate code points.
	/// @param[in]     __from_encoding The encoding that will be used to decode the input's code units into
	/// intermediate code points.
	/// @param[in]     __to_encoding The encoding that will be used to encode the intermediate code points into the
	/// final code units.
	///
	/// @remarks A @c from_error_handler similar to ztd::text::default_handler is created, but it is marked as
	/// careless.
	///
	/// @returns A @c std::basic_string<ztd::text::code_unit_t<_ToEncoding>> with the result, regardless of
	/// whether an error occurs or not. If you are looking for error information and not just a quick one-off
	/// conversion function, please use ztd::text::transcode_to or ztd::text::transcode_into.
	//////
	template <typename _Input, typename _FromEncoding, typename _ToEncoding>
	constexpr auto transcode(_Input&& __input, _FromEncoding&& __from_encoding, _ToEncoding&& __to_encoding) {
		__detail::__careless_handler __handler {};

		return transcode(::std::forward<_Input>(__input), ::std::forward<_FromEncoding>(__from_encoding),
			::std::forward<_ToEncoding>(__to_encoding), __handler);
	}

	//////
	/// @brief Converts the code units of the given input view through the from encoding to code units of the to
	/// encoding for the output, which is stored std::basic_string of code units of the @p "__to_encoding".
	///
	/// @param[in]     __input An input_view to read code units from and use in the decode operation that will
	/// produce intermediate code points.
	/// @param[in]     __to_encoding The encoding that will be used to encode the intermediate code points into the
	/// final code units.
	///
	/// @remarks A @c from_encoding is created by looking at the @c value_type of the @p "__input". An object similar
	/// to ztd::text::default_handler is created, but it is marked as careless.
	///
	/// @returns A @c "std::basic_string<ztd::text::default_code_unit_encoding_t<Encoding-From-_Input>>" with the
	/// result, regardless of whether an error occurs or not. If you are looking for error information and not just a
	/// quick one-off conversion function, please use ztd::text::transcode_to or ztd::text::transcode_into.
	//////
	template <typename _Input, typename _ToEncoding>
	constexpr auto transcode(_Input&& __input, _ToEncoding&& __to_encoding) {
		using _UInput        = __detail::__remove_cvref_t<_Input>;
		using _UFromEncoding = default_code_unit_encoding_t<__detail::__range_value_type_t<_UInput>>;

		__detail::__careless_handler __handler {};
		_UFromEncoding __from_encoding {};

		return transcode(
			::std::forward<_Input>(__input), __from_encoding, ::std::forward<_ToEncoding>(__to_encoding), __handler);
	}

	//////
	/// @}
	//////

	ZTD_TEXT_INLINE_ABI_NAMESPACE_CLOSE_I_
}} // namespace ztd::text

#endif // ZTD_TEXT_TRANSCODE_HPP
