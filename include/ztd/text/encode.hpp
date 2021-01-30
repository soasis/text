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
// =============================================================================

#pragma once

#ifndef ZTD_TEXT_ENCODE_HPP
#define ZTD_TEXT_ENCODE_HPP

#include <ztd/text/code_unit.hpp>
#include <ztd/text/encode_result.hpp>
#include <ztd/text/error_handler.hpp>
#include <ztd/text/default_encoding.hpp>
#include <ztd/text/state.hpp>
#include <ztd/text/unbounded.hpp>
#include <ztd/text/is_unicode_code_point.hpp>

#include <ztd/text/detail/is_lossless.hpp>
#include <ztd/text/detail/encoding_range.hpp>
#include <ztd/text/detail/type_traits.hpp>
#include <ztd/text/detail/span.hpp>

#include <string>
#include <vector>
#include <string_view>

namespace ztd { namespace text {
	ZTD_TEXT_INLINE_ABI_NAMESPACE_OPEN_I_

	//////
	/// @addtogroup ztd_text_encode ztd::text::encode[_into]
	/// @brief These functions convert from a view of input code points into a view of output code units using either
	/// the inferred or specified encoding. If no error handler is provided, the equivalent of the @ref
	/// ztd::text::default_handler is used by default. If no associated state is provided for the encoding, one will be
	/// created with automatic storage duration (as a "stack" variable) for the provided encoding.
	/// @{
	//////

	//////
	/// @brief Converts the code points of the given @p __input view through the encoding to code units into the  @p
	/// __output view.
	///
	/// @param[in]     __input An input_view to read code points from and use in the encode operation that will
	/// produce code units.
	/// @param[in]     __encoding The encoding that will be used to encode the input's code points into
	/// output code units.
	/// @param[in]     __output An output_view to write code units to as the result of the encode operation from the
	/// intermediate code points.
	/// @param[in]     __error_handler The error handlers for the from and to encodings,
	/// respectively.
	/// @param[in,out] __state A reference to the associated state for the @p __encoding 's encode step.
	///
	/// @result A ztd::text::encode_result object that contains references to @p __state.
	///
	/// @remark This function detects whether or not the ADL extension point @c text_encode can be called with the
	/// provided parameters. If so, it will use that ADL extension point over the default implementation. Otherwise, it
	/// will loop over the two encodings and attempt to encode by repeatedly calling the encoding's required @c
	/// encode_one function.
	//////
	template <typename _Input, typename _Encoding, typename _Output, typename _ErrorHandler, typename _State>
	constexpr auto encode_into(_Input&& __input, _Encoding&& __encoding, _Output&& __output,
		_ErrorHandler&& __error_handler, _State& __state) {
		if constexpr (__detail::__is_detected_v<__detail::__detect_adl_text_encode, _Input, _Encoding, _Output,
			              _ErrorHandler, _State>) {
			return text_encode(::std::forward<_Input>(__input), ::std::forward<_Encoding>(__encoding),
				::std::forward<_Output>(__output), ::std::forward<_ErrorHandler>(__error_handler), __state);
		}
		else if constexpr (__detail::__is_detected_v<__detail::__detect_adl_internal_text_encode, _Input, _Encoding,
			                   _Output, _ErrorHandler, _State>) {
			return __text_encode(::std::forward<_Input>(__input), ::std::forward<_Encoding>(__encoding),
				::std::forward<_Output>(__output), ::std::forward<_ErrorHandler>(__error_handler), __state);
		}
		else {
			using _UInput         = __detail::__remove_cvref_t<_Input>;
			using _UOutput        = __detail::__remove_cvref_t<_Output>;
			using _InputValueType = __detail::__range_value_type_t<_UInput>;
			// using _OutputValueType = __detail::__range_value_type_t<_UOutput>;
			using _IntermediateInput  = __detail::__reconstruct_t<::std::conditional_t<::std::is_array_v<_UInput>,
                    ::std::conditional_t<__detail::__is_character_v<_InputValueType>,
                         ::std::basic_string_view<_InputValueType>, ::std::span<const _InputValueType>>,
                    _UInput>>;
			using _IntermediateOutput = __detail::__reconstruct_t<_UOutput>;
			using _Result             = decltype(__encoding.encode_one(::std::declval<_IntermediateInput>(),
                    ::std::declval<_IntermediateOutput>(), __error_handler, __state));
			using _WorkingInput       = __detail::__remove_cvref_t<decltype(std::declval<_Result>().input)>;
			using _WorkingOutput      = __detail::__remove_cvref_t<decltype(std::declval<_Result>().output)>;
			using _UEncoding          = __detail::__remove_cvref_t<_Encoding>;
			using _UErrorHandler      = __detail::__remove_cvref_t<_ErrorHandler>;

			static_assert(__detail::__is_encode_lossless_or_deliberate_v<_UEncoding, _UErrorHandler>,
				"This encode is a lossy, non-injective operation. This means you may lose data that you did not "
				"intend "
				"to lose; specify a 'handler' error handler parameter to encode(in, encoding, handler, ...) or "
				"encode_into(in, encoding, out, handler, ...) explicitly in order to bypass this.");

			_WorkingInput __working_input(
				__detail::__reconstruct(::std::in_place_type<_WorkingInput>, ::std::forward<_Input>(__input)));
			_WorkingOutput __working_output(
				__detail::__reconstruct(::std::in_place_type<_WorkingOutput>, ::std::forward<_Output>(__output)));
			bool __handled_error = false;

			for (;;) {
				auto __result = __encoding.encode_one(__working_input, __working_output, __error_handler, __state);
				if (__result.error_code != encoding_error::ok) {
					return __result;
				}
				__handled_error |= __result.handled_error;
				__working_input  = ::std::move(__result.input);
				__working_output = ::std::move(__result.output);
				if (__detail::__adl::__adl_empty(__result.input)) {
					break;
				}
			}
			return _Result(
				::std::move(__working_input), ::std::move(__working_output), __state, encoding_error::ok, false);
		}
	}

