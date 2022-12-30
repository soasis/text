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

#ifndef ZTD_TEXT_NO_ENCODING_HPP
#define ZTD_TEXT_NO_ENCODING_HPP

#include <ztd/text/version.hpp>

#include <ztd/text/code_point.hpp>
#include <ztd/text/code_unit.hpp>
#include <ztd/text/encode_result.hpp>
#include <ztd/text/decode_result.hpp>
#include <ztd/text/error_handler.hpp>
#include <ztd/text/forward.hpp>
#include <ztd/text/is_ignorable_error_handler.hpp>
#include <ztd/text/detail/empty_state.hpp>

#include <ztd/ranges/range.hpp>

#include <ztd/prologue.hpp>

namespace ztd { namespace text {
	ZTD_TEXT_INLINE_ABI_NAMESPACE_OPEN_I_

	//////
	/// @addtogroup ztd_text_encodings Encodings
	///
	/// @{

	//////
	/// @brief A UTF-32 Encoding that traffics in, specifically, the desired code unit type provided as a template
	/// argument.
	///
	/// @tparam _Type The code unit type to use.
	///
	/// @remarks This is a strict UTF-32 implementation that does not allow lone, unpaired surrogates either in or out.
	template <typename _CodeUnit, typename _CodePoint>
	class basic_no_encoding {
	public:
		//////
		/// @brief The state that can be used between calls to the encoder and decoder. It is an empty struct
		/// because there is no shift state to preserve between complete units of encoded information.
		using state = __txt_detail::__empty_state;
		//////
		/// @brief The individual units that result from an encode operation or are used as input to a decode
		/// operation.
		using code_unit = _CodeUnit;
		//////
		/// @brief The individual units that result from a decode operation or as used as input to an encode
		/// operation. For most encodings, this is going to be a Unicode Code Point or a Unicode Scalar Value.
		using code_point = _CodePoint;
		//////
		/// @brief Whether or not the decode operation can process all forms of input into code unit values. This is
		/// always true because this encoding does nothing.
		using is_decode_injective = ::std::integral_constant<bool, ::std::is_same_v<_CodeUnit, _CodePoint>>;
		//////
		/// @brief Whether or not the encode operation can process all forms of input into code unit values. This is
		/// always true because this encoding does nothing.
		using is_encode_injective = ::std::integral_constant<bool, ::std::is_same_v<_CodeUnit, _CodePoint>>;
		//////
		/// @brief The maximum number of code points a single complete operation of decoding can produce. This is 1
		/// because this encoding does nothing.
		inline static constexpr ::std::size_t max_code_points = 1;
		//////
		/// @brief The maximum code units a single complete operation of encoding can produce. This is 1 because this
		/// encoding does nothing.
		inline static constexpr ::std::size_t max_code_units = 1;

		//////
		/// @brief Decodes a single complete unit of information as code points and produces a result with the
		/// input and output ranges moved past what was successfully read and written; or, produces an error and
		/// returns the input and output ranges untouched.
		///
		/// @param[in] __input The input view to read code uunits from.
		/// @param[in] __output The output view to write code points into.
		/// @param[in] __error_handler The error handler to invoke if encoding fails.
		/// @param[in, out] __s The necessary state information. For this encoding, the state is empty and means
		/// very little.
		///
		/// @returns A ztd::text::decode_result object that contains the input range, output range, error handler, and a reference to the passed-in state\.
		///
		/// @remarks To the best ability of the implementation, the iterators will be returned untouched (e.g.,
		/// the input models at least a view and a forward_range). If it is not possible, returned ranges may be
		/// incremented even if an error occurs due to the semantics of any view that models an input_range.
		template <typename _Input, typename _Output, typename _ErrorHandler>
		static constexpr auto decode_one(
			_Input&& __input, _Output&& __output, _ErrorHandler&& __error_handler, state& __s) {
			using _UErrorHandler                = remove_cvref_t<_ErrorHandler>;
			using _SubInput                     = ztd::ranges::subrange_for_t<::std::remove_reference_t<_Input>>;
			using _SubOutput                    = ztd::ranges::subrange_for_t<::std::remove_reference_t<_Output>>;
			using _Result                       = decode_result<_SubInput, _SubOutput, state>;
			constexpr bool __call_error_handler = !is_ignorable_error_handler_v<_UErrorHandler>;

			auto __in_it   = ::ztd::ranges::begin(__input);
			auto __in_last = ::ztd::ranges::end(__input);
			if (__in_it == __in_last) {
				// an exhausted sequence is fine
				return _Result(_SubInput(::std::move(__in_it), ::std::move(__in_last)),
					::std::forward<_Output>(__output), __s, encoding_error::ok);
			}

			auto __out_it  = ::ztd::ranges::begin(__output);
			auto __out_last = ::ztd::ranges::end(__output);

			if constexpr (__call_error_handler) {
				if (__out_it == __out_last) {
					basic_no_encoding __self {};
					return ::std::forward<_ErrorHandler>(__error_handler)(__self,
						_Result(_SubInput(::std::move(__in_it), ::std::move(__in_last)),
						     _SubOutput(::std::move(__out_it), ::std::move(__out_last)), __s,
						     encoding_error::insufficient_output_space),
						::ztd::span<code_unit, 0>(), ::ztd::span<code_point, 0>());
				}
			}
			else {
				(void)__out_last;
			}

			code_unit __unit = *__in_it;
			::ztd::ranges::iter_advance(__in_it);

			*__out_it = static_cast<code_point>(__unit);
			::ztd::ranges::iter_advance(__out_it);

			return _Result(_SubInput(::std::move(__in_it), ::std::move(__in_last)),
				_SubOutput(::std::move(__out_it), ::std::move(__out_last)), __s, encoding_error::ok);
		}

