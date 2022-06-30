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

#ifndef ZTD_TEXT_TRANSCODE_ONE_HPP
#define ZTD_TEXT_TRANSCODE_ONE_HPP

#include <ztd/text/version.hpp>

#include <ztd/text/code_point.hpp>
#include <ztd/text/code_unit.hpp>
#include <ztd/text/error_handler.hpp>
#include <ztd/text/state.hpp>
#include <ztd/text/default_encoding.hpp>
#include <ztd/text/transcode_result.hpp>
#include <ztd/text/is_unicode_code_point.hpp>
#include <ztd/text/text_tag.hpp>
#include <ztd/text/type_traits.hpp>
#include <ztd/text/detail/transcode_routines.hpp>
#include <ztd/text/detail/forward_if_move_only.hpp>
#include <ztd/text/detail/is_lossless.hpp>
#include <ztd/text/detail/encoding_range.hpp>
#include <ztd/text/detail/transcode_extension_points.hpp>
#include <ztd/text/detail/span_or_reconstruct.hpp>

#include <ztd/ranges/unbounded.hpp>
#include <ztd/idk/span.hpp>
#include <ztd/static_containers.hpp>
#include <ztd/ranges/detail/insert_bulk.hpp>

#include <ztd/prologue.hpp>

namespace ztd { namespace text {
	ZTD_TEXT_INLINE_ABI_NAMESPACE_OPEN_I_

	//////
	/// @addtogroup ztd_text_transcode_one ztd::text::transcode_one[_into/_to]
	///
	/// @brief These functions convert from a view of input code units into a view of output code units (typically,
	/// through an intermediary code point type that is similar between the two) using either the inferred or specified
	/// encodings. If no error handler is provided, the equivalent of the ztd::text::default_handler_t is used by
	/// default, but it is marked as careless. If no associated state is provided for either the "to" or "from"
	/// encodings, one will be created with automatic storage duration (as a "stack" variable) for the provided
	/// encoding.
	/// @{
	//////

	//////
	/// @brief Converts the code units of the given input view through the from encoding to code units of the to
	/// encoding into the output view. nly one.
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
	/// @result A ztd::text::transcode_result object that contains references to `__from_state` and @p __to_state.
	///
	/// @remark This function detects whether or not the ADL extension point `text_transcode` can be called with the
	/// provided parameters. If so, it will use that ADL extension point over the default implementation. Otherwise, it
	/// will loop over the two encodings and attempt to transcode by first decoding the input code units to code
	/// points, then encoding the intermediate code points to the desired, output code units.
	//////
	template <typename _Input, typename _Output, typename _FromEncoding, typename _ToEncoding,
		typename _FromErrorHandler, typename _ToErrorHandler, typename _FromState, typename _ToState>
	constexpr auto basic_transcode_one_into(_Input&& __input, _FromEncoding&& __from_encoding, _Output&& __output,
		_ToEncoding&& __to_encoding, _FromErrorHandler&& __from_error_handler, _ToErrorHandler&& __to_error_handler,
		_FromState& __from_state, _ToState& __to_state) {
		using _InitialInput          = __txt_detail::__string_view_or_span_or_reconstruct_t<_Input>;
		using _InitialOutput         = ranges::range_reconstruct_t<_Output>;
		using _UFromEncoding         = remove_cvref_t<_FromEncoding>;
		using _UToEncoding           = remove_cvref_t<_ToEncoding>;
		using _UFromErrorHandler     = remove_cvref_t<_FromErrorHandler>;
		using _UToErrorHandler       = remove_cvref_t<_ToErrorHandler>;
		using _IntermediateCodePoint = code_point_t<_UFromEncoding>;
		constexpr ::std::size_t _IntermediateCodePointMax = max_code_points_v<_UFromEncoding>;
		using _IntermediateStorage                        = _IntermediateCodePoint[_IntermediateCodePointMax];
		using _Intermediate  = ::ztd::span<_IntermediateCodePoint, _IntermediateCodePointMax>;
		using _Result        = decltype(__txt_detail::__basic_transcode_one<__txt_detail::__consume::__no>(
               ::std::declval<_InitialInput>(), __from_encoding, ::std::declval<_InitialOutput>(), __to_encoding,
               __from_error_handler, __to_error_handler, __from_state, __to_state, ::std::declval<_Intermediate&>()));
		using _WorkingInput  = decltype(::std::declval<_Result>().input);
		using _WorkingOutput = decltype(::std::declval<_Result>().output);

		static_assert(__txt_detail::__is_decode_lossless_or_deliberate_v<_UFromEncoding, _UFromErrorHandler>,
			ZTD_TEXT_LOSSY_TRANSCODE_DECODE_MESSAGE_I_);
		static_assert(__txt_detail::__is_encode_lossless_or_deliberate_v<_UToEncoding, _UToErrorHandler>,
			ZTD_TEXT_LOSSY_TRANSCODE_ENCODE_MESSAGE_I_);

		_WorkingInput __working_input(
			__txt_detail::__string_view_or_span_or_reconstruct(::std::forward<_Input>(__input)));
		_WorkingOutput __working_output
			= ranges::reconstruct(::std::in_place_type<_WorkingOutput>, ::std::forward<_Output>(__output));

		_IntermediateStorage __intermediate_storage {};
		_Intermediate __intermediate(__intermediate_storage);
		return __txt_detail::__basic_transcode_one<__txt_detail::__consume::__no>(::std::move(__working_input),
			__from_encoding, ::std::move(__working_output), __to_encoding, __from_error_handler, __to_error_handler,
			__from_state, __to_state, __intermediate);
	}

