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

#ifndef ZTD_TEXT_DETAIL_EXECUTION_CUCHAR_HPP
#define ZTD_TEXT_DETAIL_EXECUTION_CUCHAR_HPP

#include <ztd/text/version.hpp>

#include <ztd/text/encode_result.hpp>
#include <ztd/text/decode_result.hpp>
#include <ztd/text/encoding_error.hpp>
#include <ztd/text/error_handler.hpp>
#include <ztd/text/is_ignorable_error_handler.hpp>
#include <ztd/text/unicode_code_point.hpp>
#include <ztd/text/state.hpp>
#include <ztd/text/utf8.hpp>
#include <ztd/text/utf16.hpp>
#include <ztd/text/assert.hpp>
#include <ztd/text/detail/progress_handler.hpp>
#include <ztd/text/detail/encoding_name.hpp>

#include <ztd/ranges/range.hpp>
#include <ztd/idk/span.hpp>
#include <ztd/idk/encoding_detection.hpp>
#include <ztd/idk/type_traits.hpp>
#include <ztd/idk/mbstate_t.hpp>
#include <ztd/idk/detail/windows.hpp>

#if (ZTD_IS_ON(ZTD_HEADER_CUCHAR) || ZTD_IS_ON(ZTD_HEADER_UCHAR_H)) && ZTD_IS_OFF(ZTD_PLATFORM_MAC_OS)

// clang-format off
#if ZTD_IS_ON(ZTD_HEADER_CUCHAR)
	#include <cuchar>
#else
extern "C" {
	#include <uchar.h>
}
#endif
// clang-format on
#include <cwchar>
#include <cstdint>

#include <ztd/prologue.hpp>

namespace ztd { namespace text {
	ZTD_TEXT_INLINE_ABI_NAMESPACE_OPEN_I_

	namespace __txt_detail {
		class __execution_decode_state {
		public:
			//////
			/// @brief The narrow mbstate_t from the standard library for multibyte conversion sequences.
			ztd_mbstate_t __narrow_state;
			//////
			/// @brief Whether or not there might be some accumulated data in the state.
			bool __output_pending;

			//////
			/// @brief Zero-initializes to its initial state, which includes the initial conversion sequence.
			__execution_decode_state() noexcept : __narrow_state(), __output_pending(false) {
				ZTD_TEXT_ASSERT_I_(::std::mbsinit(&__narrow_state) != 0);
			}

			//////
			/// @brief Finds out whether or not the state contains any unused data that needs to complete an
			/// indivisible unit of work.
			///
			/// @returns Whether or not there are additional information stored in any part of the standard-based
			/// streams have accumulated information for a continual decode operation.
			bool is_complete() const noexcept {
				return !this->__output_pending && (::std::mbsinit(&__narrow_state) != 0);
			}
		};

		class __execution_encode_state {
		public:
			//////
			/// @brief The narrow mbstate_t from the standard library for multibyte conversion sequences.
			ztd_mbstate_t __narrow_state;
			//////
			/// @brief Whether or not there might be some accumulated data in the state.
			bool __output_pending;

			//////
			/// @brief Zero-initializes to its initial state, which includes the initial conversion sequence.
			__execution_encode_state() noexcept : __narrow_state(), __output_pending(false) {
				ZTD_TEXT_ASSERT_I_(::std::mbsinit(&this->__narrow_state) != 0);
			}


			//////
			/// @brief Finds out whether or not the state contains any unused data that needs to complete an
			/// indivisible unit of work.
			///
			/// @returns Whether or not there are additional information stored in any part of the standard-based
			/// streams have accumulated information for a continual encode operation.
			bool is_complete() const noexcept {
				return !this->__output_pending && (::std::mbsinit(&__narrow_state) != 0);
			}
		};
	} // namespace __txt_detail

	namespace __txt_impl {

