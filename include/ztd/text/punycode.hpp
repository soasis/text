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

#ifndef ZTD_TEXT_PUNYCODE_HPP
#define ZTD_TEXT_PUNYCODE_HPP

#include <ztd/text/version.hpp>

#include <ztd/text/impl/fixed_cuneicode.hpp>

#include <ztd/prologue.hpp>

namespace ztd { namespace text {
	ZTD_TEXT_INLINE_ABI_NAMESPACE_OPEN_I_

	//////
	/// @brief Whether or not the encode and decode steps of an punycode check need to take into account IDNA-specific
	/// quirks.
	enum class idna {
		//////
		/// @brief Do not use IDNA-specific quirks and do pure punycode encoding/decoding.
		no,
		//////
		/// @brief Use IDNA-specific quirks (e.g., looking for the prefix and passing-through strings as ASCII if they
		/// do not meet the requirements on decode.)
		yes
	};

	//////
	/// @brief The punycode encoding, as envisioned by RFC 3492 and (potentially) influenced by IDNA (RFC 5890).
	///
	/// @tparam _IsIdna Whether or not this punycode type looks for IDNA prefixes and obeys its encoding rules.
	/// @tparam _CodeUnit The code unit type for the encoded ASCII text.
	/// @tparam _CodePoint The code point type for the Unicode Code Point decoded text.
	///
	/// @remarks See https://datatracker.ietf.org/doc/html/rfc5890 and https://datatracker.ietf.org/doc/html/rfc3492.
	template <idna _IsIdna, typename _CodeUnit = char, typename _CodePoint = unicode_code_point>
	class basic_any_punycode
	: public __txt_impl::__fixed_cuneicode<basic_any_punycode<_IsIdna, _CodeUnit, _CodePoint>,
		  // code unit / code point type
		  _CodeUnit, _CodePoint,
		  // max code units / code points
		  1, 1,
		  // decode functions, type + (constexpr) value
		  decltype(_IsIdna == idna::yes ? &::cnc_mcnrtoc32n_punycode_idna : &::cnc_mcnrtoc32n_punycode),
		  (_IsIdna == idna::yes ? &::cnc_mcnrtoc32n_punycode_idna : &::cnc_mcnrtoc32n_punycode),
		  // encode functions, type + (constexpr) value
		  decltype(_IsIdna == idna::yes ? &::cnc_c32nrtomcn_punycode_idna : &::cnc_c32nrtomcn_punycode),
		  (_IsIdna == idna::yes ? &::cnc_c32nrtomcn_punycode_idna : &::cnc_c32nrtomcn_punycode),
		  // decode state/completion
		  cnc_pny_decode_state_t, decltype(&::cnc_pny_decode_state_is_complete), &::cnc_pny_decode_state_is_complete,
		  // encode state/completion
		  cnc_pny_encode_state_t, decltype(&::cnc_pny_encode_state_is_complete), &::cnc_pny_encode_state_is_complete,
		  // is_injective
		  true, true> { };

	//////
	/// @brief A convenience typedef for ztd::text::basic_any_punycode with the ztd::text::idna::no provided.
	///
	/// @tparam _CodeUnit The code unit type for encoded text.
	/// @tparam _CodePoint The code point type for decoded text.
	template <typename _CodeUnit = char, typename _CodePoint = unicode_code_point>
	using basic_punycode = basic_any_punycode<idna::no, _CodeUnit, _CodePoint>;

	//////
	/// @brief A convenience typedef for ztd::text::basic_any_punycode with the ztd::text::idna::yes provided.
	///
	/// @tparam _CodeUnit The code unit type for encoded text.
	/// @tparam _CodePoint The code point type for decoded text.
	template <typename _CodeUnit, typename _CodePoint>
	using basic_punycode_idna = basic_any_punycode<idna::yes, _CodeUnit, _CodePoint>;

	//////
	/// @brief A ztd::text::basic_any_punycode type using `char` and ztd::text::unicode_code_point
	using punycode_t = basic_punycode<char, unicode_code_point>;

	//////
	/// @brief A ztd::text::basic_any_punycode type using `char` and ztd::text::unicode_code_point
	using punycode_idna_t = basic_punycode_idna<char, unicode_code_point>;

	//////
	/// @brief A preexisting object of type ztd::text::punycode_t for ease-of-use.
	inline constexpr punycode_t punycode = punycode_t {};

	//////
	/// @brief A preexisting object of type ztd::text::punycode_idna_t for ease-of-use.
	inline constexpr punycode_idna_t punycode_idna = punycode_idna_t {};

	ZTD_TEXT_INLINE_ABI_NAMESPACE_CLOSE_I_
}} // namespace ztd::text

#include <ztd/epilogue.hpp>

#endif
