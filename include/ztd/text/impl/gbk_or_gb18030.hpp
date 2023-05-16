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

#ifndef ZTD_TEXT_IMPL_GBK_OR_GB18030_HPP
#define ZTD_TEXT_IMPL_GBK_OR_GB18030_HPP

#include <ztd/text/version.hpp>

#include <ztd/text/decode_result.hpp>
#include <ztd/text/encode_result.hpp>
#include <ztd/text/unicode_code_point.hpp>
#include <ztd/text/is_ignorable_error_handler.hpp>
#include <ztd/text/detail/empty_state.hpp>
#include <ztd/text/detail/replacement_units.hpp>

#include <ztd/encoding_tables/gb18030.tables.hpp>
#include <ztd/idk/size.hpp>
#include <ztd/ranges/adl.hpp>

#include <climits>

#include <ztd/prologue.hpp>

namespace ztd { namespace text {
	ZTD_TEXT_INLINE_ABI_NAMESPACE_OPEN_I_

	namespace __txt_impl {
		template <typename _Derived, typename _CodeUnit = char, typename _CodePoint = unicode_code_point,
			bool _IsGbk = true>
		struct __basic_gb18030 {
			//////
			/// @brief Shift-JIS is generally stored as minimum-8-bit values in a sequence.
			using code_unit = _CodeUnit;

			//////
			/// @brief Shift-JIS outputs Unicode Scalar Values.
			using code_point = _CodePoint;

			//////
			/// @brief Shift-JIS requires no state.
			using state = __txt_detail::__empty_state;

			//////
			/// @brief The Shift-JIS encoding can put out at most 1 code_point units per decoding action.
			static constexpr inline std::size_t max_code_points = 1;

			//////
			/// @brief The Shift-JIS encoding can put out at most 4 code_point units per encoding action.
			static constexpr inline std::size_t max_code_units = _IsGbk ? 2 : 4;

			//////
			/// @brief A fixed-size replacement for either the GBK or GB18030 encoding.
			constexpr ztd::span<const code_unit, _IsGbk ? 1 : 4> replacement_code_units() const noexcept {
				if constexpr (_IsGbk) {
					return ztd::span<const code_unit, 1>(
						__txt_detail::__question_mark_replacement_units<code_unit>.data(),
						__txt_detail::__question_mark_replacement_units<code_unit>.size());
				}
				else {
					return ztd::span<const code_unit, 4>(
						__txt_detail::__0xfffd_gb18030_replacement_units<code_unit>.data(),
						__txt_detail::__0xfffd_gb18030_replacement_units<code_unit>.size());
				}
			}

			//////
			/// @brief A fixed-size replacement for either the GBK or GB18030 encoding.
			constexpr ztd::span<const code_point, 1> replacement_code_points() const noexcept {
				if constexpr (_IsGbk) {
					return ztd::span<const code_point, 1>(
						__txt_detail::__question_mark_replacement_units<code_unit>.data(),
						__txt_detail::__question_mark_replacement_units<code_unit>.size());
				}
				else {
					return ztd::span<const code_point, 4>(
						__txt_detail::__0xfffd_gb18030_replacement_units<code_unit>.data(),
						__txt_detail::__0xfffd_gb18030_replacement_units<code_unit>.size());
				}
			}

			//////
			/// @brief Marks this encoding as injective for the decode portion of its encoding actions.
			using is_decode_injective = std::true_type;

