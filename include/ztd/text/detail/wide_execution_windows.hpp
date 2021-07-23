// =============================================================================
//
// ztd.text
// Copyright © 2021 JeanHeyd "ThePhD" Meneide and Shepherd's Oasis, LLC
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
// ============================================================================>

#pragma once

#ifndef ZTD_TEXT_DETAIL_WIDE_EXECUTION_WINDOWS_HPP
#define ZTD_TEXT_DETAIL_WIDE_EXECUTION_WINDOWS_HPP

#include <ztd/text/version.hpp>

#include <ztd/text/utf16.hpp>
#include <ztd/text/encode_result.hpp>
#include <ztd/text/decode_result.hpp>
#include <ztd/text/error_handler.hpp>
#include <ztd/text/unicode_code_point.hpp>
#include <ztd/text/is_ignorable_error_handler.hpp>
#include <ztd/text/is_unicode_encoding.hpp>
#include <ztd/text/is_full_range_representable.hpp>
#include <ztd/text/type_traits.hpp>
#include <ztd/text/detail/progress_handler.hpp>

#include <ztd/ranges/range.hpp>
#include <ztd/ranges/span.hpp>

#include <ztd/text/detail/windows.hpp>

#if ZTD_IS_ON(ZTD_PLATFORM_WINDOWS_I_)

#include <iterator>
#include <utility>

#include <ztd/prologue.hpp>

namespace ztd { namespace text {
	ZTD_TEXT_INLINE_ABI_NAMESPACE_OPEN_I_

	//////
	/// @addtogroup ztd_text_encodings Encodings
	/// @{
	//////

	namespace __impl {

		//////
		/// @brief The Encoding that represents the "Wide Execution" (wide locale-based) encoding, as it exists on
		/// Windows. The wide execution encoding is typically associated with the locale, which is tied to the C
		/// standard library's setlocale function.
		///
		/// @remarks Windows uses UTF-16, unless you call the C Standard Library directly. If @c
		/// ZTD_TEXT_USE_CUNEICODE and @c ZTD_TEXT_USE_ICONV is not defined, this object may use the C Standard
		/// Library to perform transcoding if certain platform facilities are disabled or not available (e.g., a
		/// Windows-like machine without the Windows SDK). If this is the case, the C Standard Library has fundamental
		/// limitations which may treat your UTF-16 data like UCS-2, and result in broken input/output. This object
		/// uses UTF-16 directly on Windows when possible to avoid some of the platform-specific shenanigans.
		//////
		class __wide_execution_windows : private basic_utf16<wchar_t> {
		private:
			using __base_t = basic_utf16<wchar_t>;

		public:
			using __base_t::code_point;
			using __base_t::code_unit;
			using __base_t::state;

			//////
			/// @brief Whether or not this encoding is a unicode encoding or not.
			//////
			using is_unicode_encoding = ::std::integral_constant<bool, is_unicode_encoding_v<__base_t>>;
			//////
			/// @brief Whether or not this encoding's @c decode_one step is injective or not.
			//////
			using is_decode_injective = ::std::integral_constant<bool, is_decode_injective_v<__base_t>>;
			//////
			/// @brief Whether or not this encoding's @c encode_one step is injective or not.
			//////
			using is_encode_injective = ::std::integral_constant<bool, is_encode_injective_v<__base_t>>;

			//////
			/// @brief The maximum code units a single complete operation of encoding can produce.
			///
			//////
			inline static constexpr const ::std::size_t max_code_units = 8;
			//////
			/// @brief The maximum number of code points a single complete operation of decoding can produce.
			///
			//////
			inline static constexpr const ::std::size_t max_code_points = 8;

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
			template <typename _InputRange, typename _OutputRange, typename _ErrorHandler>
			static constexpr auto decode_one(
				_InputRange&& __input, _OutputRange&& __output, _ErrorHandler&& __error_handler, state& __s) {
				using _UErrorHandler                = remove_cvref_t<_ErrorHandler>;
				constexpr bool __call_error_handler = !is_ignorable_error_handler_v<_UErrorHandler>;

				// just go straight from UTF16
				__base_t __base_encoding {};
				__txt_detail::__progress_handler<!__call_error_handler, __wide_execution_windows>
					__intermediate_handler {};
				auto __intermediate_result = __base_encoding.decode_one(::std::forward<_InputRange>(__input),
					::std::forward<_OutputRange>(__output), __intermediate_handler, __s);

				if constexpr (__call_error_handler) {
					if (__intermediate_result.error_code != encoding_error::ok) {
						__wide_execution_windows __self {};
						return __error_handler(__self, ::std::move(__intermediate_result),
							::ztd::ranges::span<code_unit>(__intermediate_handler._M_code_units.data(),
							     __intermediate_handler._M_code_units_size));
					}
				}
				return __intermediate_result;
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
			template <typename _InputRange, typename _OutputRange, typename _ErrorHandler>
			static constexpr auto encode_one(
				_InputRange&& __input, _OutputRange&& __output, _ErrorHandler&& __error_handler, state& __s) {
				using _UErrorHandler                = remove_cvref_t<_ErrorHandler>;
				constexpr bool __call_error_handler = !is_ignorable_error_handler_v<_UErrorHandler>;

				// just go straight to UTF16
				__base_t __base_encoding {};
				__txt_detail::__progress_handler<!__call_error_handler, __wide_execution_windows>
					__intermediate_handler {};
				auto __intermediate_result = __base_encoding.encode_one(::std::forward<_InputRange>(__input),
					::std::forward<_OutputRange>(__output), __intermediate_handler, __s);

				if constexpr (__call_error_handler) {
					if (__intermediate_result.error_code != encoding_error::ok) {
						__wide_execution_windows __self {};
						return __error_handler(__self, ::std::move(__intermediate_result),
							::ztd::ranges::span<code_point>(__intermediate_handler._M_code_points.data(),
							     __intermediate_handler._M_code_points_size));
					}
				}
				return __intermediate_result;
			}
		};

	} // namespace __impl

	ZTD_TEXT_INLINE_ABI_NAMESPACE_CLOSE_I_
}} // namespace ztd::text

#include <ztd/epilogue.hpp>

#endif

#endif // ZTD_TEXT_DETAIL_WIDE_EXECUTION_WINDOWS_HPP
