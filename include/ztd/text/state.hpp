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

#ifndef ZTD_TEXT_STATE_HPP
#define ZTD_TEXT_STATE_HPP

#include <ztd/text/version.hpp>

#include <ztd/text/detail/type_traits.hpp>

namespace ztd { namespace text {
	ZTD_TEXT_INLINE_ABI_NAMESPACE_OPEN_I_

	namespace __txt_detail {
		template <typename _Type, typename = void>
		struct __decode_state {
			using type = typename _Type::state;
		};

		template <typename _Type>
		struct __decode_state<_Type, std::void_t<typename _Type::decode_state>> {
			using type = typename _Type::decode_state;
		};

		template <typename _Type, typename = void>
		struct __encode_state {
			using type = typename _Type::state;
		};

		template <typename _Type>
		struct __encode_state<_Type, std::void_t<typename _Type::encode_state>> {
			using type = typename _Type::encode_state;
		};
	} // namespace __txt_detail

	//////
	/// @addtogroup ztd_text_state Encoding State Management
	///
	/// @brief These allow a person to inspect the state of an encoding type, and create them when necessary. It is
	/// important to use these properties and functions in generic code, lest you make assumptions that will break down
	/// in the future or in the face of more advanced encoding objects.
	///
	/// @{
	/////

	//////
	/// @brief Retrieves the @c decode_state of the encoding type if it has one, or the @c state type of the
	/// encoding.
	//////
	template <typename _Type>
	class decode_state {
	public:
		//////
		/// @brief The @c decode_state type or @c state type on a given encoding type.
		///
		//////
		using type = typename __txt_detail::__decode_state<__txt_detail::__remove_cvref_t<_Type>>::type;
	};

	//////
	/// @brief Typename alias for ztd::text::decode_state.
	///
	//////
	template <typename _Type>
	using decode_state_t = typename decode_state<_Type>::type;

	//////
	/// @brief Retrieves the @c encode_state of the encoding type if it has one, or the @c state type of the
	/// encoding.
	//////
	template <typename _Type>
	class encode_state {
	public:
		//////
		/// @brief The @c encode_state type or @c state type on a given encoding type.
		///
		//////
		using type = typename __txt_detail::__encode_state<__txt_detail::__remove_cvref_t<_Type>>::type;
	};

	//////
	/// @brief Typename alias for ztd::text::encode_state.
	///
	//////
	template <typename _Type>
	using encode_state_t = typename encode_state<_Type>::type;

	//////
	/// @brief Whether or not the given type can be constructed without information from the encoding
	/// itself.
	///
	/// @tparam _Encoding The encoding that may contain necessary information.
	/// @tparam _Type The state type that may need information from the encoding to be successfully constructed.
	///
	/// @remarks This value tells users at compile-time whether or not they need to be careful with the state. Rather
	/// than let users have to work this independently, two functions — ztd::text::make_encode_state(_Encoding) and
	/// ztd::text::make_encode_state(_Encoding) — handle the details here.
	//////
	template <typename _Encoding, typename _Type>
	inline constexpr bool is_state_independent_v
		= !::std::is_constructible_v<_Type, _Encoding> && ::std::is_default_constructible_v<_Type>;

	//////
	/// @brief Whether or not the encoding's @c decode_state can be constructed without information from the
	/// encoding itself.
	//////
	template <typename _Encoding>
	inline constexpr bool is_decode_state_independent_v
		= is_state_independent_v<_Encoding, decode_state_t<_Encoding>>;

	//////
	/// @brief Whether or not the encoding's @c decode_state can be constructed without information from the
	/// encoding itself.
	//////
	template <typename _Encoding>
	inline constexpr bool is_encode_state_independent_v
		= is_state_independent_v<_Encoding, encode_state_t<_Encoding>>;

	//////
	/// @brief Constructs the @c decode_state of the given encoding, based on whether or not the encoding and state
	/// meet the criteria of ztd::text::is_decode_state_independent_v.
	///
	/// @param[in] __encoding The encoding object to use, if applicable, for the construction of the state.
	//////
	template <typename _Encoding>
	constexpr decode_state_t<__txt_detail::__remove_cvref_t<_Encoding>> make_decode_state(
		_Encoding& __encoding) noexcept {
		if constexpr (is_decode_state_independent_v<__txt_detail::__remove_cvref_t<_Encoding>>) {
			(void)__encoding;
			return decode_state_t<__txt_detail::__remove_cvref_t<_Encoding>>();
		}
		else {
			return decode_state_t<__txt_detail::__remove_cvref_t<_Encoding>>(__encoding);
		}
	}

	//////
	/// @brief Constructs the @c encode_state of the given encoding, based on whether or not the encoding and state
	/// meet the criteria of ztd::text::is_decode_state_independent_v.
	///
	/// @param[in] __encoding The encoding object to use, if applicable, for the construction of the state.
	//////
	template <typename _Encoding>
	constexpr encode_state_t<__txt_detail::__remove_cvref_t<_Encoding>> make_encode_state(
		_Encoding& __encoding) noexcept {
		if constexpr (is_encode_state_independent_v<__txt_detail::__remove_cvref_t<_Encoding>>) {
			(void)__encoding;
			return encode_state_t<__txt_detail::__remove_cvref_t<_Encoding>>();
		}
		else {
			return encode_state_t<__txt_detail::__remove_cvref_t<_Encoding>>(__encoding);
		}
	}

	//////
	/// @}
	/////

	ZTD_TEXT_INLINE_ABI_NAMESPACE_CLOSE_I_
}} // namespace ztd::text

#endif // ZTD_TEXT_STATE_HPP