			//////
			/// @brief Decodes a single complete unit of information as __code_point points and produces a result
			/// with the input and output ranges moved past what was successfully read and written; or, produces an
			/// error and returns the input and output ranges untouched.
			///
			/// @param[in] __input The input view to read __code_point uunits from.
			/// @param[in] __output The output view to write __code_point points into.
			/// @param[in] __error_handler The error handler to invoke if encoding fails.
			/// @param[in, out] __state The necessary state information. For this encoding, the state is empty and
			/// means very little.
			///
			/// @returns A ztd::text::decode_result object that contains the input range, output range, error
			/// handler, and a reference to the passed-in state\.
			///
			/// @remarks To the best ability of the implementation, the iterators will be returned untouched (e.g.,
			/// the input models at least a view and a forward_range). If it is not possible, returned ranges may be
			/// incremented even if an error occurs due to the semantics of any view that models an input_range.
			template <typename _Input, typename _Output, typename _ErrorHandler>
			static constexpr auto decode_one(
				_Input&& __input, _Output&& __output, _ErrorHandler&& __error_handler, state& __state) noexcept {
				using _UErrorHandler = remove_cvref_t<_ErrorHandler>;
				using _SubInput      = ztd::ranges::subrange_for_t<::std::remove_reference_t<_Input>>;
				using _SubOutput     = ztd::ranges::subrange_for_t<::std::remove_reference_t<_Output>>;
				using _Result        = decode_result<_SubInput, _SubOutput, state>;
				;
				constexpr bool __call_error_handler = !is_ignorable_error_handler_v<_UErrorHandler>;

				auto __in_it   = ::ztd::ranges::cbegin(__input);
				auto __in_last = ::ztd::ranges::cend(__input);

				if (__in_it == __in_last) {
					// we don't need more, so we can just say the emptiness is a-okay.
					return _Result(
						::std::move(__input), ::std::move(__output), __state, ztd::text::encoding_error::ok);
				}

				::std::size_t __read_units  = 0;
				code_unit __units[4]        = { static_cast<code_unit>(*__in_it) };
				const unsigned char __unit0 = static_cast<unsigned char>(__units[0]);
				auto __out_it               = ztd::ranges::begin(__output);
				auto __out_last             = ztd::ranges::end(__output);

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
								::ztd::span<const code_unit, 0>(), ::ztd::span<const code_point, 0>());
						}
					}
					*__out_it = __code_point;
					++__in_it;
					++__out_it;
					return _Result(_SubInput(::std::move(__in_it), ::std::move(__in_last)),
						_SubOutput(::std::move(__out_it), ::std::move(__out_last)), __state,
						ztd::text::encoding_error::ok);
				}
				else if (__unit0 == 0x80) {
					// Top-Level case 1: 0x80 case
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
					const code_point __code_point = static_cast<code_point>(0x20AC);
					*__out_it                     = __code_point;
					++__in_it;
					++__out_it;
					return _Result(_SubInput(::std::move(__in_it), ::std::move(__in_last)),
						_SubOutput(::std::move(__out_it), ::std::move(__out_last)), __state,
						ztd::text::encoding_error::ok);
				}
				else if ((__unit0 >= 0x81 && __unit0 <= 0xFE)) {
					// Top-Level case 2: this is a double-byte sequence!
					if constexpr (__call_error_handler) {
						if (__in_it == __in_last) {
							_Derived __self {};
							return ::std::forward<_ErrorHandler>(__error_handler)(__self,
								_Result(::std::move(__input), ::std::move(__output), __state,
								     ztd::text::encoding_error::incomplete_sequence),
								::ztd::span<const code_unit, 1>(::std::addressof(__units[0]), 1),
								::ztd::span<const code_point, 0>());
						}
					}
					++__in_it;
					++__read_units;
					__units[1]                        = static_cast<code_unit>(*__in_it);
					const unsigned char __second_byte = static_cast<unsigned char>(__units[1]);

					auto __lookup_and_write_out = [&](::std::size_t __lookup_index, auto __use_ranges_type_value) {
						constexpr bool __use_ranges    = decltype(__use_ranges_type_value)::value;
						constexpr auto __to_code_point = __use_ranges
							? &::ztd::et::gb18030_ranges_index_to_code_point
							: &::ztd::et::gb18030_index_to_code_point;
						const ::std::optional<::std::uint_least32_t> __maybe_code
							= __to_code_point(__lookup_index);
						if (__maybe_code) {
							if constexpr (__call_error_handler) {
								if (__out_it == __out_last) {
									_Derived __self {};
									return ::std::forward<_ErrorHandler>(__error_handler)(__self,
										_Result(_SubInput(::std::move(__in_it), ::std::move(__in_last)),
										     _SubOutput(::std::move(__out_it), ::std::move(__out_last)),
										     __state, ztd::text::encoding_error::insufficient_output_space),
										::ztd::span<const code_unit, 3>(
										     ::std::addressof(__units[0]), __read_units),
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
						// Otherwise, we failed.
						_Derived __self {};
						return ::std::forward<_ErrorHandler>(__error_handler)(__self,
							_Result(_SubInput(::std::move(__in_it), ::std::move(__in_last)),
							     _SubOutput(::std::move(__out_it), ::std::move(__out_last)), __state,
							     ztd::text::encoding_error::invalid_sequence),
							::ztd::span<const code_unit>(::std::addressof(__units[0]), __read_units),
							::ztd::span<const code_point, 0>());
					};

					if (__second_byte >= 0x30 && __second_byte <= 0x39) {
						// Nested: this is a potential 3/4 byte sequence sequence!
						if constexpr (_IsGbk) {
							if constexpr (__call_error_handler) {
								// we cannot enter this criteria with GBK; it is not allowed to encode more than
								// 2 bytes!
								_Derived __self {};
								return ::std::forward<_ErrorHandler>(__error_handler)(__self,
									_Result(::std::move(__input), ::std::move(__output), __state,
									     ztd::text::encoding_error::invalid_sequence),
									::ztd::span<const code_unit, 2>(
									     ::std::addressof(__units[0]), __read_units),
									::ztd::span<const code_point, 0>());
							}
						}
						if constexpr (__call_error_handler) {
							if (__in_it == __in_last) {
								_Derived __self {};
								return ::std::forward<_ErrorHandler>(__error_handler)(__self,
									_Result(::std::move(__input), ::std::move(__output), __state,
									     ztd::text::encoding_error::incomplete_sequence),
									::ztd::span<const code_unit, 2>(
									     ::std::addressof(__units[0]), __read_units),
									::ztd::span<const code_point, 0>());
							}
						}
						++__in_it;
						++__read_units;
						__units[2]                 = static_cast<code_unit>(*__in_it);
						unsigned char __third_byte = static_cast<unsigned char>(__units[2]);
						if (__third_byte >= 0x81 && __third_byte <= 0xFE) {
							if constexpr (__call_error_handler) {
								if (__in_it == __in_last) {
									_Derived __self {};
									return ::std::forward<_ErrorHandler>(__error_handler)(__self,
										_Result(::std::move(__input), ::std::move(__output), __state,
										     ztd::text::encoding_error::incomplete_sequence),
										::ztd::span<const code_unit, 2>(
										     ::std::addressof(__units[0]), __read_units),
										::ztd::span<const code_point, 0>());
								}
							}
							++__in_it;
							++__read_units;
							__units[3]                        = static_cast<code_unit>(*__in_it);
							const unsigned char __fourth_byte = static_cast<unsigned char>(__units[3]);
							if constexpr (__call_error_handler) {
								if (__fourth_byte < 0x30 || __fourth_byte > 0x39) {
									_Derived __self {};
									return ::std::forward<_ErrorHandler>(__error_handler)(__self,
										_Result(::std::move(__input), ::std::move(__output), __state,
										     ztd::text::encoding_error::invalid_sequence),
										::ztd::span<const code_unit, 3>(
										     ::std::addressof(__units[0]), __read_units),
										::ztd::span<const code_point, 0>());
								}
							}
							const ::std::size_t __lookup_index = ((__unit0 - 0x81) * (10 * 126 * 10))
								+ ((__second_byte - 0x30) * (10 * 126)) + ((__third_byte - 0x81) * 10)
								+ (__fourth_byte - 0x30);
							return __lookup_and_write_out(__lookup_index, std::true_type {});
						}
					}
					else {
						if ((__second_byte >= 0x40 && __second_byte <= 0x7E)
							|| (__second_byte >= 0x80 && __second_byte <= 0xFE)) {
							const uint_least32_t __second_byte_offset = __second_byte < 0x7F ? 0x40 : 0x41;
							::std::size_t __lookup_index
								= ((__unit0 - 0x81) * 190) + (__second_byte - __second_byte_offset);
							return __lookup_and_write_out(__lookup_index, std::false_type {});
						}
					}
				}

				// Final case (default): unrecognized byte sequence!!
				_Derived __self {};
				return ::std::forward<_ErrorHandler>(__error_handler)(__self,
					_Result(_SubInput(::std::move(__in_it), ::std::move(__in_last)),
					     _SubOutput(::std::move(__out_it), ::std::move(__out_last)), __state,
					     ztd::text::encoding_error::invalid_sequence),
					::ztd::span<const code_unit>(::std::addressof(__units[0]), __read_units),
					::ztd::span<const code_point, 0>());
			}

			//////
			/// @brief Encodes a single complete unit of information as __code_point units and produces a result with
			/// the input and output ranges moved past what was successfully read and written; or, produces an error
			/// and returns the input and output ranges untouched.
			///
			/// @param[in] __input The input view to read __code_point points from.
			/// @param[in] __output The output view to write __code_point units into.
			/// @param[in] __error_handler The error handler to invoke if encoding fails.
			/// @param[in, out] __state The necessary state information. For this encoding, the state is empty and
			/// means very little.
			///
			/// @returns A ztd::text::encode_result object that contains the input range, output range, error
			/// handler, and a reference to the passed-in state\.
			///
			/// @remarks To the best ability of the implementation, the iterators will be returned untouched (e.g.,
			/// the input models at least a view and a forward_range). If it is not possible, returned ranges may be
			/// incremented even if an error occurs due to the semantics of any view that models an input_range.
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
				code_point __code_point     = static_cast<code_point>(__code_point32);
				auto __out_it               = ztd::ranges::begin(__output);
				auto __out_last             = ztd::ranges::end(__output);
				if (__code_point32 <= 0x7F) {
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
					*__out_it = static_cast<code_unit>(__code_point);
					++__out_it;
					return _Result(_SubInput(::std::move(__in_it), ::std::move(__in_last)),
						_SubOutput(::std::move(__out_it), ::std::move(__out_last)), __state,
						ztd::text::encoding_error::ok);
				}


				if constexpr (__call_error_handler) {
					if (__code_point == 0xE5E5) {
						_Derived __self {};
						return ::std::forward<_ErrorHandler>(__error_handler)(__self,
							_Result(::std::move(__input), ::std::move(__output), __state,
							     ztd::text::encoding_error::invalid_sequence),
							::ztd::span<const code_point, 0>(), ::ztd::span<const code_unit, 0>());
					}
				}
				if constexpr (_IsGbk) {
					if (__code_point == 0x80) {
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
						*__out_it = static_cast<code_unit>(__code_point);
						++__out_it;
						return _Result(_SubInput(::std::move(__in_it), ::std::move(__in_last)),
							_SubOutput(::std::move(__out_it), ::std::move(__out_last)), __state,
							ztd::text::encoding_error::ok);
					}
				}

				::std::optional<::std::size_t> __maybe_lookup_gbk_index
					= ::ztd::et::gb18030_code_point_to_index(__code_point32);
				if (__maybe_lookup_gbk_index) {
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
					const ::std::size_t __lookup_gbk_index    = *__maybe_lookup_gbk_index;
					const unsigned char __first_byte          = (__lookup_gbk_index / 190) + 0x81;
					const unsigned char __second_byte_initial = (__lookup_gbk_index % 190);
					const unsigned char __second_byte_offset  = __second_byte_initial < 0x3F ? 0x40 : 0x41;
					const unsigned char __second_byte         = __second_byte_initial + __second_byte_offset;
					const code_unit __second_unit             = static_cast<code_unit>(__second_byte);
					*__out_it                                 = static_cast<code_unit>(__first_byte);
					++__out_it;
					if constexpr (__call_error_handler) {
						if (__out_it == __out_last) {
							// output is empty :(
							_Derived __self {};
							return ::std::forward<_ErrorHandler>(__error_handler)(__self,
								_Result(::std::move(__input), ::std::move(__output), __state,
								     ztd::text::encoding_error::insufficient_output_space),
								::ztd::span<const code_point, 0>(),
								::ztd::span<const code_unit, 1>(::std::addressof(__second_unit), 1));
						}
					}
					++__in_it;
					*__out_it = __second_unit;
					++__out_it;
					return _Result(_SubInput(::std::move(__in_it), ::std::move(__in_last)),
						_SubOutput(::std::move(__out_it), ::std::move(__out_last)), __state,
						ztd::text::encoding_error::ok);
				}

				if constexpr (!_IsGbk) {
					::std::optional<::std::size_t> __maybe_lookup_index
						= ztd::et::gb18030_ranges_code_point_to_index(__code_point32);
					if (__maybe_lookup_index) {
						const ::std::size_t __lookup_index = *__maybe_lookup_index;
						code_unit __units[4]               = {};
						__units[0]                         = static_cast<code_unit>(
                                   static_cast<unsigned char>((__lookup_index / (10 * 126 * 10)) + 0x81));
						const ::std::size_t __lookup_index_unit1 = __lookup_index % (10 * 126 * 10);
						__units[1]                               = static_cast<code_unit>(
                                   static_cast<unsigned char>((__lookup_index_unit1 / (10 * 126)) + 0x30));
						const ::std::size_t __lookup_index_unit2 = __lookup_index_unit1 % (10 * 126);
						__units[2]                               = static_cast<code_unit>(
                                   static_cast<unsigned char>((__lookup_index_unit2 / 10) + 0x81));
						__units[3] = static_cast<code_unit>(
							static_cast<unsigned char>((__lookup_index_unit2 % 10) + 0x30));

						for (::std::size_t __index = 0; __index < ztdc_c_array_size(__units); ++__index) {
							if constexpr (__call_error_handler) {
								if (__out_it == __out_last) {
									// output is empty :(
									_Derived __self {};
									return ::std::forward<_ErrorHandler>(__error_handler)(__self,
										_Result(_SubInput(::std::move(__in_it), ::std::move(__in_last)),
										     _SubOutput(::std::move(__out_it), ::std::move(__out_last)),
										     __state, ztd::text::encoding_error::insufficient_output_space),
										::ztd::span<const code_point, 0>(),
										::ztd::span<const code_unit>(::std::addressof(__units[__index]),
										     ztdc_c_array_size(__units) - __index));
								}
							}
							const code_unit __unit = static_cast<code_unit>(__units[__index]);
							*__out_it              = __unit;
							++__out_it;
						}
						++__in_it;
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

		private:
			static_assert((sizeof(code_point) * CHAR_BIT) > (_IsGbk ? 15 : 21),
				"The code point type for a GBK encoding must be at least 16 bits wide. The code point type for a "
				"GB18030 encoding must be at least 22 bits wide");
		};
	} // namespace __txt_impl

	ZTD_TEXT_INLINE_ABI_NAMESPACE_CLOSE_I_
}} // namespace ztd::text


#include <ztd/epilogue.hpp>

#endif
