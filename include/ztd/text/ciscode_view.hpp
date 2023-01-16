// =============================================================================
//
// ztd.text
// Copyright © 2022-2023 JeanHeyd "ThePhD" Meneide and Shepherd's Oasis, LLC
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

#ifndef ZTD_TEXT_CISCODE_VIEW_HPP
#define ZTD_TEXT_CISCODE_VIEW_HPP

#include <ztd/text/version.hpp>

#include <ztd/text/recode_view.hpp>

#include <string_view>

#include <ztd/prologue.hpp>

namespace ztd { namespace text {
	ZTD_TEXT_INLINE_ABI_NAMESPACE_OPEN_I_

	//////
	/// @addtogroup ztd_text_ranges Ranges, Views, and Iterators
	///
	/// @{

	//////
	/// @copydoc ztd::text::transcode_view
	template <typename _FromEncoding, typename _ToEncoding = utf8_t,
		typename _Range            = __txt_detail::__default_char_view_t<code_unit_t<_FromEncoding>>,
		typename _FromErrorHandler = default_handler_t, typename _ToErrorHandler = default_handler_t,
		typename _FromState = decode_state_t<_FromEncoding>, typename _ToState = encode_state_t<_ToEncoding>>
	using ciscode_view
		= recode_view<_FromEncoding, _ToEncoding, _Range, _FromErrorHandler, _ToErrorHandler, _FromState, _ToState>;

	//////
	/// @}

	ZTD_TEXT_INLINE_ABI_NAMESPACE_CLOSE_I_
}} // namespace ztd::text


#include <ztd/epilogue.hpp>

#endif
