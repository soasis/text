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

#ifndef ZTD_TEXT_ENCODE_HPP
#define ZTD_TEXT_ENCODE_HPP

#include <ztd/text/version.hpp>

#include <ztd/text/code_unit.hpp>
#include <ztd/text/encode_result.hpp>
#include <ztd/text/error_handler.hpp>
#include <ztd/text/default_encoding.hpp>
#include <ztd/text/state.hpp>
#include <ztd/text/is_unicode_code_point.hpp>
#include <ztd/text/detail/is_lossless.hpp>
#include <ztd/text/detail/encoding_range.hpp>
#include <ztd/text/detail/transcode_routines.hpp>
#include <ztd/text/detail/span_or_reconstruct.hpp>
#include <ztd/text/detail/forward_if_move_only.hpp>

#include <ztd/ranges/unbounded.hpp>
#include <ztd/ranges/detail/insert_bulk.hpp>
#include <ztd/idk/span.hpp>
#include <ztd/idk/type_traits.hpp>
#include <ztd/idk/char_traits.hpp>

#include <string>
#include <vector>

#include <ztd/prologue.hpp>

namespace ztd { namespace text {
	ZTD_TEXT_INLINE_ABI_NAMESPACE_OPEN_I_

	//////
	/// @addtogroup ztd_text_encode ztd::text::encode[_into]
	/// @brief These functions convert from a view of input code points into a view of output code units using
	/// either the inferred or specified encoding. If no error handler is provided, the equivalent of the
	/// ztd::text::default_handler_t is used by default. If no associated state is provided for the encoding, one
	/// will be created with automatic storage duration (as a "stack" variable) for the provided encoding.
	/// @{

	//////
	/// @brief Converts the code points of the given `__input` view through the encoding to code units into the
	/// `__output` view.
	///
	/// @param[in]     __input An input_view to read code points from and use in the encode operation that will
	/// produce code units.
	/// @param[in]     __encoding The encoding that will be used to encode the input's code points into
	/// output code units.
	/// @param[in]     __output An output_view to write code units to as the result of the encode operation from
	/// the intermediate code points.
	/// @param[in]     __error_handler The error handlers for the from and to encodings,
	/// respectively.
	/// @param[in,out] __state A reference to the associated state for the `__encoding` 's encode step.
	///
	/// @result A ztd::text::encode_result object that contains references to `__state`.
	///
	/// @remarks This function does not attempt to call any extension points for encoding. It simply uses the
	/// encoding and attempts to encode by repeatedly calling the encoding's required `encode_one` function.
	template <typename _Input, typename _Encoding, typename _Output, typename _ErrorHandler, typename _State>
	constexpr auto basic_encode_into(_Input&& __input, _Encoding&& __encoding, _Output&& __output,
		_ErrorHandler&& __error_handler, _State& __state) {
		using _IntermediateInput  = __txt_detail::__string_view_or_span_or_reconstruct_t<_Input>;
		using _IntermediateOutput = ranges::reconstruct_t<_Output>;
		using _Result             = decltype(__encoding.encode_one(
               ::std::declval<_IntermediateInput>(), ::std::declval<_IntermediateOutput>(), __error_handler, __state));
		using _WorkingInput       = remove_cvref_t<decltype(::std::declval<_Result>().input)>;
		using _WorkingOutput      = remove_cvref_t<decltype(::std::declval<_Result>().output)>;
		using _UEncoding          = remove_cvref_t<_Encoding>;
		using _UErrorHandler      = remove_cvref_t<_ErrorHandler>;

		static_assert(__txt_detail::__is_encode_lossless_or_deliberate_v<_UEncoding, _UErrorHandler>,
			ZTD_TEXT_LOSSY_ENCODE_MESSAGE_I_);

		_WorkingInput __working_input
			= __txt_detail::__string_view_or_span_or_reconstruct(::std::forward<_Input>(__input));
		_WorkingOutput __working_output
			= ranges::reconstruct(::std::in_place_type<_WorkingOutput>, ::std::forward<_Output>(__output));
		::std::size_t __error_count = 0;

		for (;;) {
			auto __result = __encoding.encode_one(
				::std::move(__working_input), ::std::move(__working_output), __error_handler, __state);
			if (__result.error_code != encoding_error::ok) {
				return __result;
			}
			__error_count += __result.error_count;
			__working_input  = ::std::move(__result.input);
			__working_output = ::std::move(__result.output);
			if (::ztd::ranges::empty(__result.input)) {
				if (!::ztd::text::is_state_complete(__encoding, __state)) {
					continue;
				}
				break;
			}
		}
		return _Result(
			::std::move(__working_input), ::std::move(__working_output), __state, encoding_error::ok, __error_count);
	}