	//////
	/// @brief Converts the code points of the given @p __input view through the encoding to code units into the  @p
	/// __output view.
	///
	/// @param[in]     __input An input_view to read code points from and use in the encode operation that will
	/// produce code units.
	/// @param[in]     __encoding The encoding that will be used to encode the input's code points into
	/// output code units.
	/// @param[in]     __output An output_view to write code units to as the result of the encode operation from the
	/// intermediate code points.
	/// @param[in]     __error_handler The error handlers for the from and to encodings,
	/// respectively.
	///
	/// @result A ztd::text::stateless_encode_result object that contains references to @p __state.
	///
	/// @remark Creates a default @c state using ztd::text::make_encode_state.
	//////
	template <typename _Input, typename _Encoding, typename _Output, typename _ErrorHandler>
	constexpr auto encode_into(
		_Input&& __input, _Encoding&& __encoding, _Output&& __output, _ErrorHandler&& __error_handler) {
		using _UEncoding = __detail::__remove_cvref_t<_Encoding>;
		using _State     = encoding_encode_state_t<_UEncoding>;

		_State __state         = make_encode_state(__encoding);
		auto __stateful_result = encode_into(::std::forward<_Input>(__input), ::std::forward<_Encoding>(__encoding),
			::std::forward<_Output>(__output), ::std::forward<_ErrorHandler>(__error_handler), __state);
		return __detail::__slice_to_stateless(::std::move(__stateful_result));
	}

