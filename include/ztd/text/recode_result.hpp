// =============================================================================
//
// ztd.text
// Copyright © 2022-2023 JeanHeyd "ThePhD" Meneide and Shepherd's Oasis, LLC
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

#ifndef ZTD_TEXT_RECODE_RESULT_HPP
#define ZTD_TEXT_RECODE_RESULT_HPP

#include <ztd/text/version.hpp>

#include <ztd/text/unicode_code_point.hpp>
#include <ztd/text/encoding_error.hpp>
#include <ztd/text/code_unit.hpp>
#include <ztd/text/code_point.hpp>
#include <ztd/text/state.hpp>

#include <ztd/idk/reference_wrapper.hpp>
#include <ztd/ranges/reconstruct.hpp>

#include <cstddef>
#include <array>
#include <utility>
#include <system_error>

#include <ztd/prologue.hpp>

namespace ztd { namespace text {
	ZTD_TEXT_INLINE_ABI_NAMESPACE_OPEN_I_

	//////
	/// @addtogroup ztd_text_result Result Types
	///
	/// @brief The result types are used in the transcoding, validation and counting functions. Their sole goal is to
	/// make sure.
	///
	/// @{

	//////
	/// @brief The result of transcoding operations (such as ztd_text_recode) that specifically do not include
	/// a reference to the state.
	template <typename _Input, typename _Output>
	class stateless_recode_result {
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
		/// @brief Whether or not the error handler was invoked, regardless of if the error_code is set or not set to
		/// ztd::text::encoding_error::ok.
		::std::size_t error_count;


		//////
		/// @brief Constructs a ztd::text::stateless_recode_result, defaulting the error code to
		/// ztd::text::encoding_error::ok if not provided.
		///
		/// @param[in] __input The input range to store.
		/// @param[in] __output The output range to store.
		/// @param[in] __error_code The error code for the encode operation, taken as the first of either the encode
		/// or decode operation that failed.
		template <typename _ArgInput, typename _ArgOutput>
		constexpr stateless_recode_result(_ArgInput&& __input, _ArgOutput&& __output,
			encoding_error __error_code
			= encoding_error::ok) noexcept(noexcept(stateless_recode_result(::std::forward<_ArgInput>(__input),
			::std::forward<_ArgOutput>(__output), __error_code, __error_code != encoding_error::ok)))
		: stateless_recode_result(::std::forward<_ArgInput>(__input), ::std::forward<_ArgOutput>(__output),
			__error_code, __error_code != encoding_error::ok) {
		}

