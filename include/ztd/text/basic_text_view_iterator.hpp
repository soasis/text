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

#ifndef ZTD_TEXT_BASIC_TEXT_VIEW_ITERATOR_HPP
#define ZTD_TEXT_BASIC_TEXT_VIEW_ITERATOR_HPP

#include <ztd/text/version.hpp>

#include <ztd/text/error_handler.hpp>
#include <ztd/text/code_unit.hpp>
#include <ztd/text/code_point.hpp>
#include <ztd/text/decode_view.hpp>
#include <ztd/text/normalized_iterator.hpp>
#include <ztd/text/error_handler.hpp>
#include <ztd/text/detail/default_char_view.hpp>

#include <ztd/idk/unwrap.hpp>
#include <ztd/ranges/range.hpp>
#include <ztd/ranges/default_sentinel.hpp>

#include <string_view>

#include <ztd/prologue.hpp>

namespace ztd { namespace text {
	ZTD_TEXT_INLINE_ABI_NAMESPACE_OPEN_I_

	using text_view_sentinel_t = ranges::default_sentinel_t;

	template <typename _Encoding, typename _NormalizationForm = nfkc,
		typename _Range        = __txt_detail::__default_char_view_t<code_unit_t<_Encoding>>,
		typename _ErrorHandler = default_handler_t, typename _State = decode_state_t<_Encoding>,
		typename _NormalizationStorage = __txt_detail::__default_normal_storage_t<_Range, _NormalizationForm>>
	class basic_text_view_iterator
	: public normalized_iterator<_NormalizationForm, decode_view<_Encoding, _Range, _ErrorHandler, _State>,
		  _NormalizationStorage> {
	private:
		using __base_normalized_iterator_t = normalized_iterator<_NormalizationForm,
			decode_view<_Encoding, _Range, _ErrorHandler, _State>, _NormalizationStorage>;

	public:
		using __base_normalized_iterator_t::__base_normalized_iterator_t;
	};

	ZTD_TEXT_INLINE_ABI_NAMESPACE_CLOSE_I_
}} // namespace ztd::text

#include <ztd/epilogue.hpp>

#endif
