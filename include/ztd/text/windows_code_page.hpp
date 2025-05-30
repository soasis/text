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

#ifndef ZTD_TEXT_WINDOWS_CODE_PAGE_HPP
#define ZTD_TEXT_WINDOWS_CODE_PAGE_HPP

#include <ztd/text/version.hpp>

#include <ztd/text/encode_result.hpp>
#include <ztd/text/decode_result.hpp>
#include <ztd/text/error_handler.hpp>
#include <ztd/text/utf16.hpp>
#include <ztd/text/unicode_code_point.hpp>
#include <ztd/text/is_ignorable_error_handler.hpp>

#include <ztd/ranges/range.hpp>
#include <ztd/idk/span.hpp>
#include <ztd/idk/type_traits.hpp>
#include <ztd/idk/detail/windows.hpp>

#include <utility>
#include <optional>
#include <cstdint>

#include <ztd/prologue.hpp>

namespace ztd { namespace text {
	ZTD_TEXT_INLINE_ABI_NAMESPACE_OPEN_I_

	template <typename _CodeUnit, typename _CodePoint>
	class basic_windows_code_page;

	//////
	/// @brief The encoding representing a Windows conversion using WideCharToMultiByte (encode) and
	/// MultiByteToWideChar (decode) conversion sequences.
	///
	/// @remarks This is slow due to the bad design of WideCharToMultiByte/MultiByteToWideChar, but it does guarantee
	/// access to all of the wide variety of legacy encodings Windows supports. If possible, a different named encoding
	/// type should be used to avoid the performance penalties that comes from API limitations of this encoding object.
	/// Occasionally, for correct text, this API limitations can be mitigated when using the bulk APIs: for the
	/// single-conversion APIs that do not perform bulk conversion, the performance penalty is endemic.
	template <typename _CodeUnit, typename _CodePoint = unicode_code_point>
	class basic_windows_code_page {
	public:
		//////
		/// @brief The code point type that is decoded to, and encoded from.
		using code_point = _CodePoint;
		//////
		/// @brief The code unit type that is decoded from, and encoded to.
		using code_unit = _CodeUnit;

		//////
		/// @brief The associated state for decode operations.
		struct decode_state {
			std::optional<code_point> maybe_replacement_code_point;

			constexpr decode_state() : maybe_replacement_code_point() {
			}

			constexpr decode_state(code_point __replacement_code_point)
			: maybe_replacement_code_point(__replacement_code_point) {
			}

			constexpr decode_state(const basic_windows_code_page& __encoding)
			: maybe_replacement_code_point(__encoding._M_replacement_code_point) {
			}
		};

		//////
		/// @brief The associated state for encode operations.
		struct encode_state {
			std::optional<code_unit> maybe_replacement_code_unit;

			constexpr encode_state() : maybe_replacement_code_unit() {
			}

			constexpr encode_state(code_point __replacement_code_unit)
			: maybe_replacement_code_unit(__replacement_code_unit) {
			}

			constexpr encode_state(const basic_windows_code_page& __encoding)
			: maybe_replacement_code_unit(__encoding._M_replacement_code_unit) {
			}
		};

		//////
		/// @brief Returns a potential view into the code points to use when an error happens and a replacement
		/// strategy is being used.
		///
		/// @param[in] __state The decode state of this particular conversion operation.
		///
		/// @return A potential view of code points to do replacements with. This is influenced by either what the
		/// state for the conversion was initialized with, and it defaults to what was stored in the actual encoding
		/// object.
		constexpr ::std::optional<::ztd::span<const code_point>> maybe_replacement_code_points(
			const decode_state& __state) noexcept {
			if (__state.maybe_replacement_code_point.has_value()) {
				return ::std::optional<::ztd::span<const code_point>>(
					::std::in_place, ::std::addressof(__state.maybe_replacement_code_point.value()), 1);
			}
			return ::std::nullopt;
		}

		//////
		/// @brief Returns a potential view into the code points to use when an error happens and a replacement
		/// strategy is being used.
		///
		/// @param[in] __state The encode state of this particular conversion operation.
		///
		/// @return A potential view of code units to do replacements with. This is influenced by either what the
		/// state for the conversion was initialized with, and it defaults to what was stored in the actual encoding
		/// object.
		constexpr ::std::optional<::ztd::span<const code_unit>> maybe_replacement_code_units(
			const encode_state& __state) noexcept {
			if (__state.maybe_replacement_code_unit.has_value()) {
				return ::std::optional<::ztd::span<const code_point>>(
					::std::in_place, ::std::addressof(__state.maybe_replacement_code_unit.value()), 1);
			}
			return ::std::nullopt;
		}

