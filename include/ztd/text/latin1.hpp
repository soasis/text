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

#ifndef ZTD_TEXT_LATIN1_HPP
#define ZTD_TEXT_LATIN1_HPP

#include <ztd/text/version.hpp>

#include <ztd/text/impl/single_byte_lookup_encoding.hpp>

#include <ztd/idk/latin1.tables.hpp>
#include <ztd/ranges/adl.hpp>

#include <ztd/prologue.hpp>

namespace ztd { namespace text {
	ZTD_TEXT_INLINE_ABI_NAMESPACE_OPEN_I_

	template <typename _CodeUnit = char, typename _CodePoint = unicode_code_point>
	struct basic_latin1
	: public __txt_impl::__single_byte_lookup_encoding<basic_latin1<_CodeUnit, _CodePoint>,
		  &::ztd::latin1_index_to_code_point, &::ztd::latin1_code_point_to_index, _CodeUnit, _CodePoint> { };

	//////
	/// @brief An instance of skip_handler_t for ease of use.
	inline constexpr basic_latin1<char> latin1 = {};

	ZTD_TEXT_INLINE_ABI_NAMESPACE_CLOSE_I_
}} // namespace ztd::text


#include <ztd/epilogue.hpp>

#endif
