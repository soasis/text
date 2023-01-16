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

#ifndef ZTD_TEXT_UTF32_HPP
#define ZTD_TEXT_UTF32_HPP

#include <ztd/text/version.hpp>

#include <ztd/text/code_point.hpp>
#include <ztd/text/code_unit.hpp>
#include <ztd/text/encode_result.hpp>
#include <ztd/text/decode_result.hpp>
#include <ztd/text/error_handler.hpp>
#include <ztd/text/forward.hpp>
#include <ztd/text/is_ignorable_error_handler.hpp>
#include <ztd/text/skip_input_error.hpp>
#include <ztd/text/detail/empty_state.hpp>

#include <ztd/ranges/range.hpp>
#include <ztd/idk/text_encoding_id.hpp>

#include <ztd/prologue.hpp>

namespace ztd { namespace text {
	ZTD_TEXT_INLINE_ABI_NAMESPACE_OPEN_I_

	namespace __txt_impl {
		//////
		/// @brief Internal ag for detecting a ztd::text-derved UTF-32 type.
		///
		/// @internal
		class __utf32_tag { };

		//////
		/// @brief An internal type meant to provide the bulk of the UTF-32 functionality.
		///
		/// @internal
		///
		/// @remarks Relies on CRTP.
		template <typename _Derived = void, typename _CodeUnit = char32_t, typename _CodePoint = unicode_code_point,
			bool __validate_decodable_as = true, bool __surrogates_allowed = false>
		class __utf32_with : public __utf32_tag {
		private:
			using __self_t = ::std::conditional_t<::std::is_void_v<_Derived>, __utf32_with, _Derived>;

		public:
			//////
			/// @brief Whether or not this encoding that can encode all of Unicode.
			using is_unicode_encoding = ::std::true_type;
			//////
			/// @brief The start of a sequence can be found unambiguously when dropped into the middle of a sequence
			/// or after an error in reading as occurred for encoded text.
			///
			/// @remarks Unicode has definitive bit patterns which resemble start and end sequences. For UTF-32,
			/// there is only 1 code point per fully encoded character.
			using self_synchronizing_code = ::std::true_type;
			//////
			/// @brief The state that can be used between calls to the encoder and decoder. It is an empty struct
			/// because there is no shift state to preserve between complete units of encoded information.
			using state = __txt_detail::__empty_state;
			//////
			/// @brief The individual units that result from an encode operation or are used as input to a decode
			/// operation. For UTF-32 formats, this is usually char32_t, but this can change (see
			/// ztd::text::basic_utf32).
			using code_unit = _CodeUnit;
			//////
			/// @brief The individual units that result from a decode operation or as used as input to an encode
			/// operation. For most encodings, this is going to be a Unicode Code Point or a Unicode Scalar Value.
			using code_point = _CodePoint;
			//////
			/// @brief Whether or not the decode operation can process all forms of input into code point values.
			/// Thsi is true for all Unicode Transformation Formats (UTFs), which can encode and decode without a
			/// loss of information from a valid collection of code units.
			using is_decode_injective = ::std::true_type;
			//////
			/// @brief Whether or not the encode operation can process all forms of input into code unit values.
			/// This is true for all Unicode Transformation Formats (UTFs), which can encode and decode without loss
			/// of information from a valid input code point.
			using is_encode_injective = ::std::true_type;
			//////
			/// @brief The maximum number of code points a single complete operation of decoding can produce. This is
			/// 1 for all Unicode Transformation Format (UTF) encodings.
			inline static constexpr ::std::size_t max_code_points = 1;
			//////
			/// @brief The maximum code units a single complete operation of encoding can produce.
			inline static constexpr ::std::size_t max_code_units = 1;
			//////
			///@brief The encoding ID for this type. Used for optimization purposes.
			inline static constexpr ::ztd::text_encoding_id encoded_id
				= __surrogates_allowed ? ::ztd::text_encoding_id::ucs4 : ::ztd::text_encoding_id::utf32;
			//////
			///@brief The encoding ID for this type. Used for optimization purposes.
			inline static constexpr ::ztd::text_encoding_id decoded_id
				= __surrogates_allowed ? ::ztd::text_encoding_id::ucs4 : ::ztd::text_encoding_id::utf32;

