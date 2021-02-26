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

#ifndef ZTD_TEXT_DETAIL_ENCODING_RANGE_HPP
#define ZTD_TEXT_DETAIL_ENCODING_RANGE_HPP

#include <ztd/text/version.hpp>

#include <ztd/text/state.hpp>
#include <ztd/text/code_point.hpp>
#include <ztd/text/code_unit.hpp>

#include <ztd/text/detail/adl.hpp>
#include <ztd/text/detail/type_traits.hpp>

#include <utility>

namespace ztd { namespace text {
	ZTD_TEXT_INLINE_ABI_NAMESPACE_OPEN_I_
	namespace __detail {

		// validation: code units
		template <typename _Input, typename _Encoding, typename _State>
		using __detect_adl_internal_text_validate_code_units = decltype(__text_validate_code_units(
			::std::declval<_Input>(), ::std::declval<_Encoding>(), ::std::declval<_State&>()));

		template <typename _Input, typename _Encoding, typename _State>
		using __detect_adl_text_validate_code_units = decltype(text_validate_code_units(
			::std::declval<_Input>(), ::std::declval<_Encoding>(), ::std::declval<_State&>()));

		template <typename _Encoding, typename _Input, typename _State>
		using __detect_object_validate_code_units_one = decltype(::std::declval<_Encoding>().validate_code_units_one(
			::std::declval<_Input>(), ::std::declval<_State&>()));

		// validation: code points
		template <typename _Input, typename _Encoding, typename _State>
		using __detect_adl_internal_text_validate_code_points = decltype(__text_validate_code_points(
			::std::declval<_Input>(), ::std::declval<_Encoding>(), ::std::declval<_State&>()));

		template <typename _Input, typename _Encoding, typename _State>
		using __detect_adl_text_validate_code_points = decltype(text_validate_code_points(
			::std::declval<_Input>(), ::std::declval<_Encoding>(), ::std::declval<_State&>()));

		template <typename _Encoding, typename _Input, typename _State>
		using __detect_object_validate_code_points_one
			= decltype(::std::declval<_Encoding>().validate_code_points_one(
			     ::std::declval<_Input>(), ::std::declval<_State&>()));

		// counting: decode
		template <typename _Encoding, typename _Input, typename _Handler, typename _State>
		using __detect_object_count_code_units_one = decltype(::std::declval<_Encoding>().count_code_units_one(
			::std::declval<_Input>(), ::std::declval<_Handler>(), ::std::declval<_State&>()));

		template <typename _Input, typename _Encoding, typename _Handler, typename _State>
		using __detect_adl_text_count_code_units = decltype(text_count_code_units(::std::declval<_Input>(),
			::std::declval<_Encoding>(), ::std::declval<_Handler>(), ::std::declval<_State&>()));

		template <typename _Input, typename _Encoding, typename _Handler, typename _State>
		using __detect_adl_internal_text_count_code_units = decltype(__text_count_code_units(::std::declval<_Input>(),
			::std::declval<_Encoding>(), ::std::declval<_Handler>(), ::std::declval<_State&>()));

		// counting: encode
		template <typename _Encoding, typename _Input, typename _Handler, typename _State>
		using __detect_object_count_code_points_one = decltype(::std::declval<_Encoding>().count_code_points_one(
			::std::declval<_Input>(), ::std::declval<_Handler>(), ::std::declval<_State&>()));

		template <typename _Input, typename _Encoding, typename _Handler, typename _State>
		using __detect_adl_text_count_code_points = decltype(text_count_code_points(::std::declval<_Input>(),
			::std::declval<_Encoding>(), ::std::declval<_Handler>(), ::std::declval<_State&>()));

		template <typename _Input, typename _Encoding, typename _Handler, typename _State>
		using __detect_adl_internal_text_count_code_points
			= decltype(__text_count_code_points(::std::declval<_Input>(), ::std::declval<_Encoding>(),
			     ::std::declval<_Handler>(), ::std::declval<_State&>()));


		// decode
		template <typename _Encoding, typename _Input, typename _Output, typename _Handler, typename _State>
		using __detect_object_decode_one = decltype(::std::declval<_Encoding>().decode_one(::std::declval<_Input>(),
			::std::declval<_Output>(), ::std::declval<_Handler>(), ::std::declval<_State&>()));

		template <typename _Encoding, typename _Input, typename _Output, typename _Handler, typename _State>
		using __detect_object_decode_one_backwards
			= decltype(::std::declval<_Encoding>().decode_one_backwards(::std::declval<_Input>(),
			     ::std::declval<_Output>(), ::std::declval<_Handler>(), ::std::declval<_State&>()));

