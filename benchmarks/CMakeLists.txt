# =============================================================================
#
# ztd.text
# Copyright © 2022-2022 JeanHeyd "ThePhD" Meneide and Shepherd's Oasis, LLC
# Contact: opensource@soasis.org
#
# Commercial License Usage
# Licensees holding valid commercial ztd.text licenses may use this file
# in accordance with the commercial license agreement provided with the
# Software or, alternatively, in accordance with the terms contained in
# a written agreement between you and Shepherd's Oasis, LLC.
# For licensing terms and conditions see your agreement. For
# further information contact opensource@soasis.org.
#
# Apache License Version 2 Usage
# Alternatively, this file may be used under the terms of Apache License
# Version 2.0 (the "License"); you may not use this file except in compliance
# with the License. You may obtain a copy of the License at
#
# https://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.
#
# =========================================================================== #

include(FetchContent)
include(GraphBenchmarks)

# # Fetch dependencies
# google benchmark
function (google_benchmark_dependency_jail)
	FetchContent_Declare(
		google_benchmark
		GIT_REPOSITORY https://github.com/google/benchmark.git
		GIT_TAG main
	)
	# Benchmark variables, internal setup
	set(BENCHMARK_ENABLE_EXCEPTIONS ON)
	set(BENCHMARK_ENABLE_TESTING OFF)
	set(BENCHMARK_DOWNLOAD_DEPENDENCIES OFF)
	set(BENCHMARK_ENABLE_GTEST_TESTS OFF)
	set(BENCHMARK_ENABLE_GTEST_TESTS OFF)
	set(BENCHMARK_ENABLE_WERROR OFF)
	set(BENCHMARK_FORCE_WERROR OFF)
	set(BENCHMARK_ENABLE_INSTALL OFF)
	set(BENCHMARK_ENABLE_TESTING OFF)
	set(BENCHMARK_ENABLE_GTEST_TESTS OFF)
	set(BENCHMARK_ENABLE_WERROR OFF)
	set(BENCHMARK_FORCE_WERROR OFF)
	if (CMAKE_CXX_COMPILER_ID MATCHES "Clang" AND WIN32)
		# google benchmark is broken, so we need to force HAVE_STD_REGEX to be on for this.
		set(HAVE_STD_REGEX ON)
	endif()
	FetchContent_MakeAvailable(google_benchmark)
	set_property(DIRECTORY "${google_benchmark_SOURCE_DIR}" PROPERTY EXCLUDE_FROM_ALL YES)
endfunction()
google_benchmark_dependency_jail()


function (utf8cpp_dependency_jail)
	FetchContent_Declare(
		utf8cpp
		GIT_REPOSITORY https://github.com/nemtrif/utfcpp
		GIT_SHALLOW    ON
	)
	FetchContent_MakeAvailable(utf8cpp)
endfunction()
utf8cpp_dependency_jail()

function (ctre_dependency_jail)
	FetchContent_Declare(
		ctre
		GIT_REPOSITORY https://github.com/hanickadot/compile-time-regular-expressions
		GIT_SHALLOW    ON
		GIT_TAG        main)
	FetchContent_MakeAvailable(ctre)
endfunction()
ctre_dependency_jail()

function(ztd_platform_dependency_jail)
	# ztd.platform
	FetchContent_Declare(
		ztd.platform
		GIT_REPOSITORY https://github.com/soasis/platform
		GIT_SHALLOW    ON
		GIT_TAG        main)
	FetchContent_MakeAvailable(ztd.platform)
endfunction()
ztd_platform_dependency_jail()

add_subdirectory(barrier)
add_subdirectory(function_form)
add_subdirectory(conversion_speed)
