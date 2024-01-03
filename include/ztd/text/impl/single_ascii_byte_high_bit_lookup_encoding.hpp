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

#ifndef ZTD_TEXT_IMPL_SINGLE_ASCII_BYTE_HIGH_BIT_LOOKUP_ENCODING_HPP
#define ZTD_TEXT_IMPL_SINGLE_ASCII_BYTE_HIGH_BIT_LOOKUP_ENCODING_HPP

#include <ztd/text/version.hpp>

#include <ztd/text/decode_result.hpp>
#include <ztd/text/encode_result.hpp>
#include <ztd/text/unicode_code_point.hpp>
#include <ztd/text/is_ignorable_error_handler.hpp>
#include <ztd/text/detail/empty_state.hpp>
#include <ztd/text/detail/replacement_units.hpp>

#include <ztd/ranges/adl.hpp>
#include <ztd/encoding_tables/table_types.hpp>

#include <optional>
#include <cstddef>

#include <ztd/prologue.hpp>

namespace ztd { namespace text {
	ZTD_TEXT_INLINE_ABI_NAMESPACE_OPEN_I_

	namespace __txt_impl {

		template <typename _Derived, ::ztd::et::basic_lookup_index_to_code_point_function* _LookupCodePoint,
			::ztd::et::basic_lookup_code_point_to_index_function* _LookupIndex, typename _CodeUnit = char,
			typename _CodePoint = unicode_code_point>
		struct __single_ascii_byte_high_bit_lookup_encoding {
			using code_unit = _CodeUnit;

			using code_point = _CodePoint;

			using state = __txt_detail::__empty_state;

			static constexpr inline std::size_t max_code_points = 1;

			static constexpr inline std::size_t max_code_units = 1;

			constexpr ztd::span<const code_unit, 1> replacement_code_units() const noexcept {
				return ztd::span<const code_unit, 1>(
					__txt_detail::__question_mark_replacement_units<code_unit>.data(),
					__txt_detail::__question_mark_replacement_units<code_unit>.size());
			}

			using is_decode_injective = std::true_type;

			template <typename _Input, typename _Output, typename _ErrorHandler>
			static constexpr auto decode_one(
				_Input&& __input, _Output&& __output, _ErrorHandler&& __error_handler, state& __state) noexcept {
				using _UErrorHandler = remove_cvref_t<_ErrorHandler>;
				using _SubInput      = ztd::ranges::subrange_for_t<::std::remove_reference_t<_Input>>;
				using _SubOutput     = ztd::ranges::subrange_for_t<::std::remove_reference_t<_Output>>;
				using _Result        = decode_result<_SubInput, _SubOutput, state>;
				constexpr bool __call_error_handler = !is_ignorable_error_handler_v<_UErrorHandler>;

				auto __in_it   = ::ztd::ranges::cbegin(__input);
				auto __in_last = ::ztd::ranges::cend(__input);

				if (__in_it == __in_last) {
					// we don't need more, so we can just say the emptiness is a-okay.
					return _Result(
						::std::move(__input), ::std::move(__output), __state, ztd::text::encoding_error::ok);
				}

				code_unit __units[max_code_units] = { static_cast<code_unit>(*__in_it) };
				unsigned char __unit0             = static_cast<unsigned char>(__units[0]);
				auto __out_it                     = ztd::ranges::begin(__output);
				auto __out_last                   = ztd::ranges::end(__output);

				if (__unit0 <= 0x7F) {
					// Top-Level case 0: it's an ASCII byte
					const code_point __code_point = static_cast<code_point>(__unit0);
					if constexpr (__call_error_handler) {
						if (__out_it == __out_last) {
							_Derived __self {};
							return ::std::forward<_ErrorHandler>(__error_handler)(__self,
								_Result(_SubInput(::std::move(__in_it), ::std::move(__in_last)),
								     _SubOutput(::std::move(__out_it), ::std::move(__out_last)), __state,
								     ztd::text::encoding_error::insufficient_output_space),
								::ztd::span<const code_unit, 1>(::std::addressof(__units[0]), 1),
								::ztd::span<const code_point, 0>());
						}
					}
					*__out_it = __code_point;
					++__in_it;
					++__out_it;
					return _Result(_SubInput(::std::move(__in_it), ::std::move(__in_last)),
						_SubOutput(::std::move(__out_it), ::std::move(__out_last)), __state,
						ztd::text::encoding_error::ok);
				}
				else {
					::std::size_t __lookup_index = static_cast<::std::size_t>(__unit0) - 0x80u;
					const ::std::optional<::std::uint_least32_t> __maybe_code = _LookupCodePoint(__lookup_index);
					if (__maybe_code) {
						if constexpr (__call_error_handler) {
							if (__out_it == __out_last) {
								_Derived __self {};
								return ::std::forward<_ErrorHandler>(__error_handler)(__self,
									_Result(_SubInput(::std::move(__in_it), ::std::move(__in_last)),
									     _SubOutput(::std::move(__out_it), ::std::move(__out_last)), __state,
									     ztd::text::encoding_error::insufficient_output_space),
									::ztd::span<const code_unit, 1>(::std::addressof(__units[0]), 1),
									::ztd::span<const code_point, 0>());
							}
						}
						const code_point __code_point = static_cast<code_point>(*__maybe_code);
						*__out_it                     = __code_point;
						++__in_it;
						++__out_it;
						return _Result(_SubInput(::std::move(__in_it), ::std::move(__in_last)),
							_SubOutput(::std::move(__out_it), ::std::move(__out_last)), __state,
							ztd::text::encoding_error::ok);
					}
				}

				_Derived __self {};
				return ::std::forward<_ErrorHandler>(__error_handler)(__self,
					_Result(_SubInput(::std::move(__in_it), ::std::move(__in_last)),
					     _SubOutput(::std::move(__out_it), ::std::move(__out_last)), __state,
					     ztd::text::encoding_error::invalid_sequence),
					::ztd::span<const code_unit, 1>(::std::addressof(__units[0]), 1),
					::ztd::span<const code_point, 0>());
			}

