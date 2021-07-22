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

#ifndef ZTD_TEXT_VERSION_HPP
#define ZTD_TEXT_VERSION_HPP

#include <ztd/version.hpp>

#define ZTD_TEXT_VERSION_MAJOR 0
#define ZTD_TEXT_VERSION_MINOR 0
#define ZTD_TEXT_VERSION_PATCH 0
#define ZTD_TEXT_VERSION_STRING "0.0.0"
#define ZTD_TEXT_VERSION \
	((ZTD_TEXT_VERSION_MAJOR * 100'000) + (ZTD_TEXT_VERSION_MINOR * 100) + (ZTD_TEXT_VERSION_PATCH))

#if ZTD_IS_ON(ZTD_STD_LIBRARY_TEXT_ENCODING_ID_I_)
#include <text_encoding_id>
#endif

// clang-format off

#if defined(ZTD_TEXT_ICONV)
	#if (ZTD_TEXT_ICONV != 0)
		#define ZTD_TEXT_ICONV_I_ ZTD_ON
	#else
		#define ZTD_TEXT_ICONV_I_ ZTD_OFF
	#endif
#else
	#define ZTD_TEXT_ICONV_I_ ZTD_DEFAULT_OFF
#endif

#if defined(ZTD_TEXT_CUNEICODE)
	#if (ZTD_TEXT_CUNEICODE != 0)
		#define ZTD_TEXT_CUNEICODE_I_ ZTD_ON
	#else
		#define ZTD_TEXT_CUNEICODE_I_ ZTD_OFF
	#endif
#else
	#define ZTD_TEXT_CUNEICODE_I_ ZTD_DEFAULT_OFF
#endif

#if defined(ZTD_TEXT_DEFAULT_HANDLER_THROWS)
	#if (ZTD_TEXT_DEFAULT_HANDLER_THROWS != 0)
		#define ZTD_TEXT_DEFAULT_HANDLER_THROWS_I_ ZTD_ON
	#else
		#define ZTD_TEXT_DEFAULT_HANDLER_THROWS_I_ ZTD_OFF
	#endif
#else
	#define ZTD_TEXT_DEFAULT_HANDLER_THROWS_I_ ZTD_DEFAULT_OFF
#endif

#if defined(ZTD_TEXT_INTERMEDIATE_TRANSCODE_BUFFER_BYTE_SIZE)
	#define ZTD_TEXT_INTERMEDIATE_TRANSCODE_BUFFER_BYTE_SIZE_I_ ZTD_TEXT_INTERMEDIATE_TRANSCODE_BUFFER_BYTE_SIZE
#else
	#define ZTD_TEXT_INTERMEDIATE_TRANSCODE_BUFFER_BYTE_SIZE_I_ ZTD_INTERMEDIATE_BUFFER_SUGGESTED_BYTE_SIZE_I_
#endif // Intermediate buffer sizing

#define ZTD_TEXT_INTERMEDIATE_TRANSCODE_BUFFER_SIZE_I_(...) (ZTD_TEXT_INTERMEDIATE_TRANSCODE_BUFFER_BYTE_SIZE_I_ / sizeof(__VA_ARGS__))

#if defined(ZTD_TEXT_COMPILE_TIME_ENCODING_NAME)
	#define ZTD_TEXT_COMPILE_TIME_ENCODING_NAME_GET_I_()         ZTD_TEXT_COMPILE_TIME_ENCODING_NAME
	#define ZTD_TEXT_COMPILE_TIME_ENCODING_NAME_DESCRIPTION_I_() "set by the user with ZTD_TEXT_COMPILE_TIME_ENCODING_NAME as (" ZTD_TOKEN_TO_STRING_I_(ZTD_TEXT_COMPILE_TIME_ENCODING_NAME) ")"
	#define ZTD_TEXT_COMPILE_TIME_ENCODING_NAME_I_               ZTD_DEFAULT_ON
#elif ZTD_IS_ON(ZTD_STD_LIBRARY_TEXT_ENCODING_ID_I_)
	#define ZTD_TEXT_COMPILE_TIME_ENCODING_NAME_GET_I_()         ::std::text_encoding::literal().name()
	#define ZTD_TEXT_COMPILE_TIME_ENCODING_NAME_DESCRIPTION_I_() "from std::text_encoding::literal().name()"
	#define ZTD_TEXT_COMPILE_TIME_ENCODING_NAME                  ZTD_DEFAULT_ON
#elif defined(__MSC_EXECUTION_CHARSET_ID)
	#define ZTD_TEXT_COMPILE_TIME_ENCODING_NAME_GET_I_()         ::ztd::text::__txt_detail::__msvc_encoding_name_from_id(__MSC_EXECUTION_CHARSET_ID)
	#define ZTD_TEXT_COMPILE_TIME_ENCODING_NAME_DESCRIPTION_I_() "MSVC CodePage ID: " ZTD_TOKEN_TO_STRING_I_(__MSC_EXECUTION_CHARSET_ID)
	#define ZTD_TEXT_COMPILE_TIME_ENCODING_NAME_I_               ZTD_DEFAULT_ON
#elif defined(__GNUC_EXECUTION_CHARSET_NAME)
	#define ZTD_TEXT_COMPILE_TIME_ENCODING_NAME_GET_I_()         __GNUC_EXECUTION_CHARSET_NAME
	#define ZTD_TEXT_COMPILE_TIME_ENCODING_NAME_DESCRIPTION_I_() __GNUC_EXECUTION_CHARSET_NAME
	#define ZTD_TEXT_COMPILE_TIME_ENCODING_NAME_I_               ZTD_DEFAULT_ON
#elif defined(__clang_literal_encoding__)
	#define ZTD_TEXT_COMPILE_TIME_ENCODING_NAME_GET_I_()         __clang_literal_encoding__
	#define ZTD_TEXT_COMPILE_TIME_ENCODING_NAME_DESCRIPTION_I_() __clang_literal_encoding__
	#define ZTD_TEXT_COMPILE_TIME_ENCODING_NAME_I_               ZTD_DEFAULT_ON
#elif ZTD_IS_ON(ZTD_COMPILER_CLANG_I_)
	#define ZTD_TEXT_COMPILE_TIME_ENCODING_NAME_GET_I_()         "UTF-8"
	#define ZTD_TEXT_COMPILE_TIME_ENCODING_NAME_DESCRIPTION_I_() "UTF-8"
	#define ZTD_TEXT_COMPILE_TIME_ENCODING_NAME_I_               ZTD_DEFAULT_ON
#else
	#define ZTD_TEXT_COMPILE_TIME_ENCODING_NAME_GET_I_()         "UTF-8"
	#define ZTD_TEXT_COMPILE_TIME_ENCODING_NAME_DESCRIPTION_I_() "UTF-8"
	#define ZTD_TEXT_COMPILE_TIME_ENCODING_NAME_I_               ZTD_DEFAULT_OFF
#endif

#if defined(ZTD_TEXT_COMPILE_TIME_WIDE_ENCODING_NAME)
	#define ZTD_TEXT_COMPILE_TIME_WIDE_ENCODING_NAME_GET_I_()         ZTD_TEXT_COMPILE_TIME_WIDE_ENCODING_NAME
	#define ZTD_TEXT_COMPILE_TIME_WIDE_ENCODING_NAME_DESCRIPTION_I_() "set by the user with ZTD_TEXT_COMPILE_TIME_ENCODING_NAME as (" ZTD_TOKEN_TO_STRING_I_(ZTD_TEXT_COMPILE_TIME_WIDE_ENCODING_NAME) ")"
	#define ZTD_TEXT_COMPILE_TIME_WIDE_ENCODING_NAME_I_               ZTD_ON
#elif ZTD_IS_ON(ZTD_STD_LIBRARY_TEXT_ENCODING_ID_I_)
	#define ZTD_TEXT_COMPILE_TIME_WIDE_ENCODING_NAME_GET_I_()         ::std::text_encoding::wide_literal().name()
	#define ZTD_TEXT_COMPILE_TIME_WIDE_ENCODING_NAME_DESCRIPTION_I_() "from ::std::text_encoding::wide_literal().name()"
	#define ZTD_TEXT_COMPILE_TIME_WIDE_ENCODING_NAME_I_               ZTD_DEFAULT_ON
#elif defined(__GNUC_WIDE_EXECUTION_CHARSET_NAME)
	#define ZTD_TEXT_COMPILE_TIME_WIDE_ENCODING_NAME_GET_I_()         __GNUC_WIDE_EXECUTION_CHARSET_NAME
	#define ZTD_TEXT_COMPILE_TIME_WIDE_ENCODING_NAME_DESCRIPTION_I_() ZTD_TEXT_COMPILE_TIME_WIDE_ENCODING_NAME_GET_I_()
	#define ZTD_TEXT_COMPILE_TIME_WIDE_ENCODING_NAME_I_               ZTD_DEFAULT_ON
#elif defined(__clang_wide_literal_encoding__)
	#define ZTD_TEXT_COMPILE_TIME_WIDE_ENCODING_NAME_GET_I_()         __clang_wide_literal_encoding__
	#define ZTD_TEXT_COMPILE_TIME_WIDE_ENCODING_NAME_DESCRIPTION_I_() ZTD_TEXT_COMPILE_TIME_WIDE_ENCODING_NAME_GET_I_()
	#define ZTD_TEXT_COMPILE_TIME_WIDE_ENCODING_NAME_I_               ZTD_DEFAULT_ON
#elif ZTD_IS_ON(ZTD_PLATFORM_WINDOWS_I_)
	#define ZTD_TEXT_COMPILE_TIME_WIDE_ENCODING_NAME_GET_I_()         "UTF-16"
	#define ZTD_TEXT_COMPILE_TIME_WIDE_ENCODING_NAME_DESCRIPTION_I_() "UTF-16"
	#define ZTD_TEXT_COMPILE_TIME_WIDE_ENCODING_NAME_I_               ZTD_DEFAULT_ON
#elif (WCHAR_MAX >= 0x001FFFFF) && ZTD_IS_ON(ZTD_PLATFORM_MAC_OS_I_)
	#define ZTD_TEXT_COMPILE_TIME_WIDE_ENCODING_NAME_GET_I_()         "UTF-32"
	#define ZTD_TEXT_COMPILE_TIME_WIDE_ENCODING_NAME_DESCRIPTION_I_() "UTF-32"
	#define ZTD_TEXT_COMPILE_TIME_WIDE_ENCODING_NAME_I_               ZTD_DEFAULT_ON
#elif (WCHAR_MAX >= 0x001FFFFF) && ZTD_IS_ON(ZTD_WCHAR_T_UTF32_COMPATIBLE_I_)
	#define ZTD_TEXT_COMPILE_TIME_WIDE_ENCODING_NAME_GET_I_()         "UTF-32"
	#define ZTD_TEXT_COMPILE_TIME_WIDE_ENCODING_NAME_DESCRIPTION_I_() "UTF-32"
	#define ZTD_TEXT_COMPILE_TIME_WIDE_ENCODING_NAME_I_               ZTD_DEFAULT_ON
#elif (WCHAR_MAX >= 0xFFFF)
	#define ZTD_TEXT_COMPILE_TIME_WIDE_ENCODING_NAME_GET_I_()         "UTF-16"
	#define ZTD_TEXT_COMPILE_TIME_WIDE_ENCODING_NAME_DESCRIPTION_I_() "UTF-16"
	#define ZTD_TEXT_COMPILE_TIME_WIDE_ENCODING_NAME_I_               ZTD_DEFAULT_ON
#elif (WCHAR_MAX >= 0xFF)
	#define ZTD_TEXT_COMPILE_TIME_WIDE_ENCODING_NAME_GET_I_()         "UTF-8"
	#define ZTD_TEXT_COMPILE_TIME_WIDE_ENCODING_NAME_DESCRIPTION_I_() "UTF-8"
	#define ZTD_TEXT_COMPILE_TIME_WIDE_ENCODING_NAME_I_               ZTD_DEFAULT_ON
#else
	#define ZTD_TEXT_COMPILE_TIME_WIDE_ENCODING_NAME_GET_I_()         "UTF-32"
	#define ZTD_TEXT_COMPILE_TIME_WIDE_ENCODING_NAME_DESCRIPTION_I_() "UTF-32"
	#define ZTD_TEXT_COMPILE_TIME_WIDE_ENCODING_NAME_I_               ZTD_DEFAULT_OFF
#endif

#if defined(ZTD_TEXT_YES_PLEASE_DESTROY_MY_LITERALS_UTTERLY_I_MEAN_IT)
	#if (ZTD_TEXT_YES_PLEASE_DESTROY_MY_LITERALS_UTTERLY_I_MEAN_IT != 0)
		#define ZTD_TEXT_YES_PLEASE_DESTROY_MY_LITERALS_UTTERLY_I_MEAN_IT_I_ ZTD_ON
	#else
		#define ZTD_TEXT_YES_PLEASE_DESTROY_MY_LITERALS_UTTERLY_I_MEAN_IT_I_ ZTD_OFF
	#endif
#else
	#define ZTD_TEXT_YES_PLEASE_DESTROY_MY_LITERALS_UTTERLY_I_MEAN_IT_I_ ZTD_DEFAULT_OFF
#endif

#if defined(ZTD_TEXT_YES_PLEASE_DESTROY_MY_WIDE_LITERALS_UTTERLY_I_MEAN_IT)
	#if (ZTD_TEXT_YES_PLEASE_DESTROY_MY_WIDE_LITERALS_UTTERLY_I_MEAN_IT != 0)
		#define ZTD_TEXT_YES_PLEASE_DESTROY_MY_WIDE_LITERALS_UTTERLY_I_MEAN_IT_I_ ZTD_ON
	#else
		#define ZTD_TEXT_YES_PLEASE_DESTROY_MY_WIDE_LITERALS_UTTERLY_I_MEAN_IT_I_ ZTD_OFF
	#endif
#else
	#define ZTD_TEXT_YES_PLEASE_DESTROY_MY_WIDE_LITERALS_UTTERLY_I_MEAN_IT_I_ ZTD_DEFAULT_OFF
#endif

#if defined (ZTD_TEXT_UNICODE_CODE_POINT_DISTINCT_TYPE)
	#if (ZTD_TEXT_UNICODE_CODE_POINT_DISTINCT_TYPE != 0)
		#define ZTD_TEXT_UNICODE_CODE_POINT_DISTINCT_TYPE_I_ ZTD_ON
	#else
		#define ZTD_TEXT_UNICODE_CODE_POINT_DISTINCT_TYPE_I_ ZTD_OFF
	#endif
#else
	#define ZTD_TEXT_UNICODE_CODE_POINT_DISTINCT_TYPE_I_ ZTD_DEFAULT_OFF
#endif

#if defined(ZTD_TEXT_UNICODE_CODE_POINT_INVARIANT_ABORT)
	#if (ZTD_TEXT_UNICODE_CODE_POINT_INVARIANT_ABORT != 0)
		#define ZTD_TEXT_UNICODE_CODE_POINT_INVARIANT_ABORT_I_ ZTD_ON
	#else
		#define ZTD_TEXT_UNICODE_CODE_POINT_INVARIANT_ABORT_I_ ZTD_OFF
	#endif
#else
	#define ZTD_TEXT_UNICODE_CODE_POINT_INVARIANT_ABORT_I_ ZTD_OFF
#endif

#if defined (ZTD_TEXT_UNICODE_SCALAR_VALUE_DISTINCT_TYPE)
	#if (ZTD_TEXT_UNICODE_CODE_POINT_DISTINCT_TYPE != 0)
		#define ZTD_TEXT_UNICODE_SCALAR_VALUE_DISTINCT_TYPE_I_ ZTD_ON
	#else
		#define ZTD_TEXT_UNICODE_SCALAR_VALUE_DISTINCT_TYPE_I_ ZTD_OFF
	#endif
#else
	#define ZTD_TEXT_UNICODE_SCALAR_VALUE_DISTINCT_TYPE_I_ ZTD_DEFAULT_ON
#endif

#if defined(ZTD_TEXT_UNICODE_SCALAR_VALUE_INVARIANT_ABORT)
	#if (ZTD_TEXT_UNICODE_SCALAR_VALUE_INVARIANT_ABORT != 0)
		#define ZTD_TEXT_UNICODE_SCALAR_VALUE_INVARIANT_ABORT_I_ ZTD_ON
	#else
		#define ZTD_TEXT_UNICODE_SCALAR_VALUE_INVARIANT_ABORT_I_ ZTD_OFF
	#endif
#else
	#define ZTD_TEXT_UNICODE_SCALAR_VALUE_INVARIANT_ABORT_I_ ZTD_OFF
#endif

#if defined(ZTD_TEXT_ABI_NAMESPACE)
	#define ZTD_TEXT_INLINE_ABI_NAMESPACE_OPEN_I_ inline namespace ZTD_TEXT_ABI_NAMESPACE {
	#define ZTD_TEXT_INLINE_ABI_NAMESPACE_CLOSE_I_ }
#else
	#define ZTD_TEXT_INLINE_ABI_NAMESPACE_OPEN_I_ inline namespace __v0 {
	#define ZTD_TEXT_INLINE_ABI_NAMESPACE_CLOSE_I_ }
#endif

#include <ztd/prologue.hpp>
#include <ztd/epilogue.hpp>

#endif // ZTD_TEXT_VERSION_HPP
