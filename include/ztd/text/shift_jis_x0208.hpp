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

#ifndef ZTD_TEXT_SHIFT_JIS_X0208_HPP
#define ZTD_TEXT_SHIFT_JIS_X0208_HPP

#include <ztd/text/version.hpp>

#include <ztd/text/decode_result.hpp>
#include <ztd/text/encode_result.hpp>
#include <ztd/text/unicode_code_point.hpp>
#include <ztd/text/is_ignorable_error_handler.hpp>
#include <ztd/text/detail/empty_state.hpp>
#include <ztd/text/detail/replacement_units.hpp>

#include <ztd/encoding_tables/shift_jis_x0208.tables.hpp>
#include <ztd/ranges/adl.hpp>

#include <ztd/prologue.hpp>

namespace ztd { namespace text {
	ZTD_TEXT_INLINE_ABI_NAMESPACE_OPEN_I_

	//////
	/// @brief The Shift-JIS Encoding (with extensions x0208) for use with most Shift-JIS applications. Identical
	/// version of what is a part of the WHATWG encoding standard for Shift-JIS.
	///
	/// @tparam _CodeUnit The code unit type to use.
	/// @tparam _CodePoint The code point type to use.
	template <typename _CodeUnit = char, typename _CodePoint = unicode_code_point>
	class basic_shift_jis_x0208 {
	public:
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
		/// @brief The Shift-JIS encoding can put out at most 1 __code_point point per decoding action.
		static constexpr inline std::size_t max_code_points = 1;

		//////
		/// @brief The Shift-JIS encoding can put out at most 2 __code_point units per encoding action.
		static constexpr inline std::size_t max_code_units = 2;

		//////
		/// @brief A fixed-size 1-element span with the __code_point unit "?"
		constexpr ztd::span<const code_unit, 1> replacement_code_units() const noexcept {
			return ztd::span<const code_unit, 1>(__txt_detail::__question_mark_replacement_units<code_unit>.data(),
				__txt_detail::__question_mark_replacement_units<code_unit>.size());
		}

		//////
		/// @brief Marks this encoding as injective for the decode portion of its encoding actions.
		using is_decode_injective = std::true_type;

		//////
		/// @brief Decodes a single complete unit of information as __code_point points and produces a result with the
		/// input and output ranges moved past what was successfully read and written; or, produces an error and
		/// returns the input and output ranges untouched.
		///
		/// @param[in] __input The input view to read __code_point uunits from.
		/// @param[in] __output The output view to write __code_point points into.
		/// @param[in] __error_handler The error handler to invoke if encoding fails.
		/// @param[in, out] __state The necessary state information. For this encoding, the state is empty and means
		/// very little.
		///
		/// @returns A ztd::text::decode_result object that contains the input range, output range, error handler, and
		/// a reference to the passed-in state.
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
				return _Result(::std::move(__input), ::std::move(__output), __state, ztd::text::encoding_error::ok);
			}

			code_unit __units[max_code_units] = { static_cast<code_unit>(*__in_it) };
			unsigned char __unit0             = static_cast<unsigned char>(__units[0]);
			auto __out_it                     = ztd::ranges::begin(__output);
			auto __out_last                   = ztd::ranges::end(__output);

