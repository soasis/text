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

#ifndef ZTD_TEXT_ENCODE_ONE_HPP
#define ZTD_TEXT_ENCODE_ONE_HPP

#include <ztd/text/version.hpp>

#include <ztd/text/code_unit.hpp>
#include <ztd/text/encode_result.hpp>
#include <ztd/text/error_handler.hpp>
#include <ztd/text/default_encoding.hpp>
#include <ztd/text/state.hpp>
#include <ztd/text/is_unicode_code_point.hpp>
#include <ztd/text/type_traits.hpp>
#include <ztd/text/detail/is_lossless.hpp>
#include <ztd/text/detail/encoding_range.hpp>
#include <ztd/text/detail/transcode_one.hpp>
#include <ztd/text/detail/span_or_reconstruct.hpp>
#include <ztd/text/detail/forward_if_move_only.hpp>

#include <ztd/ranges/unbounded.hpp>
#include <ztd/idk/span.hpp>
#include <ztd/fixed_container.hpp>
#include <ztd/ranges/detail/insert_bulk.hpp>

#include <ztd/prologue.hpp>

namespace ztd { namespace text {
	ZTD_TEXT_INLINE_ABI_NAMESPACE_OPEN_I_

	//////
	/// @addtogroup ztd_text_encode_one ztd::text::encode_one[_into/_to]
	/// @brief These functions convert from a view of input code points into a view of output code units using
	/// either the inferred or specified encoding. If no error handler is provided, the equivalent of the
	/// ztd::text::default_handler_t is used by default. If no associated state is provided for the encoding, one
	/// will be created with automatic storage duration (as a "stack" variable) for the provided encoding.
	/// @{
	//////

