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

#ifndef ZTD_TEXT_ENCODING_SCHEME_HPP
#define ZTD_TEXT_ENCODING_SCHEME_HPP

#include <ztd/text/version.hpp>

#include <ztd/text/utf16.hpp>
#include <ztd/text/utf32.hpp>
#include <ztd/text/basic_encoding_scheme.hpp>

#include <ztd/prologue.hpp>

namespace ztd { namespace text {
	ZTD_TEXT_INLINE_ABI_NAMESPACE_OPEN_I_

	//////
	/// @addtogroup ztd_text_encodings Encodings
	///
	/// @{

	//////
	/// @brief A UTF-16 encoding, in Little Endian format, with inputs as a sequence of bytes.
	///
	/// @tparam _Byte The byte type to use. Typically, this is `std::byte` or `uchar.`
	template <typename _Byte>
	using basic_utf16_le = encoding_scheme<utf16_t, endian::little, _Byte>;

	//////
	/// @brief A UTF-16 encoding, in Big Endian format, with inputs as a sequence of bytes.
	///
	/// @tparam _Byte The byte type to use. Typically, this is `std::byte` or <tt>unsigned char</tt>.
	template <typename _Byte>
	using basic_utf16_be = encoding_scheme<utf16_t, endian::big, _Byte>;

	//////
	/// @brief A UTF-16 encoding, in Native Endian format, with inputs as a sequence of bytes.
	///
	/// @tparam _Byte The byte type to use. Typically, this is `std::byte` or <tt>unsigned char</tt>.
	template <typename _Byte>
	using basic_utf16_ne = encoding_scheme<utf16_t, endian::native, _Byte>;

	//////
	/// @brief A UTF-16 encoding, in Little Endian format, with inputs as a sequence of bytes.
	using utf16_le_t = basic_utf16_le<::std::byte>;

	//////
	/// @brief A UTF-16 encoding, in Big Endian format, with inputs as a sequence of bytes.
	using utf16_be_t = basic_utf16_be<::std::byte>;

	//////
	/// @brief A UTF-16 encoding, in Native Endian format, with inputs as a sequence of bytes.
	using utf16_ne_t = basic_utf16_ne<::std::byte>;

	//////
	/// @brief A UTF-32 encoding, in Little Endian format, with inputs as a sequence of bytes.
	///
	/// @tparam _Byte The byte type to use. Typically, this is `std::byte` or <tt>unsigned char</tt> .
	template <typename _Byte>
	using basic_utf32_le = encoding_scheme<utf32_t, endian::little, _Byte>;

	//////
	/// @brief A UTF-32 encoding, in Big Endian format, with inputs as a sequence of bytes.
	///
	/// @tparam _Byte The byte type to use. Typically, this is `std::byte` or <tt>unsigned char</tt> .
	template <typename _Byte>
	using basic_utf32_be = encoding_scheme<utf32_t, endian::big, _Byte>;

	//////
	/// @brief A UTF-32 encoding, in Native Endian format, with inputs as a sequence of bytes.
	///
	/// @tparam _Byte The byte type to use. Typically, this is `std::byte` or <tt>unsigned char</tt> .
	template <typename _Byte>
	using basic_utf32_ne = encoding_scheme<utf32_t, endian::native, _Byte>;

	//////
	/// @brief A UTF-32 encoding, in Little Endian format, with inputs as a sequence of bytes.
	using utf32_le_t = basic_utf32_le<::std::byte>;

	//////
	/// @brief A UTF-32 encoding, in Big Endian format, with inputs as a sequence of bytes.
	using utf32_be_t = basic_utf32_be<::std::byte>;

	//////
	/// @brief A UTF-32 encoding, in Big Endian format, with inputs as a sequence of bytes.
	using utf32_ne_t = basic_utf32_ne<::std::byte>;

	//////
	/// @brief An instance of the utf16_le_t type for ease of use.
	inline constexpr utf16_le_t utf16_le = {};

	//////
	/// @brief An instance of the utf16_be_t type for ease of use.
	inline constexpr utf16_be_t utf16_be = {};

	//////
	/// @brief An instance of the utf16_ne_t type for ease of use.
	inline constexpr utf16_ne_t utf16_ne = {};

	//////
	/// @brief An instance of the utf32_le_t type for ease of use.
	inline constexpr utf32_le_t utf32_le = {};

	//////
	/// @brief An instance of the utf32_ne_t type for ease of use.
	inline constexpr utf32_be_t utf32_be = {};

	//////
	/// @brief An instance of the utf32_ne_t type for ease of use.
	inline constexpr utf32_ne_t utf32_ne = {};

	//////
	/// @}

	ZTD_TEXT_INLINE_ABI_NAMESPACE_CLOSE_I_
}} // namespace ztd::text

#include <ztd/epilogue.hpp>

#endif // ZTD_TEXT_ENCODING_SCHEME_HPP
