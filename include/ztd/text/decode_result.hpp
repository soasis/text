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

#ifndef ZTD_TEXT_DECODE_RESULT_HPP
#define ZTD_TEXT_DECODE_RESULT_HPP

#include <ztd/text/version.hpp>

#include <ztd/text/unicode_code_point.hpp>
#include <ztd/text/encoding_error.hpp>
#include <ztd/text/code_unit.hpp>
#include <ztd/text/code_point.hpp>
#include <ztd/text/state.hpp>
#include <ztd/text/detail/result_type_constraints.hpp>

#include <ztd/idk/span.hpp>
#include <ztd/idk/reference_wrapper.hpp>
#include <ztd/ranges/reconstruct.hpp>

#include <cstddef>
#include <array>
#include <utility>
#include <functional>
#include <system_error>

#include <ztd/prologue.hpp>

namespace ztd { namespace text {
	ZTD_TEXT_INLINE_ABI_NAMESPACE_OPEN_I_

	//////
	/// @addtogroup ztd_text_result Result Types
	///
	/// @{
	/////

	//////
	/// @brief The result of all decode operations from encoding objects and higher-level calls (such as
	/// ztd_text_decode).
	template <typename _Input, typename _Output>
	class stateless_decode_result {
	public:
		//////
		/// @brief The reconstructed input_view object, with its .begin() incremented by the number of code units
		/// successfully read (can be identical to .begin() on original range on failure).
		_Input input;
		//////
		/// @brief The reconstructed output_view object, with its .begin() incremented by the number of code units
		/// successfully written (can be identical to .begin() on original range on failure).
		_Output output;
		//////
		/// @brief The kind of error that occured, if any.
		encoding_error error_code;
		//////
		/// @brief The number of times an error occurred in the processed input text.
		::std::size_t error_count;

		//////
		/// @brief Constructs a ztd::text::decode_result with the provided parameters and information,
		/// including whether or not an error was handled.
		///
		/// @param[in] __other A different but related result type.
		template <typename _ArgInput, typename _ArgOutput,
			::std::enable_if_t<__txt_detail::__result_type_copy_constraint<::ztd::text::stateless_decode_result,
			     _Input, _ArgInput, _Output, _ArgOutput>()>* = nullptr>
		constexpr stateless_decode_result(const stateless_decode_result<_ArgInput, _ArgOutput>& __other) noexcept(
			__txt_detail::__result_type_copy_noexcept<::ztd::text::stateless_decode_result, _Input, _ArgInput,
			     _Output, _ArgOutput>)
		: input(__other.input)
		, output(__other.output)
		, error_code(__other.error_code)
		, error_count(__other.error_count) {
		}

		//////
		/// @brief Constructs a ztd::text::decode_result with the provided parameters and information,
		/// including whether or not an error was handled.
		///
		/// @param[in] __other A different but related result type.
		template <typename _ArgInput, typename _ArgOutput,
			::std::enable_if_t<__txt_detail::__result_type_move_constraint<::ztd::text::stateless_decode_result,
			     _Input, _ArgInput, _Output, _ArgOutput>()>* = nullptr>
		constexpr stateless_decode_result(stateless_decode_result<_ArgInput, _ArgOutput>&& __other) noexcept(
			__txt_detail::__result_type_move_noexcept<::ztd::text::stateless_decode_result, _Input, _ArgInput,
			     _Output, _ArgOutput>)
		: input(::std::move(__other.input))
		, output(::std::move(__other.output))
		, error_code(__other.error_code)
		, error_count(__other.error_count) {
		}

		//////
		/// @brief Constructs a ztd::text::decode_result, defaulting the error code to
		/// ztd::text::encoding_error::ok if not provided.
		///
		/// @param[in] __input The input range to store.
		/// @param[in] __output The output range to store.
		/// @param[in] __error_code The error code for the decoding opertion, if any.
		template <typename _ArgInput, typename _ArgOutput, typename _ArgState>
		constexpr stateless_decode_result(_ArgInput&& __input, _ArgOutput&& __output,
			encoding_error __error_code
			= encoding_error::ok) noexcept(noexcept(stateless_decode_result(::std::forward<_ArgInput>(__input),
			::std::forward<_ArgOutput>(__output), __error_code, __error_code != encoding_error::ok)))
		: stateless_decode_result(::std::forward<_ArgInput>(__input), ::std::forward<_ArgOutput>(__output),
			__error_code,
			__error_code != encoding_error::ok ? static_cast<::std::size_t>(1) : static_cast<::std::size_t>(0)) {
		}

		//////
		/// @brief Constructs a ztd::text::decode_result with the provided parameters and information,
		/// including whether or not an error was handled.
		///
		/// @param[in] __input The input range to store.
		/// @param[in] __output The output range to store.
		/// @param[in] __error_code The error code for the decode operation, taken as the first of either the decode
		/// operation that failed.
		/// @param[in] __error_count Whether or not an error was handled. Some error handlers are corrective (see
		/// ztd::text::replacement_handler_t), and so the error code is not enough to determine if the handler was
		/// invoked. This allows the value to be provided directly when constructing this result type.
		template <typename _ArgInput, typename _ArgOutput>
		constexpr stateless_decode_result(_ArgInput&& __input, _ArgOutput&& __output, encoding_error __error_code,
			::std::size_t __error_count) noexcept(::std::is_nothrow_constructible_v<_Input,
			_ArgInput>&& ::std::is_nothrow_constructible_v<_Output, _ArgOutput>)
		: input(::std::forward<_ArgInput>(__input))
		, output(::std::forward<_ArgOutput>(__output))
		, error_code(__error_code)
		, error_count(__error_count) {
		}

