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

#ifndef ZTD_TEXT_IMPL_FIXED_CUNEICODE_ENCODING_HPP
#define ZTD_TEXT_IMPL_FIXED_CUNEICODE_ENCODING_HPP

#include <ztd/text/version.hpp>

#include <ztd/text/unicode_code_point.hpp>
#include <ztd/text/state.hpp>
#include <ztd/text/code_point.hpp>
#include <ztd/text/code_unit.hpp>
#include <ztd/text/decode_result.hpp>
#include <ztd/text/encode_result.hpp>
#include <ztd/text/is_ignorable_error_handler.hpp>

#include <ztd/cuneicode.h>
#include <ztd/idk/ebco.hpp>
#include <ztd/idk/to_address.hpp>
#include <ztd/idk/size.hpp>
#include <ztd/ranges/adl.hpp>
#include <ztd/ranges/range.hpp>

#include <ztd/prologue.hpp>

namespace ztd { namespace text {
	ZTD_TEXT_INLINE_ABI_NAMESPACE_OPEN_I_

	namespace __txt_impl {

		//////
		/// @brief A cuneicode-based encoding that uses templates to control most of the aspects of the encoding.
		/// Allows for a single implementation for the vast majority of cuneicode-based encodings.
		template <typename _Derived, typename _CodeUnit, typename _CodePoint, ::std::size_t _MaxCodeUnits,
			::std::size_t _MaxCodePoints, typename _DecodeFunc, _DecodeFunc __decode_func, typename _EncodeFunc,
			_EncodeFunc __encode_func, typename _DecodeState, typename _DecodeStateIsCompleteFunc,
			_DecodeStateIsCompleteFunc __decode_state_complete, typename _EncodeState = _DecodeState,
			typename _EncodeStateIsCompleteFunc                = _DecodeStateIsCompleteFunc,
			_EncodeStateIsCompleteFunc __encode_state_complete = __decode_state_complete,
			bool _IsDecodeInjective = false, bool _IsEncodeInjective = false>
		class __fixed_cuneicode {
		public:
			//////
			/// @brief The state for encoding Unicode code points into ASCII-compatible punycode text.
			class encode_state : public _EncodeState {
			public:
				//////
				/// @brief Whether or not this state has completed its current encoding operation.
				bool is_complete() const noexcept {
					return __encode_state_complete(this);
				}
			};

			//////
			/// @brief The state for decoding ASCII-compatible punycode text into Unicode code points.
			class decode_state : public _DecodeState {
			public:
				//////
				/// @brief Whether or not this state has completed its current decoding operation.
				bool is_complete() const noexcept {
					return __decode_state_complete(this);
				}
			};

			//////
			/// @brief The code unit type for encoded text.
			using code_unit = _CodeUnit;

			//////
			/// @brief The code point type for decoded text.
			using code_point = _CodePoint;

			//////
			/// @brief The maximum number of code points output by a decode operation. In this case, only 1 code
			/// point is output at a time.
			inline static constexpr ::std::size_t max_code_points = _MaxCodePoints;
			//////
			/// @brief The maximum number of code units output by a encode operation. In this case, only 1 code unit
			/// is output at a time.
			inline static constexpr ::std::size_t max_code_units = _MaxCodeUnits;

			//////
			/// @brief The whether or not the decode portion of this encoding is injective.
			using is_decode_injective = ::std::integral_constant<bool, _IsDecodeInjective>;

			//////
			/// @brief The whether or not the decode portion of this encoding is injective.
			using is_encode_injective = ::std::integral_constant<bool, _IsEncodeInjective>;

