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

#ifndef ZTD_TEXT_BASIC_TEXT_VIEW_HPP
#define ZTD_TEXT_BASIC_TEXT_VIEW_HPP

#include <ztd/text/version.hpp>

#include <ztd/text/code_unit.hpp>
#include <ztd/text/error_handler.hpp>
#include <ztd/text/normalization.hpp>

#include <string_view>

namespace ztd { namespace text {
	ZTD_TEXT_INLINE_ABI_NAMESPACE_OPEN_I_

	//////
	/// @brief A view over a sequence of code units. The code units are expected to be of the given encoding and
	/// normalization form.
	///
	/// @remarks The default type for this is a basic_string_view templated on the code unit type from the encoding.
	/// The error handler is also the default handler, meaning that any lossy conversions will automatically cause a
	/// compile-time error.
	//////
	template <typename _Encoding, typename _NormalizationForm = nfkc,
		typename _Range        = ::std::basic_string_view<code_unit_of_t<_Encoding>>,
		typename _ErrorHandler = default_handler>
	class basic_text_view {
	public:
		//////
		/// @brief The type that this view is wrapping.
		///
		//////
		using range_type = _Range;
		//////
		/// @brief The encoding type that this view is using to interpret the underlying sequence of code units.
		///
		//////
		using encoding_type = _Encoding;
		//////
		/// @brief The normalization form type this view is imposing on top of the encoded sequence.
		///
		//////
		using normalization_type = _NormalizationForm;
		//////
		/// @brief The error handling type used by default for any problems in conversions.
		///
		//////
		using error_handler_type = _ErrorHandler;

	private:
		range_type _M_storage;
		encoding_type _M_encoding;
		normalization_type _M_normalization;
		error_handler_type _M_error_handler;

	public:
		//////
		/// @brief Access the storage as an r-value reference.
		//////
		range_type&& base() && {
			return ::std::move(this->_M_storage);
		}

		//////
		/// @brief Access the storage as a const-qualified l-value reference.
		//////
		const range_type& base() const& {
			return this->_M_storage;
		}

		//////
		/// @brief Access the storage as an l-value reference.
		//////
		range_type& base() & {
			return this->_M_storage;
		}
	};

	ZTD_TEXT_INLINE_ABI_NAMESPACE_CLOSE_I_
}} // namespace ztd::text

#endif // ZTD_TEXT_BASIC_TEXT_VIEW_HPP
