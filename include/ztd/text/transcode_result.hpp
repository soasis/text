// =============================================================================
//
// ztd.text
// Copyright © JeanHeyd "ThePhD" Meneide and Shepherd's Oasis, LLC
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

#ifndef ZTD_TEXT_TRANSCODE_RESULT_HPP
#define ZTD_TEXT_TRANSCODE_RESULT_HPP

#include <ztd/text/version.hpp>

#include <ztd/text/unicode_code_point.hpp>
#include <ztd/text/encoding_error.hpp>
#include <ztd/text/code_unit.hpp>
#include <ztd/text/code_point.hpp>
#include <ztd/text/state.hpp>
#include <ztd/text/detail/span_reconstruct.hpp>
#include <ztd/text/detail/result_type_constraints.hpp>

#include <ztd/idk/reference_wrapper.hpp>
#include <ztd/ranges/reconstruct.hpp>

#include <cstddef>
#include <array>
#include <utility>

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
	/// @brief The result of transcoding operations (such as ztd::text::transcode) that specifically do not include
	/// a reference to the state.
	template <typename _Input, typename _Output>
	class stateless_transcode_result {
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
		/// @brief Constructs a ztd::text::stateless_transcode_result with the provided parameters and information,
		/// including whether or not an error was handled.
		///
		/// @param[in] __other A different but related result type.
		template <typename _ArgInput, typename _ArgOutput,
			::std::enable_if_t<__txt_detail::__result_type_copy_constraint<::ztd::text::stateless_transcode_result,
			     _Input, _ArgInput, _Output, _ArgOutput>()>* = nullptr>
		constexpr stateless_transcode_result(const stateless_transcode_result<_ArgInput, _ArgOutput>&
			     __other) noexcept(__txt_detail::__result_type_copy_noexcept<::ztd::text::stateless_transcode_result,
			_Input, _ArgInput, _Output, _ArgOutput>())
		: input(__other.input)
		, output(__other.output)
		, error_code(__other.error_code)
		, error_count(__other.error_count) {
		}

		//////
		/// @brief Constructs a ztd::text::stateless_transcode_result with the provided parameters and information,
		/// including whether or not an error was handled.
		///
		/// @param[in] __other A different but related result type.
		template <typename _ArgInput, typename _ArgOutput,
			::std::enable_if_t<__txt_detail::__result_type_move_constraint<::ztd::text::stateless_transcode_result,
			     _Input, _ArgInput, _Output, _ArgOutput>()>* = nullptr>
		constexpr stateless_transcode_result(stateless_transcode_result<_ArgInput, _ArgOutput>&& __other) noexcept(
			__txt_detail::__result_type_move_noexcept<::ztd::text::stateless_transcode_result, _Input, _ArgInput,
			     _Output, _ArgOutput>)
		: input(::std::move(__other.input))
		, output(::std::move(__other.output))
		, error_code(__other.error_code)
		, error_count(__other.error_count) {
		}


		//////
		/// @brief Constructs a ztd::text::stateless_transcode_result, defaulting the error code to
		/// ztd::text::encoding_error::ok if not provided.
		///
		/// @param[in] __input The input range to store.
		/// @param[in] __output The output range to store.
		/// @param[in] __error_code The error code for the encode operation, taken as the first of either the encode
		/// or decode operation that failed.
		template <typename _ArgInput, typename _ArgOutput>
		constexpr stateless_transcode_result(_ArgInput&& __input, _ArgOutput&& __output,
			encoding_error __error_code
			= encoding_error::ok) noexcept(noexcept(stateless_transcode_result(::std::forward<_ArgInput>(__input),
			::std::forward<_ArgOutput>(__output), __error_code, __error_code != encoding_error::ok)))
		: stateless_transcode_result(::std::forward<_ArgInput>(__input), ::std::forward<_ArgOutput>(__output),
			__error_code, __error_code != encoding_error::ok) {
		}

		//////
		/// @brief Constructs a ztd::text::stateless_transcode_result with the provided parameters and
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
		constexpr stateless_transcode_result(_ArgInput&& __input, _ArgOutput&& __output, encoding_error __error_code,
			::std::size_t __error_count) noexcept(::std::is_nothrow_constructible_v<_Input, _ArgInput>
			&& ::std::is_nothrow_constructible_v<_Output, _ArgOutput>)
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
	/// @brief The result of transcoding operations (such as ztd::text::transcode with the state argument provided).
	template <typename _Input, typename _Output, typename _FromState, typename _ToState>
	class pivotless_transcode_result : public stateless_transcode_result<_Input, _Output> {
	private:
		using __base_t = stateless_transcode_result<_Input, _Output>;

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
		/// @brief Constructs a ztd::text::pivotless_transcode_result from a previous pivotless_transcode_result.
		///
		/// @param[in] __other A different but related result type.
		template <typename _ArgInput, typename _ArgOutput, typename _ArgFromState, typename _ArgToState,
			::std::enable_if_t<__txt_detail::__result_type_copy_constraint<::ztd::text::pivotless_transcode_result,
			     _Input, _ArgInput, _Output, _ArgOutput, _FromState, _ArgFromState, _ToState,
			     _ArgToState>()>* = nullptr>
		constexpr pivotless_transcode_result(
			const pivotless_transcode_result<_ArgInput, _ArgOutput, _ArgFromState, _ArgToState>&
			     __other) noexcept(__txt_detail::__result_type_copy_noexcept<::ztd::text::pivotless_transcode_result,
			_Input, _ArgInput, _Output, _ArgOutput, _FromState, _ArgFromState, _ToState, _ArgToState>())
		: __base_t(__other.input, __other.output, __other.error_code, __other.error_count)
		, from_state(__other.from_state)
		, to_state(__other.to_state) {
		}

		//////
		/// @brief Constructs a ztd::text::pivotless_transcode_result from a previous pivotless_transcode_result.
		///
		/// @param[in] __other A different but related result type.
		template <typename _ArgInput, typename _ArgOutput, typename _ArgFromState, typename _ArgToState,
			::std::enable_if_t<__txt_detail::__result_type_move_constraint<::ztd::text::pivotless_transcode_result,
			     _Input, _ArgInput, _Output, _ArgOutput, _FromState, _ArgFromState, _ToState,
			     _ArgToState>()>* = nullptr>
		constexpr pivotless_transcode_result(
			pivotless_transcode_result<_ArgInput, _ArgOutput, _ArgFromState, _ArgToState>&&
			     __other) noexcept(__txt_detail::__result_type_move_noexcept<::ztd::text::pivotless_transcode_result,
			_Input, _ArgInput, _Output, _ArgOutput, _FromState, _ArgFromState, _ToState, _ArgToState>())
		: __base_t(::std::move(__other.input), ::std::move(__other.output), __other.error_code, __other.error_count)
		, from_state(__other.from_state)
		, to_state(__other.to_state) {
		}

		//////
		/// @brief Constructs a ztd::text::pivotless_transcode_result, defaulting the error code to
		/// ztd::text::encoding_error::ok if not provided.
		///
		/// @param[in] __input The input range to store.
		/// @param[in] __output The output range to store.
		/// @param[in] __from_state The state related to the "From Encoding" that performed the decode half of the
		/// operation.
		/// @param[in] __to_state The state related to the "To Encoding" that performed the encode half of the
		/// operation.
		/// @param[in] __error_code The error code for the transcoding operation, taken as the first of either the
		/// encode or decode operation that failed.
		template <typename _ArgInput, typename _ArgOutput, typename _ArgFromState, typename _ArgToState>
		constexpr pivotless_transcode_result(_ArgInput&& __input, _ArgOutput&& __output, _ArgFromState&& __from_state,
			_ArgToState&& __to_state, encoding_error __error_code = encoding_error::ok)
		: pivotless_transcode_result(::std::forward<_ArgInput>(__input), ::std::forward<_ArgOutput>(__output),
			::std::forward<_ArgFromState>(__from_state), ::std::forward<_ArgToState>(__to_state), __error_code,
			__error_code != encoding_error::ok ? static_cast<::std::size_t>(1) : static_cast<::std::size_t>(0)) {
		}

		//////
		/// @brief Constructs a ztd::text::pivotless_transcode_result with the provided parameters and information,
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
		constexpr pivotless_transcode_result(_ArgInput&& __input, _ArgOutput&& __output, _ArgFromState&& __from_state,
			_ArgToState&& __to_state, encoding_error __error_code, ::std::size_t __error_count)
		: __base_t(
			::std::forward<_ArgInput>(__input), ::std::forward<_ArgOutput>(__output), __error_code, __error_count)
		, from_state(::std::forward<_ArgFromState>(__from_state))
		, to_state(::std::forward<_ArgToState>(__to_state)) {
		}
	};

	//////
	/// @brief The result of low-level transcoding operations (such as ztd::text::transcode_into with the pivot
	/// provided as an argument).
	template <typename _Input, typename _Output, typename _FromState, typename _ToState, typename _Pivot>
	class transcode_result : public pivotless_transcode_result<_Input, _Output, _FromState, _ToState> {
	private:
		using __base_t = pivotless_transcode_result<_Input, _Output, _FromState, _ToState>;

	public:
		//////
		/// @brief The range used to hold the intermediate pivot transcoding units.
		ZTD_USE(ZTD_NO_UNIQUE_ADDRESS) _Pivot pivot;
		//////
		/// @brief The kind of error that occured, if any, for the intermediate pivot.
		encoding_error pivot_error_code;
		//////
		/// @brief Whether or not the error handler for the pivot point was invoked, regardless of if the
		/// `pivot_error_code` is set or not set to ztd::text::encoding_error::ok.
		::std::size_t pivot_error_count;

		//////
		/// @brief Constructs a ztd::text::pivotless_transcode_result from a previous pivotless_transcode_result.
		///
		/// @param[in] __other A different but related result type.
		template <typename _ArgInput, typename _ArgOutput, typename _ArgFromState, typename _ArgToState,
			typename _ArgPivot,
			::std::enable_if_t<__txt_detail::__result_type_copy_constraint<::ztd::text::transcode_result, _Input,
			     _ArgInput, _Output, _ArgOutput, _FromState, _ArgFromState, _ToState, _ArgToState, _Pivot,
			     _ArgPivot>()>* = nullptr>
		constexpr transcode_result(
			const transcode_result<_ArgInput, _ArgOutput, _ArgFromState, _ArgToState, _ArgPivot>&
			     __other) noexcept(__txt_detail::__result_type_copy_noexcept<::ztd::text::transcode_result, _Input,
			_ArgInput, _Output, _ArgOutput, _FromState, _ArgFromState, _ToState, _ArgToState, _Pivot, _ArgPivot>())
		: __base_t(__other.input, __other.output, __other.error_code, __other.error_count, __other.from_state,
			__other.to_state)
		, pivot(__other.pivot)
		, pivot_error_code(__other.pivot_error_code)
		, pivot_error_count(__other.pivot_error_count) {
		}

		//////
		/// @brief Constructs a ztd::text::pivotless_transcode_result from a previous pivotless_transcode_result.
		///
		/// @param[in] __other A different but related result type.
		template <typename _ArgInput, typename _ArgOutput, typename _ArgFromState, typename _ArgToState,
			typename _ArgPivot,
			::std::enable_if_t<__txt_detail::__result_type_move_constraint<::ztd::text::transcode_result, _Input,
			     _ArgInput, _Output, _ArgOutput, _FromState, _ArgFromState, _ToState, _ArgToState, _Pivot,
			     _ArgPivot>()>* = nullptr>
		constexpr transcode_result(transcode_result<_ArgInput, _ArgOutput, _ArgFromState, _ArgToState, _ArgPivot>&&
			     __other) noexcept(__txt_detail::__result_type_move_noexcept<::ztd::text::transcode_result, _Input,
			_ArgInput, _Output, _ArgOutput, _FromState, _ArgFromState, _ToState, _ArgToState, _Pivot, _ArgPivot>())
		: __base_t(::std::move(__other.input), ::std::move(__other.output), __other.error_code, __other.error_count,
			__other.from_state, __other.to_state)
		, pivot(::std::move(__other.pivot))
		, pivot_error_code(::std::move(__other.pivot_error_code))
		, pivot_error_count(::std::move(__other.pivot_error_count)) {
		}

		//////
		/// @brief Constructs a ztd::text::pivotless_transcode_result with the provided parameters and information,
		/// including whether or not an error was handled.
		///
		/// @param[in] __input The input range to store.
		/// @param[in] __output The output range to store.
		/// @param[in] __from_state The state related to the "From Encoding" that performed the decode half of the
		/// operation.
		/// @param[in] __to_state The state related to the "To Encoding" that performed the encode half of the
		/// operation.
		/// @param[in] __error_code The error code for the transcode operation, taken as the first of either the
		/// encode or decode operation that failed.
		/// @param[in] __error_count Whether or not an error was handled. Some error handlers are corrective (see
		/// ztd::text::replacement_handler_t), and so the error code is not enough to determine if the handler was
		/// invoked. This allows the value to be provided directly when constructing this result type.
		/// @param[in] __pivot The pivot for this transcode operation.
		/// @param[in] __pivot_error_code The error code for the decode step of the transcode oepration, if it failed.
		/// @param[in] __pivot_error_count Whether or not an error was handled during the decode step of the transcode
		/// operation. Some error handlers are corrective (see ztd::text::replacement_handler_t), and so the error
		/// code is not enough to determine if the handler was invoked. This allows the value to be provided directly
		/// when constructing this result type.
		template <typename _ArgInput, typename _ArgOutput, typename _ArgFromState, typename _ArgToState,
			typename _ArgPivot>
		constexpr transcode_result(_ArgInput&& __input, _ArgOutput&& __output, _ArgFromState&& __from_state,
			_ArgToState&& __to_state, encoding_error __error_code, ::std::size_t __error_count, _ArgPivot&& __pivot,
			encoding_error __pivot_error_code, ::std::size_t __pivot_error_count)
		: __base_t(::std::forward<_ArgInput>(__input), ::std::forward<_ArgOutput>(__output), __from_state, __to_state,
			__error_code, __error_count)
		, pivot(::std::forward<_ArgPivot>(__pivot))
		, pivot_error_code(::std::move(__pivot_error_code))
		, pivot_error_count(::std::move(__pivot_error_count)) {
		}
	};

	//////
	/// @}
	/////

	namespace __txt_detail {
		template <typename _Input, typename _Output, typename _FromState, typename _ToState>
		constexpr stateless_transcode_result<_Input, _Output>
		__slice_to_stateless(pivotless_transcode_result<_Input, _Output, _FromState, _ToState>&& __result) noexcept(
			::std::is_nothrow_constructible_v<stateless_transcode_result<_Input, _Output>,
			     stateless_transcode_result<_Input, _Output>>) {
			return ::std::move(__result);
		}

		template <typename _Input, typename _Output, typename _FromState, typename _ToState, typename _Pivot>
		constexpr pivotless_transcode_result<_Input, _Output, _FromState, _ToState>
		__slice_to_pivotless(transcode_result<_Input, _Output, _FromState, _ToState, _Pivot>&& __result) noexcept(
			::std::is_nothrow_constructible_v<pivotless_transcode_result<_Input, _Output, _FromState, _ToState>,
			     pivotless_transcode_result<_Input, _Output, _FromState, _ToState>>) {
			return ::std::move(__result);
		}

		template <typename _Input, typename _Output, typename _FromState, typename _ToState, typename _Pivot>
		constexpr stateless_transcode_result<_Input, _Output> __slice_to_stateless_pivotless(
			transcode_result<_Input, _Output, _FromState, _ToState, _Pivot>&&
			     __result) noexcept(::std::is_nothrow_constructible_v<stateless_transcode_result<_Input, _Output>,
			stateless_transcode_result<_Input, _Output>>) {
			return ::std::move(__result);
		}

		template <typename _Input, typename _Output, typename _DesiredOutput, typename _FromState, typename _ToState>
		constexpr auto __replace_transcode_result_output_no_state(
			pivotless_transcode_result<_Input, _Output, _FromState, _ToState>&& __result,
			_DesiredOutput&& __desired_output) noexcept(::std::
			     is_nothrow_constructible_v<stateless_transcode_result<_Input, _Output>, _Input&&, _DesiredOutput,
			          encoding_error, ::std::size_t>) {
			using _Result = stateless_transcode_result<_Input, remove_cvref_t<_DesiredOutput>>;
			return _Result(::std::move(__result.input), ::std::forward<_DesiredOutput>(__desired_output),
				__result.error_code, __result.error_count);
		}

		template <typename _Input, typename _Output, typename _FromState, typename _ToState, typename _DesiredOutput>
		constexpr auto __replace_transcode_result_output(
			pivotless_transcode_result<_Input, _Output, _FromState, _ToState>&& __result,
			_DesiredOutput&& __desired_output) noexcept(::std::
			     is_nothrow_constructible_v<pivotless_transcode_result<_Input, _Output, _FromState, _ToState>,
			          _Input&&, _DesiredOutput, _FromState&, _ToState&, encoding_error, ::std::size_t>) {
			using _Result = pivotless_transcode_result<_Input, remove_cvref_t<_DesiredOutput>, _FromState, _ToState>;
			return _Result(::std::move(__result.input), ::std::forward<_DesiredOutput>(__desired_output),
				__result.from_state, __result.to_state, __result.error_code, __result.error_count);
		}

		template <typename _InputRange, typename _OutputRange, typename _FromState, typename _ToState>
		using __reconstruct_transcode_result_t = pivotless_transcode_result<ranges::range_reconstruct_t<_InputRange>,
			ranges::range_reconstruct_t<_OutputRange>, _FromState, _ToState>;

		template <typename _InputRange, typename _OutputRange, typename _FromState, typename _ToState,
			typename _Pivot>
		using __reconstruct_pivot_transcode_result_t = transcode_result<ranges::range_reconstruct_t<_InputRange>,
			ranges::range_reconstruct_t<_OutputRange>, _FromState, _ToState, ranges::range_reconstruct_t<_Pivot>>;
	} // namespace __txt_detail

	ZTD_TEXT_INLINE_ABI_NAMESPACE_CLOSE_I_
}} // namespace ztd::text

#include <ztd/epilogue.hpp>

#endif