		template <typename _Input, typename _Encoding, typename _Output, typename _Handler, typename _State>
		using __detect_adl_text_decode = decltype(text_decode(::std::declval<_Input>(), ::std::declval<_Encoding>(),
			::std::declval<_Output>(), ::std::declval<_Handler>(), ::std::declval<_State&>()));

		template <typename _Input, typename _Encoding, typename _Output, typename _Handler, typename _State>
		using __detect_adl_internal_text_decode
			= decltype(__text_decode(::std::declval<_Input>(), ::std::declval<_Encoding>(),
			     ::std::declval<_Output>(), ::std::declval<_Handler>(), ::std::declval<_State&>()));

		// encode
		template <typename _Encoding, typename _Input, typename _Output, typename _Handler, typename _State>
		using __detect_object_encode_one = decltype(::std::declval<_Encoding>().encode_one(::std::declval<_Input>(),
			::std::declval<_Output>(), ::std::declval<_Handler>(), ::std::declval<_State&>()));

		template <typename _Encoding, typename _Input, typename _Output, typename _Handler, typename _State>
		using __detect_object_encode_one_backwards
			= decltype(::std::declval<_Encoding>().encode_one_backwards(::std::declval<_Input>(),
			     ::std::declval<_Output>(), ::std::declval<_Handler>(), ::std::declval<_State&>()));

		template <typename _Input, typename _Encoding, typename _Output, typename _Handler, typename _State>
		using __detect_adl_text_encode = decltype(text_encode(::std::declval<_Input>(), ::std::declval<_Encoding>(),
			::std::declval<_Output>(), ::std::declval<_Handler>(), ::std::declval<_State&>()));

		template <typename _Input, typename _Encoding, typename _Output, typename _Handler, typename _State>
		using __detect_adl_internal_text_encode
			= decltype(__text_encode(::std::declval<_Input>(), ::std::declval<_Encoding>(),
			     ::std::declval<_Output>(), ::std::declval<_Handler>(), ::std::declval<_State&>()));

		// transcode
		template <typename _Input, typename _FromEncoding, typename _Output, typename _ToEncoding,
			typename _FromHandler, typename _ToHandler, typename _FromState, typename _ToState>
		using __detect_adl_text_transcode
			= decltype(text_transcode(::std::declval<_Input>(), ::std::declval<_FromEncoding>(),
			     ::std::declval<_Output>(), ::std::declval<_ToEncoding>(), ::std::declval<_FromHandler>(),
			     ::std::declval<_ToHandler>(), ::std::declval<_FromState&>(), ::std::declval<_ToState&>()));

		template <typename _Input, typename _FromEncoding, typename _Output, typename _ToEncoding,
			typename _FromHandler, typename _ToHandler, typename _FromState, typename _ToState>
		using __detect_adl_internal_text_transcode
			= decltype(__text_transcode(::std::declval<_Input>(), ::std::declval<_FromEncoding>(),
			     ::std::declval<_Output>(), ::std::declval<_ToEncoding>(), ::std::declval<_FromHandler>(),
			     ::std::declval<_ToHandler>(), ::std::declval<_FromState&>(), ::std::declval<_ToState&>()));

		template <typename _Input, typename _FromEncoding, typename _Output, typename _ToEncoding,
			typename _FromHandler, typename _ToHandler, typename _FromState, typename _ToState>
		using __detect_adl_internal_text_transcode_one
			= decltype(__text_transcode_one(::std::declval<_Input>(), ::std::declval<_FromEncoding>(),
			     ::std::declval<_Output>(), ::std::declval<_ToEncoding>(), ::std::declval<_FromHandler>(),
			     ::std::declval<_ToHandler>(), ::std::declval<_FromState&>(), ::std::declval<_ToState&>()));

		template <typename _Input, typename _FromEncoding, typename _Output, typename _ToEncoding,
			typename _FromHandler, typename _ToHandler, typename _FromState, typename _ToState>
		using __detect_adl_text_transcode_one
			= decltype(text_transcode_one(::std::declval<_Input>(), ::std::declval<_FromEncoding>(),
			     ::std::declval<_Output>(), ::std::declval<_ToEncoding>(), ::std::declval<_FromHandler>(),
			     ::std::declval<_ToHandler>(), ::std::declval<_FromState&>(), ::std::declval<_ToState&>()));

		template <typename _Handler, typename _Encoding, typename _Result, typename _Progress>
		using __detect_callable_handler = decltype(::std::declval<_Handler>()(
			::std::declval<const _Encoding&>(), ::std::declval<_Result>(), ::std::declval<_Progress>()));

