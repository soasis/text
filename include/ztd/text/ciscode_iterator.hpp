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

#ifndef ZTD_TEXT_CISCODE_ITERATOR_HPP
#define ZTD_TEXT_CISCODE_ITERATOR_HPP

#include <ztd/text/version.hpp>

#include <ztd/text/recode_iterator.hpp>

#include <ztd/prologue.hpp>

namespace ztd { namespace text {
	ZTD_TEXT_INLINE_ABI_NAMESPACE_OPEN_I_

	//////
	/// @copydoc ztd::text::recode_sentinel_t
	using ciscode_sentinel_t = recode_sentinel_t;

	//////
	/// @copydoc ztd::text::recode_iterator
	template <typename _FromEncoding, typename _ToEncoding, typename _Range, typename _FromErrorHandler,
		typename _ToErrorHandler, typename _FromState, typename _ToState>
	using ciscode_iterator = recode_iterator<_FromEncoding, _ToEncoding, _Range, _FromErrorHandler, _ToErrorHandler,
		_FromState, _ToState>;


	ZTD_TEXT_INLINE_ABI_NAMESPACE_CLOSE_I_
}} // namespace ztd::text

#include <ztd/epilogue.hpp>

#endif
