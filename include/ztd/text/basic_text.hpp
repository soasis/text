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

#ifndef ZTD_TEXT_BASIC_TEXT_HPP
#define ZTD_TEXT_BASIC_TEXT_HPP

#include <ztd/text/version.hpp>

#include <ztd/text/basic_text_view.hpp>
#include <ztd/text/error_handler.hpp>
#include <ztd/text/normalization.hpp>
#include <ztd/text/code_unit.hpp>

#include <string>

namespace ztd { namespace text {
	ZTD_TEXT_INLINE_ABI_NAMESPACE_OPEN_I_

	template <typename _Encoding, typename _NormalizationForm = nfkc,
		typename _Container    = ::std::basic_string<code_unit_of_t<_Encoding>>,
		typename _ErrorHandler = default_handler>
	class basic_text : private basic_text_view<_Encoding, _NormalizationForm, _Container, _ErrorHandler> { };

	ZTD_TEXT_INLINE_ABI_NAMESPACE_CLOSE_I_
}} // namespace ztd::text

#endif // ZTD_TEXT_BASIC_TEXT_HPP