		//////
		/// @brief Constructs a ztd::text::stateless_recode_result with the provided parameters and
		/// information, including whether or not an error was handled.
		///
		/// @param[in] __input The input range to store.
		/// @param[in] __output The output range to store.
		/// @param[in] __error_code The error code for the encode operation, taken as the first of either the encode
		/// or decode operation that failed.
		/// @param[in] __error_count Whether or not an error was handled. Some error handlers are corrective (see
		/// ztd::text::replacement_handler_t), and so the error code is not enough to determine if the handler was
		/// invoked. This allows the value to be provided directly when constructing this result type.
		template <typename _ArgInput, typename _ArgOutput>
		constexpr stateless_recode_result(_ArgInput&& __input, _ArgOutput&& __output, encoding_error __error_code,
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
	/// @brief The result of transcoding operations (such as ztd_text_recode).
	template <typename _Input, typename _Output, typename _FromState, typename _ToState>
	class recode_result : public stateless_recode_result<_Input, _Output> {
	private:
		using __base_t = stateless_recode_result<_Input, _Output>;

	public:
		//////
		/// @brief A reference to the state of the associated Encoding used for decoding input code units to
		/// intermediate code points.
		::ztd::reference_wrapper<_FromState> from_state;
		//////
		/// @brief A reference to the state of the associated Encoding used for encoding intermediate code points to
		/// code units.
		::ztd::reference_wrapper<_ToState> to_state;

		//////
		/// @brief Constructs a ztd::text::recode_result from a previous recode_result.
		///
		/// @param[in] __other A different but related result type.
		template <typename _ArgInput, typename _ArgOutput, typename _ArgFromState, typename _ArgToState,
			::std::enable_if_t<!::std::is_same_v<recode_result,                                    // cf
			                        recode_result<_ArgInput, _ArgOutput, _FromState, _ToState>>    // cf
			     && ::std::is_constructible_v<__base_t, const _ArgInput&, const _ArgOutput&,       // cf
			          encoding_error, ::std::size_t>                                               // cf
			     && ::std::is_constructible_v<::ztd::reference_wrapper<_FromState>, _ArgFromState> // cf
			     && ::std::is_constructible_v<::ztd::reference_wrapper<_ToState>, _ArgToState>     // cf
			     >* = nullptr>
		constexpr recode_result(const recode_result<_ArgInput, _ArgOutput, _ArgFromState, _ArgToState>&
			     __other) noexcept(::std::is_nothrow_constructible_v<__base_t, _ArgInput, _ArgOutput, encoding_error,
			::std::size_t>)
		: __base_t(__other.input, __other.output, __other.error_code, __other.error_count)
		, from_state(__other.from_state)
		, to_state(__other.to_state) {
		}

		//////
		/// @brief Constructs a ztd::text::recode_result from a previous recode_result.
		///
		/// @param[in] __other A different but related result type.
		template <typename _ArgInput, typename _ArgOutput, typename _ArgFromState, typename _ArgToState,
			::std::enable_if_t<!::std::is_same_v<recode_result,                                    // cf
			                        recode_result<_ArgInput, _ArgOutput, _FromState, _ToState>>    // cf
			     && ::std::is_constructible_v<__base_t, _ArgInput&&, _ArgOutput&&,                 // cf
			          encoding_error, ::std::size_t>                                               // cf
			     && ::std::is_constructible_v<::ztd::reference_wrapper<_FromState>, _ArgFromState> // cf
			     && ::std::is_constructible_v<::ztd::reference_wrapper<_ToState>, _ArgToState>     // cf
			     >* = nullptr>
		constexpr recode_result(recode_result<_ArgInput, _ArgOutput, _ArgFromState, _ArgToState>&& __other) noexcept(
			::std::is_nothrow_constructible_v<__base_t, _ArgInput&&, _ArgOutput&&, encoding_error, ::std::size_t>)
		: __base_t(::std::move(__other.input), ::std::move(__other.output), __other.error_code, __other.error_count)
		, from_state(__other.from_state)
		, to_state(__other.to_state) {
		}

		//////
		/// @brief Constructs a ztd::text::recode_result, defaulting the error code to
		/// ztd::text::encoding_error::ok if not provided.
		///
		/// @param[in] __input The input range to store.
		/// @param[in] __output The output range to store.
		/// @param[in] __from_state The state related to the "From Encoding" that performed the decode half of the
		/// operation.
		/// @param[in] __to_state The state related to the "To Encoding" that performed the encode half of the
		/// operation.
		/// @param[in] __error_code The error code for the encode operation, taken as the first of either the encode
		/// or decode operation that failed.
		template <typename _ArgInput, typename _ArgOutput, typename _ArgFromState, typename _ArgToState>
		constexpr recode_result(_ArgInput&& __input, _ArgOutput&& __output, _ArgFromState&& __from_state,
			_ArgToState&& __to_state, encoding_error __error_code = encoding_error::ok)
		: recode_result(::std::forward<_ArgInput>(__input), ::std::forward<_ArgOutput>(__output),
			::std::forward<_ArgFromState>(__from_state), ::std::forward<_ArgToState>(__to_state), __error_code,
			__error_code != encoding_error::ok ? static_cast<::std::size_t>(1) : static_cast<::std::size_t>(0)) {
		}

		//////
		/// @brief Constructs a ztd::text::recode_result with the provided parameters and information,
		/// including whether or not an error was handled.
		///
		/// @param[in] __input The input range to store.
		/// @param[in] __output The output range to store.
		/// @param[in] __from_state The state related to the "From Encoding" that performed the decode half of the
		/// operation.
		/// @param[in] __to_state The state related to the "To Encoding" that performed the encode half of the
		/// operation.
		/// @param[in] __error_code The error code for the encode operation, taken as the first of either the encode
		/// or decode operation that failed.
		/// @param[in] __error_count Whether or not an error was handled. Some error handlers are corrective (see
		/// ztd::text::replacement_handler_t), and so the error code is not enough to determine if the handler was
		/// invoked. This allows the value to be provided directly when constructing this result type.
		template <typename _ArgInput, typename _ArgOutput, typename _ArgFromState, typename _ArgToState>
		constexpr recode_result(_ArgInput&& __input, _ArgOutput&& __output, _ArgFromState&& __from_state,
			_ArgToState&& __to_state, encoding_error __error_code, ::std::size_t __error_count)
		: __base_t(
			::std::forward<_ArgInput>(__input), ::std::forward<_ArgOutput>(__output), __error_code, __error_count)
		, from_state(::std::forward<_ArgFromState>(__from_state))
		, to_state(::std::forward<_ArgToState>(__to_state)) {
		}
	};

	//////
	/// @}
	/////

	namespace __txt_detail {
		template <typename _Input, typename _Output, typename _FromState, typename _ToState>
		constexpr stateless_recode_result<_Input, _Output>
		__slice_to_stateless(recode_result<_Input, _Output, _FromState, _ToState>&& __result) noexcept(
			::std::is_nothrow_constructible_v<stateless_recode_result<_Input, _Output>,
			     stateless_recode_result<_Input, _Output>>) {
			return ::std::move(__result);
		}

		template <typename _InputTag, typename _Input, typename _Output, typename _DesiredOutput, typename _FromState,
			typename _ToState>
		constexpr stateless_recode_result<_Input, remove_cvref_t<_DesiredOutput>>
		__replace_recode_result_output_no_state(recode_result<_Input, _Output, _FromState, _ToState>&& __result,
			_DesiredOutput&& __desired_output) noexcept(::std::
			     is_nothrow_constructible_v<stateless_recode_result<_Input, _Output>, _Input&&, _DesiredOutput,
			          encoding_error, ::std::size_t>) {
			using _ReconstructedInput = ranges::tag_range_reconstruct_t<_InputTag, _Input>;
			using _Result             = stateless_recode_result<_ReconstructedInput, remove_cvref_t<_DesiredOutput>>;
			return _Result(ranges::reconstruct(::std::in_place_type<_InputTag>, ::std::move(__result.input)),
				::std::forward<_DesiredOutput>(__desired_output), __result.error_code, __result.error_count);
		}

		template <typename _InputTag, typename _Input, typename _Output, typename _FromState, typename _ToState,
			typename _DesiredOutput>
		constexpr recode_result<_Input, remove_cvref_t<_DesiredOutput>, _FromState, _ToState>
		__replace_recode_result_output(recode_result<_Input, _Output, _FromState, _ToState>&& __result,
			_DesiredOutput&& __desired_output) noexcept(::std::
			     is_nothrow_constructible_v<recode_result<_Input, _Output, _FromState, _ToState>, _Input&&,
			          _DesiredOutput, _FromState&, _ToState&, encoding_error, ::std::size_t>) {
			using _ReconstructedInput = ranges::tag_range_reconstruct_t<_InputTag, _Input>;
			using _Result = recode_result<_ReconstructedInput, remove_cvref_t<_DesiredOutput>, _FromState, _ToState>;
			return _Result(ranges::reconstruct(::std::in_place_type<_InputTag>, ::std::move(__result.input)),
				::std::forward<_DesiredOutput>(__desired_output), __result.from_state, __result.to_state,
				__result.error_code, __result.error_count);
		}

		template <typename _InputRange, typename _OutputRange, typename _FromState, typename _ToState>
		using __reconstruct_recode_result_t = recode_result<ranges::range_reconstruct_t<_InputRange>,
			ranges::range_reconstruct_t<_OutputRange>, _FromState, _ToState>;

		template <typename _InputRange, typename _OutputRange, typename _ToState, typename _FromState,
			typename _InFirst, typename _InLast, typename _OutFirst, typename _OutLast, typename _ArgToState,
			typename _ArgFromState>
		constexpr decltype(auto) __reconstruct_stateless_recode_result(_InFirst&& __in_first, _InLast&& __in_last,
			_OutFirst&& __out_first, _OutLast&& __out_last, _ArgFromState&& __to_state, _ArgToState&& __from_state,
			encoding_error __error_code, ::std::size_t __error_count) {
			decltype(auto) __in_range  = ranges::reconstruct(::std::in_place_type<_InputRange>,
				 ::std::forward<_InFirst>(__in_first), ::std::forward<_InLast>(__in_last));
			decltype(auto) __out_range = ranges::reconstruct(::std::in_place_type<_OutputRange>,
				::std::forward<_OutFirst>(__out_first), ::std::forward<_OutLast>(__out_last));
			return recode_result<_InputRange, _OutputRange, _FromState, _ToState>(
				::std::forward<decltype(__in_range)>(__in_range),
				::std::forward<decltype(__out_range)>(__out_range), ::std::forward<_ArgFromState>(__from_state),
				::std::forward<_ArgToState>(__to_state), __error_code, __error_count);
		}

		template <typename _InputRange, typename _OutputRange, typename _FromState, typename _ToState,
			typename _InFirst, typename _InLast, typename _OutFirst, typename _OutLast, typename _ArgToState,
			typename _ArgFromState>
		constexpr decltype(auto) __reconstruct_stateless_recode_result(_InFirst&& __in_first, _InLast&& __in_last,
			_OutFirst&& __out_first, _OutLast&& __out_last, _ArgFromState&& __from_state, _ArgToState&& __to_state,
			encoding_error __error_code = encoding_error::ok) {
			return __reconstruct_recode_result_t<_InputRange, _OutputRange, _FromState, _ToState>(
				::std::forward<_InFirst>(__in_first), ::std::forward<_InLast>(__in_last),
				::std::forward<_OutFirst>(__out_first), ::std::forward<_OutLast>(__out_last),
				::std::forward<_ArgFromState>(__from_state), ::std::forward<_ArgToState>(__to_state), __error_code);
		}
	} // namespace __txt_detail

	ZTD_TEXT_INLINE_ABI_NAMESPACE_CLOSE_I_
}} // namespace ztd::text

#include <ztd/epilogue.hpp>

#endif