		//////
		/// @brief Encodes a single complete unit of information as code units and produces a result with the
		/// input and output ranges moved past what was successfully read and written; or, produces an error and
		/// returns the input and output ranges untouched.
		///
		/// @param[in] __input The input view to read code points from.
		/// @param[in] __output The output view to write code units into.
		/// @param[in] __error_handler The error handler to invoke if encoding fails.
		/// @param[in, out] __s The necessary state information. For this encoding, the state is empty and means
		/// very little.
		///
		/// @returns A ztd::text::encode_result object that contains the input range, output range, error handler, and a reference to the passed-in state\.
		///
		/// @remarks To the best ability of the implementation, the iterators will be returned untouched (e.g.,
		/// the input models at least a view and a forward_range). If it is not possible, returned ranges may be
		/// incremented even if an error occurs due to the semantics of any view that models an input_range.
		template <typename _Input, typename _Output, typename _ErrorHandler>
		static constexpr auto encode_one(
			_Input&& __input, _Output&& __output, _ErrorHandler&& __error_handler, state& __s) {
			using _UErrorHandler                = remove_cvref_t<_ErrorHandler>;
			using _SubInput                     = ztd::ranges::subrange_for_t<::std::remove_reference_t<_Input>>;
			using _SubOutput                    = ztd::ranges::subrange_for_t<::std::remove_reference_t<_Output>>;
			using _Result                       = encode_result<_SubInput, _SubOutput, state>;
			constexpr bool __call_error_handler = !is_ignorable_error_handler_v<_UErrorHandler>;

			auto __in_it   = ::ztd::ranges::begin(__input);
			auto __in_last = ::ztd::ranges::end(__input);
			if (__in_it == __in_last) {
				// an exhausted sequence is fine
				return _Result(_SubInput(::std::move(__in_it), ::std::move(__in_last)),
					::std::forward<_Output>(__output), __s, encoding_error::ok);
			}

			auto __out_it  = ::ztd::ranges::begin(__output);
			auto __out_last = ::ztd::ranges::end(__output);

			if constexpr (__call_error_handler) {
				if (__out_it == __out_last) {
					basic_no_encoding __self {};
					return ::std::forward<_ErrorHandler>(__error_handler)(__self,
						_Result(_SubInput(::std::move(__in_it), ::std::move(__in_last)),
						     _SubOutput(::std::move(__out_it), ::std::move(__out_last)), __s,
						     encoding_error::insufficient_output_space),
						::ztd::span<code_point, 0>(), ::ztd::span<code_unit, 0>());
				}
			}
			else {
				(void)__out_last;
			}

			code_point __points[1] {};
			__points[0]               = *__in_it;
			const code_point& __point = __points[0];
			::ztd::ranges::iter_advance(__in_it);

			*__out_it = static_cast<code_unit>(__point);
			::ztd::ranges::iter_advance(__out_it);

			return _Result(_SubInput(::std::move(__in_it), ::std::move(__in_last)),
				_SubOutput(::std::move(__out_it), ::std::move(__out_last)), __s, encoding_error::ok);
		}
	};

	//////
	/// @brief A do-nothing encoding for `char` types of input.
	using no_encoding_t = basic_no_encoding<char, unicode_code_point>;

	//////
	/// @brief An object for the ztd::text::no_encoding_t type.
	inline constexpr no_encoding_t no_encoding = {};

	//////
	/// @brief A do-nothing encoding for ztd::text::unicode_code_point types of input.
	using no_codepoint_encoding_t = basic_no_encoding<unicode_code_point, unicode_code_point>;

	//////
	/// @brief An object for the ztd::text::no_codepoint_encoding_t type.
	inline constexpr no_codepoint_encoding_t no_codepoint_encoding = {};

	//////
	/// @}

	ZTD_TEXT_INLINE_ABI_NAMESPACE_CLOSE_I_
}} // namespace ztd::text


#include <ztd/epilogue.hpp>

#endif
