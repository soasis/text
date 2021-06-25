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

#ifndef ZTD_TEXT_PROPAGATE_ERROR_HPP
#define ZTD_TEXT_PROPAGATE_ERROR_HPP

#include <ztd/text/version.hpp>

#include <ztd/text/decode_result.hpp>
#include <ztd/text/encode_result.hpp>
#include <ztd/text/transcode_result.hpp>

namespace ztd { namespace text {
	ZTD_TEXT_INLINE_ABI_NAMESPACE_OPEN_I_

	//////
	/// @brief Takes the given encodings from a transcoding operation and inspects the
	///
	//////
	template <typename _FromEncoding, typename _ToEncoding, typename _Result, typename _DecodeErrorHandler,
		typename _EncodeErrorHandler, typename _FromProgress, typename _ToProgress>
	constexpr auto propagate_error(_FromEncoding&& __from_encoding, _ToEncoding&& __to_encoding, _Result&& __result,
		_DecodeErrorHandler&& __decode_error_handler, _EncodeErrorHandler&& __encode_error_handler,
		_FromProgress&& __from_progress, _ToProgress&& __to_progress) {
		auto __decode_result           = __decode_error_handler(::std::forward<FromEncoding>(__from_encoding),
               ::std::forward<_Result>(__result), __from_progress, __from_progress);
		auto __corrected_encode_result = ();
	}

	ZTD_TEXT_INLINE_ABI_NAMESPACE_CLOSE_I_
}} // namespace ztd::text

#include <ztd/text/detail/epilogue.hpp>

#endif // ZTD_TEXT_PROPAGATE_ERROR_HPP
