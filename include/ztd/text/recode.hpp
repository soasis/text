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

#ifndef ZTD_TEXT_RECODE_HPP
#define ZTD_TEXT_RECODE_HPP

#include <ztd/text/version.hpp>

#include <ztd/text/code_point.hpp>
#include <ztd/text/code_unit.hpp>
#include <ztd/text/max_units.hpp>
#include <ztd/text/error_handler.hpp>
#include <ztd/text/state.hpp>
#include <ztd/text/default_encoding.hpp>
#include <ztd/text/recode_result.hpp>
#include <ztd/text/is_unicode_code_point.hpp>
#include <ztd/text/recode_one.hpp>
#include <ztd/text/pivot.hpp>
#include <ztd/text/detail/is_lossless.hpp>
#include <ztd/text/detail/encoding_range.hpp>
#include <ztd/text/detail/recode_extension_points.hpp>
#include <ztd/text/detail/span_reconstruct.hpp>
#include <ztd/text/detail/forward_if_move_only.hpp>

#include <ztd/idk/tag.hpp>
#include <ztd/idk/span.hpp>
#include <ztd/idk/type_traits.hpp>
#include <ztd/idk/char_traits.hpp>
#include <ztd/ranges/unbounded.hpp>
#include <ztd/ranges/detail/insert_bulk.hpp>

#include <string>
#include <vector>

#include <ztd/prologue.hpp>

namespace ztd { namespace text {
	ZTD_TEXT_INLINE_ABI_NAMESPACE_OPEN_I_

