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

#ifndef ZTD_TEXT_UTF16_HPP
#define ZTD_TEXT_UTF16_HPP

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

	namespace __txt_impl {
		//////
		/// @brief Internal tag for detecting a ztd::text-derved UTF-16 type.
		///
		/// @internal
		//////
		class __utf16_tag { };

		//////
		/// @brief An internal type meant to provide the bulk of the UTF-16 functionality.
		///
		/// @internal
		///
		/// @remarks Relies on CRTP.
		//////
		template <typename _Derived = void, typename _CodeUnit = char16_t, typename _CodePoint = unicode_code_point,
			bool __surrogates_allowed = false>
		class __utf16_with : public __utf16_tag {
		private:
			using __self_t = ::std::conditional_t<::std::is_void_v<_Derived>, __utf16_with, _Derived>;

		public:
			//////
			/// @brief Whether or not this encoding that can encode all of Unicode.
			using is_unicode_encoding = ::std::true_type;
			//////
			/// @brief The state that can be used between calls to the encoder and decoder. It is an empty struct
			/// because there is no shift state to preserve between complete units of encoded information.
			//////
			using state = __txt_detail::__empty_state;
			//////
			/// @brief The individual units that result from an encode operation or are used as input to a decode
			/// operation. For UTF-16 formats, this is usually char16_t, but this can change (see
			/// ztd::text::basic_utf16).
			//////
			using code_unit = _CodeUnit;
			//////
			/// @brief The individual units that result from a decode operation or as used as input to an encode
			/// operation. For most encodings, this is going to be a Unicode Code Point or a Unicode Scalar Value.
			//////
			using code_point = _CodePoint;
			//////
			/// @brief Whether or not the decode operation can process all forms of input into code point values.
			/// Thsi is true for all Unicode Transformation Formats (UTFs), which can encode and decode without a
			/// loss of information from a valid collection of code units.
			//////
			using is_decode_injective = ::std::true_type;
			//////
			/// @brief Whether or not the encode operation can process all forms of input into code unit values.
			/// This is true for all Unicode Transformation Formats (UTFs), which can encode and decode without loss
			/// of information from a valid input code point.
			//////
			using is_encode_injective = ::std::true_type;
			//////
			/// @brief The maximum number of code points a single complete operation of decoding can produce. This is
			/// 1 for all Unicode Transformation Format (UTF) encodings.
			//////
			inline static constexpr ::std::size_t max_code_points = 1;
			//////
			/// @brief The maximum code units a single complete operation of encoding can produce.
			inline static constexpr ::std::size_t max_code_units = 2;

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
			//////
			template <typename _InputRange, typename _OutputRange, typename _ErrorHandler>
			static constexpr auto decode_one(
				_InputRange&& __input, _OutputRange&& __output, _ErrorHandler&& __error_handler, state& __s) {
				using _UInputRange   = remove_cvref_t<_InputRange>;
				using _UOutputRange  = remove_cvref_t<_OutputRange>;
				using _UErrorHandler = remove_cvref_t<_ErrorHandler>;
				using _Result = __txt_detail::__reconstruct_decode_result_t<_InputRange, _OutputRange, state>;
				constexpr bool __call_error_handler = !is_ignorable_error_handler_v<_UErrorHandler>;

				auto __in_it   = ranges::ranges_adl::adl_begin(__input);
				auto __in_last = ranges::ranges_adl::adl_end(__input);
				if constexpr (__call_error_handler) {
					if (__in_it == __in_last) {
						// an exhausted sequence is fine
						return _Result(ranges::reconstruct(::std::in_place_type<_UInputRange>,
							               ::std::move(__in_it), ::std::move(__in_last)),
							ranges::reconstruct(
							     ::std::in_place_type<_UOutputRange>, ::std::forward<_OutputRange>(__output)),
							__s, encoding_error::ok);
					}
				}
				else {
					(void)__in_last;
				}

				auto __out_it  = ranges::ranges_adl::adl_begin(__output);
				auto __outlast = ranges::ranges_adl::adl_end(__output);

				if constexpr (__call_error_handler) {
					if (__out_it == __outlast) {
						__self_t __self {};
						return __error_handler(__self,
							_Result(ranges::reconstruct(::std::in_place_type<_UInputRange>, ::std::move(__in_it),
							             ::std::move(__in_last)),
							     ranges::reconstruct(::std::in_place_type<_UOutputRange>, ::std::move(__out_it),
							          ::std::move(__outlast)),
							     __s, encoding_error::insufficient_output_space),
							::ztd::span<code_unit, 0>(), ::ztd::span<code_point, 0>());
					}
				}
				else {
					(void)__outlast;
				}

				::std::array<code_unit, 2> __units {};
				__units[0]              = *__in_it;
				const code_unit& __lead = __units[0];
				ranges::advance(__in_it);

				if (!__ztd_idk_detail_is_surrogate(__lead)) {
					*__out_it = static_cast<code_point>(__lead);
					ranges::advance(::std::move(__out_it));
					return _Result(ranges::reconstruct(::std::in_place_type<_UInputRange>, ::std::move(__in_it),
						               ::std::move(__in_last)),
						ranges::reconstruct(
						     ::std::in_place_type<_UOutputRange>, ::std::move(__out_it), ::std::move(__outlast)),
						__s, encoding_error::ok);
				}
				if constexpr (__call_error_handler) {
					if (!__ztd_idk_detail_is_lead_surrogate(__lead)) {
						__self_t __self {};
						return __error_handler(__self,
							_Result(ranges::reconstruct(::std::in_place_type<_UInputRange>, ::std::move(__in_it),
							             ::std::move(__in_last)),
							     ranges::reconstruct(::std::in_place_type<_UOutputRange>, ::std::move(__out_it),
							          ::std::move(__outlast)),
							     __s, encoding_error::invalid_sequence),
							::ztd::span<code_unit, 1>(__units.data(), 1), ::ztd::span<code_point, 0>());
					}
				}
				if constexpr (__call_error_handler) {
					if (__in_it == __in_last) {
						__self_t __self {};
						return __error_handler(__self,
							_Result(ranges::reconstruct(::std::in_place_type<_UInputRange>, ::std::move(__in_it),
							             ::std::move(__in_last)),
							     ranges::reconstruct(::std::in_place_type<_UOutputRange>, ::std::move(__out_it),
							          ::std::move(__outlast)),
							     __s, encoding_error::incomplete_sequence),
							::ztd::span<code_unit, 1>(__units.data(), 1), ::ztd::span<code_point, 0>());
					}
				}

				__units[1]               = *__in_it;
				const code_unit& __trail = __units[1];
				ranges::advance(__in_it);
				if constexpr (__call_error_handler) {
					if (!__ztd_idk_detail_is_trail_surrogate(__trail)) {
						__self_t __self {};
						return __error_handler(__self,
							_Result(ranges::reconstruct(::std::in_place_type<_UInputRange>, ::std::move(__in_it),
							             ::std::move(__in_last)),
							     ranges::reconstruct(::std::in_place_type<_UOutputRange>, ::std::move(__out_it),
							          ::std::move(__outlast)),
							     __s, encoding_error::invalid_sequence),
							::ztd::span<code_unit, 2>(__units.data(), 2), ::ztd::span<code_point, 0>());
					}
				}
				*__out_it = static_cast<code_point>(__ztd_idk_detail_utf16_combine_surrogates(
					static_cast<char16_t>(__lead), static_cast<char16_t>(__trail)));
				ranges::advance(::std::move(__out_it));

				return _Result(ranges::reconstruct(::std::in_place_type<_UInputRange>, ::std::move(__in_it),
					               ::std::move(__in_last)),
					ranges::reconstruct(
					     ::std::in_place_type<_UOutputRange>, ::std::move(__out_it), ::std::move(__outlast)),
					__s, encoding_error::ok);
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
			//////
			template <typename _InputRange, typename _OutputRange, typename _ErrorHandler>
			static constexpr auto encode_one(
				_InputRange&& __input, _OutputRange&& __output, _ErrorHandler&& __error_handler, state& __s) {
				using _UInputRange   = remove_cvref_t<_InputRange>;
				using _UOutputRange  = remove_cvref_t<_OutputRange>;
				using _UErrorHandler = remove_cvref_t<_ErrorHandler>;
				using _Result = __txt_detail::__reconstruct_encode_result_t<_InputRange, _OutputRange, state>;
				constexpr bool __call_error_handler = !is_ignorable_error_handler_v<_UErrorHandler>;

				auto __in_it   = ranges::ranges_adl::adl_begin(__input);
				auto __in_last = ranges::ranges_adl::adl_end(__input);
				if (__in_it == __in_last) {
					// an exhausted sequence is fine
					return _Result(ranges::reconstruct(::std::in_place_type<_UInputRange>, ::std::move(__in_it),
						               ::std::move(__in_last)),
						ranges::reconstruct(
						     ::std::in_place_type<_UOutputRange>, ::std::forward<_OutputRange>(__output)),
						__s, encoding_error::ok);
				}

				auto __out_it  = ranges::ranges_adl::adl_begin(__output);
				auto __outlast = ranges::ranges_adl::adl_end(__output);

				if constexpr (__call_error_handler) {
					if (__out_it == __outlast) {
						__self_t __self {};
						return __error_handler(__self,
							_Result(ranges::reconstruct(::std::in_place_type<_UInputRange>, ::std::move(__in_it),
							             ::std::move(__in_last)),
							     ranges::reconstruct(::std::in_place_type<_UOutputRange>, ::std::move(__out_it),
							          ::std::move(__outlast)),
							     __s, encoding_error::insufficient_output_space),
							::ztd::span<code_point, 0>(), ::ztd::span<code_unit, 0>());
					}
				}
				else {
					(void)__outlast;
				}


				code_point __points[1] {};
				__points[0]               = *__in_it;
				const code_point& __point = __points[0];
				ranges::advance(__in_it);

				if constexpr (__call_error_handler) {
					if (__point > __ztd_idk_detail_last_unicode_code_point) {
						__self_t __self {};
						return __error_handler(__self,
							_Result(ranges::reconstruct(::std::in_place_type<_UInputRange>, ::std::move(__in_it),
							             ::std::move(__in_last)),
							     ranges::reconstruct(::std::in_place_type<_UOutputRange>, ::std::move(__out_it),
							          ::std::move(__outlast)),
							     __s, encoding_error::invalid_sequence),
							::ztd::span<code_point, 1>(::std::addressof(__points[0]), 1),
							::ztd::span<code_unit, 0>());
					}
				}

				if (__point <= __ztd_idk_detail_last_bmp_value) {
					*__out_it = static_cast<char16_t>(__point);
					ranges::advance(__out_it);
				}
				else {
					auto __normal = __point - __ztd_idk_detail_normalizing_value;
					auto __lead   = __ztd_idk_detail_first_lead_surrogate
						+ ((__normal & __ztd_idk_detail_lead_surrogate_bitmask)
						     >> __ztd_idk_detail_lead_shifted_bits);
					auto __trail = __ztd_idk_detail_first_trail_surrogate
						+ (__normal & __ztd_idk_detail_trail_surrogate_bitmask);

					code_unit __lead16  = static_cast<code_unit>(static_cast<char16_t>(__lead));
					code_unit __trail16 = static_cast<code_unit>(static_cast<char16_t>(__trail));

					*__out_it = __lead16;
					ranges::advance(__out_it);

					if constexpr (__call_error_handler) {
						if (__out_it == __outlast) {
							__self_t __self {};
							return __error_handler(__self,
								_Result(ranges::reconstruct(::std::in_place_type<_UInputRange>,
								             ::std::move(__in_it), ::std::move(__in_last)),
								     ranges::reconstruct(::std::in_place_type<_UOutputRange>,
								          ::std::move(__out_it), ::std::move(__outlast)),
								     __s, encoding_error::insufficient_output_space),
								::ztd::span<code_point, 1>(::std::addressof(__points[0]), 1),
								::ztd::span<code_unit, 1>(::std::addressof(__trail16), 1));
						}
					}
					*__out_it = __trail16;
					ranges::advance(__out_it);
				}

				return _Result(ranges::reconstruct(::std::in_place_type<_UInputRange>, ::std::move(__in_it),
					               ::std::move(__in_last)),
					ranges::reconstruct(
					     ::std::in_place_type<_UOutputRange>, ::std::move(__out_it), ::std::move(__outlast)),
					__s, encoding_error::ok);
			}
		};
	} // namespace __txt_impl

	//////
	/// @addtogroup ztd_text_encodings Encodings
	/// @{
	//////

	//////
	/// @brief A UTF-16 Encoding that traffics in, specifically, the desired code unit type provided as a template
	/// argument.
	///
	/// @tparam _CodeUnit The code unit type to use.
	/// @tparam _CodePoint The code point type to use.
	///
	/// @remarks This is a strict UTF-16 implementation that does not allow lone, unpaired surrogates either in or out.
	//////
	template <typename _CodeUnit, typename _CodePoint = unicode_code_point>
	class basic_utf16 : public __txt_impl::__utf16_with<basic_utf16<_CodeUnit, _CodePoint>, _CodeUnit, _CodePoint> { };

	//////
	/// @brief A UTF-16 Encoding that traffics in char16_t. See ztd::text::basic_utf16 for more details.
	using utf16_t = basic_utf16<char16_t>;

	//////
	/// @brief An instance of the UTF-16 encoding for ease of use.
	inline constexpr utf16_t utf16 = {};

	//////
	/// @brief A UTF-16 Encoding that traffics in wchar_t. See ztd::text::basic_utf16 for more details.
	using wide_utf16_t = basic_utf16<wchar_t>;

	//////
	/// @brief An instance of the UTF-16 that traffics in wchar_t for ease of use.
	inline constexpr wide_utf16_t wide_utf16 = {};

	//////
	/// @}


	ZTD_TEXT_INLINE_ABI_NAMESPACE_CLOSE_I_
}} // namespace ztd::text

#include <ztd/epilogue.hpp>

#endif // ZTD_TEXT_UTF16_HPP