	//////
	/// @brief Converts a single indivisible unit of work's worth of code points of the given `__input` view through
	/// the encoding to code units into the @p __output view.
	///
	/// @param[in]     __input An input_view to read code points from and use in the encode_one operation that will
	/// produce code units.
	/// @param[in]     __encoding The encoding that will be used to encode_one the input's code points into
	/// output code units.
	/// @param[in]     __output An output_view to write code units to as the result of the encode_one operation from
	/// the intermediate code points.
	/// @param[in]     __error_handler The error handlers for the from and to encodings,
	/// respectively.
	/// @param[in,out] __state A reference to the associated state for the `__encoding` 's encode_one step.
	///
	/// @result A ztd::text::encode_one_result object that contains references to `__state`.
	///
	/// @remarks This function takes care of performing some "normalizations" of the output and input range types (like
	/// turning them into a span or string_view if at all recognizable or preferable).
	//////
	template <typename _Input, typename _Encoding, typename _Output, typename _ErrorHandler, typename _State>
	constexpr auto encode_one_into(_Input&& __input, _Encoding&& __encoding, _Output&& __output,
		_ErrorHandler&& __error_handler, _State& __state) {
		using _IntermediateInput  = __txt_detail::__string_view_or_span_or_reconstruct_t<_Input>;
		using _IntermediateOutput = ranges::reconstruct_t<_Output>;
		using _Result             = decltype(__encoding.encode_one_one(
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

		return __encoding.encode_one(
			::std::move(__working_input), ::std::move(__working_output), __error_handler, __state);
	}

	//////
	/// @brief Converts a single indivisible unit of work's worth of code points of the given `__input` view through
	/// the encoding to code units into the `__output` view.
	///
	/// @param[in]     __input An input_view to read code points from and use in the encode_one operation that will
	/// produce code units.
	/// @param[in]     __encoding The encoding that will be used to encode_one the input's code points into
	/// output code units.
	/// @param[in]     __output An output_view to write code units to as the result of the encode_one operation from
	/// the intermediate code points.
	/// @param[in]     __error_handler The error handlers for the from and to encodings,
	/// respectively.
	///
	/// @result A ztd::text::stateless_encode_one_result object that contains references to `__state`.
	///
	/// @remarks Creates a default `state` using ztd::text::make_encode_state.
	//////
	template <typename _Input, typename _Encoding, typename _Output, typename _ErrorHandler>
	constexpr auto encode_one_into(
		_Input&& __input, _Encoding&& __encoding, _Output&& __output, _ErrorHandler&& __error_handler) {
		using _UEncoding = remove_cvref_t<_Encoding>;
		using _State     = encode_state_t<_UEncoding>;

		_State __state = make_encode_state(__encoding);
		auto __stateful_result
			= encode_one_into(::std::forward<_Input>(__input), ::std::forward<_Encoding>(__encoding),
			     ::std::forward<_Output>(__output), ::std::forward<_ErrorHandler>(__error_handler), __state);
		return __txt_detail::__slice_to_stateless(::std::move(__stateful_result));
	}

	//////
	/// @brief Converts a single indivisible unit of work's worth of code points of the given `__input` view through
	/// the encoding to code units into the @p __output view.
	///
	/// @param[in]     __input An input_view to read code points from and use in the encode_one operation that will
	/// produce code units.
	/// @param[in]     __encoding The encoding that will be used to encode_one the input's code points into
	/// output code units.
	/// @param[in]     __output An output_view to write code units to as the result of the encode_one operation from
	/// the intermediate code points.
	///
	/// @result A ztd::text::stateless_encode_one_result object that contains references to `__state`.
	///
	/// @remarks Creates a default `error_handler` that is similar to ztd::text::default_handler_t, but marked as
	/// careless.
	//////
	template <typename _Input, typename _Encoding, typename _Output>
	constexpr auto encode_one_into(_Input&& __input, _Encoding&& __encoding, _Output&& __output) {
		default_handler_t __handler {};
		return encode_one_into(::std::forward<_Input>(__input), ::std::forward<_Encoding>(__encoding),
			::std::forward<_Output>(__output), __handler);
	}

	//////
	/// @brief Converts a single indivisible unit of work's worth of code points of the given `__input` view through
	/// the encoding to code units into the @p __output view.
	///
	/// @param[in]     __input An input_view to read code points from and use in the encode_one operation that will
	/// produce code units.
	/// @param[in]     __output An output_view to write code units to as the result of the encode_one operation from
	/// the intermediate code points.
	///
	/// @result A ztd::text::stateless_encode_one_result object that contains references to `__state`.
	///
	/// @remarks Creates a default `encoding` by figuring out the `value_type` of the `__input`, then passing
	/// that type into ztd::text::default_code_point_encoding_t. That encoding is that used to encode_one the input
	/// code points, by default.
	//////
	template <typename _Input, typename _Output>
	constexpr auto encode_one_into(_Input&& __input, _Output&& __output) {
		using _UInput    = remove_cvref_t<_Input>;
		using _CodePoint = ranges::range_value_type_t<_UInput>;
#if ZTD_IS_ON(ZTD_STD_LIBRARY_IS_CONSTANT_EVALUATED_I_)
		if (::std::is_constant_evaluated()) {
			// Use literal encoding instead, if we meet the right criteria
			using _Encoding = default_consteval_code_point_encoding_t<_CodePoint>;
			_Encoding __encoding {};
			return encode_one_into(::std::forward<_Input>(__input), __encoding, ::std::forward<_Output>(__output));
		}
		else
#endif
		{
			using _Encoding = default_code_point_encoding_t<_CodePoint>;
			_Encoding __encoding {};
			return encode_one_into(::std::forward<_Input>(__input), __encoding, ::std::forward<_Output>(__output));
		}
	}

	namespace __txt_detail {
		template <bool _OutputOnly, typename _OutputContainer, typename _Input, typename _Encoding,
			typename _ErrorHandler, typename _State>
		constexpr auto __encode_one_dispatch(
			_Input&& __input, _Encoding&& __encoding, _ErrorHandler&& __error_handler, _State& __state) {
			using _UEncoding = remove_cvref_t<_Encoding>;

			_OutputContainer __output {};
			if constexpr (is_detected_v<ranges::detect_adl_size, _Input>) {
				using _SizeType = decltype(ranges::ranges_adl::adl_size(__input));
				if constexpr (is_detected_v<ranges::detect_reserve_with_size, _OutputContainer, _SizeType>) {
					_SizeType __output_size_hint = static_cast<_SizeType>(ranges::ranges_adl::adl_size(__input));
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
				auto __stateful_result = encode_one_into(__txt_detail::__forward_if_move_only<_Input>(__input),
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
				auto __stateful_result = encode_one_into(__txt_detail::__forward_if_move_only<_Input>(__input),
					::std::forward<_Encoding>(__encoding), __output,
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
	/// @brief Converts a single indivisible unit of work's worth of code points of the given `__input` view through
	/// the encoding to code units in the specified `_OutputContainer` type.
	///
	/// @tparam _OutputContainer The container type to serialize data into.
	///
	/// @param[in]     __input An input_view to read code points from and use in the encode_one operation that will
	/// produce code units.
	/// @param[in]     __encoding The encoding that will be used to encode_one the input's code points into
	/// output code units.
	/// @param[in]     __error_handler The error handlers for the from and to encodings,
	/// respectively.
	/// @param[in,out] __state A reference to the associated state for the `__encoding` 's encode_one step.
	///
	/// @result A ztd::text::encode_one_result object that contains references to `__state` and an output of type
	/// `_OutputContainer`.
	///
	/// @remarks This function detects creates a container of type `_OutputContainer` and uses a typical @c
	/// std::back_inserter or `std::push_back_inserter` to fill in elements as it is written to. The result is
	/// then returned, with the `.output` value put into the container.
	//////
	template <typename _OutputContainer, typename _Input, typename _Encoding, typename _ErrorHandler, typename _State>
	constexpr auto encode_one_to(
		_Input&& __input, _Encoding&& __encoding, _ErrorHandler&& __error_handler, _State& __state) {
		using _UEncoding                = remove_cvref_t<_Encoding>;
		using _UOutputContainer         = remove_cvref_t<_OutputContainer>;
		using _OutputCodeUnit           = code_unit_t<_UEncoding>;
		constexpr bool _IsVoidContainer = ::std::is_void_v<_UOutputContainer>;
		constexpr bool _IsStringable
			= (is_char_traitable_v<_OutputCodeUnit> || is_unicode_code_point_v<_OutputCodeUnit>);
		if constexpr (_IsVoidContainer && _IsStringable) {
			using _RealOutputContainer = ::ztd::fixed_basic_string<_OutputCodeUnit, max_code_units_v<_UEncoding>>;
			return __txt_detail::__encode_one_dispatch<false, _RealOutputContainer>(::std::forward<_Input>(__input),
				::std::forward<_Encoding>(__encoding), ::std::forward<_ErrorHandler>(__error_handler), __state);
		}
		else {
			using _RealOutputContainer = ::std::conditional_t<_IsVoidContainer,
				::ztd::fixed_vector<_OutputCodeUnit, max_code_units_v<_UEncoding>>, _OutputContainer>;
			return __txt_detail::__encode_one_dispatch<false, _RealOutputContainer>(::std::forward<_Input>(__input),
				::std::forward<_Encoding>(__encoding), ::std::forward<_ErrorHandler>(__error_handler), __state);
		}
	}

	//////
	/// @brief Converts a single indivisible unit of work's worth of code points of the given `__input` view through
	/// the encoding to code units in the specified `_OutputContainer` type.
	///
	/// @tparam _OutputContainer The container type to serialize data into.
	///
	/// @param[in]     __input An input_view to read code points from and use in the encode_one operation that will
	/// produce code units.
	/// @param[in]     __encoding The encoding that will be used to encode_one the input's code points into
	/// output code units.
	/// @param[in]     __error_handler The error handlers for the from and to encodings,
	/// respectively.
	///
	/// @result A ztd::text::stateless_encode_one_result object whose output is of type `_OutputContainer`.
	///
	/// @remarks This function creates a `state` using ztd::text::make_encode_state.
	//////
	template <typename _OutputContainer, typename _Input, typename _Encoding, typename _ErrorHandler>
	constexpr auto encode_one_to(_Input&& __input, _Encoding&& __encoding, _ErrorHandler&& __error_handler) {
		using _UEncoding = remove_cvref_t<_Encoding>;
		using _State     = encode_state_t<_UEncoding>;

		_State __state         = make_encode_state(__encoding);
		auto __stateful_result = encode_one_to<_OutputContainer>(::std::forward<_Input>(__input),
			::std::forward<_Encoding>(__encoding), ::std::forward<_ErrorHandler>(__error_handler), __state);
		return __txt_detail::__slice_to_stateless(::std::move(__stateful_result));
	}

	//////
	/// @brief Converts a single indivisible unit of work's worth of code points of the given `__input` view through
	/// the encoding to code units in the specified `_OutputContainer` type.
	///
	/// @tparam _OutputContainer The container type to serialize data into.
	///
	/// @param[in]     __input An input_view to read code points from and use in the encode_one operation that will
	/// produce code units.
	/// @param[in]     __encoding The encoding that will be used to encode_one the input's code points into
	/// output code units.
	///
	/// @result A ztd::text::stateless_encode_one_result object whose output is of type `_OutputContainer`.
	///
	/// @remarks This function creates a `handler` using ztd::text::default_handler_t, but marks it as careless.
	//////
	template <typename _OutputContainer, typename _Input, typename _Encoding>
	constexpr auto encode_one_to(_Input&& __input, _Encoding&& __encoding) {
		default_handler_t __handler {};
		return encode_one_to<_OutputContainer>(
			::std::forward<_Input>(__input), ::std::forward<_Encoding>(__encoding), __handler);
	}

	//////
	/// @brief Converts a single indivisible unit of work's worth of code points of the given `__input` view through
	/// the encoding to code units in the specified `_OutputContainer` type.
	///
	/// @tparam _OutputContainer The container type to serialize data into.
	///
	/// @param[in]     __input An input_view to read code points from and use in the encode_one operation that will
	/// produce code units.
	///
	/// @result A ztd::text::stateless_encode_one_result object whose output is of type `_OutputContainer`.
	///
	/// @remarks This function creates an `encoding` by using the `value_type` of the `__input` which is then
	/// passed through the ztd::text::default_code_point_encoding type to get the default desired encoding.
	//////
	template <typename _OutputContainer, typename _Input>
	constexpr auto encode_one_to(_Input&& __input) {
		using _UInput    = remove_cvref_t<_Input>;
		using _CodePoint = ranges::range_value_type_t<_UInput>;
#if ZTD_IS_ON(ZTD_STD_LIBRARY_IS_CONSTANT_EVALUATED_I_)
		if (::std::is_constant_evaluated()) {
			// Use literal encoding instead, if we meet the right criteria
			using _Encoding = default_consteval_code_point_encoding_t<_CodePoint>;
			_Encoding __encoding {};
			return encode_one_to<_OutputContainer>(::std::forward<_Input>(__input), __encoding);
		}
		else
#endif
		{
			using _Encoding = default_code_point_encoding_t<_CodePoint>;
			_Encoding __encoding {};
			return encode_one_to<_OutputContainer>(::std::forward<_Input>(__input), __encoding);
		}
	}

	//////
	/// @brief Converts a single indivisible unit of work's worth of code points of the given `__input` view through
	/// the encoding to code units in the specified `_OutputContainer` type.
	///
	/// @tparam _OutputContainer The container type to serialize data into.
	///
	/// @param[in]     __input An input_view to read code points from and use in the encode_one operation that will
	/// produce code units.
	/// @param[in]     __encoding The encoding that will be used to encode_one the input's code points into
	/// output code units.
	/// @param[in]     __error_handler The error handlers for the from and to encodings,
	/// respectively.
	/// @param[in,out] __state A reference to the associated state for the `__encoding` 's encode_one step.
	///
	/// @result An object of type `_OutputContainer` .
	///
	/// @remarks This function detects creates a container of type `_OutputContainer` and uses a typical @c
	/// std::back_inserter or `std::push_back_inserter` to fill in elements as it is written to.
	//////
	template <typename _OutputContainer = void, typename _Input, typename _Encoding, typename _ErrorHandler,
		typename _State>
	constexpr auto encode_one(
		_Input&& __input, _Encoding&& __encoding, _ErrorHandler&& __error_handler, _State& __state) {
		using _UEncoding                = remove_cvref_t<_Encoding>;
		using _UOutputContainer         = remove_cvref_t<_OutputContainer>;
		using _OutputCodeUnit           = code_unit_t<_UEncoding>;
		constexpr bool _IsVoidContainer = ::std::is_void_v<_UOutputContainer>;
		constexpr bool _IsStringable
			= (is_char_traitable_v<_OutputCodeUnit> || is_unicode_code_point_v<_OutputCodeUnit>);
		if constexpr (_IsVoidContainer && _IsStringable) {
			using _RealOutputContainer = ::ztd::fixed_basic_string<_OutputCodeUnit, max_code_units_v<_UEncoding>>;
			return __txt_detail::__encode_one_dispatch<true, _RealOutputContainer>(::std::forward<_Input>(__input),
				::std::forward<_Encoding>(__encoding), ::std::forward<_ErrorHandler>(__error_handler), __state);
		}
		else {
			using _RealOutputContainer = ::std::conditional_t<_IsVoidContainer,
				::ztd::fixed_vector<_OutputCodeUnit, max_code_units_v<_UEncoding>>, _OutputContainer>;
			return __txt_detail::__encode_one_dispatch<true, _RealOutputContainer>(::std::forward<_Input>(__input),
				::std::forward<_Encoding>(__encoding), ::std::forward<_ErrorHandler>(__error_handler), __state);
		}
	}

	//////
	/// @brief Converts a single indivisible unit of work's worth of code points of the given `__input` view through
	/// the encoding to code units in the specified `_OutputContainer` type.
	///
	/// @tparam _OutputContainer The container type to serialize data into.
	///
	/// @param[in]     __input An input_view to read code points from and use in the encode_one operation that will
	/// produce code units.
	/// @param[in]     __encoding The encoding that will be used to encode_one the input's code points into
	/// output code units.
	/// @param[in]     __error_handler The error handlers for the from and to encodings,
	/// respectively.
	///
	/// @result An object of type `_OutputContainer` .
	///
	/// @remarks This function creates a `state` using ztd::text::make_encode_state.
	//////
	template <typename _OutputContainer = void, typename _Input, typename _Encoding, typename _ErrorHandler>
	constexpr auto encode_one(_Input&& __input, _Encoding&& __encoding, _ErrorHandler&& __error_handler) {
		using _UEncoding = remove_cvref_t<_Encoding>;
		using _State     = encode_state_t<_UEncoding>;

		_State __state = make_encode_state(__encoding);
		return encode_one<_OutputContainer>(::std::forward<_Input>(__input), ::std::forward<_Encoding>(__encoding),
			::std::forward<_ErrorHandler>(__error_handler), __state);
	}

	//////
	/// @brief Converts a single indivisible unit of work's worth of code points of the given `__input` view through
	/// the encoding to code units in the specified `_OutputContainer` type.
	///
	/// @tparam _OutputContainer The container type to serialize data into.
	///
	/// @param[in]     __input An input_view to read code points from and use in the encode_one operation that will
	/// produce code units.
	/// @param[in]     __encoding The encoding that will be used to encode_one the input's code points into
	/// output code units.
	///
	/// @result An object of type `_OutputContainer` .
	///
	/// @remarks This function creates a `handler` using ztd::text::default_handler_t, but marks it as careless.
	//////
	template <typename _OutputContainer = void, typename _Input, typename _Encoding>
	constexpr auto encode_one(_Input&& __input, _Encoding&& __encoding) {
		default_handler_t __handler {};
		return encode_one<_OutputContainer>(
			::std::forward<_Input>(__input), ::std::forward<_Encoding>(__encoding), __handler);
	}

	//////
	/// @brief Converts a single indivisible unit of work's worth of code points of the given `__input` view through
	/// the encoding to code units in the specified `_OutputContainer` type.
	///
	/// @tparam _OutputContainer The container type to serialize data into.
	///
	/// @param[in]     __input An input_view to read code points from and use in the encode_one operation that will
	/// produce code units.
	///
	/// @result An object of type `_OutputContainer` .
	///
	/// @remarks This function creates an `encoding` by using the `value_type` of the `__input` which is then
	/// passed through the ztd::text::default_code_point_encoding type to get the default desired encoding.
	//////
	template <typename _OutputContainer = void, typename _Input>
	constexpr auto encode_one(_Input&& __input) {
		using _UInput    = remove_cvref_t<_Input>;
		using _CodePoint = ranges::range_value_type_t<_UInput>;
#if ZTD_IS_ON(ZTD_STD_LIBRARY_IS_CONSTANT_EVALUATED_I_)
		if (::std::is_constant_evaluated()) {
			// Use literal encoding instead, if we meet the right criteria
			using _Encoding = default_consteval_code_point_encoding_t<_CodePoint>;
			_Encoding __encoding {};
			return encode_one<_OutputContainer>(::std::forward<_Input>(__input), __encoding);
		}
		else
#endif
		{
			using _Encoding = default_code_point_encoding_t<_CodePoint>;
			_Encoding __encoding {};
			return encode_one<_OutputContainer>(::std::forward<_Input>(__input), __encoding);
		}
	}

	//////
	/// @}

	ZTD_TEXT_INLINE_ABI_NAMESPACE_CLOSE_I_
}} // namespace ztd::text

#include <ztd/epilogue.hpp>

#endif // ZTD_TEXT_ENCODE_ONE_HPP
