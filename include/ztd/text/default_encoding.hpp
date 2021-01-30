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
// =============================================================================

#pragma once

#ifndef ZTD_TEXT_DEFAULT_ENCODING_HPP
#define ZTD_TEXT_DEFAULT_ENCODING_HPP

#include <ztd/text/version.hpp>

#include <ztd/text/execution.hpp>
#include <ztd/text/wide_execution.hpp>
#include <ztd/text/utf8.hpp>
#include <ztd/text/utf16.hpp>
#include <ztd/text/utf32.hpp>
#include <ztd/text/encoding_scheme.hpp>

#include <ztd/text/unicode_code_point.hpp>
#include <ztd/text/unicode_scalar_value.hpp>

#include <ztd/text/detail/type_traits.hpp>

#include <cstdint>

namespace ztd { namespace text {
	ZTD_TEXT_INLINE_ABI_NAMESPACE_OPEN_I_

	//////
	/// @addtogroup ztd_text_properties Property and Trait Helpers
	/// @{
	/////

	//////
	/// @brief The default encoding associated with a given code unit type, that serves as either input to a decode
	/// operation or output from an encode operation.
	//////
	template <typename _Type>
	class default_code_unit_encoding {
		static_assert(__detail::__always_false_v<_Type>, "there is no default encoding for this type");
	};

	//////
	/// @brief The default encoding associated with a given code unit type, that serves as either input to a decode
	/// operation or output from an encode operation.
	//////
	template <>
	class default_code_unit_encoding<char> {
	public:
		//////
		/// @brief For @c char, @ref ztd::text::execution is assumed to be the default appropriate encoding.
		///
		//////
		using type = execution;
	};

	//////
	/// @copydoc ztd::text::default_code_unit_encoding
	///
	//////
	template <>
	class default_code_unit_encoding<wchar_t> {
	public:
		//////
		/// @brief For @c wchar_t, ztd::text::wide_execution is assumed to be the default appropriate encoding.
		///
		//////
		using type = wide_execution;
	};

	//////
	/// @copydoc ztd::text::default_code_unit_encoding
	///
	//////
	template <>
	class default_code_unit_encoding<char8_t> {
	public:
		//////
		/// @brief For @c char8_t, ztd::text::utf8 is assumed to be the default appropriate encoding.
		///
		//////
		using type = utf8;
	};

	//////
	/// @copydoc ztd::text::default_code_unit_encoding
	///
	//////
	template <>
	class default_code_unit_encoding<char16_t> {
	public:
		//////
		/// @brief For @c char16_t, ztd::text::utf16 is assumed to be the default appropriate encoding.
		///
		//////
		using type = utf16;
	};

	//////
	/// @copydoc ztd::text::default_code_unit_encoding
	///
	//////
	template <>
	class default_code_unit_encoding<char32_t> {
	public:
		//////
		/// @brief For @c char32_t, ztd::text::utf32 is assumed to be the default appropriate encoding.
		///
		//////
		using type = utf32;
	};

	//////
	/// @copydoc ztd::text::default_code_unit_encoding
	///
	//////
	template <>
	class default_code_unit_encoding<::std::byte> {
	public:
		//////
		/// @brief For ``std::byte``, a native-endian utf8 ztd::text::encoding_scheme is assumed to be the default
		/// appropriate encoding.
		//////
		using type = encoding_scheme<utf8, endian::native, ::std::byte>;
	};

	//////
	/// @brief A @c typename alias for ztd::text::default_code_unit_encoding.
	///
	//////
	template <typename _Type>
	using default_code_unit_encoding_t = typename default_code_unit_encoding<_Type>::type;

	//////
	/// @brief The default encoding associated with a given code point type, that serves as either input to an encode
	/// operation or output from decode operation.
	//////
	template <typename _Type>
	class default_code_point_encoding {
		static_assert(__detail::__always_false_v<_Type>, "there is no default encoding for this code_point type");
	};

	//////
	/// @copydoc ztd::text::default_code_point_encoding
	///
	//////
	template <>
	class default_code_point_encoding<char32_t> {
	public:
		//////
		/// @brief For @c char32_t, ztd::text::utf8 encoding is assumed to be the default appropriate encoding.
		///
		//////
		using type = utf8;
	};

	//////
	/// @copydoc ztd::text::default_code_point_encoding
	///
	//////
	template <>
	class default_code_point_encoding<uint_least32_t> {
	public:
		//////
		/// @brief For @c char32_t, ztd::text::utf8 encoding is assumed to be the default appropriate encoding.
		///
		//////
		using type = utf8;
	};

#if ZTD_TEXT_IS_ON(ZTD_TEXT_UNICODE_CODE_POINT_DISTINCT_TYPE_I_)
	//////
	/// @copydoc ztd::text::default_code_point_encoding
	///
	//////
	template <>
	class default_code_point_encoding<unicode_code_point> {
	public:
		//////
		/// @brief For @c unicode_code_point, ztd::text::utf8 encoding is assumed to be the default appropriate
		/// encoding.
		//////
		using type = utf8;
	};
#endif

#if ZTD_TEXT_IS_ON(ZTD_TEXT_UNICODE_SCALAR_VALUE_DISTINCT_TYPE_I_)
	//////
	/// @copydoc ztd::text::default_code_point_encoding
	///
	//////
	template <>
	class default_code_point_encoding<unicode_scalar_value> {
	public:
		//////
		/// @brief For @c unicode_scalar_value, ztd::text::utf8 encoding is assumed to be the default
		/// appropriate encoding.
		//////
		using type = utf8;
	};
#endif

	//////
	/// @brief A @c typename alias for ztd::text::default_code_point_encoding.
	///
	//////
	template <typename _Type>
	using default_code_point_encoding_t = typename default_code_unit_encoding<_Type>::type;

	//////
	/// @}
	/////

	ZTD_TEXT_INLINE_ABI_NAMESPACE_CLOSE_I_
}} // namespace ztd::text

#endif // ZTD_TEXT_DEFAULT_ENCODING_HPP
