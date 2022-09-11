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

#ifndef ZTD_TEXT_DETAIL_DEFAULT_CHAR_VIEW_HPP
#define ZTD_TEXT_DETAIL_DEFAULT_CHAR_VIEW_HPP

#include <ztd/text/version.hpp>

#include <ztd/idk/type_traits.hpp>
#include <ztd/idk/char_traits.hpp>
#include <ztd/idk/span.hpp>

#include <string_view>
#include <vector>
#include <string>

#include <ztd/prologue.hpp>

namespace ztd { namespace text {
	ZTD_TEXT_INLINE_ABI_NAMESPACE_OPEN_I_

	namespace __txt_detail {

		template <typename _CharType, bool = ::ztd::is_char_traitable_v<_CharType>>
		class __default_char_view {
		public:
			using type = ::ztd::span<_CharType>;
		};

		template <typename _CharType>
		class __default_char_view<_CharType, true> {
		public:
			using type = ::std::basic_string_view<_CharType>;
		};

		template <typename _CharType>
		using __default_char_view_t = typename __default_char_view<_CharType>::type;

	} // namespace __txt_detail
	ZTD_TEXT_INLINE_ABI_NAMESPACE_CLOSE_I_
}} // namespace ztd::text

#include <ztd/epilogue.hpp>

#endif // ZTD_TEXT_DETAIL_DEFAULT_CHAR_VIEW_HPP
