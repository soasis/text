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

#ifndef ZTD_TEXT_DECODE_RESULT_HPP
#define ZTD_TEXT_DECODE_RESULT_HPP

#include <ztd/text/version.hpp>

#include <ztd/text/char8_t.hpp>
#include <ztd/text/unicode_code_point.hpp>
#include <ztd/text/encoding_error.hpp>

#include <ztd/text/detail/reconstruct.hpp>
#include <ztd/text/detail/span.hpp>
#include <ztd/text/detail/encoding_range.hpp>

#include <cstddef>
#include <array>
#include <utility>
#include <system_error>

namespace ztd { namespace text {
	ZTD_TEXT_INLINE_ABI_NAMESPACE_OPEN_I_

	//////
	/// @addtogroup ztd_text_result Result Types
	/// @{
	/////

	//////
	/// @brief The result of all decode operations from encoding objects and higher-level calls (such as 
	/// ztd_text_decode).
	//////
	template <typename _Input, typename _Output>
	class stateless_decode_result {
	public:
		//////
		/// @brief The reconstructed input_view object, with its .begin() incremented by the number of code units
		/// successfully read (can be identical to .begin() on original range on failure).
		//////
		_Input input;
		//////
		/// @brief The reconstructed output_view object, with its .begin() incremented by the number of code units
		/// successfully written (can be identical to .begin() on original range on failure).
		//////
		_Output output;
		//////
		/// @brief The kind of error that occured, if any.
		//////
		encoding_error error_code;
		//////
		/// @brief Whether or not the error handler was invoked, regardless of if the error_code is set or not set to
		/// ztd::text::encoding_error::ok.
		//////
		bool handled_error;

		//////
		/// @brief Constructs a ztd::text::decode_result, defaulting the error code to 
		/// ztd::text::encoding_error::ok if not provided.
		///
		/// @param[in] __input The input range to store.
		/// @param[in] __output The output range to store.
		/// @param[in] __error_code The error code for the decoding opertion, if any.
		//////
		template <typename _ArgInput, typename _ArgOutput, typename _ArgState>
		constexpr stateless_decode_result(_ArgInput&& __input, _ArgOutput&& __output,
			encoding_error __error_code
			= encoding_error::ok) noexcept(noexcept(stateless_decode_result(::std::forward<_ArgInput>(__input),
			::std::forward<_ArgOutput>(__output), __error_code, __error_code != encoding_error::ok)))
		: stateless_decode_result(::std::forward<_ArgInput>(__input), ::std::forward<_ArgOutput>(__output),
			__error_code, __error_code != encoding_error::ok) {
		}

		//////
		/// @brief Constructs a ztd::text::decode_result with the provided parameters and information,
		/// including whether or not an error was handled.
		///
		/// @param[in] __input The input range to store.
		/// @param[in] __output The output range to store.
		/// @param[in] __error_code The error code for the encode operation, taken as the first of either the decode
		/// operation that failed.
		/// @param[in] __handled_error Whether or not an error was handled. Some error handlers are corrective (see
		/// ztd::text::replacement_handler), and so the error code is not enough to determine if the handler was
		/// invoked. This allows the value to be provided directly when constructing this result type.
		//////
		template <typename _ArgInput, typename _ArgOutput>
		constexpr stateless_decode_result(_ArgInput&& __input, _ArgOutput&& __output, encoding_error __error_code,
			bool __handled_error) noexcept(::std::is_nothrow_constructible_v<_Input,
			_ArgInput>&& ::std::is_nothrow_constructible_v<_Output, _ArgOutput>)
		: input(::std::forward<_ArgInput>(__input))
		, output(::std::forward<_ArgOutput>(__output))
		, error_code(__error_code)
		, handled_error(__handled_error) {
		}
	};

	//////
	/// @brief The result of all decode operations from encoding objects and higher-level calls (such as 
	/// ztd_text_decode).
	//////
	template <typename _Input, typename _Output, typename _State>
	class decode_result : public stateless_decode_result<_Input, _Output> {
	private:
		using __base_t = stateless_decode_result<_Input, _Output>;

	public:
		//////
		/// @brief The state of the associated Encoding used for decoding input code units to code points.
		///
		//////
		_State& state;

		//////
		/// @brief Constructs a ztd::text::decode_result, defaulting the error code to 
		/// ztd::text::encoding_error::ok if not provided.
		///
		/// @param[in] __input The input range to store.
		/// @param[in] __output The output range to store.
		/// @param[in] __state The state related to the Encoding that performed the decode operation.
		/// @param[in] __error_code The error code for the decoding opertion, if any.
		//////
		template <typename _ArgInput, typename _ArgOutput, typename _ArgState>
		constexpr decode_result(_ArgInput&& __input, _ArgOutput&& __output, _ArgState&& __state,
			encoding_error __error_code = encoding_error::ok)
		: decode_result(::std::forward<_ArgInput>(__input), ::std::forward<_ArgOutput>(__output),
			::std::forward<_ArgState>(__state), __error_code, __error_code != encoding_error::ok) {
		}