	//////
	/// @addtogroup ztd_text_recode ztd::text::recode[_into/_to]
	///
	/// @brief These functions convert from a view of input code units into a view of output code units (typically,
	/// through an intermediary code point type that is similar between the two) using either the inferred or specified
	/// encodings. If no error handler is provided, the equivalent of the ztd::text::default_handler_t is used by
	/// default, but it is marked as careless. If no associated state is provided for either the "to" or "from"
	/// encodings, one will be created with automatic storage duration (as a "stack" variable) for the provided
	/// encoding.
	/// @{

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
	/// @param[in,out] __from_state A reference to the associated state for the `__from_encoding` 's decode step.
	/// @param[in,out] __to_state A reference to the associated state for the `__to_encoding` 's encode step.
	/// @param[in, out] __pivot A reference to a descriptor of a (potentially usable) pivot range, usually a range of
	/// contiguous data from a span provided by the implementation but customizable by the end-user. If the
	/// intermediate conversion is what failed, then the ztd::text::pivot's `error_code` member will be set to that
	/// error. This only happens if the overall operation also fails, and need not be checked unless to obtain
	/// additional information for when a top-level operation fails.
	///
	/// @result A ztd::text::recode_result object that contains references to `__from_state` and @p
	/// __to_state.
	///
	/// @remark This function detects whether or not the ADL extension point `text_recode` can be called with the
	/// provided parameters. If so, it will use that ADL extension point over the default implementation. Otherwise, it
	/// will loop over the two encodings and attempt to recode by first decoding the input code units to code
	/// points, then encoding the intermediate code points to the desired, output code units.
	template <typename _Input, typename _FromEncoding, typename _Output, typename _ToEncoding,
		typename _FromErrorHandler, typename _ToErrorHandler, typename _FromState, typename _ToState,
		typename _PivotRange>
	constexpr auto basic_recode_into_raw(_Input&& __input, _FromEncoding&& __from_encoding, _Output&& __output,
		_ToEncoding&& __to_encoding, _FromErrorHandler&& __from_error_handler, _ToErrorHandler&& __to_error_handler,
		_FromState& __from_state, _ToState& __to_state, pivot<_PivotRange>& __pivot) {
		using _InitialInput      = ::ztd::ranges::subrange_for_t<::std::remove_reference_t<_Input>>;
		using _WorkingOutput     = ::ztd::ranges::subrange_for_t<::std::remove_reference_t<_Output>>;
		using _UFromEncoding     = remove_cvref_t<_FromEncoding>;
		using _UToEncoding       = remove_cvref_t<_ToEncoding>;
		using _UFromErrorHandler = remove_cvref_t<_FromErrorHandler>;
		using _UToErrorHandler   = remove_cvref_t<_ToErrorHandler>;
		using _Result            = decltype(recode_one_into_raw(::std::declval<_InitialInput>(), __from_encoding,
			           ::std::declval<_WorkingOutput>(), __to_encoding, __from_error_handler, __to_error_handler, __from_state,
			           __to_state, __pivot));
		using _WorkingInput      = decltype(::std::declval<_Result>().input);

		static_assert(__txt_detail::__is_decode_lossless_or_deliberate_v<_UFromEncoding, _UFromErrorHandler>,
			ZTD_TEXT_LOSSY_RECODE_DECODE_MESSAGE_I_);
		static_assert(__txt_detail::__is_encode_lossless_or_deliberate_v<_UToEncoding, _UToErrorHandler>,
			ZTD_TEXT_LOSSY_RECODE_ENCODE_MESSAGE_I_);

		_WorkingInput __working_input(::std::forward<_Input>(__input));
		_WorkingOutput __working_output(::std::forward<_Output>(__output));

		::std::size_t __error_count = 0;
		for (;;) {
			auto __recode_result
				= recode_one_into_raw(::std::move(__working_input), __from_encoding, ::std::move(__working_output),
				     __to_encoding, __from_error_handler, __to_error_handler, __from_state, __to_state, __pivot);
			if (__recode_result.error_code != encoding_error::ok) {
				return _Result(::std::move(__working_input), ::std::move(__working_output), __from_state,
					__to_state, __recode_result.error_code, __recode_result.error_count);
			}
			__error_count += __recode_result.error_count;
			__working_input  = ::std::move(__recode_result.input);
			__working_output = ::std::move(__recode_result.output);
			if (::ztd::ranges::empty(__working_input)) {
				if (!::ztd::text::is_state_complete(__from_encoding, __from_state)) {
					continue;
				}
				if (!::ztd::text::is_state_complete(__to_encoding, __to_state)) {
					continue;
				}
				break;
			}
		}
		return _Result(::std::move(__working_input), ::std::move(__working_output), __from_state, __to_state,
			encoding_error::ok, __error_count);
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
	/// @param[in,out] __from_state A reference to the associated state for the `__from_encoding` 's decode step.
	/// @param[in,out] __to_state A reference to the associated state for the `__to_encoding` 's encode step.
	/// @param[in, out] __pivot A reference to a descriptor of a (potentially usable) pivot range, usually a range of
	/// contiguous data from a span provided by the implementation but customizable by the end-user. If the
	/// intermediate conversion is what failed, then the ztd::text::pivot's `error_code` member will be set to that
	/// error. This only happens if the overall operation also fails, and need not be checked unless to obtain
	/// additional information for when a top-level operation fails.
	///
	/// @result A ztd::text::recode_result object that contains references to `__from_state` and @p
	/// __to_state.
	///
	/// @remark This function detects whether or not the ADL extension point `text_recode` can be called with the
	/// provided parameters. If so, it will use that ADL extension point over the default implementation. Otherwise, it
	/// will loop over the two encodings and attempt to recode by first decoding the input code units to code
	/// points, then encoding the intermediate code points to the desired, output code units.
	template <typename _Input, typename _FromEncoding, typename _Output, typename _ToEncoding,
		typename _FromErrorHandler, typename _ToErrorHandler, typename _FromState, typename _ToState,
		typename _PivotRange>
	constexpr auto recode_into_raw(_Input&& __input, _FromEncoding&& __from_encoding, _Output&& __output,
		_ToEncoding&& __to_encoding, _FromErrorHandler&& __from_error_handler, _ToErrorHandler&& __to_error_handler,
		_FromState& __from_state, _ToState& __to_state, pivot<_PivotRange>& __pivot) {
		if constexpr (is_detected_v<__txt_detail::__detect_adl_text_recode, _Input, _FromEncoding, _Output,
			              _ToEncoding, _FromErrorHandler, _ToErrorHandler, _FromState, _ToState, _PivotRange>) {
			return text_recode(::ztd::tag<remove_cvref_t<_FromEncoding>, remove_cvref_t<_ToEncoding>> {},
				::std::forward<_Input>(__input), ::std::forward<_FromEncoding>(__from_encoding),
				::std::forward<_Output>(__output), ::std::forward<_ToEncoding>(__to_encoding),
				::std::forward<_FromErrorHandler>(__from_error_handler),
				::std::forward<_ToErrorHandler>(__to_error_handler), __from_state, __to_state, __pivot);
		}
		else {
			using _UFromEncoding     = ::ztd::remove_cvref_t<_FromEncoding>;
			using _UToEncoding       = ::ztd::remove_cvref_t<_ToEncoding>;
			using _UFromErrorHandler = ::ztd::remove_cvref_t<_FromErrorHandler>;
			using _UToErrorHandler   = ::ztd::remove_cvref_t<_ToErrorHandler>;
			if constexpr (is_encode_redundant_v<_UFromEncoding, _UToEncoding> // cf
				&& is_decode_redundant_v<_UFromEncoding, _UToEncoding>       // cf
				&& is_ignorable_error_handler_v<_UFromErrorHandler>          // cf
				&& is_ignorable_error_handler_v<_UToErrorHandler>) {
				// we can simply copy from the input to the output, no questions asked!
				(void)__from_encoding;
				(void)__to_encoding;
				(void)__from_error_handler;
				(void)__to_error_handler;
				(void)__from_state;
				(void)__to_state;
				(void)__pivot;
				auto __result = ::ztd::ranges::__rng_detail::__copy(
					::ztd::ranges::cbegin(::std::forward<_Input>(__input)), ::ztd::ranges::cend(__input),
					::ztd::ranges::begin(::std::forward<_Output>(__output)), ::ztd::ranges::end(__output));
				using _Result
					= recode_result<decltype(__result.input), decltype(__result.output), _FromState, _ToState>;
				return _Result(::std::move(__result.input), __result.out, __from_state, __to_state);
			}
			else if constexpr (__txt_detail::__is_encode_same_as_decode_v<_UFromEncoding, _UToEncoding>) {
				// We can skip one of the steps. This tends to be the case for e.g.
				// UTF-16 to UTF-32 recoding conversions, where decoding from UTF-16 does not need a further
				// filtering through UTF-32. For example, the decoded ID for ztd::text::utf16_t is
				// ztd::text_encoding_id::utf32, and the encoded ID for ztd::text::utf32_t is
				// ztd::text_encoding_id::utf32. So, this path should trigger.
				(void)__to_encoding;
				(void)__to_error_handler;
				(void)__pivot;
				auto __result = encode_into_raw(::std::forward<_Input>(__input),
					::std::forward<_FromEncoding>(__from_encoding), ::std::forward<_Output>(__output),
					::std::forward<_FromErrorHandler>(__from_error_handler), __from_state);
				using _Result
					= recode_result<decltype(__result.input), decltype(__result.output), _FromState, _ToState>;
				return _Result(::std::move(__result.input), ::std::move(__result.output), __from_state, __to_state,
					__result.error_code, __result.error_count);
			}
			else if constexpr (__txt_detail::__is_already_encoded_v<_UFromEncoding, _UToEncoding>) {
				// We can skip one of the steps. This tends to be the case for e.g.
				// UTF-16 to UTF-32 recoding conversions, where decoding from UTF-16 does not need a further
				// filtering through UTF-32. For example, the decoded ID for ztd::text::utf16_t is
				// ztd::text_encoding_id::utf32, and the encoded ID for ztd::text::utf32_t is
				// ztd::text_encoding_id::utf32. So, this path should trigger.
				(void)__from_encoding;
				(void)__from_error_handler;
				(void)__pivot;
				auto __result = decode_into_raw(::std::forward<_Input>(__input),
					::std::forward<_ToEncoding>(__to_encoding), ::std::forward<_Output>(__output),
					::std::forward<_FromErrorHandler>(__to_error_handler), __to_state);
				using _Result
					= recode_result<decltype(__result.input), decltype(__result.output), _FromState, _ToState>;
				return _Result(::std::move(__result.input), ::std::move(__result.output), __from_state, __to_state,
					__result.error_code, __result.error_count);
			}
			else if constexpr (is_detected_v<__txt_detail::__detect_adl_internal_text_recode_one, _Input,
				                   _FromEncoding, _Output, _ToEncoding, _FromErrorHandler, _ToErrorHandler,
				                   _FromState, _ToState, _PivotRange>) {
				return __text_recode(
					::ztd::tag<::ztd::remove_cvref_t<_FromEncoding>, ::ztd::remove_cvref_t<_ToEncoding>> {},
					::std::forward<_Input>(__input), ::std::forward<_FromEncoding>(__from_encoding),
					::std::forward<_Output>(__output), ::std::forward<_ToEncoding>(__to_encoding),
					::std::forward<_FromErrorHandler>(__from_error_handler),
					::std::forward<_ToErrorHandler>(__to_error_handler), __from_state, __to_state, __pivot);
			}
			else {
				return basic_recode_into_raw(::std::forward<_Input>(__input),
					::std::forward<_FromEncoding>(__from_encoding), ::std::forward<_Output>(__output),
					::std::forward<_ToEncoding>(__to_encoding),
					::std::forward<_FromErrorHandler>(__from_error_handler),
					::std::forward<_ToErrorHandler>(__to_error_handler), __from_state, __to_state, __pivot);
			}
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
	/// @param[in,out] __from_state A reference to the associated state for the `__from_encoding` 's decode step.
	/// @param[in,out] __to_state A reference to the associated state for the `__to_encoding` 's encode step.
	///
	/// @result A ztd::text::recode_result object that contains references to `__from_state` and @p
	/// __to_state.
	///
	/// @remark This function detects whether or not the ADL extension point `text_recode` can be called with the
	/// provided parameters. If so, it will use that ADL extension point over the default implementation. Otherwise, it
	/// will loop over the two encodings and attempt to recode by first decoding the input code units to code
	/// points, then encoding the intermediate code points to the desired, output code units.
	template <typename _Input, typename _FromEncoding, typename _Output, typename _ToEncoding,
		typename _FromErrorHandler, typename _ToErrorHandler, typename _FromState, typename _ToState>
	constexpr auto recode_into_raw(_Input&& __input, _FromEncoding&& __from_encoding, _Output&& __output,
		_ToEncoding&& __to_encoding, _FromErrorHandler&& __from_error_handler, _ToErrorHandler&& __to_error_handler,
		_FromState& __from_state, _ToState& __to_state) {
		using _UFromEncoding    = ::ztd::remove_cvref_t<_FromEncoding>;
		using _CodeUnit         = code_unit_t<_UFromEncoding>;
		using _IntermediateSpan = ::ztd::span<_CodeUnit>;

		constexpr ::std::size_t __intermediate_buffer_max
			= ZTD_TEXT_INTERMEDIATE_RECODE_BUFFER_SIZE_I_(code_unit_t<_UFromEncoding>)
			     < max_code_units_v<_UFromEncoding>
			? max_code_units_v<_UFromEncoding>
			: ZTD_TEXT_INTERMEDIATE_RECODE_BUFFER_SIZE_I_(code_unit_t<_UFromEncoding>);

		_CodeUnit __intermediate[__intermediate_buffer_max] {};
		pivot<_IntermediateSpan> __pivot { _IntermediateSpan(__intermediate), encoding_error::ok };
		return recode_into_raw(::std::forward<_Input>(__input), ::std::forward<_FromEncoding>(__from_encoding),
			::std::forward<_Output>(__output), ::std::forward<_ToEncoding>(__to_encoding),
			::std::forward<_FromErrorHandler>(__from_error_handler),
			::std::forward<_ToErrorHandler>(__to_error_handler), __from_state, __to_state, __pivot);
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
	/// @param[in]     __from_error_handler The error handler for the `__from_encoding` 's decode step.
	/// @param[in]     __to_error_handler The error handler for the `__to_encoding` 's encode step.
	/// @param[in,out] __from_state A reference to the associated state for the `__from_encoding` 's decode step.
	///
	/// @result A ztd::text::stateless_recode_result object.
	///
	/// @remarks This function calls the base reference, the ztd::text::recode_into_raw after creating a `to_state`
	/// from ztd::text::make_encode_state. The result from this function returns a
	/// ztd::text::stateless_recode_result as opposed to a ztd::text::recode_result because the state
	/// information is on the stack, and returning the state in those types by reference will result in references to
	/// memory that has already been cleaned up. If you need access to the state parameters, call the lower-level
	/// functionality with your own created states.
	template <typename _Input, typename _FromEncoding, typename _Output, typename _ToEncoding,
		typename _FromErrorHandler, typename _ToErrorHandler, typename _FromState>
	constexpr auto recode_into_raw(_Input&& __input, _FromEncoding&& __from_encoding, _Output&& __output,
		_ToEncoding&& __to_encoding, _FromErrorHandler&& __from_error_handler, _ToErrorHandler&& __to_error_handler,
		_FromState& __from_state) {
		using _UToEncoding = remove_cvref_t<_ToEncoding>;
		using _ToState     = decode_state_t<_UToEncoding>;

		_ToState __to_state = make_decode_state(__to_encoding);

		auto __stateful_result = recode_into_raw(::std::forward<_Input>(__input),
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
	/// @param[in]     __from_error_handler The error handler for the `__from_encoding` 's decode step.
	/// @param[in]     __to_error_handler The error handler for the `__to_encoding` 's encode step.
	///
	/// @remarks This function creates a decode state `from_state` by calling ztd::text::make_decode_state. The
	/// result from this function returns a ztd::text::stateless_recode_result as opposed to a
	/// ztd::text::recode_result because the state information is on the stack, and returning the state in
	/// those types by reference will result in references to memory that has already been cleaned up. If you need
	/// access to the state parameters, call the lower-level functionality with your own created states.
	template <typename _Input, typename _FromEncoding, typename _Output, typename _ToEncoding,
		typename _FromErrorHandler, typename _ToErrorHandler>
	constexpr auto recode_into_raw(_Input&& __input, _FromEncoding&& __from_encoding, _Output&& __output,
		_ToEncoding&& __to_encoding, _FromErrorHandler&& __from_error_handler, _ToErrorHandler&& __to_error_handler) {
		using _UFromEncoding = remove_cvref_t<_FromEncoding>;
		using _FromState     = encode_state_t<_UFromEncoding>;

		_FromState __from_state = make_encode_state(__from_encoding);

		return recode_into_raw(::std::forward<_Input>(__input), ::std::forward<_FromEncoding>(__from_encoding),
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
	/// @param[in]     __from_error_handler The error handler for the `__from_encoding` 's decode step.
	///
	/// @remarks This function creates an `to_error_handler` from a class like ztd::text::default_handler_t, but that
	/// is marked as careless since you did not explicitly provide it. This matters for lossy conversions that are not
	/// injective. The result from this function returns a ztd::text::stateless_recode_result as opposed to a
	/// ztd::text::recode_result because the state information is on the stack, and returning the state in
	/// those types by reference will result in references to memory that has already been cleaned up. If you need
	/// access to the state parameters, call the lower-level functionality with your own created states.
	template <typename _Input, typename _FromEncoding, typename _Output, typename _ToEncoding,
		typename _FromErrorHandler>
	constexpr auto recode_into_raw(_Input&& __input, _FromEncoding&& __from_encoding, _Output&& __output,
		_ToEncoding&& __to_encoding, _FromErrorHandler&& __from_error_handler) {
		auto __handler = __txt_detail::__duplicate_or_be_careless(__from_error_handler);

		return recode_into_raw(::std::forward<_Input>(__input), ::std::forward<_FromEncoding>(__from_encoding),
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
	/// @remarks This function creates an `from_error_handler` from a class like ztd::text::default_handler_t, but
	/// that is marked as careless since you did not explicitly provide it. This matters for lossy conversions that are
	/// not injective. The result from this function returns a ztd::text::stateless_recode_result as opposed to a
	/// ztd::text::recode_result because the state information is on the stack, and returning the state in
	/// those types by reference will result in references to memory that has already been cleaned up. If you need
	/// access to the state parameters, call the lower-level functionality with your own created states.
	template <typename _Input, typename _FromEncoding, typename _Output, typename _ToEncoding>
	constexpr auto recode_into_raw(
		_Input&& __input, _FromEncoding&& __from_encoding, _Output&& __output, _ToEncoding&& __to_encoding) {
		default_handler_t __handler {};

		return recode_into_raw(::std::forward<_Input>(__input), ::std::forward<_FromEncoding>(__from_encoding),
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
	/// @remarks This function creates both: a `from_error_handler` using a ztd::text::default_handler_t that is
	/// marked as careless to pass to the next function overload; and, a `from_encoding` to interpret the `__input`
	/// by checking the `__input` 's `value_type.` This matters for lossy conversions that are not injective. The
	/// result from this function returns a ztd::text::stateless_recode_result as opposed to a
	/// ztd::text::recode_result because the state information is on the stack, and returning the state in
	/// those types by reference will result in references to memory that has already been cleaned up. If you need
	/// access to the state parameters, call the lower-level functionality with your own created states.
	template <typename _Input, typename _ToEncoding, typename _Output>
	constexpr auto recode_into_raw(_Input&& __input, _ToEncoding&& __to_encoding, _Output&& __output) {
		using _UInput        = remove_cvref_t<_Input>;
		using _UFromEncoding = default_code_unit_encoding_t<ranges::range_value_type_t<_UInput>>;

		_UFromEncoding __from_encoding {};
		default_handler_t __handler {};

		return recode_into_raw(::std::forward<_Input>(__input), __from_encoding, ::std::forward<_Output>(__output),
			::std::forward<_ToEncoding>(__to_encoding), __handler);
	}

	namespace __txt_detail {
		template <typename _Input, typename _FromEncoding, typename _OutputContainer, typename _ToEncoding,
			typename _FromErrorHandler, typename _ToErrorHandler, typename _FromState, typename _ToState,
			typename _PivotRange>
		constexpr auto __intermediate_recode_to_storage(_Input&& __input, _FromEncoding&& __from_encoding,
			_OutputContainer& __output, _ToEncoding&& __to_encoding, _FromErrorHandler&& __from_error_handler,
			_ToErrorHandler&& __to_error_handler, _FromState& __from_state, _ToState& __to_state,
			pivot<_PivotRange>& __pivot) {
			// … Weeeellll. Here we go …
			using _UToEncoding                 = remove_cvref_t<_ToEncoding>;
			using _UFromEncoding               = remove_cvref_t<_FromEncoding>;
			using _IntermediateOutputValueType = code_unit_t<_UToEncoding>;
			constexpr ::std::size_t _MinimumIntermediateOutputMax
				= max_recode_code_points_v<_UFromEncoding, _UToEncoding>;
			constexpr ::std::size_t _IntermediateOutputMax
				= ZTD_TEXT_INTERMEDIATE_RECODE_BUFFER_SIZE_I_(_IntermediateOutputValueType)
				     < _MinimumIntermediateOutputMax
				? _MinimumIntermediateOutputMax
				: ZTD_TEXT_INTERMEDIATE_RECODE_BUFFER_SIZE_I_(_IntermediateOutputValueType);
			using _InitialInput              = __span_reconstruct_t<_Input, _Input>;
			using _IntermediateOutputInitial = ::ztd::span<_IntermediateOutputValueType, _IntermediateOutputMax>;
			using _IntermediateOutput        = ::ztd::span<_IntermediateOutputValueType>;
			using _RecodeResult = decltype(recode_into_raw(::std::declval<_InitialInput>(), __from_encoding,
				::std::declval<_IntermediateOutput&>(), __to_encoding, __from_error_handler, __to_error_handler,
				__from_state, __to_state, __pivot));
			using _WorkingInput = decltype(::std::declval<_RecodeResult>().input);

			static_assert(__txt_detail::__is_decode_lossless_or_deliberate_v<remove_cvref_t<_FromEncoding>,
				              remove_cvref_t<_FromErrorHandler>>,
				ZTD_TEXT_LOSSY_RECODE_DECODE_MESSAGE_I_);
			static_assert(__txt_detail::__is_encode_lossless_or_deliberate_v<remove_cvref_t<_ToEncoding>,
				              remove_cvref_t<_ToErrorHandler>>,
				ZTD_TEXT_LOSSY_RECODE_ENCODE_MESSAGE_I_);


			_WorkingInput __working_input(__txt_detail::__span_reconstruct<_Input>(::std::forward<_Input>(__input)));
			_IntermediateOutputValueType __intermediate_output_storage[_IntermediateOutputMax] {};
			_IntermediateOutputInitial __intermediate_initial(__intermediate_output_storage);
			for (;;) {
				auto __result = recode_into_raw(::std::move(__working_input), __from_encoding,
					__intermediate_initial, __to_encoding, __from_error_handler, __to_error_handler, __from_state,
					__to_state, __pivot);
				_IntermediateOutput __intermediate_output(
					__intermediate_initial.data(), __result.output.data() - __intermediate_initial.data());
				ranges::__rng_detail::__container_insert_bulk(__output, __intermediate_output);
				if (__result.error_code == encoding_error::insufficient_output_space) {
					// loop around, we've got S P A C E for more
					__working_input = ::std::move(__result.input);
					continue;
				}
				if (__result.error_code != encoding_error::ok) {
					return __result;
				}
				if (::ztd::ranges::empty(__result.input)) {
					if (!::ztd::text::is_state_complete(__from_encoding, __from_state)) {
						__working_input = ::std::move(__result.input);
						continue;
					}
					if (!::ztd::text::is_state_complete(__to_encoding, __to_state)) {
						__working_input = ::std::move(__result.input);
						continue;
					}
					return __result;
				}
				__working_input = ::std::move(__result.input);
			}
		}

		template <bool _OutputOnly, bool _NoState, typename _OutputContainer, typename _Input, typename _FromEncoding,
			typename _ToEncoding, typename _FromErrorHandler, typename _ToErrorHandler, typename _FromState,
			typename _ToState, typename _PivotRange>
		constexpr auto __recode_dispatch(_Input&& __input, _FromEncoding&& __from_encoding,
			_ToEncoding&& __to_encoding, _FromErrorHandler&& __from_error_handler,
			_ToErrorHandler&& __to_error_handler, _FromState& __from_state, _ToState& __to_state,
			pivot<_PivotRange>& __pivot) {

			_OutputContainer __output {};
			if constexpr (is_detected_v<ranges::detect_adl_size, _Input>) {
				using _SizeType = decltype(::ztd::ranges::size(__input));
				if constexpr (is_detected_v<ranges::detect_reserve_with_size, _OutputContainer, _SizeType>) {
					_SizeType __output_size_hint = static_cast<_SizeType>(::ztd::ranges::size(__input));
					__output.reserve(__output_size_hint);
				}
			}
			auto __stateful_result = __txt_detail::__intermediate_recode_to_storage(::std::forward<_Input>(__input),
				::std::forward<_FromEncoding>(__from_encoding), __output,
				::std::forward<_ToEncoding>(__to_encoding), ::std::forward<_FromErrorHandler>(__from_error_handler),
				::std::forward<_ToErrorHandler>(__to_error_handler), __from_state, __to_state, __pivot);
			if constexpr (_OutputOnly) {
				(void)__stateful_result;
				return __output;
			}
			else if constexpr (_NoState) {
				return __txt_detail::__replace_recode_result_output_no_state(
					::std::move(__stateful_result), ::std::move(__output));
			}
			else {
				return __txt_detail::__replace_recode_result_output(
					::std::move(__stateful_result), ::std::move(__output));
			}
		}
	} // namespace __txt_detail

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
	/// @param[in,out] __from_state A reference to the associated state for the `__from_encoding` 's decode step.
	/// @param[in,out] __to_state A reference to the associated state for the `__to_encoding` 's encode step.
	/// @param[in, out] __pivot A reference to a descriptor of a (potentially usable) pivot range, usually a range of
	/// contiguous data from a span provided by the implementation but customizable by the end-user. If the
	/// intermediate conversion is what failed, then the ztd::text::pivot's `error_code` member will be set to that
	/// error. This only happens if the overall operation also fails, and need not be checked unless to obtain
	/// additional information for when a top-level operation fails.
	///
	/// @result A ztd::text::recode_result object that contains references to `__from_state` and @p
	/// __to_state.
	///
	/// @remark This function detects whether or not the ADL extension point `text_recode` can be called with the
	/// provided parameters. If so, it will use that ADL extension point over the default implementation. Otherwise, it
	/// will loop over the two encodings and attempt to recode by first decoding the input code units to code
	/// points, then encoding the intermediate code points to the desired, output code units.
	template <typename _Input, typename _FromEncoding, typename _Output, typename _ToEncoding,
		typename _FromErrorHandler, typename _ToErrorHandler, typename _FromState, typename _ToState,
		typename _PivotRange>
	constexpr auto recode_into(_Input&& __input, _FromEncoding&& __from_encoding, _Output&& __output,
		_ToEncoding&& __to_encoding, _FromErrorHandler&& __from_error_handler, _ToErrorHandler&& __to_error_handler,
		_FromState& __from_state, _ToState& __to_state, pivot<_PivotRange>& __pivot) {
		auto __reconstructed_input       = __txt_detail::__span_reconstruct<_Input>(::std::forward<_Input>(__input));
		auto __result                    = recode_into_raw(::std::move(__reconstructed_input),
			                   ::std::forward<_FromEncoding>(__from_encoding), ::std::forward<_Output>(__output),
			                   ::std::forward<_ToEncoding>(__to_encoding), ::std::forward<_FromErrorHandler>(__from_error_handler),
			                   ::std::forward<_ToErrorHandler>(__to_error_handler), __from_state, __to_state, __pivot);
		using _ReconstructedResultInput  = __txt_detail::__span_reconstruct_t<_Input, _Input>;
		using _ReconstructedResultOutput = __txt_detail::__span_reconstruct_mutable_t<_Output, _Output>;
		return recode_result<_ReconstructedResultInput, _ReconstructedResultOutput, _FromState, _ToState>(
			__txt_detail::__span_reconstruct<_Input>(::std::move(__result.input)),
			__txt_detail::__span_reconstruct_mutable<_Output>(::std::move(__result.output)), __result.from_state,
			__result.to_state, __result.error_code, __result.error_count);
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
	/// @param[in,out] __from_state A reference to the associated state for the `__from_encoding` 's decode step.
	/// @param[in,out] __to_state A reference to the associated state for the `__to_encoding` 's encode step.
	///
	/// @result A ztd::text::recode_result object that contains references to `__from_state` and @p
	/// __to_state.
	///
	/// @remark This function detects whether or not the ADL extension point `text_recode` can be called with the
	/// provided parameters. If so, it will use that ADL extension point over the default implementation. Otherwise, it
	/// will loop over the two encodings and attempt to recode by first decoding the input code units to code
	/// points, then encoding the intermediate code points to the desired, output code units.
	template <typename _Input, typename _FromEncoding, typename _Output, typename _ToEncoding,
		typename _FromErrorHandler, typename _ToErrorHandler, typename _FromState, typename _ToState>
	constexpr auto recode_into(_Input&& __input, _FromEncoding&& __from_encoding, _Output&& __output,
		_ToEncoding&& __to_encoding, _FromErrorHandler&& __from_error_handler, _ToErrorHandler&& __to_error_handler,
		_FromState& __from_state, _ToState& __to_state) {
		using _UFromEncoding    = ::ztd::remove_cvref_t<_FromEncoding>;
		using _CodeUnit         = code_unit_t<_UFromEncoding>;
		using _IntermediateSpan = ::ztd::span<_CodeUnit>;

		constexpr ::std::size_t __intermediate_buffer_max
			= ZTD_TEXT_INTERMEDIATE_RECODE_BUFFER_SIZE_I_(code_unit_t<_UFromEncoding>)
			     < max_code_units_v<_UFromEncoding>
			? max_code_units_v<_UFromEncoding>
			: ZTD_TEXT_INTERMEDIATE_RECODE_BUFFER_SIZE_I_(code_unit_t<_UFromEncoding>);

		_CodeUnit __intermediate[__intermediate_buffer_max] {};
		pivot<_IntermediateSpan> __pivot { _IntermediateSpan(__intermediate), encoding_error::ok };
		return recode_into(::std::forward<_Input>(__input), ::std::forward<_FromEncoding>(__from_encoding),
			::std::forward<_Output>(__output), ::std::forward<_ToEncoding>(__to_encoding),
			::std::forward<_FromErrorHandler>(__from_error_handler),
			::std::forward<_ToErrorHandler>(__to_error_handler), __from_state, __to_state, __pivot);
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
	/// @param[in]     __from_error_handler The error handler for the `__from_encoding` 's decode step.
	/// @param[in]     __to_error_handler The error handler for the `__to_encoding` 's encode step.
	/// @param[in,out] __from_state A reference to the associated state for the `__from_encoding` 's decode step.
	///
	/// @result A ztd::text::stateless_recode_result object.
	///
	/// @remarks This function calls the base reference, the ztd::text::recode_into after creating a `to_state`
	/// from ztd::text::make_encode_state. The result from this function returns a
	/// ztd::text::stateless_recode_result as opposed to a ztd::text::recode_result because the state
	/// information is on the stack, and returning the state in those types by reference will result in references to
	/// memory that has already been cleaned up. If you need access to the state parameters, call the lower-level
	/// functionality with your own created states.
	template <typename _Input, typename _FromEncoding, typename _Output, typename _ToEncoding,
		typename _FromErrorHandler, typename _ToErrorHandler, typename _FromState>
	constexpr auto recode_into(_Input&& __input, _FromEncoding&& __from_encoding, _Output&& __output,
		_ToEncoding&& __to_encoding, _FromErrorHandler&& __from_error_handler, _ToErrorHandler&& __to_error_handler,
		_FromState& __from_state) {
		using _UToEncoding = remove_cvref_t<_ToEncoding>;
		using _ToState     = decode_state_t<_UToEncoding>;

		_ToState __to_state = make_decode_state(__to_encoding);

		auto __stateful_result = recode_into(::std::forward<_Input>(__input),
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
	/// @param[in]     __from_error_handler The error handler for the `__from_encoding` 's decode step.
	/// @param[in]     __to_error_handler The error handler for the `__to_encoding` 's encode step.
	///
	/// @remarks This function creates a decode state `from_state` by calling ztd::text::make_decode_state. The
	/// result from this function returns a ztd::text::stateless_recode_result as opposed to a
	/// ztd::text::recode_result because the state information is on the stack, and returning the state in
	/// those types by reference will result in references to memory that has already been cleaned up. If you need
	/// access to the state parameters, call the lower-level functionality with your own created states.
	template <typename _Input, typename _FromEncoding, typename _Output, typename _ToEncoding,
		typename _FromErrorHandler, typename _ToErrorHandler>
	constexpr auto recode_into(_Input&& __input, _FromEncoding&& __from_encoding, _Output&& __output,
		_ToEncoding&& __to_encoding, _FromErrorHandler&& __from_error_handler, _ToErrorHandler&& __to_error_handler) {
		using _UFromEncoding = remove_cvref_t<_FromEncoding>;
		using _FromState     = encode_state_t<_UFromEncoding>;

		_FromState __from_state = make_encode_state(__from_encoding);

		return recode_into(::std::forward<_Input>(__input), ::std::forward<_FromEncoding>(__from_encoding),
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
	/// @param[in]     __from_error_handler The error handler for the `__from_encoding` 's decode step.
	///
	/// @remarks This function creates an `to_error_handler` from a class like ztd::text::default_handler_t, but that
	/// is marked as careless since you did not explicitly provide it. This matters for lossy conversions that are not
	/// injective. The result from this function returns a ztd::text::stateless_recode_result as opposed to a
	/// ztd::text::recode_result because the state information is on the stack, and returning the state in
	/// those types by reference will result in references to memory that has already been cleaned up. If you need
	/// access to the state parameters, call the lower-level functionality with your own created states.
	template <typename _Input, typename _FromEncoding, typename _Output, typename _ToEncoding,
		typename _FromErrorHandler>
	constexpr auto recode_into(_Input&& __input, _FromEncoding&& __from_encoding, _Output&& __output,
		_ToEncoding&& __to_encoding, _FromErrorHandler&& __from_error_handler) {
		auto __handler = __txt_detail::__duplicate_or_be_careless(__from_error_handler);

		return recode_into(::std::forward<_Input>(__input), ::std::forward<_FromEncoding>(__from_encoding),
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
	/// @remarks This function creates an `from_error_handler` from a class like ztd::text::default_handler_t, but
	/// that is marked as careless since you did not explicitly provide it. This matters for lossy conversions that are
	/// not injective. The result from this function returns a ztd::text::stateless_recode_result as opposed to a
	/// ztd::text::recode_result because the state information is on the stack, and returning the state in
	/// those types by reference will result in references to memory that has already been cleaned up. If you need
	/// access to the state parameters, call the lower-level functionality with your own created states.
	template <typename _Input, typename _FromEncoding, typename _Output, typename _ToEncoding>
	constexpr auto recode_into(
		_Input&& __input, _FromEncoding&& __from_encoding, _Output&& __output, _ToEncoding&& __to_encoding) {
		default_handler_t __handler {};

		return recode_into(::std::forward<_Input>(__input), ::std::forward<_FromEncoding>(__from_encoding),
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
	/// @remarks This function creates both: a `from_error_handler` using a ztd::text::default_handler_t that is
	/// marked as careless to pass to the next function overload; and, a `from_encoding` to interpret the `__input`
	/// by checking the `__input` 's `value_type.` This matters for lossy conversions that are not injective. The
	/// result from this function returns a ztd::text::stateless_recode_result as opposed to a
	/// ztd::text::recode_result because the state information is on the stack, and returning the state in
	/// those types by reference will result in references to memory that has already been cleaned up. If you need
	/// access to the state parameters, call the lower-level functionality with your own created states.
	template <typename _Input, typename _ToEncoding, typename _Output>
	constexpr auto recode_into(_Input&& __input, _ToEncoding&& __to_encoding, _Output&& __output) {
		using _UInput        = remove_cvref_t<_Input>;
		using _UFromEncoding = default_code_unit_encoding_t<ranges::range_value_type_t<_UInput>>;

		_UFromEncoding __from_encoding {};
		default_handler_t __handler {};

		return recode_into(::std::forward<_Input>(__input), __from_encoding, ::std::forward<_Output>(__output),
			::std::forward<_ToEncoding>(__to_encoding), __handler);
	}

	//////
	/// @brief Converts the code units of the given input view through the from encoding to code units of the to
	/// encoding for the output, which is then returned in a result structure with additional information about
	/// success.
	///
	/// @tparam _OutputContainer The container to default-construct and serialize data into. Typically, a @c
	/// std::basic_string or a `std::vector` of some sort.
	///
	/// @param[in]     __input An input_view to read code units from and use in the decode operation that will
	/// produce intermediate code points.
	/// @param[in]     __from_encoding The encoding that will be used to decode the input's code units into
	/// intermediate code points.
	/// @param[in]     __to_encoding The encoding that will be used to encode the intermediate code points into the
	/// final code units.
	/// @param[in]     __from_error_handler The error handler for the `__from_encoding` 's decode step.
	/// @param[in]     __to_error_handler The error handler for the `__to_encoding` 's encode step.
	/// @param[in,out] __from_state A reference to the associated state for the `__from_encoding` 's decode step.
	/// @param[in,out] __to_state A reference to the associated state for the `__to_encoding` 's encode step.
	/// @param[in, out] __pivot A reference to a descriptor of a (potentially usable) pivot range, usually a range of
	/// contiguous data from a span provided by the implementation but customizable by the end-user. If the
	/// intermediate conversion is what failed, then the ztd::text::pivot's `error_code` member will be set to that
	/// error. This only happens if the overall operation also fails, and need not be checked unless to obtain
	/// additional information for when a top-level operation fails.
	///
	/// @returns A ztd::text::recode_result object that contains references to `__from_state` and @p
	/// __to_state and an `output` parameter that contains the `_OutputContainer` specified. If the container has a
	/// `container.reserve` function, it is and some multiple of the input's size is used to pre-size the container,
	/// to aid with `push_back` / `insert` reallocation pains.
	template <typename _OutputContainer = void, typename _Input, typename _FromEncoding, typename _ToEncoding,
		typename _FromErrorHandler, typename _ToErrorHandler, typename _FromState, typename _ToState,
		typename _PivotRange>
	constexpr auto recode_to(_Input&& __input, _FromEncoding&& __from_encoding, _ToEncoding&& __to_encoding,
		_FromErrorHandler&& __from_error_handler, _ToErrorHandler&& __to_error_handler, _FromState& __from_state,
		_ToState& __to_state, pivot<_PivotRange>& __pivot) {
		using _UToEncoding              = remove_cvref_t<_ToEncoding>;
		using _UOutputContainer         = remove_cvref_t<_OutputContainer>;
		using _OutputCodePoint          = code_point_t<_UToEncoding>;
		constexpr bool _IsVoidContainer = ::std::is_void_v<_UOutputContainer>;
		constexpr bool _IsStringable
			= (is_char_traitable_v<_OutputCodePoint> || is_unicode_code_point_v<_OutputCodePoint>);
		if constexpr (_IsVoidContainer && _IsStringable) {
			// prevent instantiation errors with basic_string by boxing it inside of an "if constexpr"
			using _RealOutputContainer = ::std::basic_string<_OutputCodePoint>;
			return __txt_detail::__recode_dispatch<false, false, _RealOutputContainer>(
				::std::forward<_Input>(__input), ::std::forward<_FromEncoding>(__from_encoding),
				::std::forward<_ToEncoding>(__to_encoding), ::std::forward<_FromErrorHandler>(__from_error_handler),
				::std::forward<_ToErrorHandler>(__to_error_handler), __from_state, __to_state, __pivot);
		}
		else {
			using _RealOutputContainer
				= ::std::conditional_t<_IsVoidContainer, ::std::vector<_OutputCodePoint>, _OutputContainer>;
			return __txt_detail::__recode_dispatch<false, false, _RealOutputContainer>(
				::std::forward<_Input>(__input), ::std::forward<_FromEncoding>(__from_encoding),
				::std::forward<_ToEncoding>(__to_encoding), ::std::forward<_FromErrorHandler>(__from_error_handler),
				::std::forward<_ToErrorHandler>(__to_error_handler), __from_state, __to_state, __pivot);
		}
	}

	//////
	/// @brief Converts the code units of the given input view through the from encoding to code units of the to
	/// encoding for the output, which is then returned in a result structure with additional information about
	/// success.
	///
	/// @tparam _OutputContainer The container to default-construct and serialize data into. Typically, a @c
	/// std::basic_string or a `std::vector` of some sort.
	///
	/// @param[in]     __input An input_view to read code units from and use in the decode operation that will
	/// produce intermediate code points.
	/// @param[in]     __from_encoding The encoding that will be used to decode the input's code units into
	/// intermediate code points.
	/// @param[in]     __to_encoding The encoding that will be used to encode the intermediate code points into the
	/// final code units.
	/// @param[in]     __from_error_handler The error handler for the `__from_encoding` 's decode step.
	/// @param[in]     __to_error_handler The error handler for the `__to_encoding` 's encode step.
	/// @param[in,out] __from_state A reference to the associated state for the `__from_encoding` 's decode step.
	/// @param[in,out] __to_state A reference to the associated state for the `__to_encoding` 's encode step.
	///
	/// @returns A ztd::text::recode_result object that contains references to `__from_state` and @p
	/// __to_state and an `output` parameter that contains the `_OutputContainer` specified. If the container has a
	/// `container.reserve` function, it is and some multiple of the input's size is used to pre-size the container,
	/// to aid with `push_back` / `insert` reallocation pains.
	template <typename _OutputContainer = void, typename _Input, typename _FromEncoding, typename _ToEncoding,
		typename _FromErrorHandler, typename _ToErrorHandler, typename _FromState, typename _ToState>
	constexpr auto recode_to(_Input&& __input, _FromEncoding&& __from_encoding, _ToEncoding&& __to_encoding,
		_FromErrorHandler&& __from_error_handler, _ToErrorHandler&& __to_error_handler, _FromState& __from_state,
		_ToState& __to_state) {
		using _UFromEncoding    = ::ztd::remove_cvref_t<_FromEncoding>;
		using _CodeUnit         = code_unit_t<_UFromEncoding>;
		using _IntermediateSpan = ::ztd::span<_CodeUnit>;

		constexpr ::std::size_t __intermediate_buffer_max
			= ZTD_TEXT_INTERMEDIATE_RECODE_BUFFER_SIZE_I_(code_unit_t<_UFromEncoding>)
			     < max_code_units_v<_UFromEncoding>
			? max_code_units_v<_UFromEncoding>
			: ZTD_TEXT_INTERMEDIATE_RECODE_BUFFER_SIZE_I_(code_unit_t<_UFromEncoding>);

		_CodeUnit __intermediate[__intermediate_buffer_max] {};
		pivot<_IntermediateSpan> __pivot { _IntermediateSpan(__intermediate), encoding_error::ok };
		return recode_to<_OutputContainer>(::std::forward<_Input>(__input),
			::std::forward<_FromEncoding>(__from_encoding), ::std::forward<_ToEncoding>(__to_encoding),
			::std::forward<_FromErrorHandler>(__from_error_handler),
			::std::forward<_ToErrorHandler>(__to_error_handler), __from_state, __to_state, __pivot);
	}

	//////
	/// @brief Converts the code units of the given input view through the from encoding to code units of the to
	/// encoding for the output, which is then returned in a result structure with additional information about
	/// success.
	///
	/// @tparam _OutputContainer The container to default-construct and serialize data into. Typically, a @c
	/// std::basic_string or a `std::vector` of some sort.
	///
	/// @param[in]     __input An input_view to read code units from and use in the decode operation that will
	/// produce intermediate code points.
	/// @param[in]     __from_encoding The encoding that will be used to decode the input's code units into
	/// intermediate code points.
	/// @param[in]     __to_encoding The encoding that will be used to encode the intermediate code points into the
	/// final code units.
	/// @param[in]     __from_error_handler The error handler for the `__from_encoding` 's decode step.
	/// @param[in]     __to_error_handler The error handler for the `__to_encoding` 's encode step.
	/// @param[in,out] __from_state A reference to the associated state for the `__from_encoding` 's decode step.
	///
	/// @returns A ztd::text::stateless_recode_result object that contains references to an `container.output`
	/// parameter that contains the `_OutputContainer` specified.
	///
	/// @remarks A default state for the encode step of the operation is create using ztd::text::make_encode_state. The
	/// return type is stateless since both states must be passed in. If you want to have access to the states, create
	/// both of them yourself and pass them into a lower-level function that accepts those parameters.
	template <typename _OutputContainer = void, typename _Input, typename _FromEncoding, typename _ToEncoding,
		typename _FromErrorHandler, typename _ToErrorHandler, typename _FromState>
	constexpr auto recode_to(_Input&& __input, _FromEncoding&& __from_encoding, _ToEncoding&& __to_encoding,
		_FromErrorHandler&& __from_error_handler, _ToErrorHandler&& __to_error_handler, _FromState& __from_state) {
		using _UToEncoding = remove_cvref_t<_ToEncoding>;
		using _ToState     = decode_state_t<_UToEncoding>;

		_ToState __to_state = make_decode_state(__to_encoding);

		return recode_to<_OutputContainer>(::std::forward<_Input>(__input),
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
	/// std::basic_string or a `std::vector` of some sort.
	///
	/// @param[in]     __input An input_view to read code units from and use in the decode operation that will
	/// produce intermediate code points.
	/// @param[in]     __from_encoding The encoding that will be used to decode the input's code units into
	/// intermediate code points.
	/// @param[in]     __to_encoding The encoding that will be used to encode the intermediate code points into the
	/// final code units.
	/// @param[in]     __from_error_handler The error handler for the `__from_encoding` 's decode step.
	/// @param[in]     __to_error_handler The error handler for the `__to_encoding` 's encode step.
	///
	/// @returns A ztd::text::stateless_recode_result object that contains references to an `container.output`
	/// parameter that contains the `_OutputContainer` specified.
	///
	/// @remarks A default state for the decode step of the operation is create using ztd::text::make_decode_state. The
	/// return type is stateless since both states must be passed in. If you want to have access to the states, create
	/// both of them yourself and pass them into a lower-level function that accepts those parameters.
	template <typename _OutputContainer = void, typename _Input, typename _FromEncoding, typename _ToEncoding,
		typename _FromErrorHandler, typename _ToErrorHandler>
	constexpr auto recode_to(_Input&& __input, _FromEncoding&& __from_encoding, _ToEncoding&& __to_encoding,
		_FromErrorHandler&& __from_error_handler, _ToErrorHandler&& __to_error_handler) {
		using _UFromEncoding = remove_cvref_t<_FromEncoding>;
		using _FromState     = encode_state_t<_UFromEncoding>;

		_FromState __from_state = make_encode_state(__from_encoding);

		return recode_to<_OutputContainer>(::std::forward<_Input>(__input),
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
	/// std::basic_string or a `std::vector` of some sort.
	///
	/// @param[in]     __input An input_view to read code units from and use in the decode operation that will
	/// produce intermediate code points.
	/// @param[in]     __from_encoding The encoding that will be used to decode the input's code units into
	/// intermediate code points.
	/// @param[in]     __to_encoding The encoding that will be used to encode the intermediate code points into the
	/// final code units.
	/// @param[in]     __from_error_handler The error handler for the `__from_encoding` 's decode step.
	///
	/// @returns A ztd::text::stateless_recode_result object that contains references to an `container.output`
	/// parameter that contains the `_OutputContainer` specified.
	///
	/// @remarks A `to_error_handler` for the encode step of the operation is created using default construction of a
	/// ztd::text::default_handler_t that is marked as careless. The return type is stateless since both states must be
	/// passed in. If you want to have access to the states, create both of them yourself and pass them into a
	/// lower-level function that accepts those parameters.
	template <typename _OutputContainer = void, typename _Input, typename _FromEncoding, typename _ToEncoding,
		typename _FromErrorHandler>
	constexpr auto recode_to(_Input&& __input, _FromEncoding&& __from_encoding, _ToEncoding&& __to_encoding,
		_FromErrorHandler&& __from_error_handler) {
		auto __handler = __txt_detail::__duplicate_or_be_careless(__from_error_handler);

		return recode_to<_OutputContainer>(::std::forward<_Input>(__input),
			::std::forward<_FromEncoding>(__from_encoding), ::std::forward<_ToEncoding>(__to_encoding),
			::std::forward<_FromErrorHandler>(__from_error_handler), __handler);
	}

	//////
	/// @brief Converts the code units of the given input view through the from encoding to code units of the to
	/// encoding for the output, which is then returned in a result structure with additional information about
	/// success.
	///
	/// @tparam _OutputContainer The container to default-construct and serialize data into. Typically, a @c
	/// std::basic_string or a `std::vector` of some sort.
	///
	/// @param[in]     __input An input_view to read code units from and use in the decode operation that will
	/// produce intermediate code points.
	/// @param[in]     __from_encoding The encoding that will be used to decode the input's code units into
	/// intermediate code points.
	/// @param[in]     __to_encoding The encoding that will be used to encode the intermediate code points into the
	/// final code units.
	///
	/// @returns A ztd::text::stateless_recode_result object that contains references to an `container.output`
	/// parameter that contains the `_OutputContainer` specified.
	///
	/// @remarks A `from_error_handler` for the encode step of the operation is created using default construction of
	/// a ztd::text::default_handler_t that is marked as careless. The return type is stateless since both states must
	/// be passed in. If you want to have access to the states, create both of them yourself and pass them into a
	/// lower-level function that accepts those parameters.
	template <typename _OutputContainer = void, typename _Input, typename _FromEncoding, typename _ToEncoding>
	constexpr auto recode_to(_Input&& __input, _FromEncoding&& __from_encoding, _ToEncoding&& __to_encoding) {
		default_handler_t __handler {};

		return recode_to<_OutputContainer>(::std::forward<_Input>(__input),
			::std::forward<_FromEncoding>(__from_encoding), ::std::forward<_ToEncoding>(__to_encoding), __handler);
	}

	//////
	/// @brief Converts the code units of the given input view through the from encoding to code units of the to
	/// encoding for the output, which is then returned in a result structure with additional information about
	/// success.
	///
	/// @tparam _OutputContainer The container to default-construct and serialize data into. Typically, a @c
	/// std::basic_string or a `std::vector` of some sort.
	///
	/// @param[in]     __input An input_view to read code units from and use in the decode operation that will
	/// produce intermediate code points.
	/// @param[in]     __to_encoding The encoding that will be used to encode the intermediate code points into the
	/// final code units.
	///
	/// @returns A ztd::text::stateless_recode_result object that contains references to an `container.output`
	/// parameter that contains the `_OutputContainer` specified.
	///
	/// @remarks A `from_error_handler` for the encode step of the operation is created using default construction of
	/// a ztd::text::default_handler_t that is marked as careless. The return type is stateless since both states must
	/// be passed in. If you want to have access to the states, create both of them yourself and pass them into a
	/// lower-level function that accepts those parameters.
	template <typename _OutputContainer = void, typename _Input, typename _ToEncoding>
	constexpr auto recode_to(_Input&& __input, _ToEncoding&& __to_encoding) {
		using _UInput    = remove_cvref_t<_Input>;
		using _CodePoint = ranges::range_value_type_t<_UInput>;
#if ZTD_IS_ON(ZTD_STD_LIBRARY_IS_CONSTANT_EVALUATED)
		if (::std::is_constant_evaluated()) {
			using _UFromEncoding = default_consteval_code_point_encoding_t<_CodePoint>;
			default_handler_t __handler {};
			_UFromEncoding __from_encoding {};
			return recode_to<_OutputContainer>(::std::forward<_Input>(__input), __from_encoding,
				::std::forward<_ToEncoding>(__to_encoding), __handler);
		}
		else
#endif
		{
			using _UFromEncoding = default_code_point_encoding_t<_CodePoint>;
			default_handler_t __handler {};
			_UFromEncoding __from_encoding {};
			return recode_to<_OutputContainer>(::std::forward<_Input>(__input), __from_encoding,
				::std::forward<_ToEncoding>(__to_encoding), __handler);
		}
	}

	//////
	/// @brief Converts the code units of the given input view through the from encoding to code units of the to
	/// encoding for the output, which is then returned in a result structure with additional information about
	/// success.
	///
	/// @tparam _OutputContainer The container to default-construct and serialize data into. Typically, a @c
	/// std::basic_string or a `std::vector` of some sort.
	///
	/// @param[in]     __input An input_view to read code units from and use in the decode operation that will
	/// produce intermediate code points.
	/// @param[in]     __from_encoding The encoding that will be used to decode the input's code units into
	/// intermediate code points.
	/// @param[in]     __to_encoding The encoding that will be used to encode the intermediate code points into the
	/// final code units.
	/// @param[in]     __from_error_handler The error handler for the `__from_encoding` 's decode step.
	/// @param[in]     __to_error_handler The error handler for the `__to_encoding` 's encode step.
	/// @param[in,out] __from_state A reference to the associated state for the `__from_encoding` 's decode step.
	/// @param[in,out] __to_state A reference to the associated state for the `__to_encoding` 's encode step.
	/// @param[in, out] __pivot A reference to a descriptor of a (potentially usable) pivot range, usually a range of
	/// contiguous data from a span provided by the implementation but customizable by the end-user. If the
	/// intermediate conversion is what failed, then the ztd::text::pivot's `error_code` member will be set to that
	/// error. This only happens if the overall operation also fails, and need not be checked unless to obtain
	/// additional information for when a top-level operation fails.
	///
	/// @returns An `_OutputContainer` with the result, regardless of whether an error occurs or not. If you are
	/// looking for error information and not just a quick one-off conversion function, please use
	/// ztd::text::recode_to or ztd::text::recode_into_raw.
	template <typename _OutputContainer = void, typename _Input, typename _FromEncoding, typename _ToEncoding,
		typename _FromErrorHandler, typename _ToErrorHandler, typename _FromState, typename _ToState,
		typename _PivotRange>
	constexpr auto recode(_Input&& __input, _FromEncoding&& __from_encoding, _ToEncoding&& __to_encoding,
		_FromErrorHandler&& __from_error_handler, _ToErrorHandler&& __to_error_handler, _FromState& __from_state,
		_ToState& __to_state, pivot<_PivotRange>& __pivot) {
		using _UToEncoding              = remove_cvref_t<_ToEncoding>;
		using _UOutputContainer         = remove_cvref_t<_OutputContainer>;
		using _OutputCodePoint          = code_point_t<_UToEncoding>;
		constexpr bool _IsVoidContainer = ::std::is_void_v<_UOutputContainer>;
		constexpr bool _IsStringable
			= (is_char_traitable_v<_OutputCodePoint> || is_unicode_code_point_v<_OutputCodePoint>);
		if constexpr (_IsVoidContainer && _IsStringable) {
			// prevent instantiation errors with basic_string by boxing it inside of an "if constexpr"
			using _RealOutputContainer = ::std::basic_string<_OutputCodePoint>;
			return __txt_detail::__recode_dispatch<true, false, _RealOutputContainer>(
				::std::forward<_Input>(__input), ::std::forward<_FromEncoding>(__from_encoding),
				::std::forward<_ToEncoding>(__to_encoding), ::std::forward<_FromErrorHandler>(__from_error_handler),
				::std::forward<_ToErrorHandler>(__to_error_handler), __from_state, __to_state, __pivot);
		}
		else {
			using _RealOutputContainer
				= ::std::conditional_t<_IsVoidContainer, ::std::vector<_OutputCodePoint>, _OutputContainer>;
			return __txt_detail::__recode_dispatch<true, false, _RealOutputContainer>(
				::std::forward<_Input>(__input), ::std::forward<_FromEncoding>(__from_encoding),
				::std::forward<_ToEncoding>(__to_encoding), ::std::forward<_FromErrorHandler>(__from_error_handler),
				::std::forward<_ToErrorHandler>(__to_error_handler), __from_state, __to_state, __pivot);
		}
	}

	//////
	/// @brief Converts the code units of the given input view through the from encoding to code units of the to
	/// encoding for the output, which is then returned in a result structure with additional information about
	/// success.
	///
	/// @tparam _OutputContainer The container to default-construct and serialize data into. Typically, a @c
	/// std::basic_string or a `std::vector` of some sort.
	///
	/// @param[in]     __input An input_view to read code units from and use in the decode operation that will
	/// produce intermediate code points.
	/// @param[in]     __from_encoding The encoding that will be used to decode the input's code units into
	/// intermediate code points.
	/// @param[in]     __to_encoding The encoding that will be used to encode the intermediate code points into the
	/// final code units.
	/// @param[in]     __from_error_handler The error handler for the `__from_encoding` 's decode step.
	/// @param[in]     __to_error_handler The error handler for the `__to_encoding` 's encode step.
	/// @param[in,out] __from_state A reference to the associated state for the `__from_encoding` 's decode step.
	/// @param[in,out] __to_state A reference to the associated state for the `__to_encoding` 's encode step.
	///
	/// @returns An `_OutputContainer` with the result, regardless of whether an error occurs or not. If you are
	/// looking for error information and not just a quick one-off conversion function, please use
	/// ztd::text::recode_to or ztd::text::recode_into_raw.
	template <typename _OutputContainer = void, typename _Input, typename _FromEncoding, typename _ToEncoding,
		typename _FromErrorHandler, typename _ToErrorHandler, typename _FromState, typename _ToState>
	constexpr auto recode(_Input&& __input, _FromEncoding&& __from_encoding, _ToEncoding&& __to_encoding,
		_FromErrorHandler&& __from_error_handler, _ToErrorHandler&& __to_error_handler, _FromState& __from_state,
		_ToState& __to_state) {
		using _UFromEncoding    = ::ztd::remove_cvref_t<_FromEncoding>;
		using _CodeUnit         = code_unit_t<_UFromEncoding>;
		using _IntermediateSpan = ::ztd::span<_CodeUnit>;

		constexpr ::std::size_t __intermediate_buffer_max
			= ZTD_TEXT_INTERMEDIATE_RECODE_BUFFER_SIZE_I_(code_unit_t<_UFromEncoding>)
			     < max_code_units_v<_UFromEncoding>
			? max_code_units_v<_UFromEncoding>
			: ZTD_TEXT_INTERMEDIATE_RECODE_BUFFER_SIZE_I_(code_unit_t<_UFromEncoding>);

		_CodeUnit __intermediate[__intermediate_buffer_max] {};
		pivot<_IntermediateSpan> __pivot { _IntermediateSpan(__intermediate), encoding_error::ok };
		return recode<_OutputContainer>(::std::forward<_Input>(__input),
			::std::forward<_FromEncoding>(__from_encoding), ::std::forward<_ToEncoding>(__to_encoding),
			::std::forward<_FromErrorHandler>(__from_error_handler),
			::std::forward<_ToErrorHandler>(__to_error_handler), __from_state, __to_state, __pivot);
	}

	//////
	/// @brief Converts the code units of the given input view through the from encoding to code units of the to
	/// encoding for the output, which is then returned in a result structure with additional information about
	/// success.
	///
	/// @tparam _OutputContainer The container to default-construct and serialize data into. Typically, a @c
	/// std::basic_string or a `std::vector` of some sort.
	///
	/// @param[in]     __input An input_view to read code units from and use in the decode operation that will
	/// produce intermediate code points.
	/// @param[in]     __from_encoding The encoding that will be used to decode the input's code units into
	/// intermediate code points.
	/// @param[in]     __to_encoding The encoding that will be used to encode the intermediate code points into the
	/// final code units.
	/// @param[in]     __from_error_handler The error handler for the `__from_encoding` 's decode step.
	/// @param[in]     __to_error_handler The error handler for the `__to_encoding` 's encode step.
	/// @param[in,out] __from_state A reference to the associated state for the `__from_encoding` 's decode step.
	///
	/// @returns An `_OutputContainer` with the result, regardless of whether an error occurs or not. If you are
	/// looking for error information and not just a quick one-off conversion function, please use
	/// ztd::text::recode_to or ztd::text::recode_into_raw.
	///
	/// @remarks This function creates an `to_state` for the encoding step of the operation using
	/// ztd::text::make_encode_state.
	template <typename _OutputContainer = void, typename _Input, typename _FromEncoding, typename _ToEncoding,
		typename _FromErrorHandler, typename _ToErrorHandler, typename _FromState>
	constexpr auto recode(_Input&& __input, _FromEncoding&& __from_encoding, _ToEncoding&& __to_encoding,
		_FromErrorHandler&& __from_error_handler, _ToErrorHandler&& __to_error_handler, _FromState& __from_state) {
		using _UToEncoding = remove_cvref_t<_ToEncoding>;
		using _ToState     = decode_state_t<_UToEncoding>;

		_ToState __to_state = make_decode_state(__to_encoding);

		return recode<_OutputContainer>(::std::forward<_Input>(__input),
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
	/// std::basic_string or a `std::vector` of some sort.
	///
	/// @param[in]     __input An input_view to read code units from and use in the decode operation that will
	/// produce intermediate code points.
	/// @param[in]     __from_encoding The encoding that will be used to decode the input's code units into
	/// intermediate code points.
	/// @param[in]     __to_encoding The encoding that will be used to encode the intermediate code points into the
	/// final code units.
	/// @param[in]     __from_error_handler The error handler for the `__from_encoding` 's decode step.
	/// @param[in]     __to_error_handler The error handler for the `__to_encoding` 's encode step.
	///
	/// @returns An `_OutputContainer` with the result, regardless of whether an error occurs or not. If you are
	/// looking for error information and not just a quick one-off conversion function, please use
	/// ztd::text::recode_to or ztd::text::recode_into_raw.
	///
	/// @remarks This function creates an `from_state` for the encoding step of the operation using
	/// ztd::text::make_decode_state.
	template <typename _OutputContainer = void, typename _Input, typename _FromEncoding, typename _ToEncoding,
		typename _FromErrorHandler, typename _ToErrorHandler>
	constexpr auto recode(_Input&& __input, _FromEncoding&& __from_encoding, _ToEncoding&& __to_encoding,
		_FromErrorHandler&& __from_error_handler, _ToErrorHandler&& __to_error_handler) {
		using _UFromEncoding = remove_cvref_t<_FromEncoding>;
		using _FromState     = encode_state_t<_UFromEncoding>;

		_FromState __from_state = make_encode_state(__from_encoding);

		return recode<_OutputContainer>(::std::forward<_Input>(__input),
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
	/// std::basic_string or a `std::vector` of some sort.
	///
	/// @param[in]     __input An input_view to read code units from and use in the decode operation that will
	/// produce intermediate code points.
	/// @param[in]     __from_encoding The encoding that will be used to decode the input's code units into
	/// intermediate code points.
	/// @param[in]     __to_encoding The encoding that will be used to encode the intermediate code points into the
	/// final code units.
	/// @param[in]     __from_error_handler The error handler for the `__from_encoding` 's decode step.
	///
	/// @returns An `_OutputContainer` with the result, regardless of whether an error occurs or not. If you are
	/// looking for error information and not just a quick one-off conversion function, please use
	/// ztd::text::recode_to or ztd::text::recode_into_raw.
	///
	/// @remarks This function creates a `to_error_handler` from a class like ztd::text::default_handler_t, but that
	/// is marked as careless since you did not explicitly provide it. This matters for lossy conversions that are not
	/// injective.
	template <typename _OutputContainer = void, typename _Input, typename _FromEncoding, typename _ToEncoding,
		typename _FromErrorHandler>
	constexpr auto recode(_Input&& __input, _FromEncoding&& __from_encoding, _ToEncoding&& __to_encoding,
		_FromErrorHandler&& __from_error_handler) {
		auto __handler = __txt_detail::__duplicate_or_be_careless(__from_error_handler);

		return recode<_OutputContainer>(::std::forward<_Input>(__input),
			::std::forward<_FromEncoding>(__from_encoding), ::std::forward<_ToEncoding>(__to_encoding),
			::std::forward<_FromErrorHandler>(__from_error_handler), __handler);
	}

	//////
	/// @brief Converts the code units of the given input view through the from encoding to code units of the to
	/// encoding for the output, which is then returned in a result structure with additional information about
	/// success.
	///
	/// @tparam _OutputContainer The container to default-construct and serialize data into. Typically, a @c
	/// std::basic_string or a `std::vector` of some sort.
	///
	/// @param[in]     __input An input_view to read code units from and use in the decode operation that will
	/// produce intermediate code points.
	/// @param[in]     __from_encoding The encoding that will be used to decode the input's code units into
	/// intermediate code points.
	/// @param[in]     __to_encoding The encoding that will be used to encode the intermediate code points into the
	/// final code units.
	///
	/// @returns An `_OutputContainer` with the result, regardless of whether an error occurs or not. If you are
	/// looking for error information and not just a quick one-off conversion function, please use
	/// ztd::text::recode_to or ztd::text::recode_into_raw.
	///
	/// @remarks This function creates a `from_error_handler` from a class like ztd::text::default_handler_t, but that
	/// is marked as careless since you did not explicitly provide it. This matters for lossy conversions that are not
	/// injective.
	template <typename _OutputContainer = void, typename _Input, typename _FromEncoding, typename _ToEncoding>
	constexpr auto recode(_Input&& __input, _FromEncoding&& __from_encoding, _ToEncoding&& __to_encoding) {
		default_handler_t __handler {};

		return recode<_OutputContainer>(::std::forward<_Input>(__input),
			::std::forward<_FromEncoding>(__from_encoding), ::std::forward<_ToEncoding>(__to_encoding), __handler);
	}

	//////
	/// @brief Converts the code units of the given input view through the from encoding to code units of the to
	/// encoding for the output, which is then returned in a result structure with additional information about
	/// success.
	///
	/// @tparam _OutputContainer The container to default-construct and serialize data into. Typically, a @c
	/// std::basic_string or a `std::vector` of some sort.
	///
	/// @param[in]     __input An input_view to read code units from and use in the decode operation that will
	/// produce intermediate code points.
	/// @param[in]     __to_encoding The encoding that will be used to encode the intermediate code points into the
	/// final code units.
	///
	/// @returns An `_OutputContainer` with the result, regardless of whether an error occurs or not. If you are
	/// looking for error information and not just a quick one-off conversion function, please use
	/// ztd::text::recode_to or ztd::text::recode_into_raw.
	///
	/// @remarks This function creates both: a `from_error_handler` from a class like ztd::text::default_handler_t,
	/// but that is marked as careless since you did not explicitly provide it; and, a `from_encoding` derived from
	/// the `__input`'s `value_type.` The careless marking matters for lossy conversions that are not injective.
	template <typename _OutputContainer = void, typename _Input, typename _ToEncoding>
	constexpr auto recode(_Input&& __input, _ToEncoding&& __to_encoding) {
		using _UInput    = remove_cvref_t<_Input>;
		using _CodePoint = ranges::range_value_type_t<_UInput>;
#if ZTD_IS_ON(ZTD_STD_LIBRARY_IS_CONSTANT_EVALUATED)
		if (::std::is_constant_evaluated()) {
			using _UFromEncoding = default_consteval_code_point_encoding_t<_CodePoint>;
			_UFromEncoding __from_encoding {};
			return recode<_OutputContainer>(
				::std::forward<_Input>(__input), __from_encoding, ::std::forward<_ToEncoding>(__to_encoding));
		}
		else
#endif
		{
			using _UFromEncoding = default_code_point_encoding_t<_CodePoint>;

			_UFromEncoding __from_encoding {};
			return recode<_OutputContainer>(
				::std::forward<_Input>(__input), __from_encoding, ::std::forward<_ToEncoding>(__to_encoding));
		}
	}

	//////
	/// @}

	ZTD_TEXT_INLINE_ABI_NAMESPACE_CLOSE_I_
}} // namespace ztd::text

#include <ztd/epilogue.hpp>

#endif
