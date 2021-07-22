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

#ifndef ZTD_TEXT_DETAIL_EXECUTION_MAC_OS_HPP
#define ZTD_TEXT_DETAIL_EXECUTION_MAC_OS_HPP

#include <ztd/text/version.hpp>

#include <ztd/text/utf8.hpp>

#include <ztd/prologue.hpp>

namespace ztd { namespace text {
	ZTD_TEXT_INLINE_ABI_NAMESPACE_OPEN_I_

	namespace __impl {
		//////
		/// @brief The default execution ("locale") encoding for Mac OS.
		///
		/// @remarks Note that for all intents and purposes, Mac OS demands that all text is in UTF-8. However, on Big
		/// Sur, Catalina, and a few other platforms locale functionality and data has been either forgotten/left
		/// behind or intentionally kept in place on these devices. It may be possible that with very dedicated hacks
		/// one can still change the desired default encoding from UTF-8 to something else in the majority of Apple
		/// text. Their documentation states that all text "should" be UTF-8, but very explicitly goes out of its way
		/// to not make that hard guarantee. Since it is a BSD-like system and they left plenty of that data behind
		/// from C libraries, this may break in extremely obscure cases. Please be careful on Apple machines!
		//////
		class __execution_mac_os : private basic_utf8<char, char32_t> {
		private:
			using __base_t = basic_utf8<char, char32_t>;

		public:
			using __base_t::code_point;
			using __base_t::code_unit;
			using __base_t::is_decode_injective;
			using __base_t::is_encode_injective;
			using __base_t::is_unicode_encoding;
			using __base_t::state;

			//////
			/// @brief The maximum code units a single complete operation of encoding can produce.
			///
			/// @remarks There are encodings for which one input can produce 3 code points (some Tamil encodings) and
			/// there are rumours of an encoding that can produce 7 code points from a handful of input. We use a
			/// protective/conservative 8, here, to make sure ABI isn't broken later.
			//////
			inline static constexpr ::std::size_t max_code_points = 8;
			//////
			/// @brief The maximum number of code points a single complete operation of decoding can produce.
			///
			/// @remarks This is bounded by the platform's @c MB_LEN_MAX macro, which is an integral constant
			/// expression representing the maximum value of output all C locales can produce from a single complete
			/// operation.
			//////
			inline static constexpr ::std::size_t max_code_units = MB_LEN_MAX;

			using __base_t::decode_one;
			using __base_t::encode_one;
		};

	} // namespace __impl

	ZTD_TEXT_INLINE_ABI_NAMESPACE_CLOSE_I_
}} // namespace ztd::text

#endif // ZTD_TEXT_DETAIL_EXECUTION_MAC_OS_HPP