	//////
	/// @brief Converts the code points of the given `__input` view through the encoding to code units into the
	/// `__output` view.
	///
	/// @param[in]     __input An input_view to read code points from and use in the encode operation that will
	/// produce code units.
	/// @param[in]     __encoding The encoding that will be used to encode the input's code points into
	/// output code units.
	/// @param[in]     __output An output_view to write code units to as the result of the encode operation from
	/// the intermediate code points.
	/// @param[in]     __error_handler The error handlers for the from and to encodings,
	/// respectively.
	/// @param[in,out] __state A reference to the associated state for the `__encoding` 's encode step.
	///
	/// @result A ztd::text::encode_result object that contains references to `__state`.
	///
	/// @remarks This function detects whether or not the ADL extension point `text_encode` can be called with
	/// the provided parameters. If so, it will use that ADL extension point over the default implementation.
	/// Otherwise, it will loop over the two encodings and attempt to encode by repeatedly calling the encoding's
	/// required `encode_one` function.
	template <typename _Input, typename _Encoding, typename _Output, typename _ErrorHandler, typename _State>
	constexpr auto encode_into(_Input&& __input, _Encoding&& __encoding, _Output&& __output,
		_ErrorHandler&& __error_handler, _State& __state) {
		if constexpr (is_detected_v<__txt_detail::__detect_adl_text_encode, _Input, _Encoding, _Output, _ErrorHandler,
			              _State>) {
			return text_encode(::ztd::tag<remove_cvref_t<_Encoding>> {}, ::std::forward<_Input>(__input),
				::std::forward<_Encoding>(__encoding), ::std::forward<_Output>(__output),
				::std::forward<_ErrorHandler>(__error_handler), __state);
		}
		else if constexpr (is_detected_v<__txt_detail::__detect_adl_internal_text_encode, _Input, _Encoding, _Output,
			                   _ErrorHandler, _State>) {
			return __text_encode(::ztd::tag<remove_cvref_t<_Encoding>> {}, ::std::forward<_Input>(__input),
				::std::forward<_Encoding>(__encoding), ::std::forward<_Output>(__output),
				::std::forward<_ErrorHandler>(__error_handler), __state);
		}
		else {
			return basic_encode_into(::std::forward<_Input>(__input), ::std::forward<_Encoding>(__encoding),
				::std::forward<_Output>(__output), ::std::forward<_ErrorHandler>(__error_handler), __state);
		}
	}

