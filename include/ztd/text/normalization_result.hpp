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

#ifndef ZTD_TEXT_NORMALIZATION_RESULT_HPP
#define ZTD_TEXT_NORMALIZATION_RESULT_HPP

#include <ztd/text/version.hpp>

#include <ztd/text/no_normalization.hpp>

#include <type_traits>
#include <utility>

#include <ztd/prologue.hpp>

namespace ztd { namespace text {
	ZTD_TEXT_INLINE_ABI_NAMESPACE_OPEN_I_

	//////
	/// @addtogroup ztd_text_normalization Normalization
	/// @{

	//////
	/// @brief The result of an normalization operation.
	enum class normalization_error {
		//////
		/// @brief The output space was insufficient to hold all the provided input.
		insufficient_output = 0,
		//////
		/// @brief No error occurred.
		ok = 1
	};

	//////
	/// @brief The result of a normalization operation.
	template <typename _Input, typename _Output>
	class normalization_result {
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
		normalization_error error_code;

		//////
		/// @brief Constructs a ztd::text::encode_result, defaulting the error code to
		/// ztd::text::encoding_error::ok if not provided.
		///
		/// @param[in] __input The input range to store.
		/// @param[in] __output The output range to store.
		/// @param[in] __error_code The error code for the decoding opertion, if any.
		//////
		template <typename _ArgInput, typename _ArgOutput>
		constexpr normalization_result(_ArgInput&& __input, _ArgOutput&& __output,
			normalization_error __error_code) noexcept(::std::is_nothrow_constructible_v<_Input, _ArgInput> // cf
			     && ::std::is_nothrow_constructible_v<_Output, _ArgOutput>)
		: input(::std::forward<_ArgInput>(__input))
		, output(::std::forward<_ArgOutput>(__output))
		, error_code(__error_code) {
		}
	};

	//////
	/// @}



	ZTD_TEXT_INLINE_ABI_NAMESPACE_CLOSE_I_
}} // namespace ztd::text

#include <ztd/epilogue.hpp>

#endif // ZTD_TEXT_NORMALIZATION_RESULT_HPP