		//////
		/// @brief The maximum code units a single complete operation of encoding can produce.
		inline static constexpr const ::std::size_t max_code_units = 8;
		//////
		/// @brief The maximum number of code points a single complete operation of decoding can produce.
		inline static constexpr const ::std::size_t max_code_points = 8;


		//////
		/// @brief Default constructor: assumes the code page is the default (thread) code page with the value
		/// `CP_THREAD_ACP`.
		constexpr basic_windows_code_page() noexcept
		: basic_windows_code_page(::ztd::__idk_detail::__windows::__code_page_active_thread) {
		}

		//////
		/// @brief Constructor that applies the given code page to the underlying conversions.
		///
		/// @param __win32_code_page The code page. It is one of the allowed code page values
		/// (https://learn.microsoft.com/en-us/windows/win32/intl/code-page-identifiers) that are blessed by the
		/// WideCharToMultiByte and MultiByteToWideChar.
		constexpr basic_windows_code_page(uint32_t __win32_code_page) noexcept
		: _M_code_page(__win32_code_page)
		, _M_replacement_code_point()
		, _M_replacement_code_unit()
		, _M_p_codepage_info() {
#if ZTD_IS_ON(ZTD_PLATFORM_WINDOWS)
			CPINFOEXW* __p_codepage_info = nullptr;
			if (::ztd::__idk_detail::__windows::__get_codepage_descirptor(this->_M_code_page, &__p_codepage_info)) {
				this->_M_p_codepage_info = static_cast<void*>(__p_codepage_info);
			}
#endif
		}

