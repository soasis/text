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

#ifndef ZTD_TEXT_PUNYCODE_HPP
#define ZTD_TEXT_PUNYCODE_HPP

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

	//////
	/// @brief Whether or not the encode and decode steps of an punycode check need to take into account IDNA-specific
	/// quirks.
	enum class idna {
		//////
		/// @brief Do not use IDNA-specific quirks and do pure punycode encoding/decoding.
		no,
		//////
		/// @brief Use IDNA-specific quirks (e.g., looking for the prefix and passing-through strings as ASCII if they
		/// do not meet the requirements on decode.)
		yes
	};

	//////
	/// @brief The punycode encoding, as envisioned by RFC 3492 and (potentially) influenced by IDNA (RFC 5890).
	///
	/// @tparam _IsIdna Whether or not this punycode type looks for IDNA prefixes and obeys its encoding rules.
	/// @tparam _CodeUnit The code unit type for the encoded ASCII text.
	/// @tparam _CodePoint The code point type for the Unicode Code Point decoded text.
	///
	/// @remarks See https://datatracker.ietf.org/doc/html/rfc5890 and https://datatracker.ietf.org/doc/html/rfc3492.
	template <idna _IsIdna, typename _CodeUnit = char, typename _CodePoint = unicode_code_point>
	class basic_any_punycode {
	public:
		//////
		/// @brief The state for encoding Unicode code points into ASCII-compatible punycode text.
		class encode_state : public cnc_pny_encode_state_t {
		public:
			//////
			/// @brief Whether or not this state has completed its current encoding operation.
			bool is_complete() const noexcept {
				return cnc_pny_encode_state_is_complete(this);
			}
		};

		//////
		/// @brief The state for decoding ASCII-compatible punycode text into Unicode code points.
		class decode_state : public cnc_pny_decode_state_t {
		public:
			//////
			/// @brief Whether or not this state has completed its current decoding operation.
			bool is_complete() const noexcept {
				return cnc_pny_decode_state_is_complete(this);
			}
		};
		//////
		/// @brief The code unit type for encoded text.
		using code_unit = _CodeUnit;

		//////
		/// @brief The code point type for decoded text.
		using code_point = _CodePoint;

		//////
		/// @brief The maximum number of code points output by a decode operation. In this case, only 1 code point is
		/// output at a time.
		inline static constexpr ::std::size_t max_code_points = 1;
		//////
		/// @brief The maximum number of code units output by a encode operation. In this case, only 1 code unit is
		/// output at a time.
		inline static constexpr ::std::size_t max_code_units = 1;
		//////
		/// @brief Punycode is an injective encoding that is lossless, capable of encoding all input Unicode code
		/// points.
		using is_injective = std::true_type;

		//////
		/// @brief Decodes a provided range of punycode-encoded data and pulls out data.
		///
		/// @tparam _Input The input range type.
		/// @tparam _Output The output range type.
		/// @tparam _ErrorHandler The error handler type.
		///
		/// @param __input The input range.
		/// @param __output The output range.
		/// @param __error_handler The error handler; this will be called whenever an error occurs during decoding.
		/// @param __state A reference to the decode state, which contains most of the running information about a
		/// punycode decoding operation. Discarding this between calls is not advised for this encoding type.
		///
		/// @return A ztd::text::decode_result structure with the appropriate input and output types recosntructed,
		/// possibly filtered through an error handler if necessary.
		///
		/// @remarks This function may need to be called with empty input data repeatedly to fully drain any stored
		/// information in the provided `__state`. The entire input may be consumed before any information is output.
		template <typename _Input, typename _Output, typename _ErrorHandler>
		static auto decode_one(
			_Input&& __input, _Output&& __output, _ErrorHandler&& __error_handler, decode_state& __state) {
			using _UInput        = remove_cvref_t<_Input>;
			using _UOutput       = remove_cvref_t<_Output>;
			using _UErrorHandler = remove_cvref_t<_ErrorHandler>;
			using _Result        = __txt_detail::__reconstruct_decode_result_t<_Input, _Output, decode_state>;
			constexpr bool __call_error_handler = !is_ignorable_error_handler_v<_UErrorHandler>;

			if constexpr (::ztd::ranges::is_range_contiguous_range_v<_Input>                              // cf
				&& ::ztd::ranges::is_sized_range_v<_Input>                                               // cf
				&& ::ztd::is_same_sizeof_alignof_v<::ztd::ranges::range_value_type_t<_Input>, char>      // cf
				&& ::ztd::ranges::is_range_contiguous_range_v<_Output>                                   // cf
				&& ::ztd::ranges::is_sized_range_v<_Output>                                              // cf
				&& ::ztd::is_same_sizeof_alignof_v<::ztd::ranges::range_value_type_t<_Output>, char32_t> // cf
			) {
				auto __in_it               = ::ztd::ranges::ranges_adl::adl_cbegin(__input);
				auto __out_it              = ::ztd::ranges::ranges_adl::adl_begin(__output);
				auto __in_ptr              = ::ztd::to_address(__in_it);
				auto __out_ptr             = ::ztd::to_address(__out_it);
				::std::size_t __in_size    = ztd::ranges::ranges_adl::adl_size(__input);
				::std::size_t __out_size   = ztd::ranges::ranges_adl::adl_size(__output);
				const char* __typed_in_ptr = reinterpret_cast<const char*>(__in_ptr);
				char32_t* __typed_out_ptr  = reinterpret_cast<char32_t*>(__out_ptr);
				constexpr auto __decode_func
					= _IsIdna == idna::no ? &cnc_mcnrtoc32n_punycode : &cnc_mcnrtoc32n_punycode_idna;
				cnc_mcerror __err
					= __decode_func(&__out_size, &__typed_out_ptr, &__in_size, &__typed_in_ptr, &__state);
				if constexpr (__call_error_handler) {
					if (__err != CNC_MCERROR_OK) {
						basic_any_punycode __self {};
						return __error_handler(__self,
							_Result(ranges::reconstruct(::std::in_place_type<_UInput>, __typed_in_ptr,
							             __typed_in_ptr + __in_size),
							     ranges::reconstruct(::std::in_place_type<_UOutput>, __typed_out_ptr,
							          __typed_out_ptr + __out_size),
							     __state, encoding_error::insufficient_output_space),
							::ztd::span<code_unit, 0>(), ::ztd::span<code_point, 0>());
					}
				}
				return _Result(
					ranges::reconstruct(::std::in_place_type<_UInput>, __typed_in_ptr, __typed_in_ptr + __in_size),
					ranges::reconstruct(
					     ::std::in_place_type<_UOutput>, __typed_out_ptr, __typed_out_ptr + __out_size),
					__state, __call_error_handler ? static_cast<encoding_error>(__err) : encoding_error::ok);
			}
			else {
				auto __in_it                              = ::ztd::ranges::ranges_adl::adl_cbegin(__input);
				auto __in_last                            = ::ztd::ranges::ranges_adl::adl_cend(__input);
				auto __out_it                             = ::ztd::ranges::ranges_adl::adl_begin(__output);
				auto __out_last                           = ::ztd::ranges::ranges_adl::adl_end(__output);
				char __intermediate_input[max_code_units] = {};
				::std::size_t __in_size                   = 0;
				if (__in_it != __in_last) {
					__intermediate_input[0] = *__in_it;
					++__in_it;
					__in_size = ztd_c_array_size(__intermediate_input);
				}
				char32_t __intermediate_output[max_code_points] = {};
				::std::size_t __out_size                        = ztd_c_array_size(__intermediate_output);
				const char* __typed_in_ptr                      = static_cast<const char*>(__intermediate_input);
				char32_t* __typed_out_ptr                       = static_cast<char32_t*>(__intermediate_output);
				constexpr auto __decode_func
					= _IsIdna == idna::no ? &cnc_mcnrtoc32n_punycode : &cnc_mcnrtoc32n_punycode_idna;
				cnc_mcerror __err
					= __decode_func(&__out_size, &__typed_out_ptr, &__in_size, &__typed_in_ptr, &__state);
				if constexpr (__call_error_handler) {
					if (__err != CNC_MCERROR_OK) {
						basic_any_punycode __self {};
						return ::std::forward<_ErrorHandler>(__error_handler)(__self,
							_Result(ranges::reconstruct(::std::in_place_type<_UInput>, ::std::move(__in_it),
							             ::std::move(__in_last)),
							     ranges::reconstruct(::std::in_place_type<_UOutput>, ::std::move(__out_it),
							          ::std::move(__out_last), __state, static_cast<encoding_error>(__err)),
							     ::ztd::span<code_unit, 1>(__intermediate_input), ::ztd::span<code_point, 0>()));
					}
				}
				const ::std::size_t __written_output_size = ztd_c_array_size(__intermediate_output) - __out_size;
				for (::std::size_t __out_index = 0; __out_index < __written_output_size;
					++__out_it, ++__out_index) {
					if constexpr (__call_error_handler) {
						if (__out_it == __out_last) {
							basic_any_punycode __self {};
							return __error_handler(__self,
								_Result(ranges::reconstruct(::std::in_place_type<_UInput>, ::std::move(__in_it),
								             ::std::move(__in_last)),
								     ranges::reconstruct(::std::in_place_type<_UOutput>, ::std::move(__out_it),
								          ::std::move(__out_last)),
								     __state, encoding_error::insufficient_output_space),
								::ztd::span<code_unit, 1>(__intermediate_input), ::ztd::span<code_point, 0>());
						}
					}
					*__out_it = __intermediate_output[__out_index];
				}
				return _Result(ranges::reconstruct(
					               ::std::in_place_type<_UInput>, ::std::move(__in_it), ::std::move(__in_last)),
					ranges::reconstruct(
					     ::std::in_place_type<_UOutput>, ::std::move(__out_it), ::std::move(__out_last)),
					__state, static_cast<encoding_error>(__err));
			}
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
		/// @param __error_handler The error handler; this will be called whenever an error occurs during decoding.
		/// @param __state A reference to the encode state, which contains most of the running information about a
		/// punycode decoding operation. Discarding this between calls is not advised for this encoding type.
		///
		/// @return A ztd::text::encode_result structure with the appropriate input and output types recosntructed,
		/// possibly filtered through an error handler if necessary.
		///
		/// @remarks This function may need to be called with empty input data repeatedly to fully drain any stored
		/// information in the provided `__state`. The entire input may be consumed before any information is output.
		template <typename _Input, typename _Output, typename _ErrorHandler>
		static auto encode_one(
			_Input&& __input, _Output&& __output, _ErrorHandler&& __error_handler, encode_state& __state) {
			using _UInput        = remove_cvref_t<_Input>;
			using _UOutput       = remove_cvref_t<_Output>;
			using _UErrorHandler = remove_cvref_t<_ErrorHandler>;
			using _Result        = __txt_detail::__reconstruct_encode_result_t<_Input, _Output, encode_state>;
			constexpr bool __call_error_handler = !is_ignorable_error_handler_v<_UErrorHandler>;

			if constexpr (::ztd::ranges::is_range_contiguous_range_v<_Input>                             // cf
				&& ::ztd::ranges::is_sized_range_v<_Input>                                              // cf
				&& ::ztd::is_same_sizeof_alignof_v<::ztd::ranges::range_value_type_t<_Input>, char32_t> // cf
				&& ::ztd::ranges::is_range_contiguous_range_v<_Output>                                  // cf
				&& ::ztd::ranges::is_sized_range_v<_Output>                                             // cf
				&& ::ztd::is_same_sizeof_alignof_v<::ztd::ranges::range_value_type_t<_Output>, char>    // cf
			) {
				auto __in_it                   = ::ztd::ranges::ranges_adl::adl_cbegin(__input);
				auto __out_it                  = ::ztd::ranges::ranges_adl::adl_begin(__output);
				auto __in_ptr                  = ::ztd::to_address(__in_it);
				auto __out_ptr                 = ::ztd::to_address(__out_it);
				::std::size_t __in_size        = ztd::ranges::ranges_adl::adl_size(__input);
				::std::size_t __out_size       = ztd::ranges::ranges_adl::adl_size(__output);
				const char32_t* __typed_in_ptr = reinterpret_cast<const char32_t*>(__in_ptr);
				char* __typed_out_ptr          = reinterpret_cast<char*>(__out_ptr);
				constexpr auto __encode_func
					= _IsIdna == idna::no ? &cnc_c32nrtomcn_punycode : &cnc_c32nrtomcn_punycode_idna;
				cnc_mcerror __err
					= __encode_func(&__out_size, &__typed_out_ptr, &__in_size, &__typed_in_ptr, &__state);
				if constexpr (__call_error_handler) {
					if (__err != CNC_MCERROR_OK) {
						basic_any_punycode __self {};
						return ::std::forward<_ErrorHandler>(__error_handler)(__self,
							_Result(ranges::reconstruct(::std::in_place_type<_UInput>, __typed_in_ptr,
							             __typed_in_ptr + __in_size),
							     ranges::reconstruct(::std::in_place_type<_UOutput>, __typed_out_ptr,
							          __typed_out_ptr + __out_size),
							     __state, static_cast<encoding_error>(__err)),
							::ztd::span<code_point, 0>(), ::ztd::span<code_unit, 0>());
					}
				}
				return _Result(
					ranges::reconstruct(::std::in_place_type<_UInput>, __typed_in_ptr, __typed_in_ptr + __in_size),
					ranges::reconstruct(
					     ::std::in_place_type<_UOutput>, __typed_out_ptr, __typed_out_ptr + __out_size),
					__state, static_cast<encoding_error>(__err));
			}
			else {
				auto __in_it                                  = ::ztd::ranges::ranges_adl::adl_cbegin(__input);
				auto __in_last                                = ::ztd::ranges::ranges_adl::adl_cend(__input);
				auto __out_it                                 = ::ztd::ranges::ranges_adl::adl_begin(__output);
				auto __out_last                               = ::ztd::ranges::ranges_adl::adl_end(__output);
				char32_t __intermediate_input[max_code_units] = {};
				::std::size_t __in_size                       = 0;
				if (__in_it != __in_last) {
					__intermediate_input[0] = *__in_it;
					++__in_it;
					__in_size = ztd_c_array_size(__intermediate_input);
				}
				char __intermediate_output[max_code_points] = {};
				::std::size_t __out_size                    = ztd_c_array_size(__intermediate_output);
				const char32_t* __typed_in_ptr              = static_cast<const char32_t*>(__intermediate_input);
				char* __typed_out_ptr                       = static_cast<char*>(__intermediate_output);
				constexpr auto __encode_func
					= _IsIdna == idna::no ? &cnc_c32nrtomcn_punycode : &cnc_c32nrtomcn_punycode_idna;
				cnc_mcerror __err
					= __encode_func(&__out_size, &__typed_out_ptr, &__in_size, &__typed_in_ptr, &__state);
				if constexpr (__call_error_handler) {
					if (__err != CNC_MCERROR_OK) {
						basic_any_punycode __self {};
						return ::std::forward<_ErrorHandler>(__error_handler)(__self,
							_Result(ranges::reconstruct(::std::in_place_type<_UInput>, ::std::move(__in_it),
							             ::std::move(__in_last)),
							     ranges::reconstruct(::std::in_place_type<_UOutput>, ::std::move(__out_it),
							          ::std::move(__out_last)),
							     __state, static_cast<encoding_error>(__err)),
							::ztd::span<code_point, 1>(__intermediate_input), ::ztd::span<code_unit, 0>());
					}
				}
				const ::std::size_t __written_output_size = ztd_c_array_size(__intermediate_output) - __out_size;
				for (::std::size_t __out_index = 0; __out_index < __written_output_size;
					++__out_it, ++__out_index) {
					if constexpr (__call_error_handler) {
						if (__out_it == __out_last) {
							basic_any_punycode __self {};
							return __error_handler(__self,
								_Result(ranges::reconstruct(::std::in_place_type<_UInput>, ::std::move(__in_it),
								             ::std::move(__in_last)),
								     ranges::reconstruct(::std::in_place_type<_UOutput>, ::std::move(__out_it),
								          ::std::move(__out_last)),
								     __state, encoding_error::insufficient_output_space),
								::ztd::span<code_point, 1>(__intermediate_input), ::ztd::span<code_unit, 0>());
						}
					}
					*__out_it = __intermediate_output[__out_index];
				}
				return _Result(ranges::reconstruct(
					               ::std::in_place_type<_UInput>, ::std::move(__in_it), ::std::move(__in_last)),
					ranges::reconstruct(
					     ::std::in_place_type<_UOutput>, ::std::move(__out_it), ::std::move(__out_last)),
					__state, static_cast<encoding_error>(__err));
			}
		}
	};

	//////
	/// @brief A convenience typedef for ztd::text::basic_any_punycode with the ztd::text::idna::no provided.
	///
	/// @tparam _CodeUnit The code unit type for encoded text.
	/// @tparam _CodePoint The code point type for decoded text.
	template <typename _CodeUnit = char, typename _CodePoint = unicode_code_point>
	using basic_punycode = basic_any_punycode<idna::no, _CodeUnit, _CodePoint>;

	//////
	/// @brief A convenience typedef for ztd::text::basic_any_punycode with the ztd::text::idna::yes provided.
	///
	/// @tparam _CodeUnit The code unit type for encoded text.
	/// @tparam _CodePoint The code point type for decoded text.
	template <typename _CodeUnit, typename _CodePoint>
	using basic_punycode_idna = basic_any_punycode<idna::yes, _CodeUnit, _CodePoint>;

	//////
	/// @brief A ztd::text::basic_any_punycode type using `char` and ztd::text::unicode_code_point
	using punycode_t = basic_punycode<char, unicode_code_point>;

	//////
	/// @brief A ztd::text::basic_any_punycode type using `char` and ztd::text::unicode_code_point
	using punycode_idna_t = basic_punycode_idna<char, unicode_code_point>;

	//////
	/// @brief A preexisting object of type ztd::text::punycode_t for ease-of-use.
	inline constexpr punycode_t punycode = punycode_t {};

	//////
	/// @brief A preexisting object of type ztd::text::punycode_idna_t for ease-of-use.
	inline constexpr punycode_idna_t punycode_idna = punycode_idna_t {};

	ZTD_TEXT_INLINE_ABI_NAMESPACE_CLOSE_I_
}} // namespace ztd::text

#include <ztd/epilogue.hpp>

#endif // ZTD_TEXT_PUNYCODE_HPP