		//////
		/// @brief Constructs a ztd::text::decode_result with the provided parameters and information,
		/// including whether or not an error was handled.
		///
		/// @param[in] __input The input range to store.
		/// @param[in] __output The output range to store.
		/// @param[in] __state The state related to the Encoding that performed the decode operation.
		/// @param[in] __error_code The error code for the encode operation, taken as the first of either the decode
		/// operation that failed.
		/// @param[in] __handled_error Whether or not an error was handled. Some error handlers are corrective (see
		/// ztd::text::replacement_handler), and so the error code is not enough to determine if the handler was
		/// invoked. This allows the value to be provided directly when constructing this result type.
		//////
		template <typename _ArgInput, typename _ArgOutput, typename _ArgState>
		constexpr decode_result(_ArgInput&& __input, _ArgOutput&& __output, _ArgState&& __state,
			encoding_error __error_code, bool __handled_error)
		: __base_t(
			::std::forward<_ArgInput>(__input), ::std::forward<_ArgOutput>(__output), __error_code, __handled_error)
		, state(::std::forward<_ArgState>(__state)) {
		}
	};

	//////
	/// @}
	/////

	namespace __detail {
		template <typename _Input, typename _Output, typename _State>
		constexpr stateless_decode_result<_Input, _Output> __slice_to_stateless(
			decode_result<_Input, _Output, _State>&& __result) {
			return __result;
		}

		template <typename _Input, typename _Output, typename _State, typename _DesiredOutput>
		constexpr decode_result<_Input, __detail::__remove_cvref_t<_DesiredOutput>, _State> __replace_result_output(
			decode_result<_Input, _Output, _State>&& __result,
			_DesiredOutput&&
			     __desired_output) noexcept(::std::is_nothrow_constructible_v<decode_result<_Input, _Output, _State>,
			_Input&&, _DesiredOutput, _State&, encoding_error, bool>) {
			using _Result = decode_result<_Input, __detail::__remove_cvref_t<_DesiredOutput>, _State>;
			return _Result(::std::move(__result.input), ::std::forward<_DesiredOutput>(__desired_output),
				__result.state, __result.error_code, __result.handled_error);
		}

		template <typename _InputRange, typename _OutputRange, typename _State>
		using __reconstruct_decode_result_t
			= decode_result<__reconstruct_t<_InputRange>, __reconstruct_t<_OutputRange>, _State>;

		template <typename _InputRange, typename _OutputRange, typename _State, typename _InFirst, typename _InLast,
			typename _OutFirst, typename _OutLast, typename _ArgState>
		constexpr decltype(auto) __reconstruct_decode_result(_InFirst&& __in_first, _InLast&& __in_last,
			_OutFirst&& __out_first, _OutLast&& __out_last, _ArgState&& __state, encoding_error __error_code,
			bool __handled_error) {
			decltype(auto) __in_range  = __reconstruct(::std::in_place_type<_InputRange>,
                    ::std::forward<_InFirst>(__in_first), ::std::forward<_InLast>(__in_last));
			decltype(auto) __out_range = __reconstruct(::std::in_place_type<_OutputRange>,
				::std::forward<_OutFirst>(__out_first), ::std::forward<_OutLast>(__out_last));
			return decode_result<_InputRange, _OutputRange, _State>(::std::forward<decltype(__in_range)>(__in_range),
				::std::forward<decltype(__out_range)>(__out_range), ::std::forward<_ArgState>(__state),
				__error_code, __handled_error);
		}

		template <typename _InputRange, typename _OutputRange, typename _State, typename _InFirst, typename _InLast,
			typename _OutFirst, typename _OutLast, typename _ArgState>
		constexpr decltype(auto) __reconstruct_decode_result(_InFirst&& __in_first, _InLast&& __in_last,
			_OutFirst&& __out_first, _OutLast&& __out_last, _ArgState&& __state,
			encoding_error __error_code = encoding_error::ok) {
			return __reconstruct_decode_result_t<_InputRange, _OutputRange, _State>(
				::std::forward<_InFirst>(__in_first), ::std::forward<_InLast>(__in_last),
				::std::forward<_OutFirst>(__out_first), ::std::forward<_OutLast>(__out_last),
				::std::forward<_ArgState>(__state), __error_code, __error_code != encoding_error::ok);
		}

		template <typename _Encoding, typename _Input, typename _Output, typename _ErrorHandler, typename _State>
		inline constexpr bool __is_decode_one_callable_v = __is_detected_v<__detect_object_decode_one, _Encoding,
			_Input, _Output, _ErrorHandler>&& __is_detected_v<__detect_callable_handler, _ErrorHandler, _Encoding,
			__reconstruct_decode_result_t<_Input, _Output, _State>, ::ztd::text::span<code_unit_t<_Encoding>>>;
	} // namespace __detail

	ZTD_TEXT_INLINE_ABI_NAMESPACE_CLOSE_I_
}} // namespace ztd::text

#endif // ZTD_TEXT_DECODE_RESULT_HPP
