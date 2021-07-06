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

#if ZTD_IS_ON(ZTD_PLATFORM_WINDOWS_I_)

#include <ztd/text/utf16.hpp>
#include <ztd/text/encode_result.hpp>
#include <ztd/text/decode_result.hpp>
#include <ztd/text/error_handler.hpp>
#include <ztd/text/unicode_code_point.hpp>
#include <ztd/text/is_ignorable_error_handler.hpp>
#include <ztd/text/type_traits.hpp>
#include <ztd/text/detail/empty_state.hpp>
#include <ztd/text/detail/windows.hpp>
#include <ztd/text/detail/progress_handler.hpp>

#include <ztd/ranges/range.hpp>
#include <ztd/ranges/span.hpp>

#include <iterator>
#include <utility>

#include <ztd/prologue.hpp>

namespace ztd { namespace text {
	ZTD_TEXT_INLINE_ABI_NAMESPACE_OPEN_I_

	//////
	/// @addtogroup ztd_text_encodings Encodings
	/// @{
	//////

	//////
	/// @brief The Encoding that represents the "Wide Execution" (wide locale-based) encoding. The wide execution
	/// encoding is typically associated with the locale, which is tied to the C standard library's setlocale function.
	/// @remarks Windows uses UTF-16, unless you call the C Standard Library directly. If @c ZTD_TEXT_USE_CUNEICODE is
	/// not defined, this object may use the C Standard Library to perform transcoding if certain platform facilities
	/// are disabled or not available. If this is the case, the C Standard Library has fundamental limitations which
	/// may treat your UTF-16 data like UCS-2, and result in broken input/output. This object uses UTF-16 directly on
	/// Windows when possible to avoid some of the platform-specific shenanigans.
	//////
	class wide_execution {
	public:
		//////
		/// @brief The individual units that result from an encode operation or are used as input to a decode
		/// operation.
		///
		/// @remarks Please note that wchar_t is a variably sized type across platforms and may not represent either
		/// UTF-16 or UTF-32, including on *nix or POSIX platforms.
		//////
		using code_unit = wchar_t;
		//////
		/// @brief The individual units that result from a decode operation or as used as input to an encode
		/// operation. For most encodings, this is going to be a Unicode Code Point or a Unicode Scalar Value.
		//////
		using code_point = unicode_code_point;

	private:
		using __wide_decode_state = decode_state_t<__impl::__utf16_with<void, wchar_t, code_point, false>>;
		using __wide_encode_state = encode_state_t<__impl::__utf16_with<void, wchar_t, code_point, false>>;

	public:
		//////
		/// @brief The state of the wide execution encoding used between calls, which may potentially manage shift
		/// state.
		///
		/// @remarks This type can potentially have lots of state due to the way the C API is specified.
		//////
		using decode_state = __wide_decode_state;
		//////
		/// @brief The state of the wide execution encoding used between calls, which may potentially manage shift
		/// state.
		///
		/// @remarks This type can potentially have lots of state due to the way the C API is specified.
		//////
		using encode_state = __wide_encode_state;
		//////
		/// @brief Whether or not the decode operation can process all forms of input into code point values.
		///
		/// @remarks All known wide execution encodings can decode into Unicode just fine.
		//////
		using is_decode_injective = ::std::true_type;

		//////
		/// @brief Whether or not the encode operation can process all forms of input into code unit values. On
		/// Windows, this is guaranteed to be UTF-16 encoding for the platform. Normally, this is UTF-32 on *nix/POSIX
		/// machines, but it can (and has been) changed before, sometimes even at runtime.
		///
		/// @remarks IBM encodings/computers make life interesting...
		//////
		using is_encode_injective = ::std::true_type;

		//////
		/// @brief Whether or not this encoding a Unicode encoding of some type.
		///
		/// @remarks On Windows, this is always true. On other platforms, the guarantees are not quite there. IBM
		/// encodings/computers make life interesting...
		//////
		using is_unicode_encoding = ::std::true_type;

		//////
		/// @brief The maximum code units a single complete operation of encoding can produce.
		///
		//////
		inline static constexpr const ::std::size_t max_code_units = 8;
		//////
		/// @brief The maximum number of code points a single complete operation of decoding can produce.
		///
		//////
		inline static constexpr const ::std::size_t max_code_points = 4;

		//////
		/// @brief Returns whether or not this encoding is a unicode encoding.
		///
		/// @remarks This function operates at runtime and queries the existing locale through a variety of
		/// platform-specific means (such as @c nl_langinfo for POSIX, ACP probing on Windows, or fallin back to @c
		/// std::setlocale name checking otherwise).
		//////
		static constexpr bool contains_unicode_encoding() noexcept {
			return true;
		}

