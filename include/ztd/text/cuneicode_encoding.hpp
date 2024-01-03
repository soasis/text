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

#ifndef ZTD_TEXT_CUNEICODE_ENCODING_HPP
#define ZTD_TEXT_CUNEICODE_ENCODING_HPP

#include <ztd/text/version.hpp>

#include <ztd/text/impl/fixed_cuneicode.hpp>

#include <ztd/cuneicode.h>

#include <ztd/prologue.hpp>

namespace ztd { namespace text {
	ZTD_TEXT_INLINE_ABI_NAMESPACE_OPEN_I_

	//////
	/// @brief An encoding implementing WHATWG's Big5 Hong Kong Supplementary Character Set (Big5 HKSCS).
	///
	/// @tparam _CodeUnit The type for the unit of encoded data.
	/// @tparam _CodePoint The type for the unit of decoded data.
	template <typename _CodeUnit = char, typename _CodePoint = unicode_code_point>
	class basic_cnc_big5_hkscs : public __txt_impl::__fixed_cuneicode<basic_cnc_big5_hkscs<_CodeUnit, _CodePoint>,
		                             // code unit / code point type
		                             _CodeUnit, _CodePoint,
		                             // max code units / code points
		                             2, 2,
		                             // decode functions, type + (constexpr) value
		                             decltype(&::cnc_mcnrtoc32n_big5_hkscs), (&::cnc_mcnrtoc32n_big5_hkscs),
		                             // encode functions, type + (constexpr) value
		                             decltype(&::cnc_c32nrtomcn_big5_hkscs), (&::cnc_c32nrtomcn_big5_hkscs),
		                             // decode state/completion
		                             cnc_mcstate_t, decltype(&::cnc_mcstate_is_complete), &::cnc_mcstate_is_complete,
		                             // encode state/completion
		                             cnc_mcstate_t, decltype(&::cnc_mcstate_is_complete), &::cnc_mcstate_is_complete,
		                             // is_injective
		                             true, false,
		                             // true code unit / code point types
		                             char, ztd_char32_t> { };

	//////
	/// @brief A convenience for ease of use.
	inline constexpr basic_cnc_big5_hkscs<char, ztd_char32_t> big5_cnc_hkscs = {};

	//////
	/// @brief An encoding implementing WHATWG's GBK.
	///
	/// @tparam _CodeUnit The type for the unit of encoded data.
	/// @tparam _CodePoint The type for the unit of decoded data.
	template <typename _CodeUnit = char, typename _CodePoint = unicode_code_point>
	class basic_cnc_gbk : public __txt_impl::__fixed_cuneicode<basic_cnc_gbk<_CodeUnit, _CodePoint>,
		                      // code unit / code point type
		                      _CodeUnit, _CodePoint,
		                      // max code units / code points
		                      2, 1,
		                      // decode functions, type + (constexpr) value
		                      decltype(&::cnc_mcnrtoc32n_gbk), (&::cnc_mcnrtoc32n_gbk),
		                      // encode functions, type + (constexpr) value
		                      decltype(&::cnc_c32nrtomcn_gbk), (&::cnc_c32nrtomcn_gbk),
		                      // decode state/completion
		                      cnc_mcstate_t, decltype(&::cnc_mcstate_is_complete), &::cnc_mcstate_is_complete,
		                      // encode state/completion
		                      cnc_mcstate_t, decltype(&::cnc_mcstate_is_complete), &::cnc_mcstate_is_complete,
		                      // is_injective
		                      true, false,
		                      // true code unit / code point types
		                      char, ztd_char32_t> { };

	//////
	/// @brief A convenience alias for ease of use.
	inline constexpr basic_cnc_gbk<char, ztd_char32_t> cnc_gbk = {};

	//////
	/// @brief An encoding implementing WHATWG's GB18030
	///
	/// @tparam _CodeUnit The type for the unit of encoded data.
	/// @tparam _CodePoint The type for the unit of decoded data.
	template <typename _CodeUnit = char, typename _CodePoint = unicode_code_point>
	class basic_cnc_gb18030 : public __txt_impl::__fixed_cuneicode<basic_cnc_gb18030<_CodeUnit, _CodePoint>,
		                          // code unit / code point type
		                          _CodeUnit, _CodePoint,
		                          // max code units / code points
		                          4, 2,
		                          // decode functions, type + (constexpr) value
		                          decltype(&::cnc_mcnrtoc32n_gb18030), (&::cnc_mcnrtoc32n_gb18030),
		                          // encode functions, type + (constexpr) value
		                          decltype(&::cnc_c32nrtomcn_gb18030), (&::cnc_c32nrtomcn_gb18030),
		                          // decode state/completion
		                          cnc_mcstate_t, decltype(&::cnc_mcstate_is_complete), &::cnc_mcstate_is_complete,
		                          // encode state/completion
		                          cnc_mcstate_t, decltype(&::cnc_mcstate_is_complete), &::cnc_mcstate_is_complete,
		                          // is_injective
		                          true, true,
		                          // true code unit / code point types
		                          char, ztd_char32_t> { };

	//////
	/// @brief A convenience alias for ease of use.
	inline constexpr basic_cnc_gb18030<char, ztd_char32_t> cnc_gb18030 = {};

	ZTD_TEXT_INLINE_ABI_NAMESPACE_CLOSE_I_
}} // namespace ztd::text

#include <ztd/epilogue.hpp>

#endif