		//////
		/// @brief The Encoding that represents the "Execution" (narrow locale-based) encoding. The execution encoding
		/// is typically associated with the locale, which is tied to the C standard library's setlocale function.
		///
		/// @remarks Use of this type is subject to the C Standard Library or platform defaults. Some locales (such as
		/// the Big5 Hong King Supplementary Character Set (Big5-HKSCS)) are broken due to fundamental design issues
		/// in the C Standard Library and bugs in glibc/musl libc's current locale encoding support. On Apple, this is
		/// cuurrently assumed to be UTF-8 since they do not support the @c \<cuchar\> or @c
		/// \<uchar.h\> headers.
		class __execution_cuchar {
		public:
			//////
			/// @brief The state of the execution encoding used between decode calls, which may potentially manage
			/// shift state.
			///
			/// @remarks This type can potentially have lots of state due to the way the C API is specified. It is
			/// important it is preserved between calls, or text may become mangled / data may become lost.
			using decode_state = __txt_detail::__execution_decode_state;

			//////
			/// @brief The state of the execution encoding used between encode calls, which may potentially manage
			/// shift state.
			///
			/// @remarks This type can potentially have lots of state due to the way the C API is specified. It is
			/// important it is preserved between calls, or text may become mangled / data may become lost.
			using encode_state = __txt_detail::__execution_encode_state;
			//////
			/// @brief The individual units that result from an encode operation or are used as input to a decode
			/// operation.
			///
			/// @remarks Please note that char can be either signed or unsigned, and so generally can result in bad
			/// results when promoted to a plain `int` when working with code units or working with the C Standard
			/// Library.
			using code_unit = char;
			//////
			/// @brief The individual units that result from a decode operation or as used as input to an encode
			/// operation. For most encodings, this is going to be a Unicode Code Point or a Unicode Scalar Value.
			using code_point = unicode_code_point;
			//////
			/// @brief Whether or not the decode operation can process all forms of input into code point values.
			///
			/// @remarks All known execution encodings can decode into Unicode just fine. However, someone may define
			/// a platform encoding on their machine that does not transform cleanly. Therefore, decoding is not
			/// marked as injective.
			using is_decode_injective = ::std::false_type;
			//////
			/// @brief Whether or not the encode operation can process all forms of input into code unit values. This
			/// is absolutely not true: many unicode code point values cannot be safely converted to a large number
			/// of existing (legacy) encodings.
			using is_encode_injective = ::std::false_type;
			//////
			/// @brief The maximum code units a single complete operation of encoding can produce.
			///
			/// @remarks There are encodings for which one input can produce 3 code points (some Tamil encodings) and
			/// there are rumours of an encoding that can produce 7 code points from a handful of input. We use a
			/// protective/conservative 8, here, to make sure ABI isn't broken later.
			inline static constexpr ::std::size_t max_code_points = 8;
			//////
			/// @brief The maximum number of code points a single complete operation of decoding can produce.
			///
			/// @remarks This is bounded by the platform's `MB_LEN_MAX` macro, which is an integral constant
			/// expression representing the maximum value of output all C locales can produce from a single complete
			/// operation.
			inline static constexpr ::std::size_t max_code_units = MB_LEN_MAX;
			//////
			/// @brief A range of code unit values that can be used as a replacement value, instead of the ones used
			/// in ztd::text::default_handler_t.
			///
			/// @remarks The default replacement code point / code unit is U+FFFD (). This, obviously, does not fit
			/// in the majority of the (legacy) locale encodings in C and C++. '?' is a much more conservative
			/// option, here, and most (all?) locale encodings have some form of representation for it.
			inline static constexpr code_unit replacement_code_units[1] = { '?' };

			//////
			/// @brief Returns whether or not this encoding is a unicode encoding.
			///
			/// @remarks This function operates at runtime and queries the existing locale through a variety of
			/// platform-specific means (such as `nl_langinfo` for POSIX, ACP probing on Windows, or fallin back to
			/// `std::setlocale` name checking otherwise).
			static bool contains_unicode_encoding() noexcept {
				return ztd::is_execution_encoding_unicode();
			}