	//////
	/// @brief Converts the code units of the given input view through the from encoding to code units of the to
	/// encoding into the output view. Only performs one distinct unit of encoding.
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
	/// @result A ztd::text::transcode_result object that contains references to `__from_state` and @p
	/// __to_state.
	///
	/// @remark This function detects whether or not the ADL extension point `text_transcode_one` can be called with
	/// the provided parameters. If so, it will use that ADL extension point over the default implementation.
	/// Otherwise, it will loop over the two encodings and attempt to transcode by first decoding the input code units
	/// to code points, then encoding the intermediate code points to the desired, output code units.
	//////
	template <typename _Input, typename _Output, typename _FromEncoding, typename _ToEncoding,
		typename _FromErrorHandler, typename _ToErrorHandler, typename _FromState, typename _ToState>
	constexpr auto transcode_one_into(_Input&& __input, _FromEncoding&& __from_encoding, _Output&& __output,
		_ToEncoding&& __to_encoding, _FromErrorHandler&& __from_error_handler, _ToErrorHandler&& __to_error_handler,
		_FromState& __from_state, _ToState& __to_state) {
		if constexpr (is_detected_v<__txt_detail::__detect_adl_text_transcode_one, _Input, _Output, _FromEncoding,
			              _ToEncoding, _FromErrorHandler, _ToErrorHandler, _FromState, _ToState>) {
			return text_transcode_one(text_tag<remove_cvref_t<_FromEncoding>, remove_cvref_t<_ToEncoding>> {},
				::std::forward<_Input>(__input), ::std::forward<_FromEncoding>(__from_encoding),
				::std::forward<_Output>(__output), ::std::forward<_ToEncoding>(__to_encoding),
				::std::forward<_FromErrorHandler>(__from_error_handler),
				::std::forward<_ToErrorHandler>(__to_error_handler), __from_state, __to_state);
		}
		else if constexpr (is_detected_v<__txt_detail::__detect_adl_internal_text_transcode_one, _Input,
			                   _FromEncoding, _Output, _ToEncoding, _FromErrorHandler, _ToErrorHandler, _FromState,
			                   _ToState>) {
			return __text_transcode_one(text_tag<remove_cvref_t<_FromEncoding>, remove_cvref_t<_ToEncoding>> {},
				::std::forward<_Input>(__input), ::std::forward<_FromEncoding>(__from_encoding),
				::std::forward<_Output>(__output), ::std::forward<_ToEncoding>(__to_encoding),
				::std::forward<_FromErrorHandler>(__from_error_handler),
				::std::forward<_ToErrorHandler>(__to_error_handler), __from_state, __to_state);
		}
		else {
			return basic_transcode_one_into(::std::forward<_Input>(__input),
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
	/// @param[in]     __from_error_handler The error handler for the `__from_encoding` 's decode step.
	/// @param[in]     __to_error_handler The error handler for the `__to_encoding` 's encode step.
	/// @param[in,out] __from_state A reference to the associated state for the `__from_encoding` 's decode step.
	///
	/// @result A ztd::text::stateless_transcode_result object.
	///
	/// @remarks This function calls the base reference, the ztd::text::transcode_one_into after creating a `to_state`
	/// from ztd::text::make_encode_state. The result from this function returns a
	/// ztd::text::stateless_transcode_result as opposed to a ztd::text::transcode_result because the state
	/// information is on the stack, and returning the state in those types by reference will result in references to
	/// memory that has already been cleaned up. If you need access to the state parameters, call the lower-level
	/// functionality with your own created states.
	//////
	template <typename _Input, typename _FromEncoding, typename _Output, typename _ToEncoding,
		typename _FromErrorHandler, typename _ToErrorHandler, typename _FromState>
	constexpr auto transcode_one_into(_Input&& __input, _FromEncoding&& __from_encoding, _Output&& __output,
		_ToEncoding&& __to_encoding, _FromErrorHandler&& __from_error_handler, _ToErrorHandler&& __to_error_handler,
		_FromState& __from_state) {
		using _UToEncoding = remove_cvref_t<_ToEncoding>;
		using _ToState     = encode_state_t<_UToEncoding>;

		_ToState __to_state = make_encode_state(__to_encoding);

		auto __stateful_result = transcode_one_into(::std::forward<_Input>(__input),
			::std::forward<_FromEncoding>(__from_encoding), ::std::forward<_Output>(__output),
			::std::forward<_ToEncoding>(__to_encoding), ::std::forward<_FromErrorHandler>(__from_error_handler),
			::std::forward<_ToErrorHandler>(__to_error_handler), __from_state, __to_state);

		return __txt_detail::__slice_to_stateless(::std::move(__stateful_result));
	}

	//////
	/// @brief Converts the code units of the given input view through the from encoding to code units of the to
	/// encoding into the output view.
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
	/// result from this function returns a ztd::text::stateless_transcode_result as opposed to a
	/// ztd::text::transcode_result because the state information is on the stack, and returning the state in
	/// those types by reference will result in references to memory that has already been cleaned up. If you need
	/// access to the state parameters, call the lower-level functionality with your own created states.
	//////
	template <typename _Input, typename _FromEncoding, typename _Output, typename _ToEncoding,
		typename _FromErrorHandler, typename _ToErrorHandler>
	constexpr auto transcode_one_into(_Input&& __input, _FromEncoding&& __from_encoding, _Output&& __output,
		_ToEncoding&& __to_encoding, _FromErrorHandler&& __from_error_handler, _ToErrorHandler&& __to_error_handler) {
		using _UFromEncoding = remove_cvref_t<_FromEncoding>;
		using _FromState     = decode_state_t<_UFromEncoding>;

		_FromState __from_state = make_decode_state(__from_encoding);

		return transcode_one_into(::std::forward<_Input>(__input), ::std::forward<_FromEncoding>(__from_encoding),
			::std::forward<_Output>(__output), ::std::forward<_ToEncoding>(__to_encoding),
			::std::forward<_FromErrorHandler>(__from_error_handler),
			::std::forward<_ToErrorHandler>(__to_error_handler), __from_state);
	}

	//////
	/// @brief Converts the code units of the given input view through the from encoding to code units of the to
	/// encoding into the output view.
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
	/// injective. The result from this function returns a ztd::text::stateless_transcode_result as opposed to a
	/// ztd::text::transcode_result because the state information is on the stack, and returning the state in
	/// those types by reference will result in references to memory that has already been cleaned up. If you need
	/// access to the state parameters, call the lower-level functionality with your own created states.
	//////
	template <typename _Input, typename _FromEncoding, typename _Output, typename _ToEncoding,
		typename _FromErrorHandler>
	constexpr auto transcode_one_into(_Input&& __input, _FromEncoding&& __from_encoding, _Output&& __output,
		_ToEncoding&& __to_encoding, _FromErrorHandler&& __from_error_handler) {
		auto __handler = __txt_detail::__duplicate_or_be_careless(__from_error_handler);

		return transcode_one_into(::std::forward<_Input>(__input), ::std::forward<_FromEncoding>(__from_encoding),
			::std::forward<_Output>(__output), ::std::forward<_ToEncoding>(__to_encoding),
			::std::forward<_FromErrorHandler>(__from_error_handler), __handler);
	}

	//////
	/// @brief Converts the code units of the given input view through the from encoding to code units of the to
	/// encoding into the output view.
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
	/// not injective. The result from this function returns a ztd::text::stateless_transcode_result as opposed to a
	/// ztd::text::transcode_result because the state information is on the stack, and returning the state in
	/// those types by reference will result in references to memory that has already been cleaned up. If you need
	/// access to the state parameters, call the lower-level functionality with your own created states.
	//////
	template <typename _Input, typename _FromEncoding, typename _Output, typename _ToEncoding>
	constexpr auto transcode_one_into(
		_Input&& __input, _FromEncoding&& __from_encoding, _Output&& __output, _ToEncoding&& __to_encoding) {
		default_handler_t __handler {};

		return transcode_one_into(::std::forward<_Input>(__input), ::std::forward<_FromEncoding>(__from_encoding),
			::std::forward<_Output>(__output), ::std::forward<_ToEncoding>(__to_encoding), __handler);
	}

	//////
	/// @brief Converts the code units of the given input view through the from encoding to code units of the to
	/// encoding into the output view.
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
	/// result from this function returns a ztd::text::stateless_transcode_result as opposed to a
	/// ztd::text::transcode_result because the state information is on the stack, and returning the state in
	/// those types by reference will result in references to memory that has already been cleaned up. If you need
	/// access to the state parameters, call the lower-level functionality with your own created states.
	//////
	template <typename _Input, typename _ToEncoding, typename _Output>
	constexpr auto transcode_one_into(_Input&& __input, _ToEncoding&& __to_encoding, _Output&& __output) {
		using _UInput        = remove_cvref_t<_Input>;
		using _UFromEncoding = default_code_unit_encoding_t<ranges::range_value_type_t<_UInput>>;

		_UFromEncoding __from_encoding {};
		default_handler_t __handler {};

		return transcode_one_into(::std::forward<_Input>(__input), __from_encoding, ::std::forward<_Output>(__output),
			::std::forward<_ToEncoding>(__to_encoding), __handler);
	}

	namespace __txt_detail {
		template <typename _OutputContainer, typename _Input, typename _FromEncoding, typename _ToEncoding,
			typename _FromErrorHandler, typename _ToErrorHandler, typename _FromState, typename _ToState>
		constexpr auto __intermediate_transcode_one_to_storage(_Input&& __input, _FromEncoding&& __from_encoding,
			_OutputContainer& __output, _ToEncoding&& __to_encoding, _FromErrorHandler&& __from_error_handler,
			_ToErrorHandler&& __to_error_handler, _FromState& __from_state, _ToState& __to_state) {
			// Well, SHIT. Write into temporary, then serialize one-by-one/bulk to output.
			// I'll admit, this is HELLA work to support...
			using _UFromEncoding                              = remove_cvref_t<_FromEncoding>;
			using _UToEncoding                                = remove_cvref_t<_ToEncoding>;
			using _UFromErrorHandler                          = remove_cvref_t<_FromErrorHandler>;
			using _UToErrorHandler                            = remove_cvref_t<_ToErrorHandler>;
			constexpr ::std::size_t __intermediate_buffer_max = max_code_units_v<_UToEncoding>;
			using _IntermediateValueType                      = code_unit_t<_UToEncoding>;
			using _Output                                     = ::ztd::span<_IntermediateValueType>;

			static_assert(__txt_detail::__is_decode_lossless_or_deliberate_v<_UFromEncoding, _UFromErrorHandler>,
				ZTD_TEXT_LOSSY_TRANSCODE_DECODE_MESSAGE_I_);
			static_assert(__txt_detail::__is_encode_lossless_or_deliberate_v<_UToEncoding, _UToErrorHandler>,
				ZTD_TEXT_LOSSY_TRANSCODE_ENCODE_MESSAGE_I_);

			_IntermediateValueType __intermediate_translation_buffer[__intermediate_buffer_max] {};

			_Output __intermediate_initial_output(__intermediate_translation_buffer);
			auto __result = transcode_one_into(::std::forward<_Input>(__input),
				::std::forward<_FromEncoding>(__from_encoding), ::std::move(__intermediate_initial_output),
				::std::forward<_ToEncoding>(__to_encoding), ::std::forward<_FromErrorHandler>(__from_error_handler),
				::std::forward<_ToErrorHandler>(__to_error_handler), __from_state, __to_state);
			_Output __intermediate_output(__intermediate_initial_output.data(), __result.output.data());
			ranges::__rng_detail::__container_insert_bulk(__output, __intermediate_output);
			return __result;
		}

		template <bool _OutputOnly, typename _OutputContainer, typename _Input, typename _FromEncoding,
			typename _ToEncoding, typename _FromErrorHandler, typename _ToErrorHandler, typename _FromState,
			typename _ToState>
		constexpr auto __transcode_one_dispatch(_Input&& __input, _FromEncoding&& __from_encoding,
			_ToEncoding&& __to_encoding, _FromErrorHandler&& __from_error_handler,
			_ToErrorHandler&& __to_error_handler, _FromState& __from_state, _ToState& __to_state) {
			using _UToEncoding = remove_cvref_t<_ToEncoding>;

			_OutputContainer __output {};
			if constexpr (is_detected_v<ranges::detect_adl_size, _Input>) {
				using _SizeType = decltype(ranges::ranges_adl::adl_size(__input));
				if constexpr (is_detected_v<ranges::detect_reserve_with_size, _OutputContainer, _SizeType>) {
					_SizeType __output_size_hint = static_cast<_SizeType>(ranges::ranges_adl::adl_size(__input));
					__output.reserve(__output_size_hint);
				}
			}
			if constexpr (__txt_detail::__is_encode_range_category_output_v<_UToEncoding>) {
				using _BackInserterIterator = decltype(::std::back_inserter(::std::declval<_OutputContainer&>()));
				using _Unbounded            = ranges::unbounded_view<_BackInserterIterator>;
				// We can use the unbounded stuff
				_Unbounded __insert_view(::std::back_inserter(__output));
				auto __stateful_result = transcode_one_into(__txt_detail::__forward_if_move_only<_Input>(__input),
					::std::forward<_FromEncoding>(__from_encoding), ::std::move(__insert_view),
					::std::forward<_ToEncoding>(__to_encoding),
					::std::forward<_FromErrorHandler>(__from_error_handler),
					::std::forward<_ToErrorHandler>(__to_error_handler), __from_state, __to_state);
				if constexpr (_OutputOnly) {
					(void)__stateful_result;
					return __output;
				}
				else {
					return __txt_detail::__replace_result_output(
						::std::move(__stateful_result), ::std::move(__output));
				}
			}
			else {
				auto __stateful_result = __intermediate_transcode_one_to_storage(::std::forward<_Input>(__input),
					::std::forward<_FromEncoding>(__from_encoding), __output,
					::std::forward<_ToEncoding>(__to_encoding),
					::std::forward<_FromErrorHandler>(__from_error_handler),
					::std::forward<_ToErrorHandler>(__to_error_handler), __from_state, __to_state);
				if constexpr (_OutputOnly) {
					(void)__stateful_result;
					return __output;
				}
				else {
					return __txt_detail::__replace_result_output(
						::std::move(__stateful_result), ::std::move(__output));
				}
			}
		}
	} // namespace __txt_detail

	//////
	/// @brief Converts the code units of the given input view through the from encoding to code units of the to
	/// encoding for the output, which is then returned in a result structure with additional information about
	/// success.
	///
	/// @tparam _OutputContainer The container to default-construct and serialize data into. Typically, a fixed-size
	/// container of some sort.
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
	/// @returns A ztd::text::transcode_result object that contains references to `__from_state` and @p
	/// __to_state and an `output` parameter that contains the `_OutputContainer` specified. If the container has a
	/// `container.reserve` function, it is and some multiple of the input's size is used to pre-size the container,
	/// to aid with `push_back` / `insert` reallocation pains.
	//////
	template <typename _OutputContainer = void, typename _Input, typename _FromEncoding, typename _ToEncoding,
		typename _FromErrorHandler, typename _ToErrorHandler, typename _FromState, typename _ToState>
	constexpr auto transcode_one_to(_Input&& __input, _FromEncoding&& __from_encoding, _ToEncoding&& __to_encoding,
		_FromErrorHandler&& __from_error_handler, _ToErrorHandler&& __to_error_handler, _FromState& __from_state,
		_ToState& __to_state) {
		using _UToEncoding              = remove_cvref_t<_ToEncoding>;
		using _UOutputContainer         = remove_cvref_t<_OutputContainer>;
		using _OutputCodeUnit           = code_unit_t<_UToEncoding>;
		constexpr bool _IsVoidContainer = ::std::is_void_v<_UOutputContainer>;
		constexpr bool _IsStringable
			= (is_char_traitable_v<_OutputCodeUnit> || is_unicode_code_point_v<_OutputCodeUnit>);
		if constexpr (_IsVoidContainer && _IsStringable) {
			using _RealOutputContainer = ::ztd::static_basic_string<_OutputCodeUnit, max_code_units_v<_UToEncoding>>;
			return __txt_detail::__transcode_one_dispatch<false, _RealOutputContainer>(
				::std::forward<_Input>(__input), ::std::forward<_FromEncoding>(__from_encoding),
				::std::forward<_ToEncoding>(__to_encoding), ::std::forward<_FromErrorHandler>(__from_error_handler),
				::std::forward<_ToErrorHandler>(__to_error_handler), __from_state, __to_state);
		}
		else {
			using _RealOutputContainer = ::std::conditional_t<_IsVoidContainer,
				::ztd::static_vector<_OutputCodeUnit, max_code_units_v<_UToEncoding>>, _OutputContainer>;
			return __txt_detail::__transcode_one_dispatch<false, _RealOutputContainer>(
				::std::forward<_Input>(__input), ::std::forward<_FromEncoding>(__from_encoding),
				::std::forward<_ToEncoding>(__to_encoding), ::std::forward<_FromErrorHandler>(__from_error_handler),
				::std::forward<_ToErrorHandler>(__to_error_handler), __from_state, __to_state);
		}
	}

	//////
	/// @brief Converts the code units of the given input view through the from encoding to code units of the to
	/// encoding for the output, which is then returned in a result structure with additional information about
	/// success.
	///
	/// @tparam _OutputContainer The container to default-construct and serialize data into. Typically, a fixed-size
	/// container of some sort.
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
	/// @returns A ztd::text::stateless_transcode_result object that contains references to an `container.output`
	/// parameter that contains the `_OutputContainer` specified.
	///
	/// @remarks A default state for the encode step of the operation is create using ztd::text::make_encode_state. The
	/// return type is stateless since both states must be passed in. If you want to have access to the states, create
	/// both of them yourself and pass them into a lower-level function that accepts those parameters.
	//////
	template <typename _OutputContainer = void, typename _Input, typename _FromEncoding, typename _ToEncoding,
		typename _FromErrorHandler, typename _ToErrorHandler, typename _FromState>
	constexpr auto transcode_one_to(_Input&& __input, _FromEncoding&& __from_encoding, _ToEncoding&& __to_encoding,
		_FromErrorHandler&& __from_error_handler, _ToErrorHandler&& __to_error_handler, _FromState& __from_state) {
		using _UToEncoding = remove_cvref_t<_ToEncoding>;
		using _ToState     = encode_state_t<_UToEncoding>;

		_ToState __to_state = make_encode_state(__to_encoding);

		return transcode_one_to<_OutputContainer>(::std::forward<_Input>(__input),
			::std::forward<_FromEncoding>(__from_encoding), ::std::forward<_ToEncoding>(__to_encoding),
			::std::forward<_FromErrorHandler>(__from_error_handler),
			::std::forward<_ToErrorHandler>(__to_error_handler), __from_state, __to_state);
	}

	//////
	/// @brief Converts the code units of the given input view through the from encoding to code units of the to
	/// encoding for the output, which is then returned in a result structure with additional information about
	/// success.
	///
	/// @tparam _OutputContainer The container to default-construct and serialize data into. Typically, a fixed-size
	/// container of some sort.
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
	/// @returns A ztd::text::stateless_transcode_result object that contains references to an `container.output`
	/// parameter that contains the `_OutputContainer` specified.
	///
	/// @remarks A default state for the decode step of the operation is create using ztd::text::make_decode_state. The
	/// return type is stateless since both states must be passed in. If you want to have access to the states, create
	/// both of them yourself and pass them into a lower-level function that accepts those parameters.
	//////
	template <typename _OutputContainer = void, typename _Input, typename _FromEncoding, typename _ToEncoding,
		typename _FromErrorHandler, typename _ToErrorHandler>
	constexpr auto transcode_one_to(_Input&& __input, _FromEncoding&& __from_encoding, _ToEncoding&& __to_encoding,
		_FromErrorHandler&& __from_error_handler, _ToErrorHandler&& __to_error_handler) {
		using _UFromEncoding = remove_cvref_t<_FromEncoding>;
		using _FromState     = decode_state_t<_UFromEncoding>;

		_FromState __from_state = make_decode_state(__from_encoding);

		return transcode_one_to<_OutputContainer>(::std::forward<_Input>(__input),
			::std::forward<_FromEncoding>(__from_encoding), ::std::forward<_ToEncoding>(__to_encoding),
			::std::forward<_FromErrorHandler>(__from_error_handler),
			::std::forward<_ToErrorHandler>(__to_error_handler), __from_state);
	}

	//////
	/// @brief Converts the code units of the given input view through the from encoding to code units of the to
	/// encoding for the output, which is then returned in a result structure with additional information about
	/// success.
	///
	/// @tparam _OutputContainer The container to default-construct and serialize data into. Typically, a fixed-size
	/// container of some sort.
	///
	/// @param[in]     __input An input_view to read code units from and use in the decode operation that will
	/// produce intermediate code points.
	/// @param[in]     __from_encoding The encoding that will be used to decode the input's code units into
	/// intermediate code points.
	/// @param[in]     __to_encoding The encoding that will be used to encode the intermediate code points into the
	/// final code units.
	/// @param[in]     __from_error_handler The error handler for the `__from_encoding` 's decode step.
	///
	/// @returns A ztd::text::stateless_transcode_result object that contains references to an `container.output`
	/// parameter that contains the `_OutputContainer` specified.
	///
	/// @remarks A `to_error_handler` for the encode step of the operation is created using default construction of a
	/// ztd::text::default_handler_t that is marked as careless. The return type is stateless since both states must be
	/// passed in. If you want to have access to the states, create both of them yourself and pass them into a
	/// lower-level function that accepts those parameters.
	//////
	template <typename _OutputContainer = void, typename _Input, typename _FromEncoding, typename _ToEncoding,
		typename _FromErrorHandler>
	constexpr auto transcode_one_to(_Input&& __input, _FromEncoding&& __from_encoding, _ToEncoding&& __to_encoding,
		_FromErrorHandler&& __from_error_handler) {
		auto __handler = __txt_detail::__duplicate_or_be_careless(__from_error_handler);

		return transcode_one_to<_OutputContainer>(::std::forward<_Input>(__input),
			::std::forward<_FromEncoding>(__from_encoding), ::std::forward<_ToEncoding>(__to_encoding),
			::std::forward<_FromErrorHandler>(__from_error_handler), __handler);
	}

	//////
	/// @brief Converts the code units of the given input view through the from encoding to code units of the to
	/// encoding for the output, which is then returned in a result structure with additional information about
	/// success.
	///
	/// @tparam _OutputContainer The container to default-construct and serialize data into. Typically, a fixed-size
	/// container of some sort.
	///
	/// @param[in]     __input An input_view to read code units from and use in the decode operation that will
	/// produce intermediate code points.
	/// @param[in]     __from_encoding The encoding that will be used to decode the input's code units into
	/// intermediate code points.
	/// @param[in]     __to_encoding The encoding that will be used to encode the intermediate code points into the
	/// final code units.
	///
	/// @returns A ztd::text::stateless_transcode_result object that contains references to an `container.output`
	/// parameter that contains the `_OutputContainer` specified.
	///
	/// @remarks A `from_error_handler` for the encode step of the operation is created using default construction of
	/// a ztd::text::default_handler_t that is marked as careless. The return type is stateless since both states must
	/// be passed in. If you want to have access to the states, create both of them yourself and pass them into a
	/// lower-level function that accepts those parameters.
	//////
	template <typename _OutputContainer = void, typename _Input, typename _FromEncoding, typename _ToEncoding>
	constexpr auto transcode_one_to(_Input&& __input, _FromEncoding&& __from_encoding, _ToEncoding&& __to_encoding) {
		default_handler_t __handler {};

		return transcode_one_to<_OutputContainer>(::std::forward<_Input>(__input),
			::std::forward<_FromEncoding>(__from_encoding), ::std::forward<_ToEncoding>(__to_encoding), __handler);
	}

	//////
	/// @brief Converts the code units of the given input view through the from encoding to code units of the to
	/// encoding for the output, which is then returned in a result structure with additional information about
	/// success.
	///
	/// @tparam _OutputContainer The container to default-construct and serialize data into. Typically, a fixed-size
	/// container of some sort.
	///
	/// @param[in]     __input An input_view to read code units from and use in the decode operation that will
	/// produce intermediate code points.
	/// @param[in]     __to_encoding The encoding that will be used to encode the intermediate code points into the
	/// final code units.
	///
	/// @returns A ztd::text::stateless_transcode_result object that contains references to an `container.output`
	/// parameter that contains the `_OutputContainer` specified.
	///
	/// @remarks A `from_error_handler` for the encode step of the operation is created using default construction of
	/// a ztd::text::default_handler_t that is marked as careless. The return type is stateless since both states must
	/// be passed in. If you want to have access to the states, create both of them yourself and pass them into a
	/// lower-level function that accepts those parameters.
	//////
	template <typename _OutputContainer, typename _Input, typename _ToEncoding>
	constexpr auto transcode_one_to(_Input&& __input, _ToEncoding&& __to_encoding) {
		using _UInput   = remove_cvref_t<_Input>;
		using _CodeUnit = ranges::range_value_type_t<_UInput>;
#if ZTD_IS_ON(ZTD_STD_LIBRARY_IS_CONSTANT_EVALUATED)
		if (::std::is_constant_evaluated()) {
			using _UFromEncoding = default_consteval_code_unit_encoding_t<_CodeUnit>;
			default_handler_t __handler {};
			_UFromEncoding __from_encoding {};
			return transcode_one_to<_OutputContainer>(::std::forward<_Input>(__input), __from_encoding,
				::std::forward<_ToEncoding>(__to_encoding), __handler);
		}
		else
#endif
		{
			using _UFromEncoding = default_code_unit_encoding_t<_CodeUnit>;
			default_handler_t __handler {};
			_UFromEncoding __from_encoding {};
			return transcode_one_to<_OutputContainer>(::std::forward<_Input>(__input), __from_encoding,
				::std::forward<_ToEncoding>(__to_encoding), __handler);
		}
	}

	//////
	/// @brief Converts the code units of the given input view through the from encoding to code units of the to
	/// encoding for the output, which is then returned in a result structure with additional information about
	/// success.
	///
	/// @tparam _OutputContainer The container to default-construct and serialize data into. Typically, a fixed-size
	/// container of some sort.
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
	/// ztd::text::transcode_one_to or ztd::text::transcode_one_into.
	//////
	template <typename _OutputContainer = void, typename _Input, typename _FromEncoding, typename _ToEncoding,
		typename _FromErrorHandler, typename _ToErrorHandler, typename _FromState, typename _ToState>
	constexpr auto transcode_one(_Input&& __input, _FromEncoding&& __from_encoding, _ToEncoding&& __to_encoding,
		_FromErrorHandler&& __from_error_handler, _ToErrorHandler&& __to_error_handler, _FromState& __from_state,
		_ToState& __to_state) {
		using _UToEncoding              = remove_cvref_t<_ToEncoding>;
		using _UOutputContainer         = remove_cvref_t<_OutputContainer>;
		using _OutputCodeUnit           = code_unit_t<_UToEncoding>;
		constexpr bool _IsVoidContainer = ::std::is_void_v<_UOutputContainer>;
		constexpr bool _IsStringable
			= (is_char_traitable_v<_OutputCodeUnit> || is_unicode_code_point_v<_OutputCodeUnit>);
		if constexpr (_IsVoidContainer && _IsStringable) {
			using _RealOutputContainer = ::ztd::static_basic_string<_OutputCodeUnit, max_code_units_v<_UToEncoding>>;
			return __txt_detail::__transcode_one_dispatch<true, _RealOutputContainer>(
				::std::forward<_Input>(__input), ::std::forward<_FromEncoding>(__from_encoding),
				::std::forward<_ToEncoding>(__to_encoding), ::std::forward<_FromErrorHandler>(__from_error_handler),
				::std::forward<_ToErrorHandler>(__to_error_handler), __from_state, __to_state);
		}
		else {
			using _RealOutputContainer = ::std::conditional_t<_IsVoidContainer,
				::ztd::static_vector<_OutputCodeUnit, max_code_units_v<_UToEncoding>>, _OutputContainer>;
			return __txt_detail::__transcode_one_dispatch<true, _RealOutputContainer>(
				::std::forward<_Input>(__input), ::std::forward<_FromEncoding>(__from_encoding),
				::std::forward<_ToEncoding>(__to_encoding), ::std::forward<_FromErrorHandler>(__from_error_handler),
				::std::forward<_ToErrorHandler>(__to_error_handler), __from_state, __to_state);
		}
	}

	//////
	/// @brief Converts the code units of the given input view through the from encoding to code units of the to
	/// encoding for the output, which is then returned in a result structure with additional information about
	/// success.
	///
	/// @tparam _OutputContainer The container to default-construct and serialize data into. Typically, a fixed-size
	/// container of some sort.
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
	/// ztd::text::transcode_one_to or ztd::text::transcode_one_into.
	///
	/// @remarks This function creates an `to_state` for the encoding step of the operation using
	/// ztd::text::make_encode_state.
	//////
	template <typename _OutputContainer = void, typename _Input, typename _FromEncoding, typename _ToEncoding,
		typename _FromErrorHandler, typename _ToErrorHandler, typename _FromState>
	constexpr auto transcode_one(_Input&& __input, _FromEncoding&& __from_encoding, _ToEncoding&& __to_encoding,
		_FromErrorHandler&& __from_error_handler, _ToErrorHandler&& __to_error_handler, _FromState& __from_state) {
		using _UToEncoding = remove_cvref_t<_ToEncoding>;
		using _ToState     = encode_state_t<_UToEncoding>;

		_ToState __to_state = make_encode_state(__to_encoding);

		return transcode_one<_OutputContainer>(::std::forward<_Input>(__input),
			::std::forward<_FromEncoding>(__from_encoding), ::std::forward<_ToEncoding>(__to_encoding),
			::std::forward<_FromErrorHandler>(__from_error_handler),
			::std::forward<_ToErrorHandler>(__to_error_handler), __from_state, __to_state);
	}

	//////
	/// @brief Converts the code units of the given input view through the from encoding to code units of the to
	/// encoding for the output, which is then returned in a result structure with additional information about
	/// success.
	///
	/// @tparam _OutputContainer The container to default-construct and serialize data into. Typically, a fixed-size
	/// container of some sort.
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
	/// ztd::text::transcode_one_to or ztd::text::transcode_one_into.
	///
	/// @remarks This function creates an `from_state` for the encoding step of the operation using
	/// ztd::text::make_decode_state.
	//////
	template <typename _OutputContainer = void, typename _Input, typename _FromEncoding, typename _ToEncoding,
		typename _FromErrorHandler, typename _ToErrorHandler>
	constexpr auto transcode_one(_Input&& __input, _FromEncoding&& __from_encoding, _ToEncoding&& __to_encoding,
		_FromErrorHandler&& __from_error_handler, _ToErrorHandler&& __to_error_handler) {
		using _UFromEncoding = remove_cvref_t<_FromEncoding>;
		using _FromState     = decode_state_t<_UFromEncoding>;

		_FromState __from_state = make_decode_state(__from_encoding);

		return transcode_one<_OutputContainer>(::std::forward<_Input>(__input),
			::std::forward<_FromEncoding>(__from_encoding), ::std::forward<_ToEncoding>(__to_encoding),
			::std::forward<_FromErrorHandler>(__from_error_handler),
			::std::forward<_ToErrorHandler>(__to_error_handler), __from_state);
	}

	//////
	/// @brief Converts the code units of the given input view through the from encoding to code units of the to
	/// encoding for the output, which is then returned in a result structure with additional information about
	/// success.
	///
	/// @tparam _OutputContainer The container to default-construct and serialize data into. Typically, a fixed-size
	/// container of some sort.
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
	/// ztd::text::transcode_one_to or ztd::text::transcode_one_into.
	///
	/// @remarks This function creates a `to_error_handler` from a class like ztd::text::default_handler_t, but that
	/// is marked as careless since you did not explicitly provide it. This matters for lossy conversions that are not
	/// injective.
	//////
	template <typename _OutputContainer = void, typename _Input, typename _FromEncoding, typename _ToEncoding,
		typename _FromErrorHandler>
	constexpr auto transcode_one(_Input&& __input, _FromEncoding&& __from_encoding, _ToEncoding&& __to_encoding,
		_FromErrorHandler&& __from_error_handler) {
		auto __handler = __txt_detail::__duplicate_or_be_careless(__from_error_handler);

		return transcode_one<_OutputContainer>(::std::forward<_Input>(__input),
			::std::forward<_FromEncoding>(__from_encoding), ::std::forward<_ToEncoding>(__to_encoding),
			::std::forward<_FromErrorHandler>(__from_error_handler), __handler);
	}

	//////
	/// @brief Converts the code units of the given input view through the from encoding to code units of the to
	/// encoding for the output, which is then returned in a result structure with additional information about
	/// success.
	///
	/// @tparam _OutputContainer The container to default-construct and serialize data into. Typically, a fixed-size
	/// container of some sort.
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
	/// ztd::text::transcode_one_to or ztd::text::transcode_one_into.
	///
	/// @remarks This function creates a `from_error_handler` from a class like ztd::text::default_handler_t, but that
	/// is marked as careless since you did not explicitly provide it. This matters for lossy conversions that are not
	/// injective.
	//////
	template <typename _OutputContainer = void, typename _Input, typename _FromEncoding, typename _ToEncoding>
	constexpr auto transcode_one(_Input&& __input, _FromEncoding&& __from_encoding, _ToEncoding&& __to_encoding) {
		default_handler_t __handler {};

		return transcode_one<_OutputContainer>(::std::forward<_Input>(__input),
			::std::forward<_FromEncoding>(__from_encoding), ::std::forward<_ToEncoding>(__to_encoding), __handler);
	}

	//////
	/// @brief Converts the code units of the given input view through the from encoding to code units of the to
	/// encoding for the output, which is then returned in a result structure with additional information about
	/// success.
	///
	/// @tparam _OutputContainer The container to default-construct and serialize data into. Typically, a fixed-size
	/// container of some sort.
	///
	/// @param[in]     __input An input_view to read code units from and use in the decode operation that will
	/// produce intermediate code points.
	/// @param[in]     __to_encoding The encoding that will be used to encode the intermediate code points into the
	/// final code units.
	///
	/// @returns An `_OutputContainer` with the result, regardless of whether an error occurs or not. If you are
	/// looking for error information and not just a quick one-off conversion function, please use
	/// ztd::text::transcode_one_to or ztd::text::transcode_one_into.
	///
	/// @remarks This function creates both: a `from_error_handler` from a class like ztd::text::default_handler_t,
	/// but that is marked as careless since you did not explicitly provide it; and, a `from_encoding` derived from
	/// the @p
	/// "__input"'s `value_type.` The careless marking matters for lossy conversions that are not injective.
	//////
	template <typename _OutputContainer = void, typename _Input, typename _ToEncoding>
	constexpr auto transcode_one(_Input&& __input, _ToEncoding&& __to_encoding) {
		using _UInput   = remove_cvref_t<_Input>;
		using _CodeUnit = ranges::range_value_type_t<_UInput>;
#if ZTD_IS_ON(ZTD_STD_LIBRARY_IS_CONSTANT_EVALUATED)
		if (::std::is_constant_evaluated()) {
			using _UFromEncoding = default_consteval_code_unit_encoding_t<_CodeUnit>;
			_UFromEncoding __from_encoding {};
			return transcode_one<_OutputContainer>(
				::std::forward<_Input>(__input), __from_encoding, ::std::forward<_ToEncoding>(__to_encoding));
		}
		else
#endif
		{
			using _UFromEncoding = default_code_unit_encoding_t<_CodeUnit>;

			_UFromEncoding __from_encoding {};
			return transcode_one<_OutputContainer>(
				::std::forward<_Input>(__input), __from_encoding, ::std::forward<_ToEncoding>(__to_encoding));
		}
	}

	//////
	/// @}

	ZTD_TEXT_INLINE_ABI_NAMESPACE_CLOSE_I_
}} // namespace ztd::text

#include <ztd/epilogue.hpp>

#endif // ZTD_TEXT_TRANSCODE_ONE_HPP
