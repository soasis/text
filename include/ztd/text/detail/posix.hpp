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

#ifndef ZTD_TEXT_DETAIL_POSIX_HPP
#define ZTD_TEXT_DETAIL_POSIX_HPP

#include <ztd/text/version.hpp>

#include <ztd/text/detail/encoding_name.hpp>

#if ZTD_TEXT_IS_ON(ZTD_TEXT_PLATFORM_UNIX_I_)

extern "C" {
#if !defined(__has_include)
// We can't detect it automatically. Most commonly, header is in langinfo.h
#include <langinfo.h>
#elif ZTD_TEXT_HAS_INCLUDE_I_(<nl_langinfo.h>)
// Some platforms have an nl_langinfo.h instead
#include <nl_langinfo.h>
#elif ZTD_TEXT_HAS_INCLUDE_I_(<langinfo.h>)
// <langinfo.h> is the standard name per POSIX
#include <langinfo.h>
#else
#error "No langinfo.h header is available"
#endif
}

namespace ztd { namespace text {

	ZTD_TEXT_INLINE_ABI_NAMESPACE_OPEN_I_

	namespace __detail { namespace __posix {

		inline __encoding_id __determine_active_code_page() noexcept {
			const char* __name = nl_langinfo(LC_CTYPE);
			return __to_encoding_id(__name);
		}

	}} // namespace __detail::__posix

	ZTD_TEXT_INLINE_ABI_NAMESPACE_CLOSE_I_

}} // namespace ztd::text

#endif // POSIX shit

#endif // ZTD_TEXT_DETAIL_POSIX_HPP