		//////
		/// @brief Whether or not any errors were handled.
		///
		/// @returns Simply checks whether `error_count` is greater than 0.
		constexpr bool errors_were_handled() const noexcept {
			return this->error_count > 0;
		}
	};

	//////
	/// @brief The result of all decode operations from encoding objects and higher-level calls (such as
	/// ztd_text_decode).
	template <typename _Input, typename _Output, typename _State>
	class decode_result : public stateless_decode_result<_Input, _Output> {
	private:
		using __base_t = stateless_decode_result<_Input, _Output>;

	public:
		//////
		/// @brief The state of the associated Encoding used for decoding input code units to code points.
		::ztd::reference_wrapper<_State> state; //////
		/// @brief Constructs a ztd::text::decode_result from a previous decode_result.
		///
		/// @param[in] __other A different but related result type.
		template <typename _ArgInput, typename _ArgOutput, typename _ArgState,
			::std::enable_if_t<__txt_detail::__result_type_copy_constraint<::ztd::text::decode_result, _Input,
			     _ArgInput, _Output, _ArgOutput, _State, _ArgState>()>* = nullptr>
		constexpr decode_result(const decode_result<_ArgInput, _ArgOutput, _ArgState>& __other) noexcept(
			__txt_detail::__result_type_copy_noexcept<::ztd::text::decode_result, _Input, _ArgInput, _Output,
			     _ArgOutput, _State, _ArgState>)
		: __base_t(__other.input, __other.output, __other.error_code, __other.error_count), state(__other.state) {
		}

		//////
		/// @brief Constructs a ztd::text::decode_result from a previous decode_result.
		///
		/// @param[in] __other A different but related result type.
		template <typename _ArgInput, typename _ArgOutput, typename _ArgState,
			::std::enable_if_t<__txt_detail::__result_type_move_constraint<::ztd::text::decode_result, _Input,
			     _ArgInput, _Output, _ArgOutput, _State, _ArgState>()>* = nullptr>
		constexpr decode_result(decode_result<_ArgInput, _ArgOutput, _ArgState>&& __other) noexcept(
			__txt_detail::__result_type_move_noexcept<::ztd::text::decode_result, _Input, _ArgInput, _Output,
			     _ArgOutput, _State, _ArgState>)
		: __base_t(::std::move(__other.input), ::std::move(__other.output), __other.error_code, __other.error_count)
		, state(__other.state) {
		}

		//////
		/// @brief Constructs a ztd::text::decode_result, defaulting the error code to
		/// ztd::text::encoding_error::ok if not provided.
		///
		/// @param[in] __input The input range to store.
		/// @param[in] __output The output range to store.
		/// @param[in] __state The state related to the Encoding that performed the decode operation.
		/// @param[in] __error_code The error code for the decoding opertion, if any.
		template <typename _ArgInput, typename _ArgOutput, typename _ArgState>
		constexpr decode_result(_ArgInput&& __input, _ArgOutput&& __output, _ArgState&& __state,
			encoding_error __error_code = encoding_error::ok)
		: decode_result(::std::forward<_ArgInput>(__input), ::std::forward<_ArgOutput>(__output),
			::std::forward<_ArgState>(__state), __error_code,
			__error_code != encoding_error::ok ? static_cast<::std::size_t>(1) : static_cast<::std::size_t>(0)) {
		}

		//////
		/// @brief Constructs a ztd::text::decode_result with the provided parameters and information,
		/// including whether or not an error was handled.
		///
		/// @param[in] __input The input range to store.
		/// @param[in] __output The output range to store.
		/// @param[in] __state The state related to the Encoding that performed the decode operation.
		/// @param[in] __error_code The error code for the decode operation, taken as the first of either the decode
		/// operation that failed.
		/// @param[in] __error_count Whether or not an error was handled. Some error handlers are corrective (see
		/// ztd::text::replacement_handler_t), and so the error code is not enough to determine if the handler was
		/// invoked. This allows the value to be provided directly when constructing this result type.
		template <typename _ArgInput, typename _ArgOutput, typename _ArgState>
		constexpr decode_result(_ArgInput&& __input, _ArgOutput&& __output, _ArgState&& __state,
			encoding_error __error_code, ::std::size_t __error_count)
		: __base_t(
			::std::forward<_ArgInput>(__input), ::std::forward<_ArgOutput>(__output), __error_code, __error_count)
		, state(::std::forward<_ArgState>(__state)) {
		}
	};