	namespace __txt_detail {
		template <typename _Input, typename _Encoding, typename _OutputContainer, typename _ErrorHandler,
			typename _State>
		constexpr auto __intermediate_encode_to_storage(_Input&& __input, _Encoding&& __encoding,
			_OutputContainer& __output, _ErrorHandler&& __error_handler, _State& __state) {
			// Well, SHIT. Write into temporary, then serialize one-by-one/bulk to output.
			// I'll admit, this is HELLA work to support...
			using _UEncoding     = remove_cvref_t<_Encoding>;
			using _UErrorHandler = remove_cvref_t<_ErrorHandler>;
			constexpr ::std::size_t __intermediate_buffer_max
				= ZTD_TEXT_INTERMEDIATE_ENCODE_BUFFER_SIZE_I_(code_unit_t<_UEncoding>)
				     < max_code_units_v<_UEncoding>
				? max_code_units_v<_UEncoding>
				: ZTD_TEXT_INTERMEDIATE_ENCODE_BUFFER_SIZE_I_(code_unit_t<_UEncoding>);
			using _IntermediateValueType = code_unit_t<_UEncoding>;
			using _IntermediateInput     = __string_view_or_span_or_reconstruct_t<_Input>;
			using _InitialOutput         = ::ztd::span<_IntermediateValueType, __intermediate_buffer_max>;
			using _Output                = ::ztd::span<_IntermediateValueType>;
			using _Result                = decltype(__encoding.encode_one(
                    ::std::declval<_IntermediateInput>(), ::std::declval<_Output>(), __error_handler, __state));
			using _WorkingInput          = remove_cvref_t<decltype(::std::declval<_Result>().input)>;

			static_assert(__txt_detail::__is_encode_lossless_or_deliberate_v<_UEncoding, _UErrorHandler>,
				ZTD_TEXT_LOSSY_ENCODE_MESSAGE_I_);

			// We MUST use a temporary error handler
			// as well as a pass-throuugh handler if we end up with lots of intermediary input
			__progress_handler<::std::integral_constant<bool, is_ignorable_error_handler_v<_UErrorHandler>>,
				_UEncoding>
				__intermediate_handler {};
			__progress_handler<::std::integral_constant<bool, is_ignorable_error_handler_v<_UErrorHandler>>,
				_UEncoding>
				__unused_intermediate_handler {};

			_WorkingInput __working_input
				= __txt_detail::__string_view_or_span_or_reconstruct(::std::forward<_Input>(__input));
			_IntermediateValueType __intermediate_translation_buffer[__intermediate_buffer_max] {};

			for (;;) {
				// Ignore "out of output" errors and do our best to recover properly along the way...
				_InitialOutput __intermediate_initial_output(__intermediate_translation_buffer);
				auto __result = encode_into(::std::move(__working_input), __encoding, __intermediate_initial_output,
					__intermediate_handler, __state);
				_Output __intermediate_output(__intermediate_initial_output.data(), __result.output.data());
				ranges::__rng_detail::__container_insert_bulk(__output, __intermediate_output);
				if (__result.error_code == encoding_error::insufficient_output_space) {
					if (__intermediate_handler._M_code_units_progress_size() != 0) {
						// add any partially-unwritten characters to our output
						ranges::__rng_detail::__container_insert_bulk(
							__output, __intermediate_handler._M_code_units_progress());
					}
					else if (__intermediate_handler._M_code_points_progress_size() != 0) {
						// make sure to empty any read-but-unused characters here...!
						auto __unused_input_progress = __intermediate_handler._M_const_code_points_progress();
						_InitialOutput __unused_input_intermediate_initial_output(
							__intermediate_translation_buffer);
						auto __unused_input_result = encode_into(__unused_input_progress, __encoding,
							__unused_input_intermediate_initial_output, __unused_intermediate_handler, __state);
						_Output __unused_input_intermediate_output(
							__unused_input_intermediate_initial_output.data(),
							__unused_input_result.output.data());
						ranges::__rng_detail::__container_insert_bulk(
							__output, __unused_input_intermediate_output);
						if (__unused_input_result.error_code != encoding_error::ok) {
							// mill result through actual error handler!
							// adjust for the amount of code points potentially read from the input of
							// the unused buffer.
							::std::size_t __progress_offset
								= __unused_input_result.input.data() - __unused_input_progress.data();
							auto __final_progress
								= __intermediate_handler._M_code_points_progress().subspan(__progress_offset);
							auto __error_result = ::std::forward<_ErrorHandler>(__error_handler)(
								::std::forward<_Encoding>(__encoding), ::std::move(__result), __final_progress,
								__unused_intermediate_handler._M_code_units_progress());
							return __error_result;
						}
					}
					// loop around, we've got S P A C E for more
					__working_input = ::std::move(__result.input);
					continue;
				}
				if (__result.error_code != encoding_error::ok) {
					// mill result through actual error handler!
					auto __error_result
						= ::std::forward<_ErrorHandler>(__error_handler)(::std::forward<_Encoding>(__encoding),
						     ::std::move(__result), __intermediate_handler._M_code_points_progress(),
						     __intermediate_handler._M_code_units_progress());
					return __error_result;
				}
				if (::ztd::ranges::empty(__result.input)) {
					if (!::ztd::text::is_state_complete(__encoding, __state)) {
						continue;
					}
					return __result;
				}
			}
		}

