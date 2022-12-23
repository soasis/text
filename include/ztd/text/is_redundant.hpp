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

#ifndef ZTD_TEXT_IS_REDUNDANT_HPP
#define ZTD_TEXT_IS_REDUNDANT_HPP

#include <ztd/text/version.hpp>

#include <ztd/text/is_unicode_encoding.hpp>

#include <ztd/idk/type_traits.hpp>

#include <ztd/prologue.hpp>

namespace ztd { namespace text {
	ZTD_TEXT_INLINE_ABI_NAMESPACE_OPEN_I_

	namespace __txt_detail {
		template <typename _FromEncoding, typename _ToEncoding>
		inline constexpr bool __is_decode_same_as_encode_v
			= decoded_id_v<_FromEncoding> == encoded_id_v<_ToEncoding>         // cf
			&& decoded_id_v<_FromEncoding> == decoded_id_v<_ToEncoding>        // cf
			&& decoded_id_v<_FromEncoding> != ::ztd::text_encoding_id::unknown // cf
			&& encoded_id_v<_ToEncoding> != ::ztd::text_encoding_id::unknown;

		template <typename _FromEncoding, typename _ToEncoding>
		inline constexpr bool __is_already_decoded_v = encoded_id_v<_FromEncoding> == decoded_id_v<_ToEncoding> // cf
			&& decoded_id_v<_FromEncoding> != ::ztd::text_encoding_id::unknown                                 // cf
			&& encoded_id_v<_ToEncoding> != ::ztd::text_encoding_id::unknown;

		template <typename _FromEncoding, typename _ToEncoding>
		inline constexpr bool __is_encode_same_as_decode_v
			= encoded_id_v<_FromEncoding> == decoded_id_v<_ToEncoding>         // cf
			&& encoded_id_v<_FromEncoding> == encoded_id_v<_ToEncoding>        // cf
			&& encoded_id_v<_FromEncoding> != ::ztd::text_encoding_id::unknown // cf
			&& decoded_id_v<_ToEncoding> != ::ztd::text_encoding_id::unknown;

		template <typename _FromEncoding, typename _ToEncoding>
		inline constexpr bool __is_already_encoded_v = decoded_id_v<_FromEncoding> == encoded_id_v<_ToEncoding> // cf
			&& encoded_id_v<_FromEncoding> != ::ztd::text_encoding_id::unknown                                 // cf
			&& decoded_id_v<_ToEncoding> != ::ztd::text_encoding_id::unknown;
	} // namespace __txt_detail

	//////
	/// @addtogroup ztd_text_properties Property and Trait Helpers
	///
	/// @{

	//////
	/// @brief Whether or not the decode encoding is
	template <typename _From, typename _To>
	using is_decode_redundant = ::std::is_same<::ztd::remove_cvref_t<_From>, ::ztd::remove_cvref_t<_To>>;

	//////
	/// @brief An alias of the inner `value` for `ztd::text::is_decode_redundant<_From, _To>`.
	template <typename _From, typename _To>
	inline constexpr bool is_decode_redundant_v = is_decode_redundant<_From, _To>::value;

	//////
	/// @brief Whether or not the decode encoding is
	template <typename _From, typename _To>
	using is_encode_redundant = ::std::is_same<::ztd::remove_cvref_t<_From>, ::ztd::remove_cvref_t<_To>>;

	//////
	/// @brief An alias of the inner `value` for `ztd::text::is_encode_redundant<_From, _To>`.
	template <typename _From, typename _To>
	inline constexpr bool is_encode_redundant_v
		= is_encode_redundant<::ztd::remove_cvref_t<_From>, ::ztd::remove_cvref_t<_To>>::value;

	//////
	/// @}

	ZTD_TEXT_INLINE_ABI_NAMESPACE_CLOSE_I_
}} // namespace ztd::text

#include <ztd/epilogue.hpp>

#endif