	//////
	/// @brief A type alias to produce a span-containing decode result type. Useful for end-users with fairly standard,
	/// pointer-based buffer usages.
	template <typename _Encoding>
	using span_decode_result_for = decode_result<::ztd::span<const code_unit_t<_Encoding>>,
		::ztd::span<code_point_t<_Encoding>>, decode_state_t<_Encoding>>;

	//////
	/// @brief A type alias to produce a concrete error handler for the encoding result of the specified `_Encoding`
	/// type.
	///
	/// @tparam _Encoding The encoding to base this error handler off of.
	/// @tparam _Function The template function type that will be used as the base type to insert the function
	/// signature into.
	template <typename _Encoding, template <class...> class _Function = std::function>
	using basic_decode_error_handler_for = _Function<span_decode_result_for<_Encoding>(
		const _Encoding&, span_decode_result_for<_Encoding>, ::ztd::span<const code_unit_t<_Encoding>>)>;

	//////
	/// @}
	/////

	namespace __txt_detail {
		template <typename _Input, typename _Output, typename _State>
		constexpr stateless_decode_result<_Input, _Output>
		__slice_to_stateless_decode(decode_result<_Input, _Output, _State>&& __result) noexcept(
			::std::is_nothrow_constructible_v<stateless_decode_result<_Input, _Output>,
			     stateless_decode_result<_Input, _Output>>) {
			return ::std::move(__result);
		}

		template <typename _Input, typename _Output, typename _DesiredOutput>
		constexpr stateless_decode_result<_Input, remove_cvref_t<_DesiredOutput>>
		__replace_decode_result_output_no_state(
			stateless_decode_result<_Input, _Output>&& __result, _DesiredOutput&& __desired_output) noexcept(::std::
			     is_nothrow_constructible_v<stateless_decode_result<_Input, _Output>, _Input&&, _DesiredOutput,
			          encoding_error, ::std::size_t>) {
			using _Result = stateless_decode_result<_Input, remove_cvref_t<_DesiredOutput>>;
			return _Result(::std::move(__result.input), ::std::forward<_DesiredOutput>(__desired_output),
				__result.error_code, __result.error_count);
		}

		template <typename _Input, typename _Output, typename _State, typename _DesiredOutput>
		constexpr decode_result<_Input, remove_cvref_t<_DesiredOutput>, _State> __replace_decode_result_output(
			decode_result<_Input, _Output, _State>&& __result,
			_DesiredOutput&&
			     __desired_output) noexcept(::std::is_nothrow_constructible_v<decode_result<_Input, _Output, _State>,
			_Input&&, _DesiredOutput, _State&, encoding_error, ::std::size_t>) {
			using _Result = decode_result<_Input, remove_cvref_t<_DesiredOutput>, _State>;
			return _Result(::std::move(__result.input), ::std::forward<_DesiredOutput>(__desired_output),
				__result.state, __result.error_code, __result.error_count);
		}

		template <typename _InputRange, typename _OutputRange, typename _State>
		using __reconstruct_decode_result_t = decode_result<ranges::range_reconstruct_t<_InputRange>,
			ranges::range_reconstruct_t<_OutputRange>, _State>;

		template <typename _InputRange, typename _OutputRange, typename _State, typename _InFirst, typename _InLast,
			typename _OutFirst, typename _OutLast, typename _ArgState>
		constexpr decltype(auto) __reconstruct_stateless_decode_result(_InFirst&& __in_first, _InLast&& __in_last,
			_OutFirst&& __out_first, _OutLast&& __out_last, _ArgState&& __state, encoding_error __error_code,
			::std::size_t __error_count) {
			decltype(auto) __in_range  = ranges::reconstruct(::std::in_place_type<_InputRange>,
				 ::std::forward<_InFirst>(__in_first), ::std::forward<_InLast>(__in_last));
			decltype(auto) __out_range = ranges::reconstruct(::std::in_place_type<_OutputRange>,
				::std::forward<_OutFirst>(__out_first), ::std::forward<_OutLast>(__out_last));
			return decode_result<_InputRange, _OutputRange, _State>(::std::forward<decltype(__in_range)>(__in_range),
				::std::forward<decltype(__out_range)>(__out_range), ::std::forward<_ArgState>(__state),
				__error_code, __error_count);
		}

		template <typename _InputRange, typename _OutputRange, typename _State, typename _InFirst, typename _InLast,
			typename _OutFirst, typename _OutLast, typename _ArgState>
		constexpr decltype(auto) __reconstruct_stateless_decode_result(_InFirst&& __in_first, _InLast&& __in_last,
			_OutFirst&& __out_first, _OutLast&& __out_last, _ArgState&& __state,
			encoding_error __error_code = encoding_error::ok) {
			return __reconstruct_decode_result_t<_InputRange, _OutputRange, _State>(
				::std::forward<_InFirst>(__in_first), ::std::forward<_InLast>(__in_last),
				::std::forward<_OutFirst>(__out_first), ::std::forward<_OutLast>(__out_last),
				::std::forward<_ArgState>(__state), __error_code);
		}
	} // namespace __txt_detail

	ZTD_TEXT_INLINE_ABI_NAMESPACE_CLOSE_I_
}} // namespace ztd::text

#include <ztd/epilogue.hpp>

#endif