			template <typename _Input, typename _Output, typename _ErrorHandler>
			static constexpr auto encode_one(
				_Input&& __input, _Output&& __output, _ErrorHandler&& __error_handler, state& __state) noexcept {
				using _UErrorHandler = remove_cvref_t<_ErrorHandler>;
				using _SubInput      = ztd::ranges::subrange_for_t<::std::remove_reference_t<_Input>>;
				using _SubOutput     = ztd::ranges::subrange_for_t<::std::remove_reference_t<_Output>>;
				using _Result        = encode_result<_SubInput, _SubOutput, state>;
				constexpr bool __call_error_handler = !is_ignorable_error_handler_v<_UErrorHandler>;

				auto __in_it   = ::ztd::ranges::cbegin(__input);
				auto __in_last = ::ztd::ranges::cend(__input);

				if (__in_it == __in_last) {
					// we don't need more, so we can just say the emptiness is a-okay.
					return _Result(
						::std::move(__input), ::std::move(__output), __state, ztd::text::encoding_error::ok);
				}

				ztd_char32_t __code_point32 = static_cast<ztd_char32_t>(*__in_it);
				auto __out_it               = ztd::ranges::begin(__output);
				auto __out_last             = ztd::ranges::end(__output);

				if (__code_point32 < 0x80) {
					if constexpr (__call_error_handler) {
						if (__out_it == __out_last) {
							// output is empty :(
							_Derived __self {};
							return ::std::forward<_ErrorHandler>(__error_handler)(__self,
								_Result(::std::move(__input), ::std::move(__output), __state,
								     ztd::text::encoding_error::insufficient_output_space),
								::ztd::span<const code_point, 0>(), ::ztd::span<const code_unit, 0>());
						}
					}
					++__in_it;
					*__out_it = static_cast<code_unit>(__code_point32);
					++__out_it;
					return _Result(_SubInput(::std::move(__in_it), ::std::move(__in_last)),
						_SubOutput(::std::move(__out_it), ::std::move(__out_last)), __state,
						ztd::text::encoding_error::ok);
				}
				else {
					const ::std::optional<::std::size_t> __maybe_index = _LookupIndex(__code_point32);
					if (__maybe_index) {
						if constexpr (__call_error_handler) {
							if (__out_it == __out_last) {
								// output is empty :(
								_Derived __self {};
								return ::std::forward<_ErrorHandler>(__error_handler)(__self,
									_Result(::std::move(__input), ::std::move(__output), __state,
									     ztd::text::encoding_error::insufficient_output_space),
									::ztd::span<const code_point, 0>(), ::ztd::span<const code_unit, 0>());
							}
						}
						const code_unit __code_unit = static_cast<code_unit>(*__maybe_index + 0x80);
						*__out_it                   = __code_unit;
						++__in_it;
						++__out_it;
						return _Result(_SubInput(::std::move(__in_it), ::std::move(__in_last)),
							_SubOutput(::std::move(__out_it), ::std::move(__out_last)), __state,
							ztd::text::encoding_error::ok);
					}
				}

				_Derived __self {};
				return ::std::forward<_ErrorHandler>(__error_handler)(__self,
					_Result(_SubInput(::std::move(__in_it), ::std::move(__in_last)),
					     _SubOutput(::std::move(__out_it), ::std::move(__out_last)), __state,
					     ztd::text::encoding_error::invalid_sequence),
					::ztd::span<const code_point, 0>(), ::ztd::span<const code_unit, 0>());
			}
		};
	} // namespace __txt_impl

	ZTD_TEXT_INLINE_ABI_NAMESPACE_CLOSE_I_
}} // namespace ztd::text


#include <ztd/epilogue.hpp>

#endif
