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

#pragma once

#ifndef ZTD_TEXT_BENCHMARKS_BARRIER_API_H
#define ZTD_TEXT_BENCHMARKS_BARRIER_API_H

#include <barrier/version.h>

// clang-format off

#if defined(ZTD_TEXT_BENCHMARKS_BARRIER_BUILD)
	#if (ZTD_TEXT_BENCHMARKS_BARRIER_BUILD != 0)
		#define ZTD_TEXT_BENCHMARKS_BARRIER_BUILD_I_ ZTD_ON
	#else
		#define ZTD_TEXT_BENCHMARKS_BARRIER_BUILD_I_ ZTD_OFF
	#endif
#else
	#define ZTD_TEXT_BENCHMARKS_BARRIER_BUILD_I_ ZTD_DEFAULT_OFF
#endif // Building or not

#if defined(ZTD_TEXT_BENCHMARKS_BARRIER_DLL)
	#if (ZTD_TEXT_BENCHMARKS_BARRIER_DLL != 0)
		#define ZTD_TEXT_BENCHMARKS_BARRIER_DLL_I_ ZTD_ON
	#else
		#define ZTD_TEXT_BENCHMARKS_BARRIER_DLL_I_ ZTD_ON
	#endif
#else
	#define ZTD_TEXT_BENCHMARKS_BARRIER_DLL_I_ ZTD_OFF
#endif // Building or not

#if defined(ZTD_TEXT_BENCHMARKS_BARRIER_API_LINKAGE)
	#define ZTD_TEXT_BENCHMARKS_BARRIER_API_LINKAGE_I_ ZTD_TEXT_BENCHMARKS_BARRIER_API_LINKAGE
#else
	#if ZTD_IS_ON(ZTD_TEXT_BENCHMARKS_BARRIER_DLL)
		#if ZTD_IS_ON(ZTD_COMPILER_VCXX) || ZTD_IS_ON(ZTD_PLATFORM_WINDOWS) || ZTD_IS_ON(ZTD_PLATFORM_CYGWIN)
			// MSVC Compiler; or, Windows, or Cygwin platforms
			#if ZTD_IS_ON(ZTD_TEXT_BENCHMARKS_BARRIER_BUILD)
				// Building the library
				#if ZTD_IS_ON(ZTD_COMPILER_GCC)
					// Using GCC
					#define ZTD_TEXT_BENCHMARKS_BARRIER_API_LINKAGE_I_ __attribute__((dllexport))
				#else
					// Using Clang, MSVC, etc...
					#define ZTD_TEXT_BENCHMARKS_BARRIER_API_LINKAGE_I_ __declspec(dllexport)
				#endif
			#else
				#if ZTD_IS_ON(ZTD_COMPILER_GCC)
					#define ZTD_TEXT_BENCHMARKS_BARRIER_API_LINKAGE_I_ __attribute__((dllimport))
				#else
					#define ZTD_TEXT_BENCHMARKS_BARRIER_API_LINKAGE_I_ __declspec(dllimport)
				#endif
			#endif
		#else
			#define ZTD_TEXT_BENCHMARKS_BARRIER_API_LINKAGE_I_
		#endif
	#else
		#define ZTD_TEXT_BENCHMARKS_BARRIER_API_LINKAGE_I_
	#endif // DLL or not
#endif // Build definitions

// clang-format on

#endif