			//////
			/// @brief Decodes a provided range of punycode-encoded data and pulls out data.
			///
			/// @tparam _Input The input range type.
			/// @tparam _Output The output range type.
			/// @tparam _ErrorHandler The error handler type.
			///
			/// @param __input The input range.
			/// @param __output The output range.
			/// @param __error_handler The error handler; this will be called whenever an error occurs during
			/// decoding.
			/// @param __state A reference to the decode state, which contains most of the running information about
			/// a punycode decoding operation. Discarding this between calls is not advised for this encoding type.
			///
			/// @return A ztd::text::decode_result structure with the appropriate input and output types
			/// recosntructed, possibly filtered through an error handler if necessary.
			///
			/// @remarks This function may need to be called with empty input data repeatedly to fully drain any
			/// stored information in the provided `__state`. The entire input may be consumed before any information
			/// is output.
			template <typename _Input, typename _Output, typename _ErrorHandler>
			static auto decode_one(_Input&& __input, _Output&& __output, _ErrorHandler&& __error_handler,
				decode_state& __state) noexcept {
				using _UInput        = remove_cvref_t<_Input>;
				using _UOutput       = remove_cvref_t<_Output>;
				using _UErrorHandler = remove_cvref_t<_ErrorHandler>;
				using _Result        = __txt_detail::__reconstruct_decode_result_t<_UInput, _UOutput, decode_state>;
				constexpr bool __call_error_handler = !is_ignorable_error_handler_v<_UErrorHandler>;

				auto __in_it                              = ::ztd::ranges::cbegin(__input);
				auto __in_last                            = ::ztd::ranges::cend(__input);
				auto __out_it                             = ::ztd::ranges::begin(__output);
				auto __out_last                           = ::ztd::ranges::end(__output);
				char __intermediate_input[max_code_units] = {};
				::std::size_t __in_size                   = 0;
				::std::size_t __incremented_in_size       = 0;
				_Derived __self {};
				if (__in_it != __in_last) {
					__intermediate_input[0] = *__in_it;
					__in_size               = 1;
				}
				else if (::ztd::text::is_state_complete(__self, __state)) {
					return _Result(ranges::reconstruct(::std::in_place_type<_UInput>, ::std::move(__in_it),
						               ::std::move(__in_last)),
						ranges::reconstruct(
						     ::std::in_place_type<_UOutput>, ::std::move(__out_it), ::std::move(__out_last)),
						__state, encoding_error::ok);
				}
				char32_t __intermediate_output[max_code_points] = {};
				::std::size_t __out_size                        = ztd_c_array_size(__intermediate_output);
				for (::std::size_t __index = 1;; ++__index) {
					const char* __typed_in_ptr = static_cast<const char*>(__intermediate_input);
					char32_t* __typed_out_ptr  = static_cast<char32_t*>(__intermediate_output);
					cnc_mcerror __err
						= __decode_func(&__out_size, &__typed_out_ptr, &__in_size, &__typed_in_ptr, &__state);
					if (__err == CNC_MCERROR_INCOMPLETE_INPUT) {
						if constexpr (__call_error_handler) {
							if (__index == max_code_units) {
								return ::std::forward<_ErrorHandler>(__error_handler)(__self,
									_Result(ranges::reconstruct(::std::in_place_type<_UInput>,
									             ::std::move(__in_it), ::std::move(__in_last)),
									     ranges::reconstruct(::std::in_place_type<_UOutput>,
									          ::std::move(__out_it), ::std::move(__out_last)),
									     __state, encoding_error::invalid_sequence),
									::ztd::span<const code_unit>(__intermediate_input, __incremented_in_size),
									::ztd::span<const code_point, 0>());
							}
						}
						++__in_it;
						if (__in_it != __in_last) {
							__intermediate_input[__in_size] = *__in_it;
							++__in_size;
							++__incremented_in_size;
						}
						__out_size = ztd_c_array_size(__intermediate_output);
						continue;
					}
					if constexpr (__call_error_handler) {
						if (__err != CNC_MCERROR_OK) {
							return ::std::forward<_ErrorHandler>(__error_handler)(__self,
								_Result(ranges::reconstruct(::std::in_place_type<_UInput>, ::std::move(__in_it),
								             ::std::move(__in_last)),
								     ranges::reconstruct(::std::in_place_type<_UOutput>, ::std::move(__out_it),
								          ::std::move(__out_last)),
								     __state, static_cast<encoding_error>(__err)),
								::ztd::span<const code_unit>(__intermediate_input, __incremented_in_size),
								::ztd::span<const code_point, 0>());
						}
					}
					break;
				}
				const ::std::size_t __written_output_size = ztd_c_array_size(__intermediate_output) - __out_size;
				for (::std::size_t __out_index = 0; __out_index < __written_output_size;
					++__out_it, ++__out_index) {
					if constexpr (__call_error_handler) {
						if (__out_it == __out_last) {
							return ::std::forward<_ErrorHandler>(__error_handler)(__self,
								_Result(ranges::reconstruct(::std::in_place_type<_UInput>, ::std::move(__in_it),
								             ::std::move(__in_last)),
								     ranges::reconstruct(::std::in_place_type<_UOutput>, ::std::move(__out_it),
								          ::std::move(__out_last)),
								     __state, encoding_error::insufficient_output_space),
								::ztd::span<const code_unit>(__intermediate_input, __incremented_in_size),
								::ztd::span<const code_point, 0>());
						}
					}
					*__out_it = static_cast<code_point>(__intermediate_output[__out_index]);
				}
				if (__in_it != __in_last) {
					++__in_it;
				}
				return _Result(ranges::reconstruct(
					               ::std::in_place_type<_UInput>, ::std::move(__in_it), ::std::move(__in_last)),
					ranges::reconstruct(
					     ::std::in_place_type<_UOutput>, ::std::move(__out_it), ::std::move(__out_last)),
					__state, encoding_error::ok);
			}

