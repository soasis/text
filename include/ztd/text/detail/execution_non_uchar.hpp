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

#ifndef ZTD_TEXT_DETAIL_EXECUTION_NON_UCHAR_HPP
#define ZTD_TEXT_DETAIL_EXECUTION_NON_UCHAR_HPP

#include <ztd/text/version.hpp>

#if ZTD_IS_OFF(ZTD_CUCHAR_I_) && ZTD_IS_OFF(ZTD_UCHAR_I_)

#if ZTD_IS_ON(ZTD_TEXT_ICONV_I_)
// TODO: we can likely save ourselves by using the iconv encoding instead.
#include <ztd/text/iconv_encoding.hpp>
#endif

#error \
     "This platform configuration (no POSIX conversions, no <uchar.h> or <cuchar> is currently not supported. One way to work aroudn this is by making sure iconv is available and turning on ZTD_TEXT_ICONV."

#endif

#endif // ZTD_TEXT_DETAIL_EXECUTION_NON_UCHAR_HPP