		//////
		/// @brief Encodes a single complete unit of information as code units and produces a result with the
		/// input and output ranges moved past what was successfully read and written; or, produces an error and
		/// returns the input and output ranges untouched.
		///
		/// @param[in] __input The input view to read code uunits from.
		/// @param[in] __output The output view to write code points into.
		/// @param[in] __error_handler The error handler to invoke if encoding fails.
		/// @param[in, out] __s The necessary state information. Most encodings have no state, but because this is
		/// effectively a runtime encoding and therefore it is important to preserve and manage this state.
		///
		/// @returns A ztd::text::encode_result object that contains the reconstructed input range,
		/// reconstructed output range, error handler, and a reference to the passed-in state.
		///
		/// @remarks Platform APIs and/or the C Standard Library may be used to properly decode one complete unit of
		/// information (alongside std::mbstate_t usage). Whether or not the state is used is based on the
		/// implementation and what it chooses. If ZTD_TEXT_USE_CUNEICODE is defined, the ztd.cuneicode library may be
		/// used to fulfill this functionality.
		///
		/// @remarks To the best ability of the implementation, the iterators will be
		/// returned untouched (e.g., the input models at least a view and a forward_range). If it is not possible,
		/// returned ranges may be incremented even if an error occurs due to the semantics of any view that models an
		/// input_range.
		//////
		template <typename _InputRange, typename _OutputRange, typename _ErrorHandler>
		static constexpr auto encode_one(
			_InputRange&& __input, _OutputRange&& __output, _ErrorHandler&& __error_handler, encode_state& __s) {
			using _UErrorHandler = remove_cvref_t<_ErrorHandler>;
			using _Result = __txt_detail::__reconstruct_encode_result_t<_InputRange, _OutputRange, encode_state>;
			constexpr bool __call_error_handler = !is_ignorable_error_handler_v<_UErrorHandler>;
			using __u16e                        = __impl::__utf16_with<void, wchar_t, code_point, false>;
			using __intermediate_state          = encode_state_t<__u16e>;

			__u16e __u16enc {};
			__intermediate_state __intermediate_s {};
			__txt_detail::__progress_handler<!__call_error_handler, wide_execution> __intermediate_handler {};
			auto __result = __u16enc.encode_one(::std::forward<_InputRange>(__input),
				::std::forward<_OutputRange>(__output), __intermediate_handler, __intermediate_s);
			if (__result.error_code != encoding_error::ok) {
				wide_execution __self {};
				return __error_handler(__self,
					_Result(::std::move(__result.input), ::std::move(__result.output), __s, __result.error_code),
					::ztd::ranges::span<code_point>(__intermediate_handler._M_code_points.data(),
					     __intermediate_handler._M_code_points_size));
			}
			return _Result(::std::move(__result.input), ::std::move(__result.output), __s, __result.error_code);
		}

		//////
		/// @brief Decodes a single complete unit of information as code points and produces a result with the
		/// input and output ranges moved past what was successfully read and written; or, produces an error and
		/// returns the input and output ranges untouched.
		///
		/// @param[in] __input The input view to read code uunits from.
		/// @param[in] __output The output view to write code points into.
		/// @param[in] __error_handler The error handler to invoke if encoding fails.
		/// @param[in, out] __s The necessary state information. Most encodings have no state, but because this is
		/// effectively a runtime encoding and therefore it is important to preserve and manage this state.
		///
		/// @returns A ztd::text::decode_result object that contains the reconstructed input range,
		/// reconstructed output range, error handler, and a reference to the passed-in state.
		///
		/// @remarks Platform APIs and/or the C Standard Library may be used to properly decode one complete unit of
		/// information (alongside std::mbstate_t usage). Whether or not the state is used is based on the
		/// implementation and what it chooses. If @c ZTD_TEXT_USE_CUNEICODE is defined, the ztd.cuneicode library may
		/// be used to fulfill this functionality.
		///
		/// @remarks To the best ability of the implementation, the iterators will be
		/// returned untouched (e.g., the input models at least a view and a forward_range). If it is not possible,
		/// returned ranges may be incremented even if an error occurs due to the semantics of any view that models an
		/// input_range.
		//////
		template <typename _InputRange, typename _OutputRange, typename _ErrorHandler>
		static constexpr auto decode_one(
			_InputRange&& __input, _OutputRange&& __output, _ErrorHandler&& __error_handler, decode_state& __s) {
			using _UErrorHandler = remove_cvref_t<_ErrorHandler>;
			using _Result = __txt_detail::__reconstruct_decode_result_t<_InputRange, _OutputRange, decode_state>;
			constexpr bool __call_error_handler = !is_ignorable_error_handler_v<_UErrorHandler>;
			using __u16e                        = __impl::__utf16_with<void, wchar_t, code_point, false>;
			using __intermediate_state          = decode_state_t<__u16e>;

			__u16e __u16enc {};
			__txt_detail::__progress_handler<!__call_error_handler, wide_execution> __intermediate_handler {};
			__intermediate_state __intermediate_s {};

			auto __result = __u16enc.decode_one(::std::forward<_InputRange>(__input),
				::std::forward<_OutputRange>(__output), __intermediate_handler, __intermediate_s);
			if constexpr (__call_error_handler) {
				if (__result.error_code != encoding_error::ok) {
					return __error_handler(wide_execution {},
						_Result(
						     ::std::move(__result.input), ::std::move(__result.output), __s, __result.error_code),
						::ztd::ranges::span<code_point>(__intermediate_handler._M_code_points.data(),
						     __intermediate_handler._M_code_points_size));
				}
			}
			return _Result(::std::move(__result.input), ::std::move(__result.output), __s, __result.error_code,
				__result.handled_errors);
		}
	};

	//////
	/// @}
	///
	//////



	ZTD_TEXT_INLINE_ABI_NAMESPACE_CLOSE_I_
}} // namespace ztd::text

#include <ztd/epilogue.hpp>

#endif

#endif // ZTD_TEXT_DETAIL_WIDE_EXECUTION_WINDOWS_HPP
