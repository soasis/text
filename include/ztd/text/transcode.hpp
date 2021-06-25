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

#include <ztd/text/version.hpp>

#include <ztd/text/code_point.hpp>
#include <ztd/text/code_unit.hpp>
#include <ztd/text/error_handler.hpp>
#include <ztd/text/detail/is_lossless.hpp>
#include <ztd/text/state.hpp>
#include <ztd/text/default_encoding.hpp>
#include <ztd/text/transcode_result.hpp>
#include <ztd/text/is_unicode_code_point.hpp>
#include <ztd/text/unbounded.hpp>
#include <ztd/text/tag.hpp>

#include <ztd/text/detail/transcode_one.hpp>
#include <ztd/text/detail/encoding_range.hpp>
#include <ztd/text/detail/type_traits.hpp>
#include <ztd/text/detail/span.hpp>
#include <ztd/text/detail/transcode_extension_points.hpp>

#include <string>
#include <vector>
#include <string_view>

#include <ztd/text/detail/prologue.hpp>

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
	/// @result A ztd::text::transcode_result object that contains references to @p __from_state and @p
	/// __to_state.
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
		using _InitialInput          = __txt_detail::__string_view_or_span_or_reconstruct_t<_Input>;
		using _InitialOutput         = __txt_detail::__range_reconstruct_t<_Output>;
		using _UFromEncoding         = __txt_detail::__remove_cvref_t<_FromEncoding>;
		using _IntermediateCodePoint = code_point_t<_UFromEncoding>;
		constexpr ::std::size_t _IntermediateCodePointMax = max_code_points_v<_UFromEncoding>;
		using _IntermediateStorage                        = _IntermediateCodePoint[_IntermediateCodePointMax];
		using _Intermediate  = ::ztd::text::span<_IntermediateCodePoint, _IntermediateCodePointMax>;
		using _Result        = decltype(__txt_detail::__basic_transcode_one<__txt_detail::__consume::__no>(
               ::std::declval<_InitialInput>(), __from_encoding, ::std::declval<_InitialOutput>(), __to_encoding,
               __from_error_handler, __to_error_handler, __from_state, __to_state, ::std::declval<_Intermediate&>()));
		using _WorkingInput  = decltype(::std::declval<_Result>().input);
		using _WorkingOutput = decltype(::std::declval<_Result>().output);

		_WorkingInput __working_input(
			__txt_detail::__string_view_or_span_or_reconstruct(::std::forward<_Input>(__input)));
		_WorkingOutput __working_output(
			__txt_detail::__reconstruct(::std::in_place_type<_WorkingOutput>, ::std::forward<_Output>(__output)));

		_IntermediateStorage __intermediate_storage {};
		_Intermediate __intermediate(__intermediate_storage);
		::std::size_t __handled_errors = 0;
		for (;;) {
			auto __transcode_result = __txt_detail::__basic_transcode_one<__txt_detail::__consume::__no>(
				::std::move(__working_input), __from_encoding, ::std::move(__working_output), __to_encoding,
				__from_error_handler, __to_error_handler, __from_state, __to_state, __intermediate);
			if (__transcode_result.error_code != encoding_error::ok) {
				return _Result(::std::move(__working_input), ::std::move(__working_output), __from_state,
					__to_state, __transcode_result.error_code, __transcode_result.handled_errors);
			}
			__handled_errors += __transcode_result.handled_errors;
			__working_input  = ::std::move(__transcode_result.input);
			__working_output = ::std::move(__transcode_result.output);
			if (__txt_detail::__adl::__adl_empty(__working_input)) {
				break;
			}
		}
		return _Result(::std::move(__working_input), ::std::move(__working_output), __from_state, __to_state,
			encoding_error::ok, __handled_errors);
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
	/// @result A ztd::text::transcode_result object that contains references to @p __from_state and @p
	/// __to_state.
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
		if constexpr (__txt_detail::__is_detected_v<__txt_detail::__detect_adl_text_transcode, _Input, _Output,
			              _FromEncoding, _ToEncoding, _FromErrorHandler, _ToErrorHandler, _FromState, _ToState>) {
			return text_transcode(
				tag<__txt_detail::__remove_cvref_t<_FromEncoding>, __txt_detail::__remove_cvref_t<_ToEncoding>> {},
				::std::forward<_Input>(__input), ::std::forward<_FromEncoding>(__from_encoding),
				::std::forward<_Output>(__output), ::std::forward<_ToEncoding>(__to_encoding),
				::std::forward<_FromErrorHandler>(__from_error_handler),
				::std::forward<_ToErrorHandler>(__to_error_handler), __from_state, __to_state);
		}
		else if constexpr (__txt_detail::__is_detected_v<__txt_detail::__detect_adl_internal_text_transcode, _Input,
			                   _FromEncoding, _Output, _ToEncoding, _FromErrorHandler, _ToErrorHandler, _FromState,
			                   _ToState>) {
			return __text_transcode(
				tag<__txt_detail::__remove_cvref_t<_FromEncoding>, __txt_detail::__remove_cvref_t<_ToEncoding>> {},
				::std::forward<_Input>(__input), ::std::forward<_FromEncoding>(__from_encoding),
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
	/// ztd::text::stateless_transcode_result as opposed to a ztd::text::transcode_result because the state
	/// information is on the stack, and returning the state in those types by reference will result in references to
	/// memory that has already been cleaned up. If you need access to the state parameters, call the lower-level
	/// functionality with your own created states.
	//////
	template <typename _Input, typename _FromEncoding, typename _Output, typename _ToEncoding,
		typename _FromErrorHandler, typename _ToErrorHandler, typename _FromState>
	constexpr auto transcode_into(_Input&& __input, _FromEncoding&& __from_encoding, _Output&& __output,
		_ToEncoding&& __to_encoding, _FromErrorHandler&& __from_error_handler, _ToErrorHandler&& __to_error_handler,
		_FromState& __from_state) {
		using _UToEncoding = __txt_detail::__remove_cvref_t<_ToEncoding>;
		using _ToState     = encode_state_t<_UToEncoding>;

		_ToState __to_state = make_encode_state(__to_encoding);

		auto __stateful_result = transcode_into(::std::forward<_Input>(__input),
			::std::forward<_FromEncoding>(__from_encoding), ::std::forward<_Output>(__output),
			::std::forward<_ToEncoding>(__to_encoding), ::std::forward<_FromErrorHandler>(__from_error_handler),
			::std::forward<_ToErrorHandler>(__to_error_handler), __from_state, __to_state);

		return __txt_detail::__slice_to_stateless(::std::move(__stateful_result));
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
	/// ztd::text::transcode_result because the state information is on the stack, and returning the state in
	/// those types by reference will result in references to memory that has already been cleaned up. If you need
	/// access to the state parameters, call the lower-level functionality with your own created states.
	//////
	template <typename _Input, typename _FromEncoding, typename _Output, typename _ToEncoding,
		typename _FromErrorHandler, typename _ToErrorHandler>
	constexpr auto transcode_into(_Input&& __input, _FromEncoding&& __from_encoding, _Output&& __output,
		_ToEncoding&& __to_encoding, _FromErrorHandler&& __from_error_handler, _ToErrorHandler&& __to_error_handler) {
		using _UFromEncoding = __txt_detail::__remove_cvref_t<_FromEncoding>;
		using _FromState     = decode_state_t<_UFromEncoding>;

		_FromState __from_state = make_decode_state(__from_encoding);

		return transcode_into(::std::forward<_Input>(__input), ::std::forward<_FromEncoding>(__from_encoding),
			::std::forward<_Output>(__output), ::std::forward<_ToEncoding>(__to_encoding),
			::std::forward<_FromErrorHandler>(__from_error_handler),
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
	/// ztd::text::transcode_result because the state information is on the stack, and returning the state in
	/// those types by reference will result in references to memory that has already been cleaned up. If you need
	/// access to the state parameters, call the lower-level functionality with your own created states.
	//////
	template <typename _Input, typename _FromEncoding, typename _Output, typename _ToEncoding,
		typename _FromErrorHandler>
	constexpr auto transcode_into(_Input&& __input, _FromEncoding&& __from_encoding, _Output&& __output,
		_ToEncoding&& __to_encoding, _FromErrorHandler&& __from_error_handler) {
		auto __handler = __txt_detail::__duplicate_or_be_careless(__from_error_handler);

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
	/// ztd::text::transcode_result because the state information is on the stack, and returning the state in
	/// those types by reference will result in references to memory that has already been cleaned up. If you need
	/// access to the state parameters, call the lower-level functionality with your own created states.
	//////
	template <typename _Input, typename _FromEncoding, typename _Output, typename _ToEncoding>
	constexpr auto transcode_into(
		_Input&& __input, _FromEncoding&& __from_encoding, _Output&& __output, _ToEncoding&& __to_encoding) {
		default_handler __handler {};

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
	/// ztd::text::transcode_result because the state information is on the stack, and returning the state in
	/// those types by reference will result in references to memory that has already been cleaned up. If you need
	/// access to the state parameters, call the lower-level functionality with your own created states.
	//////
	template <typename _Input, typename _ToEncoding, typename _Output>
	constexpr auto transcode_into(_Input&& __input, _ToEncoding&& __to_encoding, _Output&& __output) {
		using _UInput        = __txt_detail::__remove_cvref_t<_Input>;
		using _UFromEncoding = default_code_unit_encoding_t<__txt_detail::__range_value_type_t<_UInput>>;

		_UFromEncoding __from_encoding {};
		default_handler __handler {};

		return transcode_into(::std::forward<_Input>(__input), __from_encoding, ::std::forward<_Output>(__output),
			::std::forward<_ToEncoding>(__to_encoding), __handler);
	}

	namespace __txt_detail {
		template <typename _Input, typename _FromEncoding, typename _OutputContainer, typename _ToEncoding,
			typename _FromErrorHandler, typename _ToErrorHandler, typename _FromState, typename _ToState>
		constexpr auto __intermediate_transcode_to_storage(_Input&& __input, _FromEncoding&& __from_encoding,
			_OutputContainer& __output, _ToEncoding&& __to_encoding, _FromErrorHandler&& __from_error_handler,
			_ToErrorHandler&& __to_error_handler, _FromState& __from_state, _ToState& __to_state) {
			// … Weeeellll. Here we go …
			using _UToEncoding = __txt_detail::__remove_cvref_t<_ToEncoding>;
			constexpr ::std::size_t _IntermediateOutputMax
				= ZTD_TEXT_INTERMEDIATE_BUFFER_SIZE_I_ < max_code_units_v<_UToEncoding>
				? max_code_units_v<_UToEncoding>
				: ZTD_TEXT_INTERMEDIATE_BUFFER_SIZE_I_;
			using _IntermediateOutputValueType = code_unit_t<_UToEncoding>;
			using _InitialInput                = __string_view_or_span_or_reconstruct_t<_Input>;
			using _IntermediateOutputInitial
				= ::ztd::text::span<_IntermediateOutputValueType, _IntermediateOutputMax>;
			using _IntermediateOutput = ::ztd::text::span<_IntermediateOutputValueType>;
			using _DecodeResult       = decltype(__from_encoding.decode_one(::std::declval<_InitialInput>(),
                    ::std::declval<_IntermediateOutput>(), __from_error_handler, __from_state));
			using _WorkingInput       = decltype(::std::declval<_DecodeResult>().input);

			_WorkingInput __working_input(
				__txt_detail::__string_view_or_span_or_reconstruct(::std::forward<_Input>(__input)));
			_IntermediateOutputValueType __intermediate_output_storage[_IntermediateOutputMax] {};
			_IntermediateOutputInitial __intermediate_initial(__intermediate_output_storage);
			for (;;) {
				auto __result
					= transcode_into(::std::move(__working_input), __from_encoding, __intermediate_initial,
					     __to_encoding, __from_error_handler, __to_error_handler, __from_state, __to_state);
				_IntermediateOutput __intermediate_output(
					__intermediate_initial.data(), __result.output.data() - __intermediate_initial.data());
				using _SpanIterator = typename _IntermediateOutput::iterator;
				if constexpr (__txt_detail::__is_detected_v<__txt_detail::__detect_insert_bulk, _OutputContainer,
					              _SpanIterator, _SpanIterator>) {
					// inserting in bulk
					// can be faster, more performant,
					// save us some coding too
					__output.insert(__output.cend(), __intermediate_output.begin(), __intermediate_output.end());
				}
				else {
					// O O F! we have to insert one at a time.
					for (auto&& __intermediate_code_unit : __intermediate_output) {
						if constexpr (__txt_detail::__is_detected_v<__txt_detail::__detect_push_back,
							              _OutputContainer, _IntermediateOutputValueType>) {
							__output.push_back(__intermediate_code_unit);
						}
						else {
							__output.insert(__output.cend(), __intermediate_code_unit);
						}
					}
				}
				if (__result.error_code == encoding_error::insufficient_output_space) {
					// loop around, we've got S P A C E for more
					__working_input = ::std::move(__result.input);
					continue;
				}
				if (__result.error_code != encoding_error::ok) {
					return __result;
				}
				if (__txt_detail::__adl::__adl_empty(__result.input)) {
					return __result;
				}
			}
		}

		template <typename _OutputContainer, typename _Input, typename _FromEncoding, typename _ToEncoding,
			typename _FromErrorHandler, typename _ToErrorHandler, typename _FromState, typename _ToState>
		constexpr auto __transcode_dispatch(_Input&& __input, _FromEncoding&& __from_encoding,
			_ToEncoding&& __to_encoding, _FromErrorHandler&& __from_error_handler,
			_ToErrorHandler&& __to_error_handler, _FromState& __from_state, _ToState& __to_state) {

			using _UFromEncoding        = __remove_cvref_t<_FromEncoding>;
			using _BackInserterIterator = decltype(::std::back_inserter(::std::declval<_OutputContainer&>()));
			using _Unbounded            = unbounded_view<_BackInserterIterator>;
			using _Intermediate = ::ztd::text::span<code_point_t<_UFromEncoding>, max_code_points_v<_UFromEncoding>>;

			constexpr bool _IsErrorHandlerCallable
				= __txt_detail::__is_encode_error_handler_callable_v<_ToEncoding, _Intermediate, _Unbounded,
				       _ToErrorHandler,
				       _ToState> && __txt_detail::__is_decode_error_handler_callable_v<_FromEncoding, _Input, _Intermediate, _FromErrorHandler, _FromState>;

			_OutputContainer __output {};
			if constexpr (__txt_detail::__is_detected_v<__txt_detail::__detect_adl_size, _Input>) {
				using _SizeType = decltype(__txt_detail::__adl::__adl_size(__input));
				if constexpr (__txt_detail::__is_detected_v<__txt_detail::__detect_reserve_with_size_type,
					              _OutputContainer, _SizeType>) {
					// TODO: better estimates
					__output.reserve(__txt_detail::__adl::__adl_size(__input));
				}
			}
			if constexpr (_IsErrorHandlerCallable) {
				if constexpr (__txt_detail::__is_encode_one_callable_v<_ToEncoding, _Intermediate, _Unbounded,
					              _ToErrorHandler, _ToState>) {
					// We can use the unbounded stuff
					_Unbounded __insert_view(::std::back_inserter(__output));
					auto __stateful_result = transcode_into(::std::forward<_Input>(__input),
						::std::forward<_FromEncoding>(__from_encoding), ::std::move(__insert_view),
						::std::forward<_ToEncoding>(__to_encoding),
						::std::forward<_FromErrorHandler>(__from_error_handler),
						::std::forward<_ToErrorHandler>(__to_error_handler), __from_state, __to_state);
					(void)__stateful_result;
					return __output;
				}
				else {
					auto __stateful_result = __txt_detail::__intermediate_transcode_to_storage(
						::std::forward<_Input>(__input), ::std::forward<_FromEncoding>(__from_encoding), __output,
						::std::forward<_ToEncoding>(__to_encoding),
						::std::forward<_FromErrorHandler>(__from_error_handler),
						::std::forward<_ToErrorHandler>(__to_error_handler), __from_state, __to_state);
					(void)__stateful_result;
					return __output;
				}
			}
			else {
				auto __stateful_result = __txt_detail::__intermediate_transcode_to_storage(
					::std::forward<_Input>(__input), ::std::forward<_FromEncoding>(__from_encoding), __output,
					::std::forward<_ToEncoding>(__to_encoding),
					::std::forward<_FromErrorHandler>(__from_error_handler),
					::std::forward<_ToErrorHandler>(__to_error_handler), __from_state, __to_state);
				(void)__stateful_result;
				return __output;
			}
		}
	} // namespace __txt_detail

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
	/// @returns A ztd::text::transcode_result object that contains references to @p __from_state and @p
	/// __to_state and an @c output parameter that contains the @p _OutputContainer specified. If the container has a
	/// @c container.reserve function, it is and some multiple of the input's size is used to pre-size the container,
	/// to aid with @c push_back / @c insert reallocation pains.
	//////
	template <typename _OutputContainer, typename _Input, typename _FromEncoding, typename _ToEncoding,
		typename _FromErrorHandler, typename _ToErrorHandler, typename _FromState, typename _ToState>
	constexpr auto transcode_to(_Input&& __input, _FromEncoding&& __from_encoding, _ToEncoding&& __to_encoding,
		_FromErrorHandler&& __from_error_handler, _ToErrorHandler&& __to_error_handler, _FromState& __from_state,
		_ToState& __to_state) {

		_OutputContainer __output {};
		if constexpr (__txt_detail::__is_detected_v<__txt_detail::__detect_adl_size, _Input>) {
			using _SizeType = decltype(__txt_detail::__adl::__adl_size(__input));
			if constexpr (__txt_detail::__is_detected_v<__txt_detail::__detect_reserve_with_size_type,
				              _OutputContainer, _SizeType>) {
				// TODO: better estimates
				__output.reserve(__txt_detail::__adl::__adl_size(__input));
			}
		}

		auto __insert_view     = unbounded_view(::std::back_inserter(__output));
		auto __stateful_result = transcode_into(::std::forward<_Input>(__input),
			::std::forward<_FromEncoding>(__from_encoding), ::std::move(__insert_view),
			::std::forward<_ToEncoding>(__to_encoding), ::std::forward<_FromErrorHandler>(__from_error_handler),
			::std::forward<_ToErrorHandler>(__to_error_handler), __from_state, __to_state);
		return __txt_detail::__replace_result_output(::std::move(__stateful_result), ::std::move(__output));
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
	/// @returns A ztd::text::stateless_transcode_result object that contains references to an @c container.output
	/// parameter that contains the @p _OutputContainer specified.
	///
	/// @remarks A default state for the encode step of the operation is create using ztd::text::make_encode_state. The
	/// return type is stateless since both states must be passed in. If you want to have access to the states, create
	/// both of them yourself and pass them into a lower-level function that accepts those parameters.
	//////
	template <typename _OutputContainer, typename _Input, typename _FromEncoding, typename _ToEncoding,
		typename _FromErrorHandler, typename _ToErrorHandler, typename _FromState>
	constexpr auto transcode_to(_Input&& __input, _FromEncoding&& __from_encoding, _ToEncoding&& __to_encoding,
		_FromErrorHandler&& __from_error_handler, _ToErrorHandler&& __to_error_handler, _FromState& __from_state) {
		using _UToEncoding = __txt_detail::__remove_cvref_t<_ToEncoding>;
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
	/// @returns A ztd::text::stateless_transcode_result object that contains references to an @c container.output
	/// parameter that contains the @p _OutputContainer specified.
	///
	/// @remarks A default state for the decode step of the operation is create using ztd::text::make_decode_state. The
	/// return type is stateless since both states must be passed in. If you want to have access to the states, create
	/// both of them yourself and pass them into a lower-level function that accepts those parameters.
	//////
	template <typename _OutputContainer, typename _Input, typename _FromEncoding, typename _ToEncoding,
		typename _FromErrorHandler, typename _ToErrorHandler>
	constexpr auto transcode_to(_Input&& __input, _FromEncoding&& __from_encoding, _ToEncoding&& __to_encoding,
		_FromErrorHandler&& __from_error_handler, _ToErrorHandler&& __to_error_handler) {
		using _UFromEncoding = __txt_detail::__remove_cvref_t<_FromEncoding>;
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
	/// @returns A ztd::text::stateless_transcode_result object that contains references to an @c container.output
	/// parameter that contains the @p _OutputContainer specified.
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
		auto __handler = __txt_detail::__duplicate_or_be_careless(__from_error_handler);

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
	/// @returns A ztd::text::stateless_transcode_result object that contains references to an @c container.output
	/// parameter that contains the @p _OutputContainer specified.
	///
	/// @remarks A @c from_error_handler for the encode step of the operation is created using default construction of
	/// a ztd::text::default_handler that is marked as careless. The return type is stateless since both states must be
	/// passed in. If you want to have access to the states, create both of them yourself and pass them into a
	/// lower-level function that accepts those parameters.
	//////
	template <typename _OutputContainer, typename _Input, typename _FromEncoding, typename _ToEncoding>
	constexpr auto transcode_to(_Input&& __input, _FromEncoding&& __from_encoding, _ToEncoding&& __to_encoding) {
		default_handler __handler {};

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
	/// @returns A ztd::text::stateless_transcode_result object that contains references to an @c container.output
	/// parameter that contains the @p _OutputContainer specified.
	///
	/// @remarks A @c from_error_handler for the encode step of the operation is created using default construction of
	/// a ztd::text::default_handler that is marked as careless. The return type is stateless since both states must be
	/// passed in. If you want to have access to the states, create both of them yourself and pass them into a
	/// lower-level function that accepts those parameters.
	//////
	template <typename _OutputContainer, typename _Input, typename _ToEncoding>
	constexpr auto transcode_to(_Input&& __input, _ToEncoding&& __to_encoding) {
		using _UInput   = __txt_detail::__remove_cvref_t<_Input>;
		using _CodeUnit = __txt_detail::__range_value_type_t<_UInput>;
#if ZTD_TEXT_IS_ON(ZTD_TEXT_STD_LIBRARY_IS_CONSTANT_EVALUATED_I_)
		if (::std::is_constant_evaluated()) {
			using _UFromEncoding = default_consteval_code_unit_encoding_t<_CodeUnit>;
			default_handler __handler {};
			_UFromEncoding __from_encoding {};
			return transcode_to<_OutputContainer>(::std::forward<_Input>(__input), __from_encoding,
				::std::forward<_ToEncoding>(__to_encoding), __handler);
		}
		else
#endif
		{
			using _UFromEncoding = default_code_unit_encoding_t<_CodeUnit>;
			default_handler __handler {};
			_UFromEncoding __from_encoding {};
			return transcode_to<_OutputContainer>(::std::forward<_Input>(__input), __from_encoding,
				::std::forward<_ToEncoding>(__to_encoding), __handler);
		}
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
	template <typename _OutputContainer = void, typename _Input, typename _FromEncoding, typename _ToEncoding,
		typename _FromErrorHandler, typename _ToErrorHandler, typename _FromState, typename _ToState>
	constexpr auto transcode(_Input&& __input, _FromEncoding&& __from_encoding, _ToEncoding&& __to_encoding,
		_FromErrorHandler&& __from_error_handler, _ToErrorHandler&& __to_error_handler, _FromState& __from_state,
		_ToState& __to_state) {
		using _UToEncoding              = __txt_detail::__remove_cvref_t<_ToEncoding>;
		using _UOutputContainer         = __txt_detail::__remove_cvref_t<_OutputContainer>;
		using _OutputCodeUnit           = code_unit_t<_UToEncoding>;
		constexpr bool _IsVoidContainer = ::std::is_void_v<_UOutputContainer>;
		constexpr bool _IsStringable
			= (__txt_detail::__is_character_v<_OutputCodeUnit> || is_unicode_code_point_v<_OutputCodeUnit>);
		if constexpr (_IsVoidContainer && _IsStringable) {
			// prevent instantiation errors with basic_string by boxing it inside of an "if constexpr"
			using _RealOutputContainer = ::std::basic_string<_OutputCodeUnit>;
			return __txt_detail::__transcode_dispatch<_RealOutputContainer>(::std::forward<_Input>(__input),
				::std::forward<_FromEncoding>(__from_encoding), ::std::forward<_ToEncoding>(__to_encoding),
				::std::forward<_FromErrorHandler>(__from_error_handler),
				::std::forward<_ToErrorHandler>(__to_error_handler), __from_state, __to_state);
		}
		else {
			using _RealOutputContainer
				= ::std::conditional_t<_IsVoidContainer, ::std::vector<_OutputCodeUnit>, _OutputContainer>;
			return __txt_detail::__transcode_dispatch<_RealOutputContainer>(::std::forward<_Input>(__input),
				::std::forward<_FromEncoding>(__from_encoding), ::std::forward<_ToEncoding>(__to_encoding),
				::std::forward<_FromErrorHandler>(__from_error_handler),
				::std::forward<_ToErrorHandler>(__to_error_handler), __from_state, __to_state);
		}
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
	template <typename _OutputContainer = void, typename _Input, typename _FromEncoding, typename _ToEncoding,
		typename _FromErrorHandler, typename _ToErrorHandler, typename _FromState>
	constexpr auto transcode(_Input&& __input, _FromEncoding&& __from_encoding, _ToEncoding&& __to_encoding,
		_FromErrorHandler&& __from_error_handler, _ToErrorHandler&& __to_error_handler, _FromState& __from_state) {
		using _UToEncoding = __txt_detail::__remove_cvref_t<_ToEncoding>;
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
	template <typename _OutputContainer = void, typename _Input, typename _FromEncoding, typename _ToEncoding,
		typename _FromErrorHandler, typename _ToErrorHandler>
	constexpr auto transcode(_Input&& __input, _FromEncoding&& __from_encoding, _ToEncoding&& __to_encoding,
		_FromErrorHandler&& __from_error_handler, _ToErrorHandler&& __to_error_handler) {
		using _UFromEncoding = __txt_detail::__remove_cvref_t<_FromEncoding>;
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
	template <typename _OutputContainer = void, typename _Input, typename _FromEncoding, typename _ToEncoding,
		typename _FromErrorHandler>
	constexpr auto transcode(_Input&& __input, _FromEncoding&& __from_encoding, _ToEncoding&& __to_encoding,
		_FromErrorHandler&& __from_error_handler) {
		auto __handler = __txt_detail::__duplicate_or_be_careless(__from_error_handler);

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
	template <typename _OutputContainer = void, typename _Input, typename _FromEncoding, typename _ToEncoding>
	constexpr auto transcode(_Input&& __input, _FromEncoding&& __from_encoding, _ToEncoding&& __to_encoding) {
		default_handler __handler {};

		return transcode<_OutputContainer>(::std::forward<_Input>(__input),
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
	/// @returns An @p _OutputContainer with the result, regardless of whether an error occurs or not. If you are
	/// looking for error information and not just a quick one-off conversion function, please use
	/// ztd::text::transcode_to or ztd::text::transcode_into.
	///
	/// @remarks This function creates both: a @c from_error_handler from a class like ztd::text::default_handler, but
	/// that is marked as careless since you did not explicitly provide it; and, a @c from_encoding derived from the @p
	/// "__input"'s @c value_type. The careless marking matters for lossy conversions that are not injective.
	//////
	template <typename _OutputContainer = void, typename _Input, typename _ToEncoding>
	constexpr auto transcode(_Input&& __input, _ToEncoding&& __to_encoding) {
		using _UInput   = __txt_detail::__remove_cvref_t<_Input>;
		using _CodeUnit = __txt_detail::__range_value_type_t<_UInput>;
#if ZTD_TEXT_IS_ON(ZTD_TEXT_STD_LIBRARY_IS_CONSTANT_EVALUATED_I_)
		if (::std::is_constant_evaluated()) {
			using _UFromEncoding = default_consteval_code_unit_encoding_t<_CodeUnit>;
			_UFromEncoding __from_encoding {};
			return transcode<_OutputContainer>(
				::std::forward<_Input>(__input), __from_encoding, ::std::forward<_ToEncoding>(__to_encoding));
		}
		else
#endif
		{
			using _UFromEncoding = default_code_unit_encoding_t<_CodeUnit>;

			_UFromEncoding __from_encoding {};
			return transcode<_OutputContainer>(
				::std::forward<_Input>(__input), __from_encoding, ::std::forward<_ToEncoding>(__to_encoding));
		}
	}

	//////
	/// @}
	//////

	ZTD_TEXT_INLINE_ABI_NAMESPACE_CLOSE_I_
}} // namespace ztd::text

#include <ztd/text/detail/epilogue.hpp>

#endif // ZTD_TEXT_TRANSCODE_HPP
