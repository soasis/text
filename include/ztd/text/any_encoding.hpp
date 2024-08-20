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

#ifndef ZTD_TEXT_ANY_ENCODING_HPP
#define ZTD_TEXT_ANY_ENCODING_HPP

#include <ztd/text/version.hpp>

#include <ztd/text/any_encoding_with.hpp>

#include <ztd/prologue.hpp>

namespace ztd { namespace text {

	ZTD_TEXT_INLINE_ABI_NAMESPACE_OPEN_I_

	//////
	/// @brief A type-erased encoding that uses the specified code unit, code point, and input/output ranges for the
	/// various operations.
	template <typename _EncodeCodeUnit, typename _EncodeCodePoint = const unicode_code_point,
		typename _DecodeCodeUnit     = ::std::add_const_t<_EncodeCodeUnit>,
		typename _DecodeCodePoint    = ::std::remove_const_t<_EncodeCodePoint>,
		::std::size_t _MaxCodeUnits  = __txt_detail::__default_max_code_units_any_encoding,
		::std::size_t _MaxCodePoints = __txt_detail::__default_max_code_points_any_encoding>
	using any_encoding_of = any_encoding_with<::ztd::span<_EncodeCodeUnit>, ::ztd::span<_EncodeCodePoint>,
		::ztd::span<_DecodeCodeUnit>, ::ztd::span<_DecodeCodePoint>, _MaxCodeUnits, _MaxCodePoints>;

	//////
	/// @brief An encoding type that wraps up other encodings to specifically traffic in the given `_Byte` type
	/// provided, which is typically set to `std::byte` .
	///
	/// @tparam _Byte The byte type to use. Typically, this is either <tt>unsigned char</tt> or `std::byte` .
	///
	/// @remarks This type traffics solely in `std::span` s, which for most people is fine. Others may want to
	/// interface with different iterator types (e.g., from a custom Rope implementation or other). For those, one must
	/// first create ranges that can operate with those iterators, then use them themselves. (It's not an ideal process
	/// at the moment, and we are looking to make this experience better.) It is recommended to use the provided
	/// ztd::text::any_encoding type definition instead of accessing this directly, unless you have a reason for using
	/// a different byte type (e.g., interfacing with legacy APIs).
	template <typename _Byte, typename _CodePoint = unicode_code_point>
	class any_byte_encoding : public any_encoding_of<_Byte, const _CodePoint, const _Byte, _CodePoint> {
	private:
		using __base_t = any_encoding_of<_Byte, const _CodePoint, const _Byte, _CodePoint>;

	public:
		//////
		/// @brief Cannot default-construct a ztd::text::any_byte_encoding object.
		any_byte_encoding() = delete;

		//////
		/// @brief Constructs a ztd::text::any_byte_encoding with the encoding object and any additional arguments.
		///
		/// @param[in] __encoding The encoding object that informs the ztd::text::any_byte_encoding what encoding
		/// object to store.
		/// @param[in] __args Any additional arguments used to construct the encoding in the erased storage.
		///
		/// @remarks If the provided encoding does not have a byte code_unit type, it is wrapped in an
		/// ztd::text::encoding_scheme first.
		template <typename _EncodingArg, typename... _Args,
			::std::enable_if_t<!::std::is_same_v<remove_cvref_t<_EncodingArg>, any_byte_encoding>               // cf
			     && !::std::is_same_v<__txt_detail::__code_unit_or_void_t<remove_cvref_t<_EncodingArg>>, _Byte> // cf
			     && !is_specialization_of_v<remove_cvref_t<_EncodingArg>, ::ztd::text::any_byte_encoding>       // cf
			     && !::std::is_same_v<remove_cvref_t<_EncodingArg>, __base_t>                                   // cf
			     && !is_specialization_of_v<remove_cvref_t<_EncodingArg>, ::std::in_place_type_t>>* = nullptr>
		any_byte_encoding(_EncodingArg&& __encoding, _Args&&... __args)
		: any_byte_encoding(::std::in_place_type<remove_cvref_t<_EncodingArg>>,
			  ::std::forward<_EncodingArg>(__encoding), ::std::forward<_Args>(__args)...) {
		}