	//////
	/// @brief Converts the code points of the given @p __input view through the encoding to code units into the  @p
	/// __output view.
	///
	/// @param[in]     __input An input_view to read code points from and use in the encode operation that will
	/// produce code units.
	/// @param[in]     __encoding The encoding that will be used to encode the input's code points into
	/// output code units.
	/// @param[in]     __output An output_view to write code units to as the result of the encode operation from the
	/// intermediate code points.
	///
	/// @result A ztd::text::stateless_encode_result object that contains references to @p __state.
	///
	/// @remark Creates a default @c error_handler that is similar to ztd::text::default_handler, but marked as
	/// careless.
	//////
	template <typename _Input, typename _Encoding, typename _Output>
	constexpr auto encode_into(_Input&& __input, _Encoding&& __encoding, _Output&& __output) {
		__detail::__careless_handler __handler {};
		return encode_into(::std::forward<_Input>(__input), ::std::forward<_Encoding>(__encoding),
			::std::forward<_Output>(__output), __handler);
	}

	//////
	/// @brief Converts the code points of the given @p __input view through the encoding to code units into the  @p
	/// __output view.
	///
	/// @param[in]     __input An input_view to read code points from and use in the encode operation that will
	/// produce code units.
	/// @param[in]     __output An output_view to write code units to as the result of the encode operation from the
	/// intermediate code points.
	///
	/// @result A ztd::text::stateless_encode_result object that contains references to @p __state.
	///
	/// @remark Creates a default @c encoding by figuring out the @c value_type of the @p __input, then passing that
	/// type into ztd::text::default_code_point_encoding_t. That encoding is that used to encode the input code points,
	/// by default.
	//////
	template <typename _Input, typename _Output>
	constexpr auto encode_into(_Input&& __input, _Output&& __output) {
		using _UInput   = __detail::__remove_cvref_t<_Input>;
		using _Encoding = default_code_point_encoding_t<__detail::__range_value_type_t<_UInput>>;
		_Encoding __encoding {};
		return encode_into(::std::forward<_Input>(__input), __encoding, ::std::forward<_Output>(__output));
	}

	//////
	/// @brief Converts the code points of the given @p __input view through the encoding to code units in the
	/// specified @p _OutputContainer type.
	///
	/// @tparam _OutputContainer The container type to serialize data into.
	///
	/// @param[in]     __input An input_view to read code points from and use in the encode operation that will
	/// produce code units.
	/// @param[in]     __encoding The encoding that will be used to encode the input's code points into
	/// output code units.
	/// @param[in]     __error_handler The error handlers for the from and to encodings,
	/// respectively.
	/// @param[in,out] __state A reference to the associated state for the @p __encoding 's encode step.
	///
	/// @result A ztd::text::encode_result object that contains references to @p __state and an output of type @p
	/// _OutputContainer.
	///
	/// @remark This function detects creates a container of type @p _OutputContainer and uses a typical @c
	/// std::back_inserter or @c std::push_back_inserter to fill in elements as it is written to. The result is then
	/// returned, with the @c .output value put into the container.
	//////
	template <typename _OutputContainer, typename _Input, typename _Encoding, typename _ErrorHandler, typename _State>
	constexpr auto encode_to(
		_Input&& __input, _Encoding&& __encoding, _ErrorHandler&& __error_handler, _State& __state) {

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
		auto __stateful_result = encode_into(::std::forward<_Input>(__input), ::std::forward<_Encoding>(__encoding),
			::std::move(__insert_view), ::std::forward<_ErrorHandler>(__error_handler), __state);
		return __detail::__replace_result_output(::std::move(__stateful_result), ::std::move(__output));
	}

