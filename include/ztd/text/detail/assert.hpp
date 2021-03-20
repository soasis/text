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

#ifndef ZTD_TEXT_DETAIL_ASSERT_HPP
#define ZTD_TEXT_DETAIL_ASSERT_HPP

#include <ztd/text/version.hpp>

// clang-format off
#if defined(ZTD_TEXT_ASSERT)
	#define ZTD_TEXT_ASSERT_I_(...) ZTD_TEXT_ASSERT(__VA_ARGS__)
#else
	#ifndef NDEBUG
		#include <exception>
		#include <iostream>
		#include <cstdlib>

		#define ZTD_TEXT_ASSERT_I_(...)                                                                                         \
			do {                                                                                                               \
				if (!(__VA_ARGS__)) {                                                                                         \
					::std::cerr << "Assertion `" #__VA_ARGS__ "` failed in " << __FILE__ << " line " << __LINE__ << ::std::endl; \
					::std::terminate();                                                                                        \
				}                                                                                                             \
			} while (false)
	#else
		#define ZTD_TEXT_ASSERT_I_(...)     \
			do {                           \
				if (false) {              \
					(void)(__VA_ARGS__); \
				}                         \
			} while (false)
	#endif
#endif

#if defined(ZTD_TEXT_ASSERT_MESSAGE)
	#define ZTD_TEXT_ASSERT_MESSAGE_I_(...) ZTD_TEXT_ASSERT_MESSAGE(__VA_ARGS__)
#else
	#ifndef NDEBUG
		#include <exception>
		#include <iostream>
		#include <cstdlib>
		#define ZTD_TEXT_ASSERT_MESSAGE_I_(__message, ...)                                                                \
			do {                                                                                                         \
				if (!(__VA_ARGS__)) {                                                                                   \
					::std::cerr << "Assertion `" #__VA_ARGS__ "` failed in " << __FILE__ << " line " << __LINE__ << ": " \
							<< __message << ::std::endl;                                                               \
					::std::terminate();                                                                                  \
				}                                                                                                       \
			} while (false)
	#else
		#define ZTD_TEXT_ASSERT_MESSAGE_I_(__message, ...) \
			do {                                          \
				if (false) {                             \
					(void)(__VA_ARGS__);                \
					(void)sizeof(__message);            \
				}                                        \
			} while (false)
	#endif
#endif

// clang-format on

#include <ztd/text/detail/epilogue.hpp>

#endif // ZTD_TEXT_DETAIL_ASSERT_HPP