		template <bool _OutputOnly, typename _OutputContainer, typename _Input, typename _Encoding,
			typename _ErrorHandler, typename _State>
		constexpr auto __encode_dispatch(
			_Input&& __input, _Encoding&& __encoding, _ErrorHandler&& __error_handler, _State& __state) {
			using _UEncoding = remove_cvref_t<_Encoding>;

			_OutputContainer __output {};
			if constexpr (is_detected_v<ranges::detect_adl_size, _Input>) {
				using _SizeType = decltype(::ztd::ranges::size(__input));
				if constexpr (is_detected_v<ranges::detect_reserve_with_size, _OutputContainer, _SizeType>) {
					_SizeType __output_size_hint = static_cast<_SizeType>(::ztd::ranges::size(__input));
					__output_size_hint *= (max_code_units_v<_UEncoding> > 1) ? (max_code_units_v<_UEncoding> / 2)
						                                                    : max_code_units_v<_UEncoding>;
					__output.reserve(__output_size_hint);
				}
			}
			if constexpr (__txt_detail::__is_encode_range_category_output_v<_UEncoding>) {
				// We can use the unbounded stuff
				using _BackInserterIterator = decltype(::std::back_inserter(::std::declval<_OutputContainer&>()));
				using _Unbounded            = ranges::unbounded_view<_BackInserterIterator>;
				_Unbounded __insert_view(::std::back_inserter(__output));
				auto __stateful_result = encode_into(__txt_detail::__forward_if_move_only<_Input>(__input),
					::std::forward<_Encoding>(__encoding), ::std::move(__insert_view),
					::std::forward<_ErrorHandler>(__error_handler), __state);
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
				auto __stateful_result = __txt_detail::__intermediate_encode_to_storage(
					::std::forward<_Input>(__input), ::std::forward<_Encoding>(__encoding), __output,
					::std::forward<_ErrorHandler>(__error_handler), __state);
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
	/// @brief Converts the code points of the given `__input` view through the encoding to code units into the
	/// `__output` view.
	///
	/// @param[in]     __input An input_view to read code points from and use in the encode operation that will
	/// produce code units.
	/// @param[in]     __encoding The encoding that will be used to encode the input's code points into
	/// output code units.
	/// @param[in]     __output An output_view to write code units to as the result of the encode operation from
	/// the intermediate code points.
	/// @param[in]     __error_handler The error handlers for the from and to encodings,
	/// respectively.
	///
	/// @result A ztd::text::stateless_encode_result object that contains references to `__state`.
	///
	/// @remarks Creates a default `state` using ztd::text::make_encode_state.
	template <typename _Input, typename _Encoding, typename _Output, typename _ErrorHandler>
	constexpr auto encode_into(
		_Input&& __input, _Encoding&& __encoding, _Output&& __output, _ErrorHandler&& __error_handler) {
		using _UEncoding = remove_cvref_t<_Encoding>;
		using _State     = encode_state_t<_UEncoding>;

		_State __state         = make_encode_state(__encoding);
		auto __stateful_result = encode_into(::std::forward<_Input>(__input), ::std::forward<_Encoding>(__encoding),
			::std::forward<_Output>(__output), ::std::forward<_ErrorHandler>(__error_handler), __state);
		return __txt_detail::__slice_to_stateless(::std::move(__stateful_result));
	}

	//////
	/// @brief Converts the code points of the given `__input` view through the encoding to code units into the
	/// `__output` view.
	///
	/// @param[in]     __input An input_view to read code points from and use in the encode operation that will
	/// produce code units.
	/// @param[in]     __encoding The encoding that will be used to encode the input's code points into
	/// output code units.
	/// @param[in]     __output An output_view to write code units to as the result of the encode operation from
	/// the intermediate code points.
	///
	/// @result A ztd::text::stateless_encode_result object that contains references to `__state`.
	///
	/// @remarks Creates a default `error_handler` that is similar to ztd::text::default_handler_t, but marked as
	/// careless.
	template <typename _Input, typename _Encoding, typename _Output>
	constexpr auto encode_into(_Input&& __input, _Encoding&& __encoding, _Output&& __output) {
		default_handler_t __handler {};
		return encode_into(::std::forward<_Input>(__input), ::std::forward<_Encoding>(__encoding),
			::std::forward<_Output>(__output), __handler);
	}

	//////
	/// @brief Converts the code points of the given `__input` view through the encoding to code units into the
	/// `__output` view.
	///
	/// @param[in]     __input An input_view to read code points from and use in the encode operation that will
	/// produce code units.
	/// @param[in]     __output An output_view to write code units to as the result of the encode operation from
	/// the intermediate code points.
	///
	/// @result A ztd::text::stateless_encode_result object that contains references to `__state`.
	///
	/// @remarks Creates a default `encoding` by figuring out the `value_type` of the `__input`, then passing
	/// that type into ztd::text::default_code_point_encoding_t. That encoding is that used to encode the input
	/// code points, by default.
	template <typename _Input, typename _Output>
	constexpr auto encode_into(_Input&& __input, _Output&& __output) {
		using _UInput    = remove_cvref_t<_Input>;
		using _CodePoint = ranges::range_value_type_t<_UInput>;
#if ZTD_IS_ON(ZTD_STD_LIBRARY_IS_CONSTANT_EVALUATED)
		if (::std::is_constant_evaluated()) {
			// Use literal encoding instead, if we meet the right criteria
			using _Encoding = default_consteval_code_point_encoding_t<_CodePoint>;
			_Encoding __encoding {};
			return encode_into(::std::forward<_Input>(__input), __encoding, ::std::forward<_Output>(__output));
		}
		else
#endif
		{
			using _Encoding = default_code_point_encoding_t<_CodePoint>;
			_Encoding __encoding {};
			return encode_into(::std::forward<_Input>(__input), __encoding, ::std::forward<_Output>(__output));
		}
	}

	//////
	/// @brief Converts the code points of the given `__input` view through the encoding to code units in the
	/// specified `_OutputContainer` type.
	///
	/// @tparam _OutputContainer The container type to serialize data into.
	///
	/// @param[in]     __input An input_view to read code points from and use in the encode operation that will
	/// produce code units.
	/// @param[in]     __encoding The encoding that will be used to encode the input's code points into
	/// output code units.
	/// @param[in]     __error_handler The error handlers for the from and to encodings,
	/// respectively.
	/// @param[in,out] __state A reference to the associated state for the `__encoding` 's encode step.
	///
	/// @result A ztd::text::encode_result object that contains references to `__state` and an output of type
	/// `_OutputContainer`.
	///
	/// @remarks This function detects creates a container of type `_OutputContainer` and uses a typical @c
	/// std::back_inserter or `std::push_back_inserter` to fill in elements as it is written to. The result is
	/// then returned, with the `.output` value put into the container.
	template <typename _OutputContainer = void, typename _Input, typename _Encoding, typename _ErrorHandler,
		typename _State>
	constexpr auto encode_to(
		_Input&& __input, _Encoding&& __encoding, _ErrorHandler&& __error_handler, _State& __state) {
		using _UEncoding                = remove_cvref_t<_Encoding>;
		using _UOutputContainer         = remove_cvref_t<_OutputContainer>;
		using _OutputCodeUnit           = code_unit_t<_UEncoding>;
		constexpr bool _IsVoidContainer = ::std::is_void_v<_UOutputContainer>;
		constexpr bool _IsStringable
			= (is_char_traitable_v<_OutputCodeUnit> || is_unicode_code_point_v<_OutputCodeUnit>);
		if constexpr (_IsVoidContainer && _IsStringable) {
			// prevent instantiation errors with basic_string by boxing it inside of an "if constexpr"
			using _RealOutputContainer = ::std::basic_string<_OutputCodeUnit>;
			return __txt_detail::__encode_dispatch<false, _RealOutputContainer>(::std::forward<_Input>(__input),
				::std::forward<_Encoding>(__encoding), ::std::forward<_ErrorHandler>(__error_handler), __state);
		}
		else {
			using _RealOutputContainer
				= ::std::conditional_t<_IsVoidContainer, ::std::vector<_OutputCodeUnit>, _OutputContainer>;
			return __txt_detail::__encode_dispatch<false, _RealOutputContainer>(::std::forward<_Input>(__input),
				::std::forward<_Encoding>(__encoding), ::std::forward<_ErrorHandler>(__error_handler), __state);
		}
	}

	//////
	/// @brief Converts the code points of the given `__input` view through the encoding to code units in the
	/// specified `_OutputContainer` type.
	///
	/// @tparam _OutputContainer The container type to serialize data into.
	///
	/// @param[in]     __input An input_view to read code points from and use in the encode operation that will
	/// produce code units.
	/// @param[in]     __encoding The encoding that will be used to encode the input's code points into
	/// output code units.
	/// @param[in]     __error_handler The error handlers for the from and to encodings,
	/// respectively.
	///
	/// @result A ztd::text::stateless_encode_result object whose output is of type `_OutputContainer`.
	///
	/// @remarks This function creates a `state` using ztd::text::make_encode_state.
	template <typename _OutputContainer = void, typename _Input, typename _Encoding, typename _ErrorHandler>
	constexpr auto encode_to(_Input&& __input, _Encoding&& __encoding, _ErrorHandler&& __error_handler) {
		using _UEncoding = remove_cvref_t<_Encoding>;
		using _State     = encode_state_t<_UEncoding>;

		_State __state         = make_encode_state(__encoding);
		auto __stateful_result = encode_to<_OutputContainer>(::std::forward<_Input>(__input),
			::std::forward<_Encoding>(__encoding), ::std::forward<_ErrorHandler>(__error_handler), __state);
		return __txt_detail::__slice_to_stateless(::std::move(__stateful_result));
	}

	//////
	/// @brief Converts the code points of the given `__input` view through the encoding to code units in the
	/// specified `_OutputContainer` type.
	///
	/// @tparam _OutputContainer The container type to serialize data into.
	///
	/// @param[in]     __input An input_view to read code points from and use in the encode operation that will
	/// produce code units.
	/// @param[in]     __encoding The encoding that will be used to encode the input's code points into
	/// output code units.
	///
	/// @result A ztd::text::stateless_encode_result object whose output is of type `_OutputContainer`.
	///
	/// @remarks This function creates a `handler` using ztd::text::default_handler_t, but marks it as careless.
	template <typename _OutputContainer = void, typename _Input, typename _Encoding>
	constexpr auto encode_to(_Input&& __input, _Encoding&& __encoding) {
		default_handler_t __handler {};
		return encode_to<_OutputContainer>(
			::std::forward<_Input>(__input), ::std::forward<_Encoding>(__encoding), __handler);
	}

	//////
	/// @brief Converts the code points of the given `__input` view through the encoding to code units in the
	/// specified `_OutputContainer` type.
	///
	/// @tparam _OutputContainer The container type to serialize data into.
	///
	/// @param[in]     __input An input_view to read code points from and use in the encode operation that will
	/// produce code units.
	///
	/// @result A ztd::text::stateless_encode_result object whose output is of type `_OutputContainer`.
	///
	/// @remarks This function creates an `encoding` by using the `value_type` of the `__input` which is then
	/// passed through the ztd::text::default_code_point_encoding type to get the default desired encoding.
	template <typename _OutputContainer = void, typename _Input>
	constexpr auto encode_to(_Input&& __input) {
		using _UInput    = remove_cvref_t<_Input>;
		using _CodePoint = ranges::range_value_type_t<_UInput>;
#if ZTD_IS_ON(ZTD_STD_LIBRARY_IS_CONSTANT_EVALUATED)
		if (::std::is_constant_evaluated()) {
			// Use literal encoding instead, if we meet the right criteria
			using _Encoding = default_consteval_code_point_encoding_t<_CodePoint>;
			_Encoding __encoding {};
			return encode_to<_OutputContainer>(::std::forward<_Input>(__input), __encoding);
		}
		else
#endif
		{
			using _Encoding = default_code_point_encoding_t<_CodePoint>;
			_Encoding __encoding {};
			return encode_to<_OutputContainer>(::std::forward<_Input>(__input), __encoding);
		}
	}

	//////
	/// @brief Converts the code points of the given `__input` view through the encoding to code units in the
	/// specified `_OutputContainer` type.
	///
	/// @tparam _OutputContainer The container type to serialize data into.
	///
	/// @param[in]     __input An input_view to read code points from and use in the encode operation that will
	/// produce code units.
	/// @param[in]     __encoding The encoding that will be used to encode the input's code points into
	/// output code units.
	/// @param[in]     __error_handler The error handlers for the from and to encodings,
	/// respectively.
	/// @param[in,out] __state A reference to the associated state for the `__encoding` 's encode step.
	///
	/// @result An object of type `_OutputContainer` .
	///
	/// @remarks This function detects creates a container of type `_OutputContainer` and uses a typical @c
	/// std::back_inserter or `std::push_back_inserter` to fill in elements as it is written to.
	template <typename _OutputContainer = void, typename _Input, typename _Encoding, typename _ErrorHandler,
		typename _State>
	constexpr auto encode(_Input&& __input, _Encoding&& __encoding, _ErrorHandler&& __error_handler, _State& __state) {
		using _UEncoding                = remove_cvref_t<_Encoding>;
		using _UOutputContainer         = remove_cvref_t<_OutputContainer>;
		using _OutputCodeUnit           = code_unit_t<_UEncoding>;
		constexpr bool _IsVoidContainer = ::std::is_void_v<_UOutputContainer>;
		constexpr bool _IsStringable
			= (is_char_traitable_v<_OutputCodeUnit> || is_unicode_code_point_v<_OutputCodeUnit>);
		if constexpr (_IsVoidContainer && _IsStringable) {
			// prevent instantiation errors with basic_string by boxing it inside of an "if constexpr"
			using _RealOutputContainer = ::std::basic_string<_OutputCodeUnit>;
			return __txt_detail::__encode_dispatch<true, _RealOutputContainer>(::std::forward<_Input>(__input),
				::std::forward<_Encoding>(__encoding), ::std::forward<_ErrorHandler>(__error_handler), __state);
		}
		else {
			using _RealOutputContainer
				= ::std::conditional_t<_IsVoidContainer, ::std::vector<_OutputCodeUnit>, _OutputContainer>;
			return __txt_detail::__encode_dispatch<true, _RealOutputContainer>(::std::forward<_Input>(__input),
				::std::forward<_Encoding>(__encoding), ::std::forward<_ErrorHandler>(__error_handler), __state);
		}
	}

	//////
	/// @brief Converts the code points of the given `__input` view through the encoding to code units in the
	/// specified `_OutputContainer` type.
	///
	/// @tparam _OutputContainer The container type to serialize data into.
	///
	/// @param[in]     __input An input_view to read code points from and use in the encode operation that will
	/// produce code units.
	/// @param[in]     __encoding The encoding that will be used to encode the input's code points into
	/// output code units.
	/// @param[in]     __error_handler The error handlers for the from and to encodings,
	/// respectively.
	///
	/// @result An object of type `_OutputContainer` .
	///
	/// @remarks This function creates a `state` using ztd::text::make_encode_state.
	template <typename _OutputContainer = void, typename _Input, typename _Encoding, typename _ErrorHandler>
	constexpr auto encode(_Input&& __input, _Encoding&& __encoding, _ErrorHandler&& __error_handler) {
		using _UEncoding = remove_cvref_t<_Encoding>;
		using _State     = encode_state_t<_UEncoding>;

		_State __state = make_encode_state(__encoding);
		return encode<_OutputContainer>(::std::forward<_Input>(__input), ::std::forward<_Encoding>(__encoding),
			::std::forward<_ErrorHandler>(__error_handler), __state);
	}

	//////
	/// @brief Converts the code points of the given `__input` view through the encoding to code units in the
	/// specified `_OutputContainer` type.
	///
	/// @tparam _OutputContainer The container type to serialize data into.
	///
	/// @param[in]     __input An input_view to read code points from and use in the encode operation that will
	/// produce code units.
	/// @param[in]     __encoding The encoding that will be used to encode the input's code points into
	/// output code units.
	///
	/// @result An object of type `_OutputContainer` .
	///
	/// @remarks This function creates a `handler` using ztd::text::default_handler_t, but marks it as careless.
	template <typename _OutputContainer = void, typename _Input, typename _Encoding>
	constexpr auto encode(_Input&& __input, _Encoding&& __encoding) {
		default_handler_t __handler {};
		return encode<_OutputContainer>(
			::std::forward<_Input>(__input), ::std::forward<_Encoding>(__encoding), __handler);
	}

	//////
	/// @brief Converts the code points of the given `__input` view through the encoding to code units in the
	/// specified `_OutputContainer` type.
	///
	/// @tparam _OutputContainer The container type to serialize data into.
	///
	/// @param[in]     __input An input_view to read code points from and use in the encode operation that will
	/// produce code units.
	///
	/// @result An object of type `_OutputContainer` .
	///
	/// @remarks This function creates an `encoding` by using the `value_type` of the `__input` which is then
	/// passed through the ztd::text::default_code_point_encoding type to get the default desired encoding.
	template <typename _OutputContainer = void, typename _Input>
	constexpr auto encode(_Input&& __input) {
		using _UInput    = remove_cvref_t<_Input>;
		using _CodePoint = ranges::range_value_type_t<_UInput>;
#if ZTD_IS_ON(ZTD_STD_LIBRARY_IS_CONSTANT_EVALUATED)
		if (::std::is_constant_evaluated()) {
			// Use literal encoding instead, if we meet the right criteria
			using _Encoding = default_consteval_code_point_encoding_t<_CodePoint>;
			_Encoding __encoding {};
			return encode<_OutputContainer>(::std::forward<_Input>(__input), __encoding);
		}
		else
#endif
		{
			using _Encoding = default_code_point_encoding_t<_CodePoint>;
			_Encoding __encoding {};
			return encode<_OutputContainer>(::std::forward<_Input>(__input), __encoding);
		}
	}

	//////
	/// @}

	ZTD_TEXT_INLINE_ABI_NAMESPACE_CLOSE_I_
}} // namespace ztd::text

#include <ztd/epilogue.hpp>

#endif
