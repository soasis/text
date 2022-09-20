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

#ifndef ZTD_TEXT_EXTENSION_HPP
#define ZTD_TEXT_EXTENSION_HPP

#include <ztd/text/version.hpp>

#include <ztd/text/forward.hpp>
#include <ztd/text/encoding_error.hpp>
#include <ztd/text/code_unit.hpp>
#include <ztd/text/code_point.hpp>
#include <ztd/text/state.hpp>
#include <ztd/text/pivot.hpp>
#include <ztd/text/assume_valid_handler.hpp>
#include <ztd/text/is_ignorable_error_handler.hpp>
#include <ztd/text/is_full_range_representable.hpp>
#include <ztd/text/is_unicode_encoding.hpp>
#include <ztd/text/is_compatible_code_points.hpp>
#include <ztd/text/is_transcoding_compatible.hpp>
#include <ztd/text/decode_result.hpp>
#include <ztd/text/encode_result.hpp>
#include <ztd/text/transcode_result.hpp>

#include <ztd/idk/tag.hpp>

#include <ztd/prologue.hpp>

namespace ztd { namespace text {
	ZTD_TEXT_INLINE_ABI_NAMESPACE_OPEN_I_

	template <typename _Input, typename _Encoding, typename _Output, typename _ErrorHandler, typename _State>
	constexpr auto decode_one_into(_Input&& __input, _Encoding&& __encoding, _Output&& __output,
		_ErrorHandler&& __error_handler, _State& __state);

	template <typename _Input, typename _Encoding, typename _Output, typename _ErrorHandler, typename _State>
	constexpr auto encode_one_into(_Input&& __input, _Encoding&& __encoding, _Output&& __output,
		_ErrorHandler&& __error_handler, _State& __state);

	template <typename _Input, typename _Encoding, typename _Output, typename _ErrorHandler, typename _State>
	constexpr auto basic_decode_into(_Input&& __input, _Encoding&& __encoding, _Output&& __output,
		_ErrorHandler&& __error_handler, _State& __state);

	template <typename _Input, typename _Encoding, typename _Output, typename _ErrorHandler, typename _State>
	constexpr auto basic_encode_into(_Input&& __input, _Encoding&& __encoding, _Output&& __output,
		_ErrorHandler&& __error_handler, _State& __state);

	template <typename _Input, typename _Output, typename _FromEncoding, typename _ToEncoding,
		typename _FromErrorHandler, typename _ToErrorHandler, typename _FromState, typename _ToState,
		typename _PivotRange>
	constexpr auto basic_transcode_one_into(_Input&& __input, _FromEncoding&& __from_encoding, _Output&& __output,
		_ToEncoding&& __to_encoding, _FromErrorHandler&& __from_error_handler, _ToErrorHandler&& __to_error_handler,
		_FromState& __from_state, _ToState& __to_state, pivot<_PivotRange>& __pivot);

	template <typename _Input, typename _FromEncoding, typename _Output, typename _ToEncoding,
		typename _FromErrorHandler, typename _ToErrorHandler, typename _FromState, typename _ToState,
		typename _PivotRange>
	constexpr auto basic_transcode_into(_Input&& __input, _FromEncoding&& __from_encoding, _Output&& __output,
		_ToEncoding&& __to_encoding, _FromErrorHandler&& __from_error_handler, _ToErrorHandler&& __to_error_handler,
		_FromState& __from_state, _ToState& __to_state, pivot<_PivotRange>& __pivot);

	ZTD_TEXT_INLINE_ABI_NAMESPACE_CLOSE_I_
}} // namespace ztd::text

#include <ztd/epilogue.hpp>

#endif // ZTD_TEXT_EXTENSION_HPP