			//////
			/// @brief Encodes a single complete unit of information as code units and produces a result with the
			/// input and output ranges moved past what was successfully read and written; or, produces an error and
			/// returns the input and output ranges untouched.
			///
			/// @param[in] __input The input view to read code uunits from.
			/// @param[in] __output The output view to write code points into.
			/// @param[in] __error_handler The error handler to invoke if encoding fails.
			/// @param[in, out] __s The necessary state information. Most encodings have no state, but because this
			/// is effectively a runtime encoding and therefore it is important to preserve and manage this state.
			///
			/// @returns A ztd::text::encode_result object that contains the reconstructed input range,
			/// reconstructed output range, error handler, and a reference to the passed-in state.
			///
			/// @remarks Platform APIs and/or the C Standard Library may be used to properly decode one complete unit
			/// of information (alongside mbstate_t usage). Whether or not the state is used is based on the
			/// implementation and what it chooses.
			///
			/// @remarks To the best ability of the implementation, the iterators will be
			/// returned untouched (e.g., the input models at least a view and a forward_range). If it is not
			/// possible, returned ranges may be incremented even if an error occurs due to the semantics of any view
			/// that models an input_range.
			template <typename _Input, typename _Output, typename _ErrorHandler>
			static constexpr auto encode_one(
				_Input&& __input, _Output&& __output, _ErrorHandler&& __error_handler, encode_state& __s) {
				using _UErrorHandler = remove_cvref_t<_ErrorHandler>;
				using _SubInput      = ztd::ranges::csubrange_for_t<::std::remove_reference_t<_Input>>;
				using _SubOutput     = ztd::ranges::subrange_for_t<::std::remove_reference_t<_Output>>;
				using _Result        = encode_result<_SubInput, _SubOutput, encode_state>;
				constexpr bool __call_error_handler = !is_ignorable_error_handler_v<_UErrorHandler>;

				if (ztd::is_execution_encoding_utf8()) {
					// just go straight to UTF8
					using __execution_utf8 = __txt_impl::__utf8_with<__execution_cuchar, code_unit, code_point,
						decode_state, encode_state>;
					__execution_utf8 __base_encoding {};
					return __base_encoding.encode_one(::std::forward<_Input>(__input),
						::std::forward<_Output>(__output), ::std::forward<_ErrorHandler>(__error_handler), __s);
				}

				auto __in_it   = ::ztd::ranges::cbegin(__input);
				auto __in_last = ::ztd::ranges::cend(__input);

				if (__in_it == __in_last) {
					// an exhausted sequence is fine
					return _Result(_SubInput(::std::move(__in_it), ::std::move(__in_last)),
						::std::forward<_Output>(__output), __s, encoding_error::ok);
				}

#if ZTD_IS_ON(ZTD_PLATFORM_WINDOWS)
				auto __out_it   = ::ztd::ranges::begin(__output);
				auto __out_last = ::ztd::ranges::end(__output);

				if constexpr (__call_error_handler) {
					if (__out_it == __out_last) {
						__execution_cuchar __self {};
						return ::std::forward<_ErrorHandler>(__error_handler)(__self,
							_Result(::std::forward<_Input>(__input),
							     _SubOutput(::std::move(__out_it), ::std::move(__out_last)), __s,
							     encoding_error::insufficient_output_space),
							::ztd::span<code_point>(), ::ztd::span<code_unit>());
					}
				}

				using __wutf16             = __txt_impl::__utf16_with<void, wchar_t, code_point, false>;
				using __intermediate_state = encode_state_t<__wutf16>;

				__wutf16 __intermediate_encoding {};
				__intermediate_state __intermediate_s {};
				__txt_detail::__progress_handler<!__call_error_handler, __wutf16> __intermediate_handler {};
				wchar_t __wide_intermediary[8] {};
				::ztd::span<wchar_t> __wide_write_buffer(__wide_intermediary);
				auto __intermediate_result = __intermediate_encoding.encode_one(::std::forward<_Input>(__input),
					__wide_write_buffer, __intermediate_handler, __intermediate_s);
				if constexpr (__call_error_handler) {
					if (__intermediate_result.error_code != encoding_error::ok) {
						__execution_cuchar __self {};
						return ::std::forward<_ErrorHandler>(__error_handler)(__self,
							_Result(::std::move(__intermediate_result.input), ::std::forward<_Output>(__output),
							     __s, __intermediate_result.error_code),
							__intermediate_handler._M_code_points_progress(), ::ztd::span<code_unit>());
					}
				}
				constexpr const ::std::size_t __state_count_max = 12;
				code_unit __intermediary_output[__state_count_max] {};
				int __used_default_char = false;
				::ztd::span<const wchar_t> __wide_read_buffer(
					__wide_intermediary, __intermediate_result.output.data());
				int __res = ::WideCharToMultiByte(
					static_cast<UINT>(__idk_detail::__windows::__determine_active_code_page()),
					WC_ERR_INVALID_CHARS, __wide_read_buffer.data(), static_cast<int>(__wide_read_buffer.size()),
					__intermediary_output, __state_count_max, ::std::addressof(replacement_code_units[0]),
					::std::addressof(__used_default_char));
				if constexpr (__call_error_handler) {
					if (__res == 0) {
						__execution_cuchar __self {};
						return ::std::forward<_ErrorHandler>(__error_handler)(__self,
							_Result(::std::move(__intermediate_result.input), ::std::forward<_Output>(__output),
							     __s,
							     ::GetLastError() == ERROR_INSUFFICIENT_BUFFER
							          ? encoding_error::insufficient_output_space
							          : encoding_error::invalid_sequence),
							__intermediate_handler._M_code_points_progress(), ::ztd::span<code_unit>());
					}
				}
				for (auto __intermediary_it = __intermediary_output; __res-- > 0;) {
					if constexpr (__call_error_handler) {
						if (__out_it == __out_last) {
							__execution_cuchar __self {};
							::ztd::span<code_unit> __code_unit_progress(
								__intermediary_it, static_cast<::std::size_t>(__res + 1));
							return ::std::forward<_ErrorHandler>(__error_handler)(__self,
								_Result(::std::move(__intermediate_result.input),
								     _SubOutput(::std::move(__out_it), ::std::move(__out_last)), __s,
								     encoding_error::insufficient_output_space),
								__intermediate_handler._M_code_points_progress(), __code_unit_progress);
						}
					}
					*__out_it = *__intermediary_it;
					::ztd::ranges::iter_advance(__out_it);
				}
				return _Result(::std::move(__intermediate_result.input),
					_SubOutput(::std::move(__out_it), ::std::move(__out_last)), __s,
					__intermediate_result.error_code);
#else

				auto __out_it   = ::ztd::ranges::begin(__output);
				auto __out_last = ::ztd::ranges::end(__output);

				if constexpr (__call_error_handler) {
					if (__out_it == __out_last) {
						__execution_cuchar __self {};
						return ::std::forward<_ErrorHandler>(__error_handler)(__self,
							_Result(_SubInput(::std::move(__in_it), ::std::move(__in_last)),
							     _SubOutput(::std::move(__out_it), ::std::move(__out_last)), __s,
							     encoding_error::insufficient_output_space),
							::ztd::span<code_point>(), ::ztd::span<code_unit>());
					}
				}

				code_point __codepoint = *__in_it;
				::ztd::ranges::iter_advance(__in_it);
				code_unit __intermediary_output[(MB_LEN_MAX)] {};
				::std::size_t __res = ZTD_UCHAR_SCOPE_I_ c32rtomb(
					__intermediary_output, __codepoint, ::std::addressof(__s.__narrow_state));
				if constexpr (__call_error_handler) {
					if (__res == static_cast<::std::size_t>(-1)) {
						__execution_cuchar __self {};
						return ::std::forward<_ErrorHandler>(__error_handler)(__self,
							_Result(_SubInput(::std::move(__in_it), ::std::move(__in_last)),
							     _SubOutput(::std::move(__out_it), ::std::move(__out_last)), __s,
							     encoding_error::invalid_sequence),
							::ztd::span<code_point>(&__codepoint, 1), ::ztd::span<code_unit>());
					}
				}

				for (auto __intermediary_it = __intermediary_output; __res-- > 0; ++__intermediary_it) {
					if constexpr (__call_error_handler) {
						if (__out_it == __out_last) {
							__execution_cuchar __self {};
							::ztd::span<code_unit> __code_unit_progress(
								__intermediary_it, static_cast<::std::size_t>(__res + 1));
							return ::std::forward<_ErrorHandler>(__error_handler)(__self,
								_Result(_SubInput(::std::move(__in_it), ::std::move(__in_last)),
								     _SubOutput(::std::move(__out_it), ::std::move(__out_last)), __s,
								     encoding_error::insufficient_output_space),
								::ztd::span<code_point>(&__codepoint, 1), __code_unit_progress);
						}
					}
					*__out_it = *__intermediary_it;
					::ztd::ranges::iter_advance(__out_it);
				}

				return _Result(_SubInput(::std::move(__in_it), ::std::move(__in_last)),
					_SubOutput(::std::move(__out_it), ::std::move(__out_last)), __s, encoding_error::ok);
#endif // Windows is hell
			}