	//////
	/// @brief Converts the code points of the given @p __input view through the encoding to code units in the
	/// specified @p _OutputContainer type.
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
	/// @result A ztd::text::stateless_encode_result object whose output is of type @p _OutputContainer.
	///
	/// @remark This function creates a @c state using ztd::text::make_encode_state.
	//////
	template <typename _OutputContainer, typename _Input, typename _Encoding, typename _ErrorHandler>
	constexpr auto encode_to(_Input&& __input, _Encoding&& __encoding, _ErrorHandler&& __error_handler) {
		using _UEncoding = __detail::__remove_cvref_t<_Encoding>;
		using _State     = encoding_encode_state_t<_UEncoding>;

		_State __state         = make_encode_state(__encoding);
		auto __stateful_result = encode_to<_OutputContainer>(::std::forward<_Input>(__input),
			::std::forward<_Encoding>(__encoding), ::std::forward<_ErrorHandler>(__error_handler), __state);
		return __detail::__slice_to_stateless(::std::move(__stateful_result));
	}

	//////
	/// @brief Converts the code points of the given @p __input view through the encoding to code units in the
	/// specified @p _OutputContainer type.
	///
	/// @tparam _OutputContainer The container type to serialize data into.
	///
	/// @param[in]     __input An input_view to read code points from and use in the encode operation that will
	/// produce code units.
	/// @param[in]     __encoding The encoding that will be used to encode the input's code points into
	/// output code units.
	///
	/// @result A ztd::text::stateless_encode_result object whose output is of type @p _OutputContainer.
	///
	/// @remark This function creates a @c handler using ztd::text::default_handler, but marks it as careless.
	//////
	template <typename _OutputContainer, typename _Input, typename _Encoding>
	constexpr auto encode_to(_Input&& __input, _Encoding&& __encoding) {
		__detail::__careless_handler __handler {};
		return encode_to<_OutputContainer>(
			::std::forward<_Input>(__input), ::std::forward<_Encoding>(__encoding), __handler);
	}

	//////
	/// @brief Converts the code points of the given @p __input view through the encoding to code units in the
	/// specified @p _OutputContainer type.
	///
	/// @tparam _OutputContainer The container type to serialize data into.
	///
	/// @param[in]     __input An input_view to read code points from and use in the encode operation that will
	/// produce code units.
	///
	/// @result A ztd::text::stateless_encode_result object whose output is of type @p _OutputContainer.
	///
	/// @remark This function creates an @c encoding by using the @c value_type of the @p __input which is then passed
	/// through the ztd::text::default_code_point_encoding type to get the default desired encoding.
	//////
	template <typename _OutputContainer, typename _Input>
	constexpr auto encode_to(_Input&& __input) {
		using _UInput   = __detail::__remove_cvref_t<_Input>;
		using _Encoding = default_code_point_encoding_t<__detail::__range_value_type_t<_UInput>>;
		_Encoding __encoding {};
		return encode_to<_OutputContainer>(::std::forward<_Input>(__input), __encoding);
	}

	//////
	/// @brief Converts the code points of the given @p __input view through the encoding to code units in the
	/// specified @p _OutputContainer type.
	///
	/// @tparam _OutputContainer The container type to serialize data into.
	///
	/// @param[in]     __input An input_view to read code points from and use in the encode operation that will
	/// produce code units.
	/// @param[in]     __encoding The encoding that will be used to encode the input's code points into
	/// output code units.
	/// @param[in]     __error_handler The error handlers for the from and to encodings,
	/// respectively.
	/// @param[in,out] __state A reference to the associated state for the @p __encoding 's encode step.
	///
	/// @result An object of type @p _OutputContainer .
	///
	/// @remark This function detects creates a container of type @p _OutputContainer and uses a typical @c
	/// std::back_inserter or @c std::push_back_inserter to fill in elements as it is written to.
	//////
	template <typename _OutputContainer, typename _Input, typename _Encoding, typename _ErrorHandler, typename _State>
	constexpr auto encode(_Input&& __input, _Encoding&& __encoding, _ErrorHandler&& __error_handler, _State& __state) {

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
		auto __stateful_result = encode_into(::std::forward<_Input>(__input), ::std::forward<_Encoding>(__encoding),
			::std::move(__insert_view), ::std::forward<_ErrorHandler>(__error_handler), __state);
		(void)__stateful_result;
		return __output;
	}