		template <typename _Encoding, typename _CodeUnits, typename _CodePoints, typename _Handler, typename _State>
		inline constexpr bool __is_decode_encoding_for_v
			= __is_detected_v<__detect_object_decode_one, _Encoding, _CodeUnits, _CodePoints, _Handler, _State>;

		template <typename _Encoding, typename _CodeUnits, typename _CodePoints, typename _Handler, typename _State>
		inline constexpr bool __is_encode_encoding_for_v
			= __is_detected_v<__detect_object_encode_one, _Encoding, _CodePoints, _CodeUnits, _Handler, _State>;

		template <typename _Encoding, typename _CodeUnits, typename _CodePoints, typename _Handler, typename _State>
		inline constexpr bool __is_encoding_for_v
			= __is_decode_encoding_for_v<_Encoding, _CodeUnits, _CodePoints, _Handler, _State>&&
			     __is_encode_encoding_for_v<_Encoding, _CodePoints, _CodeUnits, _Handler, _State>;

		template <typename _Encoding, typename _CodeUnits, typename _CodePoints, typename _Handler, typename _State>
		inline constexpr bool __is_counting_decode_encoding_for_v
			= __is_detected_v<__detect_object_count_code_units_one, _Encoding, _CodeUnits, _CodePoints, _Handler,
			     _State>;

		template <typename _Encoding, typename _CodeUnits, typename _CodePoints, typename _Handler, typename _State>
		inline constexpr bool __is_counting_encode_encoding_for_v
			= __is_detected_v<__detect_object_count_code_points_one, _Encoding, _CodePoints, _CodeUnits, _Handler,
			     _State>;

		template <typename _Encoding, typename _CodeUnits, typename _CodePoints, typename _Handler, typename _State>
		inline constexpr bool __is_counting_encoding_for_v
			= __is_counting_decode_encoding_for_v<_Encoding, _CodeUnits, _CodePoints, _Handler, _State>&&
			     __is_counting_encode_encoding_for_v<_Encoding, _CodeUnits, _CodePoints, _Handler, _State>;

		template <typename _Encoding, typename _CodeUnits, typename _Handler, typename _State>
		inline constexpr bool __is_validating_code_units_encoding_for_v
			= __is_detected_v<__detect_object_validate_code_units_one, _Encoding, _CodeUnits, _State>;

		template <typename _Encoding, typename _CodePoints, typename _Handler, typename _State>
		inline constexpr bool __is_validating_code_points_encoding_for_v
			= __is_detected_v<__detect_object_validate_code_points_one, _Encoding, _CodePoints, _State>;

		template <typename _Encoding, typename _CodeUnits, typename _CodePoints, typename _Handler, typename _State>
		inline constexpr bool __is_validating_encoding_for_v
			= __is_validating_code_units_encoding_for_v<_Encoding, _CodeUnits, _State>&&
			     __is_validating_code_points_encoding_for_v<_Encoding, _CodePoints, _State>;

		template <typename _Encoding, typename _CodeUnits, typename _CodePoints, typename _Handler, typename _State>
		inline constexpr bool __is_decode_backwards_encoding_for_v
			= __is_decode_encoding_for_v<_Encoding, _CodeUnits, _CodePoints, _Handler, _State>&& __is_detected_v<
			     __detect_object_decode_one_backwards, _Encoding, _CodeUnits, _CodePoints, _Handler, _State>;

		template <typename _Encoding, typename _CodeUnits, typename _CodePoints, typename _Handler, typename _State>
		inline constexpr bool __is_encode_backwards_encoding_for_v
			= __is_encode_encoding_for_v<_Encoding, _CodeUnits, _CodePoints, _Handler, _State>&& __is_detected_v<
			     __detect_object_encode_one_backwards, _Encoding, _CodePoints, _CodeUnits, _Handler, _State>;

		template <typename _Encoding, typename _CodeUnits, typename _CodePoints, typename _Handler, typename _State>
		inline constexpr bool __is_backwards_encoding_for_v
			= __is_encode_backwards_encoding_for_v<_Encoding, _CodeUnits, _CodePoints, _Handler, _State>&&
			     __is_decode_backwards_encoding_for_v<_Encoding, _CodeUnits, _CodePoints, _Handler, _State>;

	} // namespace __detail
	ZTD_TEXT_INLINE_ABI_NAMESPACE_CLOSE_I_
}} // namespace ztd::text

#endif // ZTD_TEXT_DETAIL_ENCODING_RANGE_HPP
