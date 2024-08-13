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

#ifndef ZTD_TEXT_ISO_8859_1_HPP
#define ZTD_TEXT_ISO_8859_1_HPP

#include <ztd/text/version.hpp>

#include <ztd/prologue.hpp>

namespace ztd { namespace text {
	ZTD_TEXT_INLINE_ABI_NAMESPACE_OPEN_I_

	//////
	/// @brief The ISO/IEC 8859-1 encoding, occasionally referred to as Latin-1 (erroneously). Matches Unicode's
	/// encoding of the first 256 bytes one-to-one.
	///
	/// @tparam _CodeUnit The code unit type to work over.
	/// @tparam _CodePoint The code point type to work over.
	template <typename _CodeUnit, typename _CodePoint = unicode_code_point>
	class basic_iso_8859_1 {
	public:
		//////
		/// @brief The individual units that result from an encode operation or are used as input to a decode
		/// operation.
		using code_unit = _CodeUnit;

		//////
		/// @brief The individual units that result from a decode operation or as used as input to an encode
		/// operation. For most encodings, this is going to be a Unicode Code Point or a Unicode Scalar Value.
		using code_point = _CodePoint;

		//////
		/// @brief The state that can be used between calls to the encoder and decoder. This is empty for this
		/// encoding.
		using state = __txt_detail::__empty_state;

		//////
		/// @brief Whether or not the decode operation can process all forms of input into code point values.
		///
		/// @remarks ISO/IEC 8859-1 can decode from its 8-bit (unpacked) code units to Unicode Code Points. Since the
		/// converion is lossless, this property is true.
		using is_decode_injective = ::std::true_type;
		//////

		/// @brief Whether or not the encode operation can process all forms of input into code unit values. This is
		/// not true for ISO/IEC 8859-1, as many Unicode Code Point and Unicode Scalar Values cannot be represented in
		/// ISO/IEC 8859-1. Since the conversion is lossy, this property is false.
		using is_encode_injective = ::std::false_type;

		//////
		/// @brief The maximum code units a single complete operation of encoding can produce.
		inline static constexpr const ::std::size_t max_code_units = 1;

		//////
		/// @brief The maximum number of code points a single complete operation of decoding can produce.
		inline static constexpr const ::std::size_t max_code_points = 1;

		//////
		/// @brief A range of code units representing the values to use when a replacement happen. For ISO/IEC 8859-1,
		/// this must be '?' instead of the usual Unicode Replacement Character U'�'.
		static constexpr ::ztd::span<const code_unit, 1> replacement_code_units() noexcept {
			return __txt_detail::__question_mark_replacement_units<code_unit>;
		}

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
		/// @returns A ztd::text::decode_result object that contains the input range, output range, error handler, and
		/// a reference to the passed-in state\.
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
					_SubOutput(::std::forward<_Output>(__output)), __s, encoding_error::ok);
			}

			auto __out_it   = ::ztd::ranges::begin(__output);
			auto __out_last = ::ztd::ranges::end(__output);

			if constexpr (__call_error_handler) {
				if (__out_it == __out_last) {
					basic_iso_8859_1 __self {};
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

			code_unit __units[1] {};
			__units[0]              = *__in_it;
			const code_unit& __unit = __units[0];

			if constexpr (__call_error_handler) {
				if (static_cast<unsigned int>(__unit) > static_cast<unsigned int>(0xFF)) {
					basic_iso_8859_1 __self {};
					return ::std::forward<_ErrorHandler>(__error_handler)(__self,
						_Result(_SubInput(::std::move(__in_it), ::std::move(__in_last)),
						     _SubOutput(::std::move(__out_it), ::std::move(__out_last)), __s,
						     encoding_error::invalid_sequence),
						::ztd::span<code_unit, 1>(::std::addressof(__units[0]), 1), ::ztd::span<code_point, 0>());
				}
			}

			::ztd::ranges::iter_advance(__in_it);

			*__out_it = __unit;
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
		/// @returns A ztd::text::encode_result object that contains the input range, output range, error handler, and
		/// a reference to the passed-in state\.
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

			auto __out_it   = ::ztd::ranges::begin(__output);
			auto __out_last = ::ztd::ranges::end(__output);

			if constexpr (__call_error_handler) {
				if (__out_it == __out_last) {
					basic_iso_8859_1 __self {};
					return _Result(::std::forward<_ErrorHandler>(__error_handler)(__self,
						_Result(_SubInput(::std::move(__in_it), ::std::move(__in_last)),
						     _SubOutput(::std::move(__out_it), ::std::move(__out_last)), __s,
						     encoding_error::insufficient_output_space),
						::ztd::span<code_point, 0>(), ::ztd::span<code_unit, 0>()));
				}
			}
			else {
				(void)__out_last;
			}

			code_point __points[1] {};
			__points[0]               = *__in_it;
			const code_point& __point = __points[0];

			if constexpr (__call_error_handler) {
				if (static_cast<unsigned int>(__point) > 0xFF) {
					basic_iso_8859_1 __self {};
					return _Result(::std::forward<_ErrorHandler>(__error_handler)(__self,
						_Result(_SubInput(::std::move(__in_it), ::std::move(__in_last)),
						     _SubOutput(::std::move(__out_it), ::std::move(__out_last)), __s,
						     encoding_error::invalid_sequence),
						::ztd::span<code_point, 1>(::std::addressof(__points[0]), 1),
						::ztd::span<code_unit, 0>()));
				}
			}

			::ztd::ranges::iter_advance(__in_it);

			*__out_it = static_cast<code_unit>(__point);
			::ztd::ranges::iter_advance(__out_it);

			return _Result(_SubInput(::std::move(__in_it), ::std::move(__in_last)),
				_SubOutput(::std::move(__out_it), ::std::move(__out_last)), __s, encoding_error::ok);
		}
	};

	//////
	/// @brief An instance of the basic_iso_8859_1 type for ease of use.
	inline constexpr basic_iso_8859_1<char> iso_8859_1 = {};


	ZTD_TEXT_INLINE_ABI_NAMESPACE_CLOSE_I_
}} // namespace ztd::text

#include <ztd/epilogue.hpp>

#endif