			///////
			/// @brief Allows an encoding to discard input characters if an error occurs, taking in both the
			/// state and the input sequence (by reference) to modify.
			///
			/// @remarks This will skip every input value until a proper UTF-32 unicode scalar value (or code point)
			/// is found.
			template <typename _Result, typename _InputProgress, typename _OutputProgress>
			static constexpr auto skip_input_error(_Result&& __result, const _InputProgress& __input_progress,
				const _OutputProgress& __output_progress) noexcept {
				if constexpr (__surrogates_allowed) {
					return ::ztd::text::skip_utf32_with_surrogates_input_error(
						::std::forward<_Result>(__result), __input_progress, __output_progress);
				}
				else {
					return ::ztd::text::skip_utf32_input_error(
						::std::forward<_Result>(__result), __input_progress, __output_progress);
				}
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
			/// @returns A ztd::text::decode_result object that contains the reconstructed input range,
			/// reconstructed output range, error handler, and a reference to the passed-in state.
			///
			/// @remarks To the best ability of the implementation, the iterators will be returned untouched (e.g.,
			/// the input models at least a view and a forward_range). If it is not possible, returned ranges may be
			/// incremented even if an error occurs due to the semantics of any view that models an input_range.
			template <typename _Input, typename _Output, typename _ErrorHandler>
			static constexpr auto decode_one(
				_Input&& __input, _Output&& __output, _ErrorHandler&& __error_handler, state& __s) {
				using _UErrorHandler = remove_cvref_t<_ErrorHandler>;
				using _SubInput      = ztd::ranges::subrange_for_t<::std::remove_reference_t<_Input>>;
				using _SubOutput     = ztd::ranges::subrange_for_t<::std::remove_reference_t<_Output>>;
				using _Result        = decode_result<_SubInput, _SubOutput, state>;
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
						__self_t __self {};
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

				code_unit __unit  = static_cast<code_unit>(*__in_it);
				char32_t __unit32 = static_cast<char32_t>(__unit);
				if constexpr (__validate_decodable_as && __call_error_handler) {
					if (__unit32 > __ztd_idk_detail_last_unicode_code_point
						|| (!__surrogates_allowed
						     && __ztd_idk_detail_is_surrogate(static_cast<char32_t>(__unit)))) {
						__self_t __self {};
						return ::std::forward<_ErrorHandler>(__error_handler)(__self,
							_Result(_SubInput(::std::move(__in_it), ::std::move(__in_last)),
							     _SubOutput(::std::move(__out_it), ::std::move(__out_last)), __s,
							     encoding_error::invalid_sequence),
							::ztd::span<code_unit, 0>(), ::ztd::span<code_point, 0>());
					}
				}

				*__out_it = __unit;
				::ztd::ranges::iter_advance(__out_it);
				::ztd::ranges::iter_advance(__in_it);

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
			/// @returns A ztd::text::encode_result object that contains the reconstructed input range,
			/// reconstructed output range, error handler, and a reference to the passed-in state.
			///
			/// @remarks To the best ability of the implementation, the iterators will be returned untouched (e.g.,
			/// the input models at least a view and a forward_range). If it is not possible, returned ranges may be
			/// incremented even if an error occurs due to the semantics of any view that models an input_range.
			template <typename _Input, typename _Output, typename _ErrorHandler>
			static constexpr auto encode_one(
				_Input&& __input, _Output&& __output, _ErrorHandler&& __error_handler, state& __s) {
				using _UErrorHandler = remove_cvref_t<_ErrorHandler>;
				using _SubInput      = ztd::ranges::subrange_for_t<::std::remove_reference_t<_Input>>;
				using _SubOutput     = ztd::ranges::subrange_for_t<::std::remove_reference_t<_Output>>;
				using _Result        = encode_result<_SubInput, _SubOutput, state>;
				constexpr bool __call_error_handler = !is_ignorable_error_handler_v<_UErrorHandler>;

				auto __in_it   = ::ztd::ranges::begin(__input);
				auto __in_last = ::ztd::ranges::end(__input);
				if (__in_it == __in_last) {
					// an exhausted sequence is fine
					return _Result(_SubInput(::std::move(__in_it), ::std::move(__in_last)),
						::std::forward<_Output>(__output), __s, encoding_error::ok);
				}

				auto __out_it                    = ::ztd::ranges::begin(__output);
				[[maybe_unused]] auto __out_last = ::ztd::ranges::end(__output);

				char32_t __point32 = static_cast<char32_t>(*__in_it);

				if constexpr (__call_error_handler) {
					if (__point32 > __ztd_idk_detail_last_unicode_code_point
						|| (!__surrogates_allowed && __ztd_idk_detail_is_surrogate(__point32))) {
						__self_t __self {};
						return ::std::forward<_ErrorHandler>(__error_handler)(__self,
							_Result(_SubInput(::std::move(__in_it), ::std::move(__in_last)),
							     _SubOutput(::std::move(__out_it), ::std::move(__out_last)), __s,
							     encoding_error::invalid_sequence),
							::ztd::span<code_point, 0>(), ::ztd::span<code_unit, 0>());
					}

					if (__out_it == __out_last) {
						__self_t __self {};
						return ::std::forward<_ErrorHandler>(__error_handler)(__self,
							_Result(_SubInput(::std::move(__in_it), ::std::move(__in_last)),
							     _SubOutput(::std::move(__out_it), ::std::move(__out_last)), __s,
							     encoding_error::insufficient_output_space),
							::ztd::span<code_point, 0>(), ::ztd::span<code_unit, 0>());
					}
				}


				*__out_it = static_cast<code_point>(__point32);
				::ztd::ranges::iter_advance(__out_it);
				::ztd::ranges::iter_advance(__in_it);

				return _Result(_SubInput(::std::move(__in_it), ::std::move(__in_last)),
					_SubOutput(::std::move(__out_it), ::std::move(__out_last)), __s, encoding_error::ok);
			}
		};
	} // namespace __txt_impl