	//////
	/// @brief Converts the code points of the given @p __input view through the encoding to code units in the
	/// specified @p _OutputContainer type.
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
	/// @result An object of type @p _OutputContainer .
	///
	/// @remark This function creates a @c state using ztd::text::make_encode_state.
	//////
	template <typename _OutputContainer, typename _Input, typename _Encoding, typename _ErrorHandler>
	constexpr auto encode(_Input&& __input, _Encoding&& __encoding, _ErrorHandler&& __error_handler) {
		using _UEncoding = __detail::__remove_cvref_t<_Encoding>;
		using _State     = encoding_encode_state_t<_UEncoding>;

		_State __state = make_encode_state(__encoding);
		return encode<_OutputContainer>(::std::forward<_Input>(__input), ::std::forward<_Encoding>(__encoding),
			::std::forward<_ErrorHandler>(__error_handler), __state);
	}

	//////
	/// @brief Converts the code points of the given @p __input view through the encoding to code units in the
	/// specified @p _OutputContainer type.
	///
	/// @tparam _OutputContainer The container type to serialize data into.
	///
	/// @param[in]     __input An input_view to read code points from and use in the encode operation that will
	/// produce code units.
	/// @param[in]     __encoding The encoding that will be used to encode the input's code points into
	/// output code units.
	///
	/// @result An object of type @p _OutputContainer .
	///
	/// @remark This function creates a @c handler using ztd::text::default_handler, but marks it as careless.
	//////
	template <typename _OutputContainer, typename _Input, typename _Encoding>
	constexpr auto encode(_Input&& __input, _Encoding&& __encoding) {
		__detail::__careless_handler __handler {};
		return encode<_OutputContainer>(
			::std::forward<_Input>(__input), ::std::forward<_Encoding>(__encoding), __handler);
	}

	//////
	/// @brief Converts the code points of the given @p __input view through the encoding to code units in the
	/// specified @p _OutputContainer type.
	///
	/// @tparam _OutputContainer The container type to serialize data into.
	///
	/// @param[in]     __input An input_view to read code points from and use in the encode operation that will
	/// produce code units.
	///
	/// @result An object of type @p _OutputContainer .
	///
	/// @remark This function creates an @c encoding by using the @c value_type of the @p __input which is then passed
	/// through the ztd::text::default_code_point_encoding type to get the default desired encoding.
	//////
	template <typename _OutputContainer, typename _Input>
	constexpr auto encode(_Input&& __input) {
		using _UInput   = __detail::__remove_cvref_t<_Input>;
		using _Encoding = default_code_point_encoding_t<__detail::__range_value_type_t<_UInput>>;
		_Encoding __encoding {};
		return encode<_OutputContainer>(::std::forward<_Input>(__input), __encoding);
	}

	//////
	/// @brief Converts the code points of the given @p __input view through the encoding to code units in a @c
	/// std::vector or @c std::basic_string .
	///
	/// @param[in]     __input An input_view to read code points from and use in the encode operation that will
	/// produce code units.
	/// @param[in]     __encoding The encoding that will be used to encode the input's code points into
	/// output code units.
	/// @param[in]     __error_handler The error handlers for the from and to encodings,
	/// respectively.
	/// @param[in,out] __state A reference to the associated state for the @p __encoding 's encode step.
	///
	/// @result An object of type @c std::vector or @c std::basic_string , whichever is more appropriate for the output
	/// code unt type.
	///
	/// @remark This function detects creates a container of either @c std::Vector (when @c std::byte is involved) or
	/// @c std::basic_string and uses a typical @c std::back_inserter or @c std::push_back_inserter to fill in elements
	/// as it is written to.
	//////
	template <typename _Input, typename _Encoding, typename _ErrorHandler, typename _State>
	constexpr auto encode(_Input&& __input, _Encoding&& __encoding, _ErrorHandler&& __error_handler, _State& __state) {
		using _UEncoding = __detail::__remove_cvref_t<_Encoding>;
		using _CodeUnit  = encoding_code_unit_t<_UEncoding>;
		using _OutputContainer
			= ::std::conditional_t<is_unicode_code_point_v<_CodeUnit> || __detail::__is_character_v<_CodeUnit>,
			     ::std::basic_string<_CodeUnit>, ::std::vector<_CodeUnit>>;

		return encode<_OutputContainer>(::std::forward<_Input>(__input), ::std::forward<_Encoding>(__encoding),
			::std::forward<_ErrorHandler>(__error_handler), __state);
	}