		//////
		/// @brief Constructs a ztd::text::any_byte_encoding with the encoding object and any additional arguments.
		///
		/// @tparam _EncodingArg The Encoding specified by the `std::in_place_type<...>` argument.
		///
		/// @param[in] __args Any additional arguments used to construct the encoding in the erased storage.
		///
		/// @remarks If the provided encoding does not have a byte code_unit type, it is wrapped in an
		/// ztd::text::encoding_scheme first.
		template <typename _EncodingArg, typename... _Args,
			::std::enable_if_t<!::std::is_same_v<_Byte, code_unit_t<remove_cvref_t<_EncodingArg>>>>* = nullptr>
		any_byte_encoding(::std::in_place_type_t<_EncodingArg>, _Args&&... __args)
		: __base_t(::std::in_place_type_t<encoding_scheme<remove_cvref_t<_EncodingArg>, endian::native, _Byte>> {},
			  ::std::forward<_Args>(__args)...) {
		}

		//////
		/// @brief Constructs a ztd::text::any_byte_encoding with the encoding object and any additional arguments.
		///
		/// @tparam _EncodingArg The Encoding specified by the `std::in_place_type<...>` argument.
		///
		/// @param[in] __tag A tag containing the encoding type.
		/// @param[in] __args Any additional arguments used to construct the encoding in the erased storage.
		///
		/// @remarks If the provided encoding does not have a byte code_unit type, it is wrapped in an
		/// ztd::text::encoding_scheme first.
		template <typename _EncodingArg, typename... _Args,
			::std::enable_if_t<::std::is_same_v<_Byte, code_unit_t<remove_cvref_t<_EncodingArg>>>>* = nullptr>
		any_byte_encoding(::std::in_place_type_t<_EncodingArg> __tag, _Args&&... __args)
		: __base_t(::std::move(__tag), ::std::forward<_Args>(__args)...) {
		}

		//////
		/// @brief Cannot copy-construct a ztd::text::any_byte_encoding object.
		any_byte_encoding(const any_byte_encoding&) = delete;

		//////
		/// @brief Cannot copy-assign a ztd::text::any_byte_encoding object.
		any_byte_encoding& operator=(const any_byte_encoding&) = delete;

		//////
		/// @brief Move-constructs a ztd::text::any_byte_encoding from the provided r-value reference.
		///
		/// @remarks This leaves the passed-in r-value reference without an encoding object. Calling any function on a
		/// moved-fron ztd::text::any_byte_encoding, except for destruction, is a violation and invokes Undefined
		/// Behavior (generally, a crash).
		any_byte_encoding(any_byte_encoding&&) = default;

		//////
		/// @brief Move-assigns a ztd::text::any_byte_encoding from the provided r-value reference.
		///
		/// @remarks This leaves the passed-in r-value reference without an encoding object. Calling any function on a
		/// moved-fron ztd::text::any_byte_encoding, except for destruction, is a violation and invokes Undefined
		/// Behavior (generally, a crash).
		any_byte_encoding& operator=(any_byte_encoding&&) = default;
	};

	//////
	/// @brief The canonical erased encoding type which uses a `std::byte` as its code unit type and an @c
	/// unicode_code_point as its code point type, with spans for input and output operations.
	///
	/// @remarks If the input encoding does not match `std::byte`, it will be first wrapped in a
	/// ztd::text::encoding_scheme first.
	using any_encoding = any_byte_encoding<::std::byte>;

	//////
	/// @brief The canonical erased encoding type which uses a `char` as its code unit type and an @c
	/// unicode_code_point as its code point type, with spans for input and output operations.
	///
	/// @remarks If the input encoding does not match `char`, it will be first wrapped in a
	/// ztd::text::encoding_scheme first. Use this type when dealing with what are effectively byte stream inputs but
	/// oriented in a legacy manner, such as old `std::string` or `<iostream>`-based work.
	using compat_any_encoding = any_byte_encoding<char>;

	//////
	/// @brief The canonical erased encoding type which uses a `unsigned char` as its code unit type and an @c
	/// unicode_code_point as its code point type, with spans for input and output operations.
	///
	/// @remarks If the input encoding does not match `unsigned char`, it will be first wrapped in a
	/// ztd::text::encoding_scheme first. Use this type when dealing with what are effectively byte stream inputs but
	/// oriented around a slightly more modern approach to proper unsigned data handling with `unsigned char`.
	using ucompat_any_encoding = any_byte_encoding<unsigned char>;


	ZTD_TEXT_INLINE_ABI_NAMESPACE_CLOSE_I_
}} // namespace ztd::text

#include <ztd/epilogue.hpp>

#endif
