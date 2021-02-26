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

#ifndef ZTD_TEXT_COUNT_RESULT_HPP
#define ZTD_TEXT_COUNT_RESULT_HPP

#include <ztd/text/version.hpp>

#include <ztd/text/char8_t.hpp>
#include <ztd/text/unicode_code_point.hpp>
#include <ztd/text/encoding_error.hpp>

#include <ztd/text/detail/reconstruct.hpp>

#include <cstddef>
#include <array>
#include <utility>
#include <system_error>

namespace ztd { namespace text {
	ZTD_TEXT_INLINE_ABI_NAMESPACE_OPEN_I_

	//////
	/// @addtogroup ztd_text_result Result Types
	/// @{
	//////

	//////
	/// @brief The result of counting operations (such as ztd_text_count_code_points and
	/// ztd_text_count_code_points) that specifically do not include a reference to the state.
	//////
	template <typename _Input>
	class stateless_count_result {
	public:
		//////
		/// @brief The reconstructed input_view object, with its .begin() incremented by the number of code units
		/// successfully read (can be identical to .begin() on original range on failure).
		//////
		_Input input;
		//////
		/// @brief The number of code units or code points counted successfully, so far.
		///
		//////
		::std::size_t count;
		//////
		/// @brief The kind of error that occured, if any.
		///
		//////
		encoding_error error_code;
		//////
		/// @brief Whether or not the error handler was invoked, regardless of if the error_code is set or not set to
		/// ztd::text::encoding_error::ok.
		//////
		bool handled_error;

		//////
		/// @brief Constructs a ztd::text::stateless_count_result, defaulting the error code to
		/// ztd::text::encoding_error::ok if not provided.
		///
		/// @param[in] __input The input range to store.
		/// @param[in] __count The number of code points or code units successfully counted.
		/// @param[in] __error_code The error code for the encode operation, taken as the first of either the encode
		/// or decode operation that failed.
		//////
		template <typename _ArgInput>
		constexpr stateless_count_result(
			_ArgInput&& __input, ::std::size_t __count, encoding_error __error_code = encoding_error::ok)
		: stateless_count_result(
			::std::forward<_ArgInput>(__input), __count, __error_code, __error_code != encoding_error::ok) {
		}

		//////
		/// @brief Constructs a ztd::text::stateless_count_result with the provided parameters and
		/// information, including whether or not an error was handled.
		///
		/// @param[in] __input The input range to store.
		/// @param[in] __count The number of code points or code units successfully counted.
		/// @param[in] __error_code The error code for the encode operation, taken as the first of either the encode
		/// or decode operation that failed.
		/// @param[in] __handled_error Whether or not an error was handled. Some error handlers are corrective (see
		/// ztd::text::replacement_handler), and so the error code is not enough to determine if the handler was
		/// invoked. This allows the value to be provided directly when constructing this result type.
		//////
		template <typename _ArgInput>
		constexpr stateless_count_result(
			_ArgInput&& __input, ::std::size_t __count, encoding_error __error_code, bool __handled_error)
		: input(::std::forward<_ArgInput>(__input))
		, count(__count)
		, error_code(__error_code)
		, handled_error(__handled_error) {
		}
	};

	//////
	/// @brief The result of counting operations (such as ztd_text_count_code_points and
	/// ztd_text_count_code_points).
	//////
	template <typename _Input, typename _State>
	class count_result : public stateless_count_result<_Input> {
	private:
		using __base_t = stateless_count_result<_Input>;

	public:
		//////
		/// @brief A reference to the state of the associated Encoding used for counting.
		//////
		::std::reference_wrapper<_State> state;

		//////
		/// @brief Constructs a ztd::text::count_result, defaulting the error code to
		/// ztd::text::encoding_error::ok if not provided.
		///
		/// @param[in] __input The input range to store.
		/// @param[in] __count The number of code points or code units successfully counted.
		/// @param[in] __state The state related to the encoding for the counting operation.
		/// @param[in] __error_code The error code for the encode operation, taken as the first of either the encode
		/// or decode operation that failed.
		//////
		template <typename _ArgInput, typename _ArgState>
		constexpr count_result(_ArgInput&& __input, ::std::size_t __count, _ArgState&& __state,
			encoding_error __error_code = encoding_error::ok)
		: count_result(::std::forward<_ArgInput>(__input), __count, ::std::forward<_ArgState>(__state), __error_code,
			__error_code != encoding_error::ok) {
		}

		//////
		/// @brief Constructs a ztd::text::count_result with the provided parameters and information,
		/// including whether or not an error was handled.
		///
		/// @param[in] __input The input range to store.
		/// @param[in] __count The number of code points or code units successfully counted.
		/// @param[in] __state The state related to the encode operation that counted the code units.
		/// @param[in] __error_code The error code for the encode operation, taken as the first of either the encode
		/// or decode operation that failed.
		/// @param[in] __handled_error Whether or not an error was handled. Some error handlers are corrective (see
		/// ztd::text::replacement_handler), and so the error code is not enough to determine if the handler was
		/// invoked. This allows the value to be provided directly when constructing this result type.
		//////
		template <typename _ArgInput, typename _ArgState>
		constexpr count_result(_ArgInput&& __input, ::std::size_t __count, _ArgState&& __state,
			encoding_error __error_code, bool __handled_error)
		: __base_t(::std::forward<_ArgInput>(__input), __count, __error_code, __handled_error)
		, state(::std::forward<_ArgState>(__state)) {
		}
	};

	//////
	/// @}
	//////

	namespace __detail {
		template <typename _Input, typename _State>
		constexpr stateless_count_result<_Input> __slice_to_stateless(count_result<_Input, _State>&& __result) {
			return __result;
		}

		template <typename _InputRange, typename _State>
		using __reconstruct_count_result_t = count_result<__reconstruct_t<_InputRange>, _State>;

		template <typename _InputRange, typename _State, typename _InFirst, typename _InLast, typename _ArgState>
		constexpr decltype(auto) __reconstruct_count_result(_InFirst&& __in_first, _InLast&& __in_last,
			::std::size_t __count, _ArgState&& __state, encoding_error __error_code, bool __handled_error) {
			decltype(auto) __in_range = __reconstruct(::std::in_place_type<_InputRange>,
				::std::forward<_InFirst>(__in_first), ::std::forward<_InLast>(__in_last));
			return count_result<_InputRange, _State>(::std::forward<decltype(__in_range)>(__in_range), __count,
				::std::forward<_ArgState>(__state), __error_code, __handled_error);
		}

		template <typename _InputRange, typename _State, typename _InFirst, typename _InLast, typename _ArgState>
		constexpr decltype(auto) __reconstruct_count_result(_InFirst&& __in_first, _InLast&& __in_last,
			::std::size_t __count, _ArgState&& __state, encoding_error __error_code = encoding_error::ok) {
			return __reconstruct_count_result<_InputRange, _State>(::std::forward<_InFirst>(__in_first),
				::std::forward<_InLast>(__in_last), __count, ::std::forward<_ArgState>(__state), __error_code,
				__error_code != encoding_error::ok);
		}
	} // namespace __detail

	ZTD_TEXT_INLINE_ABI_NAMESPACE_CLOSE_I_
}} // namespace ztd::text

#endif // ZTD_TEXT_COUNT_RESULT_HPP