	//////
	/// @brief Converts the code points of the given @p __input view through the encoding to code units in a @c
	/// std::vector or @c std::basic_string .
	///
	/// @param[in]     __input An input_view to read code points from and use in the encode operation that will
	/// produce code units.
	/// @param[in]     __encoding The encoding that will be used to encode the input's code points into
	/// output code units.
	/// @param[in]     __error_handler The error handlers for the from and to encodings,
	/// respectively.
	///
	/// @result An object of type @c std::vector or @c std::basic_string , whichever is more appropriate for the output
	/// code unt type.
	///
	/// @remark This function creates a @c state using ztd::text::make_encode_state.
	//////
	template <typename _Input, typename _Encoding, typename _ErrorHandler>
	constexpr auto encode(_Input&& __input, _Encoding&& __encoding, _ErrorHandler&& __error_handler) {
		using _UEncoding = __detail::__remove_cvref_t<_Encoding>;
		using _State     = encoding_encode_state_t<_UEncoding>;

		_State __state = make_encode_state(__encoding);
		return encode(::std::forward<_Input>(__input), ::std::forward<_Encoding>(__encoding),
			::std::forward<_ErrorHandler>(__error_handler), __state);
	}

	//////
	/// @brief Converts the code points of the given @p __input view through the encoding to code units in a @c
	/// std::vector or @c std::basic_string .
	///
	/// @param[in]     __input An input_view to read code points from and use in the encode operation that will
	/// produce code units.
	/// @param[in]     __encoding The encoding that will be used to encode the input's code points into
	/// output code units.
	///
	/// @result An object of type @c std::vector or @c std::basic_string , whichever is more appropriate for the output
	/// code unt type.
	///
	/// @remark This function creates a @c handler using ztd::text::default_handler, but marks it as careless.
	//////
	template <typename _Input, typename _Encoding>
	constexpr auto encode(_Input&& __input, _Encoding&& __encoding) {
		__detail::__careless_handler __handler {};
		return encode(::std::forward<_Input>(__input), ::std::forward<_Encoding>(__encoding), __handler);
	}

	//////
	/// @brief Converts the code points of the given @p __input view through the encoding to code units in a @c
	/// std::vector or @c std::basic_string .
	///
	/// @param[in]     __input An input_view to read code points from and use in the encode operation that will
	/// produce code units.
	///
	/// @result An object of type @c std::vector or @c std::basic_string , whichever is more appropriate for the output
	/// code unt type.
	///
	/// @remark This function creates an @c encoding by using the @c value_type of the @p __input which is then passed
	/// through the ztd::text::default_code_point_encoding type to get the default desired encoding.
	//////
	template <typename _Input>
	constexpr auto encode(_Input&& __input) {
		using _UInput   = __detail::__remove_cvref_t<_Input>;
		using _Encoding = default_code_point_encoding_t<__detail::__range_value_type_t<_UInput>>;
		_Encoding __encoding {};
		return encode(::std::forward<_Input>(__input), __encoding);
	}

	//////
	/// @}
	//////

	ZTD_TEXT_INLINE_ABI_NAMESPACE_CLOSE_I_
}} // namespace ztd::text

#endif // ZTD_TEXT_ENCODE_HPP