			if (__unit0 <= 0x80) {
				// Top-Level case 0: it's an ASCII byte
				const code_point __code_point = static_cast<code_point>(__unit0);
				if constexpr (__call_error_handler) {
					if (__out_it == __out_last) {
						basic_shift_jis_x0208 __self {};
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
			else if (__unit0 <= 0xDF && __unit0 >= 0xA1) {
				// Top-Level case 1: it's non-ASCII single-byte
				const code_point __code_point = static_cast<code_point>((0xFF61 - 0xA1) + __unit0);
				if constexpr (__call_error_handler) {
					if (__out_it == __out_last) {
						basic_shift_jis_x0208 __self {};
						return ::std::forward<_ErrorHandler>(__error_handler)(__self,
							_Result(_SubInput(::std::move(__in_it), ::std::move(__in_last)),
							     _SubOutput(::std::move(__out_it), ::std::move(__out_last)), __state,
							     ztd::text::encoding_error::insufficient_output_space),
							::ztd::span<const code_unit, 1>(::std::addressof(__units[0]), 1),
							::ztd::span<const code_point, 0>());
					}
				}
				++__in_it;
				*__out_it = __code_point;
				++__out_it;
				return _Result(_SubInput(::std::move(__in_it), ::std::move(__in_last)),
					_SubOutput(::std::move(__out_it), ::std::move(__out_last)), __state,
					ztd::text::encoding_error::ok);
			}
			else if ((__unit0 <= 0x9F && __unit0 >= 0x81) || (__unit0 <= 0xFC && __unit0 >= 0xE0)) {
				// Top-Level case 2: this is a double-byte sequence!
				if constexpr (__call_error_handler) {
					if (__in_it == __in_last) {
						basic_shift_jis_x0208 __self {};
						return ::std::forward<_ErrorHandler>(__error_handler)(__self,
							_Result(::std::move(__input), ::std::move(__output), __state,
							     ztd::text::encoding_error::incomplete_sequence),
							::ztd::span<const code_unit, 1>(::std::addressof(__units[0]), 1),
							::ztd::span<const code_point, 0>());
					}
				}
				++__in_it;
				__units[1]                         = static_cast<code_unit>(*__in_it);
				unsigned char __second_byte        = static_cast<unsigned char>(__units[1]);
				unsigned char __lookup_offset      = __second_byte < 0x7F ? 0x40 : 0x41;
				unsigned char __lead_lookup_offset = __unit0 < 0xA0 ? 0x81 : 0xC1;
				::std::size_t __lookup_index       = 0;
				if ((__second_byte <= 0x7E && __second_byte >= 0x40)
					|| (__second_byte <= 0xFC && __second_byte >= 0x80)) {
					__lookup_index = (((__unit0 - __lead_lookup_offset) * 188) + __second_byte) - __lookup_offset;
					if (__lookup_index <= 10715 && __lookup_index >= 8836) {
						if constexpr (__call_error_handler) {
							if (__out_it == __out_last) {
								basic_shift_jis_x0208 __self {};
								return ::std::forward<_ErrorHandler>(__error_handler)(__self,
									_Result(_SubInput(::std::move(__in_it), ::std::move(__in_last)),
									     _SubOutput(::std::move(__out_it), ::std::move(__out_last)), __state,
									     ztd::text::encoding_error::insufficient_output_space),
									::ztd::span<const code_unit, 1>(::std::addressof(__units[0]), 1),
									::ztd::span<const code_point, 0>());
							}
						}
						ztd_char32_t __code_point = static_cast<ztd_char32_t>(0xE000 - 8836 + __lookup_index);
						*__out_it                 = static_cast<code_point>(__code_point);
						++__in_it;
						++__out_it;
						return _Result(_SubInput(::std::move(__in_it), ::std::move(__in_last)),
							_SubOutput(::std::move(__out_it), ::std::move(__out_last)), __state,
							ztd::text::encoding_error::ok);
					}
					else {
						::std::optional<::std::uint_least32_t> __maybe_code
							= ztd::et::shift_jis_x0208_index_to_code_point(__lookup_index);
						if (__maybe_code) {
							if constexpr (__call_error_handler) {
								if (__out_it == __out_last) {
									basic_shift_jis_x0208 __self {};
									return ::std::forward<_ErrorHandler>(__error_handler)(__self,
										_Result(_SubInput(::std::move(__in_it), ::std::move(__in_last)),
										     _SubOutput(::std::move(__out_it), ::std::move(__out_last)),
										     __state, ztd::text::encoding_error::insufficient_output_space),
										::ztd::span<const code_unit, 1>(::std::addressof(__units[0]), 1),
										::ztd::span<const code_point, 0>());
								}
							}
							const ztd_char32_t& __code_point = *__maybe_code;
							*__out_it                        = static_cast<code_point>(__code_point);
							++__in_it;
							++__out_it;
							return _Result(_SubInput(::std::move(__in_it), ::std::move(__in_last)),
								_SubOutput(::std::move(__out_it), ::std::move(__out_last)), __state,
								ztd::text::encoding_error::ok);
						}
					}
				}
			}

			// Top-Level case 3 (default): unrecognized byte sequence!!
			basic_shift_jis_x0208 __self {};
			return ::std::forward<_ErrorHandler>(__error_handler)(__self,
				_Result(_SubInput(::std::move(__in_it), ::std::move(__in_last)),
				     _SubOutput(::std::move(__out_it), ::std::move(__out_last)), __state,
				     ztd::text::encoding_error::invalid_sequence),
				::ztd::span<const code_unit, 0>(), ::ztd::span<const code_point, 0>());
		}

		//////
		/// @brief Encodes a single complete unit of information as __code_point units and produces a result with the
		/// input and output ranges moved past what was successfully read and written; or, produces an error and
		/// returns the input and output ranges untouched.
		///
		/// @param[in] __input The input view to read __code_point points from.
		/// @param[in] __output The output view to write __code_point units into.
		/// @param[in] __error_handler The error handler to invoke if encoding fails.
		/// @param[in, out] __state The necessary state information. For this encoding, the state is empty and means
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
			_Input&& __input, _Output&& __output, _ErrorHandler&& __error_handler, state& __state) noexcept {
			using _UErrorHandler                = remove_cvref_t<_ErrorHandler>;
			using _SubInput                     = ztd::ranges::subrange_for_t<::std::remove_reference_t<_Input>>;
			using _SubOutput                    = ztd::ranges::subrange_for_t<::std::remove_reference_t<_Output>>;
			using _Result                       = encode_result<_SubInput, _SubOutput, state>;
			constexpr bool __call_error_handler = !is_ignorable_error_handler_v<_UErrorHandler>;

			auto __in_it   = ::ztd::ranges::cbegin(__input);
			auto __in_last = ::ztd::ranges::cend(__input);

			if (__in_it == __in_last) {
				// we don't need more, so we can just say the emptiness is a-okay.
				return _Result(::std::move(__input), ::std::move(__output), __state, ztd::text::encoding_error::ok);
			}

			ztd_char32_t __code_point32 = static_cast<ztd_char32_t>(*__in_it);
			code_point __code_point     = static_cast<code_point>(__code_point32);
			auto __out_it               = ztd::ranges::begin(__output);
			auto __out_last             = ztd::ranges::end(__output);

			if (__code_point32 <= 0x80) {
				if constexpr (__call_error_handler) {
					if (__out_it == __out_last) {
						// output is empty :(
						basic_shift_jis_x0208 __self {};
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
			else if (__code_point32 == 0x00A5) {
				if constexpr (__call_error_handler) {
					if (__out_it == __out_last) {
						// output is empty :(
						basic_shift_jis_x0208 __self {};
						return ::std::forward<_ErrorHandler>(__error_handler)(__self,
							_Result(_SubInput(::std::move(__in_it), ::std::move(__in_last)),
							     _SubOutput(::std::move(__out_it), ::std::move(__out_last)), __state,
							     ztd::text::encoding_error::insufficient_output_space),
							::ztd::span<const code_point, 0>(), ::ztd::span<const code_unit, 0>());
					}
				}
				++__in_it;
				*__out_it = static_cast<code_unit>(0x5C);
				++__out_it;
				return _Result(_SubInput(::std::move(__in_it), ::std::move(__in_last)),
					_SubOutput(::std::move(__out_it), ::std::move(__out_last)), __state,
					ztd::text::encoding_error::ok);
			}
			else if (__code_point32 == 0x203E) {
				if constexpr (__call_error_handler) {
					if (__out_it == __out_last) {
						// output is empty :(
						basic_shift_jis_x0208 __self {};
						return ::std::forward<_ErrorHandler>(__error_handler)(__self,
							_Result(_SubInput(::std::move(__in_it), ::std::move(__in_last)),
							     _SubOutput(::std::move(__out_it), ::std::move(__out_last)), __state,
							     ztd::text::encoding_error::insufficient_output_space),
							::ztd::span<const code_point, 0>(), ::ztd::span<const code_unit, 0>());
					}
				}
				++__in_it;
				*__out_it = static_cast<code_unit>(0x7E);
				++__out_it;
				return _Result(_SubInput(::std::move(__in_it), ::std::move(__in_last)),
					_SubOutput(::std::move(__out_it), ::std::move(__out_last)), __state,
					ztd::text::encoding_error::ok);
			}
			else if (__code_point32 >= 0xFF61 && __code_point32 <= 0xFF9F) {
				if constexpr (__call_error_handler) {
					if (__out_it == __out_last) {
						// output is empty :(
						basic_shift_jis_x0208 __self {};
						return ::std::forward<_ErrorHandler>(__error_handler)(__self,
							_Result(_SubInput(::std::move(__in_it), ::std::move(__in_last)),
							     _SubOutput(::std::move(__out_it), ::std::move(__out_last)), __state,
							     ztd::text::encoding_error::insufficient_output_space),
							::ztd::span<const code_point, 0>(), ::ztd::span<const code_unit, 0>());
					}
				}
				++__in_it;
				ztd_char32_t __intermediate = (__code_point32 - 0xFF61);
				*__out_it                   = static_cast<code_unit>(__intermediate + 0xA1);
				++__out_it;
				return _Result(_SubInput(::std::move(__in_it), ::std::move(__in_last)),
					_SubOutput(::std::move(__out_it), ::std::move(__out_last)), __state,
					ztd::text::encoding_error::ok);
			}

			if (__code_point == 0x2212) {
				__code_point = 0xFF0D;
			}

			::std::optional<::std::size_t> __maybe_index
				= ::ztd::et::shift_jis_x0208_code_point_to_index(__code_point);
			if (__maybe_index) {
				::std::size_t __index         = *__maybe_index;
				::std::size_t __first         = __index / 188;
				::std::size_t __first_offset  = __first < 0x1F ? 0x81 : 0xC1;
				::std::size_t __second        = __index % 188;
				::std::size_t __second_offset = __second < 0x3F ? 0x40 : 0x41;

				if constexpr (__call_error_handler) {
					if (__out_it == __out_last) {
						// output is empty :(
						basic_shift_jis_x0208 __self {};
						return ::std::forward<_ErrorHandler>(__error_handler)(__self,
							_Result(_SubInput(::std::move(__in_it), ::std::move(__in_last)),
							     _SubOutput(::std::move(__out_it), ::std::move(__out_last)), __state,
							     ztd::text::encoding_error::insufficient_output_space),
							::ztd::span<const code_point, 0>(), ::ztd::span<const code_unit, 0>());
					}
				}
				unsigned char __unituc = static_cast<unsigned char>(__first + __first_offset);
				const code_unit __unit = static_cast<code_unit>(__unituc);
				*__out_it              = __unit;
				++__out_it;
				if constexpr (__call_error_handler) {
					if (__out_it == __out_last) {
						// output is empty :(
						basic_shift_jis_x0208 __self {};
						return ::std::forward<_ErrorHandler>(__error_handler)(__self,
							_Result(_SubInput(::std::move(__in_it), ::std::move(__in_last)),
							     _SubOutput(::std::move(__out_it), ::std::move(__out_last)), __state,
							     ztd::text::encoding_error::insufficient_output_space),
							::ztd::span<const code_point, 0>(),
							::ztd::span<const code_unit, 1>(::std::addressof(__unit), 1));
					}
				}
				++__in_it;
				*__out_it = static_cast<code_unit>(__second + __second_offset);
				++__out_it;
				return _Result(_SubInput(::std::move(__in_it), ::std::move(__in_last)),
					_SubOutput(::std::move(__out_it), ::std::move(__out_last)), __state,
					ztd::text::encoding_error::ok);
			}

			basic_shift_jis_x0208 __self {};
			return ::std::forward<_ErrorHandler>(__error_handler)(__self,
				_Result(_SubInput(::std::move(__in_it), ::std::move(__in_last)),
				     _SubOutput(::std::move(__out_it), ::std::move(__out_last)), __state,
				     ztd::text::encoding_error::invalid_sequence),
				::ztd::span<const code_point, 0>(), ::ztd::span<const code_unit, 0>());
		}
	};

	//////
	/// @brief A convenience alais that defaults `basic_shift_jis` to the x0208 version.
	template <typename _CodeUnit, typename _CodePoint = unicode_code_point>
	using basic_shift_jis = basic_shift_jis_x0208<_CodeUnit, _CodePoint>;

	//////
	/// @brief An instance of basic_shift_jis_x0208 for ease of use.
	inline constexpr basic_shift_jis_x0208<char> shift_jis_x0208 = {};

	//////
	/// @brief A convenience alais that defaults `shift_jis` to the x0208 version.
	using shift_jis_t = basic_shift_jis_x0208<char>;

	//////
	/// @brief An instance of basic_shift_jis for ease of use.
	inline constexpr auto& shift_jis = shift_jis_x0208;

	ZTD_TEXT_INLINE_ABI_NAMESPACE_CLOSE_I_
}} // namespace ztd::text


#include <ztd/epilogue.hpp>

#endif
