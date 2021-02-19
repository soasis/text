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

#ifndef ZTD_TEXT_VALIDATE_RESULT_HPP
#define ZTD_TEXT_VALIDATE_RESULT_HPP

#include <ztd/text/version.hpp>

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
	/////

	//////
	/// @brief The result of valdation operations (such as @ref ztd_text_validate_code_units and @ref
	/// ztd_text_validate_code_points) that specifically do not include a reference to the state.
	//////
	template <typename _Input>
	class stateless_validate_result {
	public:
		//////
		/// @brief The reconstructed input_view object, with its .begin() incremented by the number of code units
		/// successfully read (can be identical to .begin() on original range on failure).
		//////
		_Input input;
		//////
		/// @brief Whether or not the specified input is valid or not.
		//////
		bool valid;

		//////
		/// @brief Constructs a @ref ztd::text::validate_result, defaulting the error code to @ref
		/// ztd::text::encoding_error::ok if not provided.
		///
		/// @param[in] __input The input range to store.
		/// @param[in] __is_valid Whether or not the validation succeeded.
		//////
		template <typename _ArgInput>
		constexpr stateless_validate_result(_ArgInput&& __input, bool __is_valid)
		: input(::std::forward<_ArgInput>(__input)), valid(__is_valid) {
		}

		//////
		/// @brief A conversion for use in if statements and conditional operators.
		///
		/// @return Whether or not the result is valid or not.
		//////
		constexpr explicit operator bool() const noexcept {
			return valid;
		}
	};

	//////
	/// @brief The result of validation operations (such as @ref ztd_text_validate_code_units and @ref
	/// ztd_text_validate_code_points).
	///
	//////
	template <typename _Input, typename _State>
	class validate_result : public stateless_validate_result<_Input> {
	private:
		using __base_t = stateless_validate_result<_Input>;

	public:
		//////
		/// @brief A reference to the state of the associated Encoding used for validating the input.
		///
		//////
		_State& state;

		//////
		/// @brief Constructs a @ref ztd::text::validate_result, defaulting the error code to @ref
		/// ztd::text::encoding_error::ok if not provided.
		///
		/// @param[in] __input The input range to store.
		/// @param[in] __is_valid Whether or not the validation succeeded.
		/// @param[in] __state The state related to the encoding that was used to do validation.
		//////
		template <typename _ArgInput, typename _ArgState>
		constexpr validate_result(_ArgInput&& __input, bool __is_valid, _ArgState&& __state)
		: __base_t(::std::forward<_ArgInput>(__input), __is_valid), state(::std::forward<_ArgState>(__state)) {
		}
	};

	//////
	/// @}
	/////

	namespace __detail {
		template <typename _Input, typename _State>
		constexpr stateless_validate_result<_Input> __slice_to_stateless(validate_result<_Input, _State>&& __result) {
			return __result;
		}

		template <typename _InputRange, typename _State>
		using __reconstruct_validate_result_t = validate_result<__reconstruct_t<_InputRange>, _State>;

		template <typename _InputRange, typename _State, typename _InFirst, typename _InLast, typename _ArgState>
		constexpr decltype(auto) __reconstruct_validate_result(
			_InFirst&& __in_first, _InLast&& __in_last, bool __is_valid, _ArgState&& __state) {
			using _Result = validate_result<__reconstruct_t<_InputRange>, _State>;

			return _Result(__reconstruct(::std::in_place_type<_InputRange>, ::std::forward<_InFirst>(__in_first),
				               ::std::forward<_InLast>(__in_last)),
				__is_valid, ::std::forward<_State>(__state));
		}
	} // namespace __detail

	ZTD_TEXT_INLINE_ABI_NAMESPACE_CLOSE_I_
}} // namespace ztd::text

#endif // ZTD_TEXT_VALIDATE_RESULT_HPP