	//////
	/// @addtogroup ztd_text_encodings Encodings
	///
	/// @{

	//////
	/// @brief A UTF-32 Encoding that traffics in, specifically, the desired code unit type provided as a template
	/// argument.
	///
	/// @tparam _CodeUnit The code unit type to use.
	/// @tparam _CodeUnit The code point type to use.
	///
	/// @remarks This is a strict UTF-32 implementation that does not allow lone, unpaired surrogates either in or out.
	template <typename _CodeUnit, typename _CodePoint = unicode_code_point>
	class basic_utf32 : public __txt_impl::__utf32_with<basic_utf32<_CodeUnit, _CodePoint>, _CodeUnit, _CodePoint> { };

	//////
	/// @brief A UTF-32 Encoding that traffics in char32_t. See ztd::text::basic_utf32 for more details.
	using utf32_t = basic_utf32<char32_t>;

	//////
	/// @brief An instance of the UTF-32 encoding for ease of use.
	inline constexpr utf32_t utf32 = {};

	//////
	/// @brief A UTF-32 Encoding that traffics in wchar_t. See ztd::text::basic_utf32 for more details.
	using wide_utf32_t = basic_utf32<wchar_t>;

	//////
	/// @brief An instance of the UTF-32 that traffics in wchar_t for ease of use.
	inline constexpr wide_utf32_t wide_utf32 = {};

	//////
	/// @}

	ZTD_TEXT_INLINE_ABI_NAMESPACE_CLOSE_I_
}} // namespace ztd::text


#include <ztd/epilogue.hpp>

#endif