			//////
			/// @brief Decodes a single complete unit of information as code points and produces a result with the
			/// input and output ranges moved past what was successfully read and written; or, produces an error and
			/// returns the input and output ranges untouched.
			///
			/// @param[in] __input The input view to read code uunits from.
			/// @param[in] __output The output view to write code points into.
			/// @param[in] __error_handler The error handler to invoke if encoding fails.
			/// @param[in, out] __s The necessary state information. Most encodings have no state, but because this
			/// is effectively a runtime encoding and therefore it is important to preserve and manage this state.
			///
			/// @returns A ztd::text::decode_result object that contains the reconstructed input range,
			/// reconstructed output range, error handler, and a reference to the passed-in state.
			///
			/// @remarks Platform APIs and/or the C Standard Library may be used to properly decode one complete unit
			/// of information (alongside mbstate_t usage). Whether or not the state is used is based on the
			/// implementation and what it chooses.
			///
			/// @remarks To the best ability of the implementation, the iterators will be
			/// returned untouched (e.g., the input models at least a view and a forward_range). If it is not
			/// possible, returned ranges may be incremented even if an error occurs due to the semantics of any view
			/// that models an input_range.
			template <typename _Input, typename _Output, typename _ErrorHandler>
			static constexpr auto decode_one(
				_Input&& __input, _Output&& __output, _ErrorHandler&& __error_handler, decode_state& __s) {
				using _UErrorHandler = remove_cvref_t<_ErrorHandler>;
				using _SubInput      = ztd::ranges::csubrange_for_t<::std::remove_reference_t<_Input>>;
				using _SubOutput     = ztd::ranges::subrange_for_t<::std::remove_reference_t<_Output>>;
				using _Result        = decode_result<_SubInput, _SubOutput, decode_state>;
				constexpr bool __call_error_handler = !is_ignorable_error_handler_v<_UErrorHandler>;

				if (ztd::is_execution_encoding_utf8()) {
					// just go straight to UTF8
					using __execution_utf8 = __txt_impl::__utf8_with<__execution_cuchar, code_unit, code_point,
						decode_state, encode_state>;
					__execution_utf8 __base_encoding {};
					return __base_encoding.decode_one(::std::forward<_Input>(__input),
						::std::forward<_Output>(__output), ::std::forward<_ErrorHandler>(__error_handler), __s);
				}

				auto __in_it   = ::ztd::ranges::cbegin(__input);
				auto __in_last = ::ztd::ranges::cend(__input);

				if (__in_it == __in_last) {
					// an exhausted sequence is fine
					return _Result(_SubInput(::std::move(__in_it), ::std::move(__in_last)),
						::std::forward<_Output>(__output), __s, encoding_error::ok);
				}

				auto __out_it   = ::ztd::ranges::begin(__output);
				auto __out_last = ::ztd::ranges::end(__output);

				if constexpr (__call_error_handler) {
					if (__out_it == __out_last) {
						__execution_cuchar __self {};
						return ::std::forward<_ErrorHandler>(__error_handler)(__self,
							_Result(_SubInput(::std::move(__in_it), ::std::move(__in_last)),
							     _SubOutput(::std::move(__out_it), ::std::move(__out_last)), __s,
							     encoding_error::insufficient_output_space),
							::ztd::span<code_unit>(), ::ztd::span<code_point>());
					}
				}

				code_unit __intermediary_input[max_code_units] {};
#if ZTD_IS_ON(ZTD_PLATFORM_WINDOWS) && ZTD_IS_OFF(ZTD_COMPILER_MINGW)
				__intermediary_input[0] = *__in_it;
				::ztd::ranges::iter_advance(__in_it);
				::std::size_t __state_count = 1;
				for (; __state_count < max_code_units; ++__state_count) {
					using __wutf16             = __txt_impl::__utf16_with<void, wchar_t, code_point, false>;
					using __intermediate_state = decode_state_t<__wutf16>;

					constexpr const int __wide_intermediary_size = 4;
					wchar_t __wide_intermediary[__wide_intermediary_size] {};
					int __res = ::MultiByteToWideChar(
						static_cast<UINT>(__idk_detail::__windows::__determine_active_code_page()),
						MB_ERR_INVALID_CHARS, __intermediary_input, static_cast<int>(__state_count),
						__wide_intermediary, __wide_intermediary_size);
					if (__res == 0) {
						if (::GetLastError() == ERROR_NO_UNICODE_TRANSLATION) {
							// loopback; we might just not have enough code units
							if constexpr (__call_error_handler) {
								if (__in_it == __in_last) {
									__execution_cuchar __self {};
									return ::std::forward<_ErrorHandler>(__error_handler)(__self,
										_Result(_SubInput(::std::move(__in_it), ::std::move(__in_last)),
										     _SubOutput(::std::move(__out_it), ::std::move(__out_last)), __s,
										     encoding_error::incomplete_sequence),
										::ztd::span<code_unit>(__intermediary_input, __state_count),
										::ztd::span<code_point>());
								}
							}
							__intermediary_input[__state_count] = *__in_it;
							::ztd::ranges::iter_advance(__in_it);
							continue;
						}
						if constexpr (__call_error_handler) {
							__execution_cuchar __self {};
							return ::std::forward<_ErrorHandler>(__error_handler)(__self,
								_Result(_SubInput(::std::move(__in_it), ::std::move(__in_last)),
								     _SubOutput(::std::move(__out_it), ::std::move(__out_last)), __s,
								     encoding_error::invalid_sequence),
								::ztd::span<code_unit>(__intermediary_input, __state_count),
								::ztd::span<code_point>());
						}
					}

					__wutf16 __intermediate_encoding {};
					__intermediate_state __intermediate_s {};
					__txt_detail::__progress_handler<!__call_error_handler, __wutf16> __intermediate_handler {};
					::ztd::span<wchar_t, sizeof(__wide_intermediary) / sizeof(wchar_t)> __wide_intermediary_view(
						__wide_intermediary);
					auto __intermediate_result = __intermediate_encoding.encode_one(__wide_intermediary_view,
						::std::forward<_Output>(__output), __intermediate_handler, __intermediate_s);
					if constexpr (__call_error_handler) {
						if (__intermediate_result.error_code != encoding_error::ok) {
							__execution_cuchar __self {};
							return ::std::forward<_ErrorHandler>(__error_handler)(__self,
								_Result(_SubInput(::std::move(__in_it), ::std::move(__in_last)),
								     ::std::move(__intermediate_result.output), __s,
								     __intermediate_result.error_code),
								::ztd::span<code_unit>(__intermediary_input, __state_count),
								__intermediate_handler._M_code_points_progress());
						}
					}
					return _Result(_SubInput(::std::move(__in_it), ::std::move(__in_last)),
						::std::move(__intermediate_result.output), __s, __intermediate_result.error_code);
#else
				if (__s.__output_pending) {
					// need to drain potential mbstate_t of any leftover code points?
					ztd_char32_t __intermediary_output[max_code_points] {};
					::std::size_t __res = ZTD_UCHAR_SCOPE_I_ mbrtoc32(::std::addressof(__intermediary_output[0]),
						nullptr, 0, ::std::addressof(__s.__narrow_state));
					if constexpr (__call_error_handler) {
						if (__res == static_cast<::std::size_t>(-1)) {
							__execution_cuchar __self {};
							return ::std::forward<_ErrorHandler>(__error_handler)(__self,
								_Result(_SubInput(::std::move(__in_it), ::std::move(__in_last)),
								     _SubOutput(::std::move(__out_it), ::std::move(__out_last)), __s,
								     encoding_error::invalid_sequence),
								::ztd::span<code_unit>(), ::ztd::span<code_point>());
						}
					}
					*__out_it = __intermediary_output[0];
					::ztd::ranges::iter_advance(__out_it);
					__s.__output_pending = __res == static_cast<::std::size_t>(-3);
					return _Result(_SubInput(::std::move(__in_it), ::std::move(__in_last)),
						_SubOutput(::std::move(__out_it), ::std::move(__out_last)), __s, encoding_error::ok);
				}

				::std::size_t __state_offset = 0;
				::std::size_t __state_count  = 1;
				for (; __state_offset < max_code_units; (void)++__state_offset, (void)++__state_count) {
					ztd_mbstate_t __preserved_state      = __s.__narrow_state;
					__intermediary_input[__state_offset] = *__in_it;
					::ztd::ranges::iter_advance(__in_it);
					ztd_char32_t __intermediary_output[1] {};
					::std::size_t __res = ZTD_UCHAR_SCOPE_I_ mbrtoc32(::std::addressof(__intermediary_output[0]),
						::std::addressof(__intermediary_input[0]), __state_count,
						::std::addressof(__preserved_state));

					switch (__res) {
					case static_cast<::std::size_t>(-2):
						// cycle around and continue
						if constexpr (__call_error_handler) {
							if (__in_it == __in_last) {
								__execution_cuchar __self {};
								return ::std::forward<_ErrorHandler>(__error_handler)(__self,
									_Result(_SubInput(::std::move(__in_it), ::std::move(__in_last)),
									     _SubOutput(::std::move(__out_it), ::std::move(__out_last)), __s,
									     encoding_error::incomplete_sequence),
									::ztd::span<code_unit>(__intermediary_input, __state_count),
									::ztd::span<code_point>());
							}
						}
						break;
					case static_cast<::std::size_t>(-3):
						*__out_it = __intermediary_output[0];
						::ztd::ranges::iter_advance(__out_it);
						__s.__narrow_state   = __preserved_state;
						__s.__output_pending = true;
						__state_offset       = __state_count;
						return _Result(_SubInput(::std::move(__in_it), ::std::move(__in_last)),
							_SubOutput(::std::move(__out_it), ::std::move(__out_last)), __s, encoding_error::ok);
					case static_cast<::std::size_t>(-1):
						if constexpr (__call_error_handler) {
							// OH GOD PANIC AAAAAAH
							// seriously we're out of spec here:
							// everything has gone to shit
							// even the __narrow_state is unspecified ;;
							__execution_cuchar __self {};
							return ::std::forward<_ErrorHandler>(__error_handler)(__self,
								_Result(_SubInput(::std::move(__in_it), ::std::move(__in_last)),
								     _SubOutput(::std::move(__out_it), ::std::move(__out_last)), __s,
								     encoding_error::invalid_sequence),
								::ztd::span<code_unit>(
								     ::std::addressof(__intermediary_input[0]), __state_count),
								::ztd::span<code_point>());
						}
						else {
							break;
						}
					case static_cast<::std::size_t>(0):
						// 0 means null character; ok
						*__out_it = __intermediary_output[0];
						::ztd::ranges::iter_advance(__out_it);
						return _Result(_SubInput(::std::move(__in_it), ::std::move(__in_last)),
							_SubOutput(::std::move(__out_it), ::std::move(__out_last)), __s, encoding_error::ok);
					default:
						*__out_it = __intermediary_output[0];
						::ztd::ranges::iter_advance(__out_it);
						__s.__narrow_state = __preserved_state;
						return _Result(_SubInput(::std::move(__in_it), ::std::move(__in_last)),
							_SubOutput(::std::move(__out_it), ::std::move(__out_last)), __s, encoding_error::ok);
					}
#endif
				}
				if constexpr (__call_error_handler) {
					// if it was invalid, we would have caught it before
					// this is for incomplete sequences only
					__execution_cuchar __self {};
					return ::std::forward<_ErrorHandler>(__error_handler)(__self,
						_Result(_SubInput(::std::move(__in_it), ::std::move(__in_last)),
						     _SubOutput(::std::move(__out_it), ::std::move(__out_last)), __s,
						     encoding_error::incomplete_sequence),
						::ztd::span<code_unit>(::std::addressof(__intermediary_input[0]), __state_count),
						::ztd::span<code_point>());
				}
				else {
					// ... I mean.
					// You asked for it???
					_Result(_SubInput(::std::move(__in_it), ::std::move(__in_last)),
						_SubOutput(::std::move(__out_it), ::std::move(__out_last)), __s, encoding_error::ok);
				}
			}
		};
	} // namespace __txt_impl


	ZTD_TEXT_INLINE_ABI_NAMESPACE_CLOSE_I_
}} // namespace ztd::text

#include <ztd/epilogue.hpp>

#endif

#endif