			//////
			/// @brief Encodes a provided range of punycode-encoded data and pulls out data.
			///
			/// @tparam _Input The input range type.
			/// @tparam _Output The output range type.
			/// @tparam _ErrorHandler The error handler type.
			///
			/// @param __input The input range.
			/// @param __output The output range.
			/// @param __error_handler The error handler; this will be called whenever an error occurs during
			/// decoding.
			/// @param __state A reference to the encode state, which contains most of the running information about
			/// a punycode decoding operation. Discarding this between calls is not advised for this encoding type.
			///
			/// @return A ztd::text::encode_result structure with the appropriate input and output types
			/// recosntructed, possibly filtered through an error handler if necessary.
			///
			/// @remarks This function may need to be called with empty input data repeatedly to fully drain any
			/// stored information in the provided `__state`. The entire input may be consumed before any information
			/// is output.
			template <typename _Input, typename _Output, typename _ErrorHandler>
			static auto encode_one(
				_Input&& __input, _Output&& __output, _ErrorHandler&& __error_handler, encode_state& __state) {
				using _UInput        = remove_cvref_t<_Input>;
				using _UOutput       = remove_cvref_t<_Output>;
				using _UErrorHandler = remove_cvref_t<_ErrorHandler>;
				using _Result        = __txt_detail::__reconstruct_encode_result_t<_UInput, _UOutput, encode_state>;
				constexpr bool __call_error_handler = !is_ignorable_error_handler_v<_UErrorHandler>;

				auto __in_it                                   = ::ztd::ranges::cbegin(__input);
				auto __in_last                                 = ::ztd::ranges::cend(__input);
				auto __out_it                                  = ::ztd::ranges::begin(__output);
				auto __out_last                                = ::ztd::ranges::end(__output);
				char32_t __intermediate_input[max_code_points] = {};
				::std::size_t __in_size                        = 0;
				::std::size_t __incremented_in_size            = 0;
				_Derived __self {};
				if (__in_it != __in_last) {
					__intermediate_input[0] = *__in_it;
					__in_size               = 1;
				}
				else if (::ztd::text::is_state_complete(__self, __state)) {
					return _Result(ranges::reconstruct(::std::in_place_type<_UInput>, ::std::move(__in_it),
						               ::std::move(__in_last)),
						ranges::reconstruct(
						     ::std::in_place_type<_UOutput>, ::std::move(__out_it), ::std::move(__out_last)),
						__state, encoding_error::ok);
				}
				char __intermediate_output[max_code_units] = {};
				::std::size_t __out_size                   = ztd_c_array_size(__intermediate_output);
				for (::std::size_t __index = 1;; ++__index) {
					const char32_t* __typed_in_ptr = static_cast<const char32_t*>(__intermediate_input);
					char* __typed_out_ptr          = static_cast<char*>(__intermediate_output);
					cnc_mcerror __err
						= __encode_func(&__out_size, &__typed_out_ptr, &__in_size, &__typed_in_ptr, &__state);
					if (__err == CNC_MCERROR_INCOMPLETE_INPUT) {
						if constexpr (__call_error_handler) {
							if (__index == max_code_points) {
								return ::std::forward<_ErrorHandler>(__error_handler)(__self,
									_Result(ranges::reconstruct(::std::in_place_type<_UInput>,
									             ::std::move(__in_it), ::std::move(__in_last)),
									     ranges::reconstruct(::std::in_place_type<_UOutput>,
									          ::std::move(__out_it), ::std::move(__out_last)),
									     __state, encoding_error::invalid_sequence),
									::ztd::span<const code_point>(__intermediate_input, __incremented_in_size),
									::ztd::span<const code_unit, 0>());
							}
						}
						++__in_it;
						if (__in_it != __in_last) {
							__intermediate_input[__in_size] = *__in_it;
							++__in_size;
							++__incremented_in_size;
						}
						__out_size = ztd_c_array_size(__intermediate_output);
						continue;
					}
					if constexpr (__call_error_handler) {
						if (__err != CNC_MCERROR_OK) {
							return ::std::forward<_ErrorHandler>(__error_handler)(__self,
								_Result(ranges::reconstruct(::std::in_place_type<_UInput>, ::std::move(__in_it),
								             ::std::move(__in_last)),
								     ranges::reconstruct(::std::in_place_type<_UOutput>, ::std::move(__out_it),
								          ::std::move(__out_last)),
								     __state, static_cast<encoding_error>(__err)),
								::ztd::span<const code_point>(__intermediate_input, __incremented_in_size),
								::ztd::span<const code_unit, 0>());
						}
					}
					break;
				}
				const ::std::size_t __written_output_size = ztd_c_array_size(__intermediate_output) - __out_size;
				for (::std::size_t __out_index = 0; __out_index < __written_output_size;
					++__out_it, ++__out_index) {
					if constexpr (__call_error_handler) {
						if (__out_it == __out_last) {
							return ::std::forward<_ErrorHandler>(__error_handler)(__self,
								_Result(ranges::reconstruct(::std::in_place_type<_UInput>, ::std::move(__in_it),
								             ::std::move(__in_last)),
								     ranges::reconstruct(::std::in_place_type<_UOutput>, ::std::move(__out_it),
								          ::std::move(__out_last)),
								     __state, encoding_error::insufficient_output_space),
								::ztd::span<const code_point>(__intermediate_input, __incremented_in_size),
								::ztd::span<const code_unit, 0>());
						}
					}
					*__out_it = __intermediate_output[__out_index];
				}
				if (__in_it != __in_last) {
					++__in_it;
				}
				return _Result(ranges::reconstruct(
					               ::std::in_place_type<_UInput>, ::std::move(__in_it), ::std::move(__in_last)),
					ranges::reconstruct(
					     ::std::in_place_type<_UOutput>, ::std::move(__out_it), ::std::move(__out_last)),
					__state, encoding_error::ok);
			}
		};
	} // namespace __txt_impl

	ZTD_TEXT_INLINE_ABI_NAMESPACE_CLOSE_I_
}} // namespace ztd::text

#include <ztd/epilogue.hpp>

#endif