		//////
		/// @brief Constructor that applies the given code page to the conversions, and sets the default-used
		/// replacement code point and replacement code unit for the underlying algorithm where possible.
		///
		/// @param __win32_code_page The code page. It is one of the allowed code page values
		/// (https://learn.microsoft.com/en-us/windows/win32/intl/code-page-identifiers) that are blessed by the
		/// WideCharToMultiByte and MultiByteToWideChar.
		///
		/// @param[in] __replacement_code_point The replacement code point (a single 2-byte UTF-16 code unit) to be
		/// used with the underlying algorithm, where permissible.
		///
		/// @param[in] __replacement_code_unit The replacement code unit (a single 1-byte `__win32_code_page`-encoded
		/// code unit) to be used with the underlying algorithm, where permissible.
		constexpr basic_windows_code_page(
			uint32_t __win32_code_page, uint16_t __replacement_code_point, int8_t __replacement_code_unit) noexcept
		: _M_code_page(__win32_code_page)
		, _M_replacement_code_point(static_cast<code_point>(__replacement_code_point))
		, _M_replacement_code_unit(static_cast<code_unit>(__replacement_code_unit))
		, _M_p_codepage_info() {
#if ZTD_IS_ON(ZTD_PLATFORM_WINDOWS)
			CPINFOEXW* __p_codepage_info = nullptr;
			if (::ztd::__idk_detail::__windows::__get_codepage_descirptor(this->_M_code_page, &__p_codepage_info)) {
				this->_M_p_codepage_info = static_cast<void*>(__p_codepage_info);
			}
#endif
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
		template <typename _Input, typename _Output, typename _ErrorHandler>
		auto decode_one(
			_Input&& __input, _Output&& __output, _ErrorHandler&& __error_handler, decode_state& __s) const {
#if ZTD_IS_ON(ZTD_PLATFORM_WINDOWS)
			using _UErrorHandler                = remove_cvref_t<_ErrorHandler>;
			using _SubInput                     = ztd::ranges::csubrange_for_t<::std::remove_reference_t<_Input>>;
			using _SubOutput                    = ztd::ranges::subrange_for_t<::std::remove_reference_t<_Output>>;
			using _Result                       = encode_result<_SubInput, _SubOutput, encode_state>;
			constexpr bool __call_error_handler = !is_ignorable_error_handler_v<_UErrorHandler>;

			auto __in_it   = ::ztd::ranges::cbegin(__input);
			auto __in_last = ::ztd::ranges::cend(__input);
			if (__in_it == __in_last) {
				// an exhausted sequence is fine
				return _Result(_SubInput(::std::move(__in_it), ::std::move(__in_last)),
					::std::forward<_Output>(__output), __s, encoding_error::ok);
			}

			auto __out_it   = ::ztd::ranges::begin(__output);
			auto __out_last = ::ztd::ranges::end(__output);

			WCHAR __intermediate_data[17]                 = {};
			const size_t __initial_intermediate_data_size = ::ztd::ranges::size(__intermediate_data);
			CHAR __input_read_data[17]                    = { *__in_it };
			size_t __input_read_size                      = 1;
			const size_t __initial_input_read_data_size   = ::ztd::ranges::size(__input_read_data);
			::std::uint32_t __code_page_id                = this->code_page();
			auto __flags = ::ztd::__idk_detail::__windows::__multibyte_to_widechar_flags(__code_page_id);
			size_t __intermediate_input_size = 0;
			for (; __in_it != __in_last; ++__input_read_size, ::ztd::ranges::iter_advance(__in_it)) {
				if constexpr (__call_error_handler) {
					if (__input_read_size > __initial_input_read_data_size) {
						return ::std::forward<_ErrorHandler>(__error_handler)(*this,
							_Result(_SubInput(::std::move(__in_it), ::std::move(__in_last)),
							     _SubOutput(::std::move(__out_it), ::std::move(__out_last)), __s,
							     encoding_error::invalid_sequence),
							::ztd::span<code_unit>(__input_read_data, __input_read_size - 1),
							::ztd::span<code_point>());
					}
				}
				int __win32_err = ::MultiByteToWideChar(static_cast<UINT>(__code_page_id), __flags,
					__input_read_data, static_cast<int>(__input_read_size), __intermediate_data,
					static_cast<int>(__initial_intermediate_data_size));
				if constexpr (__call_error_handler) {
					if (__win32_err == 0) {
						DWORD __last_win32_err = ::GetLastError();
						if (__last_win32_err == ERROR_INSUFFICIENT_BUFFER) {
							return ::std::forward<_ErrorHandler>(__error_handler)(*this,
								_Result(_SubInput(::std::move(__in_it), ::std::move(__in_last)),
								     _SubOutput(::std::move(__out_it), ::std::move(__out_last)), __s,
								     encoding_error::invalid_sequence),
								::ztd::span<code_unit>(__input_read_data, __input_read_size - 1),
								::ztd::span<code_point>());
						}
					}
					else {
						CPINFOEXW* __p_info = static_cast<CPINFOEXW*>(this->_M_p_codepage_info);
						if (::ztd::__idk_detail::__windows::__multibyte_to_widechar_conversion_failed(
							    __input_read_size, __input_read_data, __intermediate_data, __p_info)) {
							return ::std::forward<_ErrorHandler>(__error_handler)(*this,
								_Result(_SubInput(::std::move(__in_it), ::std::move(__in_last)),
								     _SubOutput(::std::move(__out_it), ::std::move(__out_last)), __s,
								     encoding_error::invalid_sequence),
								::ztd::span<code_unit>(__input_read_data, __input_read_size - 1),
								::ztd::span<code_point>());
						}
						// we've finally completed one indivisible unit of work! Move to the next part.
						__intermediate_input_size += static_cast<size_t>(__win32_err);
						break;
					}
				}
				__input_read_data[__input_read_size] = static_cast<CHAR>(*__in_it);
			}

			using __wutf16             = __txt_impl::__utf16_with<void, WCHAR, code_point, false>;
			using __intermediate_state = decode_state_t<__wutf16>;

			__wutf16 __intermediate_encoding {};
			__intermediate_state __intermediate_s {};
			__txt_detail::__progress_handler<!__call_error_handler, __wutf16> __intermediate_handler {};
			code_point __intermediate_output[17] {};
			::ztd::span<code_point> __output_buffer(__intermediate_output);
			::ztd::span<const WCHAR> __intermediate_input_buffer(__intermediate_data, __intermediate_input_size);
			for (;;) {
				auto __intermediate_result = __intermediate_encoding.decode_one(
					__intermediate_input_buffer, __output_buffer, __intermediate_handler, __intermediate_s);
				if constexpr (__call_error_handler) {
					if (__intermediate_result.error_code != encoding_error::ok) {
						return ::std::forward<_ErrorHandler>(__error_handler)(*this,
							_Result(_SubInput(::std::move(__in_it), ::std::move(__in_last)),
							     _SubOutput(::std::move(__out_it), ::std::move(__out_last)), __s,
							     __intermediate_result.error_code),
							::ztd::span<code_unit>(), __intermediate_handler._M_code_points_progress());
					}
				}
				if (!::ztd::ranges::empty(__intermediate_result.input)) {
					__intermediate_input_buffer
						= ::ztd::span<const WCHAR>(::ztd::to_address(__intermediate_result.input.cbegin()),
						     ::ztd::to_address(__intermediate_result.input.cend()));
					__output_buffer
						= ::ztd::span<code_point>(::ztd::to_address(__intermediate_result.output.cbegin()),
						     ::ztd::to_address(__intermediate_result.output.cend()));
					continue;
				}
			}
			auto __intermediate_output_begin = &__intermediate_output[0];
			auto __intermediate_output_end   = __output_buffer.data();
			for (auto __intermediate_output_it = __intermediate_output_begin;
				__intermediate_output_it != __intermediate_output_end;) {
				if constexpr (__call_error_handler) {
					if (__out_it == __out_last) {
						::ztd::span<code_unit> __code_point_leftover(
							__intermediate_output_it, __intermediate_output_end);
						return ::std::forward<_ErrorHandler>(__error_handler)(*this,
							_Result(_SubInput(::std::move(__in_it), ::std::move(__in_last)),
							     _SubOutput(::std::move(__out_it), ::std::move(__out_last)), __s,
							     encoding_error::insufficient_output_space),
							::ztd::span<code_unit>(__input_read_data, __input_read_size - 1),
							__code_point_leftover);
					}
				}
				*__out_it = *__intermediate_output_it;
				::ztd::ranges::iter_advance(__out_it);
			}

			return _Result(_SubInput(::std::move(__in_it), ::std::move(__in_last)),
				_SubOutput(::std::move(__out_it), ::std::move(__out_last)), __s, encoding_error::ok);
#else
			(void)__input;
			(void)__output;
			(void)__error_handler;
			(void)__s;
			static_assert(::ztd::always_false_v<_Input>,
				"This encoding type cannot be used on non-Windows platforms; please consider swapping this out for "
				"a different encoding type for a non-Windows platform.");
#endif
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
		template <typename _Input, typename _Output, typename _ErrorHandler>
		auto encode_one(
			_Input&& __input, _Output&& __output, _ErrorHandler&& __error_handler, encode_state& __s) const {
#if ZTD_IS_ON(ZTD_PLATFORM_WINDOWS)
			using _UErrorHandler                = remove_cvref_t<_ErrorHandler>;
			using _SubInput                     = ztd::ranges::csubrange_for_t<::std::remove_reference_t<_Input>>;
			using _SubOutput                    = ztd::ranges::subrange_for_t<::std::remove_reference_t<_Output>>;
			using _Result                       = encode_result<_SubInput, _SubOutput, encode_state>;
			constexpr bool __call_error_handler = !is_ignorable_error_handler_v<_UErrorHandler>;

			auto __in_it   = ::ztd::ranges::cbegin(__input);
			auto __in_last = ::ztd::ranges::cend(__input);
			if (__in_it == __in_last) {
				// an exhausted sequence is fine
				return _Result(_SubInput(::std::move(__in_it), ::std::move(__in_last)),
					::std::forward<_Output>(__output), __s, encoding_error::ok);
			}

			auto __out_it   = ::ztd::ranges::begin(__output);
			auto __out_last = ::ztd::ranges::end(__output);

			using __wutf16             = __txt_impl::__utf16_with<void, wchar_t, code_point, false>;
			using __intermediate_state = encode_state_t<__wutf16>;

			__wutf16 __intermediate_encoding {};
			__intermediate_state __intermediate_s {};
			__txt_detail::__progress_handler<!__call_error_handler, __wutf16> __intermediate_handler {};
			wchar_t __wide_intermediate[8] {};
			::ztd::span<wchar_t> __wide_write_buffer(__wide_intermediate);
			auto __intermediate_result
				= __intermediate_encoding.encode_one(_SubInput(::std::move(__in_it), ::std::move(__in_last)),
				     __wide_write_buffer, __intermediate_handler, __intermediate_s);
			if constexpr (__call_error_handler) {
				if (__intermediate_result.error_code != encoding_error::ok) {
					return ::std::forward<_ErrorHandler>(__error_handler)(*this,
						_Result(::std::move(__intermediate_result.input), ::std::forward<_Output>(__output), __s,
						     __intermediate_result.error_code),
						__intermediate_handler._M_code_points_progress(), ::ztd::span<code_unit>());
				}
			}
			constexpr const ::std::size_t __state_count_max = 17;
			CHAR __intermediate_output[__state_count_max] {};
			BOOL __default_char_used = false;
			CHAR __default_char      = '?';
			::ztd::span<const wchar_t> __wide_read_buffer(__wide_intermediate, __intermediate_result.output.data());
			::std::uint32_t __code_page_id = this->code_page();
			auto __used_defaults           = ::ztd::__idk_detail::__windows::__widechar_to_multibyte_used_char(
                    __code_page_id, &__default_char, &__default_char_used);
			int __win32_err = ::WideCharToMultiByte(static_cast<UINT>(__code_page_id), __used_defaults.__flags,
				__wide_read_buffer.data(), static_cast<int>(__wide_read_buffer.size()), __intermediate_output,
				__state_count_max, __used_defaults.__p_default_char, __used_defaults.__p_default_char_used);
			if constexpr (__call_error_handler) {
				if (__win32_err == 0) {
					DWORD __last_win32_err = ::GetLastError();
					if (__last_win32_err == ERROR_INSUFFICIENT_BUFFER) {
						// technically, this should never happen.
						return ::std::forward<_ErrorHandler>(__error_handler)(*this,
							_Result(::std::move(__intermediate_result.input), ::std::forward<_Output>(__output),
							     __s, encoding_error::invalid_sequence),
							__intermediate_handler._M_code_points_progress(), ::ztd::span<code_unit>());
					}
				}
				else {
					CPINFOEXW* __p_info = static_cast<CPINFOEXW*>(this->_M_p_codepage_info);
					if (::ztd::__idk_detail::__windows::__widechar_to_multibyte_conversion_failed(
						    __wide_intermediate, static_cast<size_t>(__win32_err), __intermediate_output,
						    __p_info)) {
						return ::std::forward<_ErrorHandler>(__error_handler)(*this,
							_Result(::std::move(__intermediate_result.input), ::std::forward<_Output>(__output),
							     __s, encoding_error::invalid_sequence),
							__intermediate_handler._M_code_points_progress(), ::ztd::span<code_unit>());
					}
				}
			}
			auto __intermediate_output_begin = __intermediate_output;
			auto __intermediate_output_end   = __intermediate_output_begin + __win32_err;
			for (auto __intermediate_output_it = __intermediate_output_begin;
				__intermediate_output_it != __intermediate_output_end;) {
				if constexpr (__call_error_handler) {
					if (__out_it == __out_last) {
						::ztd::span<code_unit> __code_unit_progress(
							__intermediate_output_it, __intermediate_output_end);
						return ::std::forward<_ErrorHandler>(__error_handler)(*this,
							_Result(::std::move(__intermediate_result.input),
							     _SubOutput(::std::move(__out_it), ::std::move(__out_last)), __s,
							     encoding_error::insufficient_output_space),
							__intermediate_handler._M_code_points_progress(), __code_unit_progress);
					}
				}
				*__out_it = *__intermediate_output_it;
				::ztd::ranges::iter_advance(__out_it);
			}
			return _Result(::std::move(__intermediate_result.input),
				_SubOutput(::std::move(__out_it), ::std::move(__out_last)), __s, __intermediate_result.error_code);
#else
			(void)__input;
			(void)__output;
			(void)__error_handler;
			(void)__s;
			static_assert(::ztd::always_false_v<_Input>,
				"This encoding type cannot be used on non-Windows platforms; please consider swapping this out for "
				"a different encoding type for a non-Windows platform.");
#endif
		}

		constexpr uint32_t code_page() const noexcept {
			return this->_M_code_page;
		}

		constexpr ::std::optional<code_point> maybe_default_replacement_code_point() const noexcept {
			return this->_M_replacement_code_point;
		}

		constexpr ::std::optional<code_unit> maybe_default_replacement_code_unit() const noexcept {
			return this->_M_replacement_code_unit;
		}

	private:
		::std::optional<code_point> _M_replacement_code_point;
		::std::optional<code_unit> _M_replacement_code_unit;
		void* _M_p_codepage_info;
		uint32_t _M_code_page;
	};

	using windows_code_page = basic_windows_code_page<char, unicode_code_point>;

	ZTD_TEXT_INLINE_ABI_NAMESPACE_CLOSE_I_
}} // namespace ztd::text

#include <ztd/epilogue.hpp>

#endif
