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

#ifndef ZTD_TEXT_PIVOT_HPP
#define ZTD_TEXT_PIVOT_HPP

#include <ztd/text/version.hpp>

#include <ztd/text/encoding_error.hpp>

#include <ztd/prologue.hpp>

namespace ztd { namespace text {
	ZTD_TEXT_INLINE_ABI_NAMESPACE_OPEN_I_

	//////
	/// @brief A type which stores a potential intermediate range for usage by higher-level algorithms (such as
	/// ztd::text::transcode_into_raw).
	///
	/// @tparam _Range The range type which is meant to serve as the potential intermediate space. Typically a
	/// `std::span` or span-like view type, but any view type will do fine.
	template <typename _Range>
	class pivot {
	public:
		//////
		/// @brief The range that will be written into for the data.
		_Range intermediate;
		//////
		/// @brief The error code that occurs if the operation which converts data into the pivot does not finish
		/// successfully.
		encoding_error error_code;
	};

	ZTD_TEXT_INLINE_ABI_NAMESPACE_CLOSE_I_
}} // namespace ztd::text

#include <ztd/epilogue.hpp>

#endif
