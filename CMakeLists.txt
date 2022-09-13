# =============================================================================
#
# ztd.text
# Copyright © 2022 JeanHeyd "ThePhD" Meneide and Shepherd's Oasis, LLC
# Contact: opensource@soasis.org
#
# Commercial License Usage
# Licensees holding valid commercial ztd.text licenses may use this file in
# accordance with the commercial license agreement provided with the
# Software or, alternatively, in accordance with the terms contained in
# a written agreement between you and Shepherd's Oasis, LLC.
# For licensing terms and conditions see your agreement. For
# further information contact opensource@soasis.org.
#
# Apache License Version 2 Usage
# Alternatively, this file may be used under the terms of Apache License
# Version 2.0 (the "License") for non-commercial use; you may not use this
# file except in compliance with the License. You may obtain a copy of the
# License at
#
# http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.
#
# ============================================================================>

cmake_minimum_required(VERSION 3.21.0)
cmake_policy(VERSION 3.21)

# # Project kickstart
# Includes a bunch of basic flags and utilities shared across projects
# See more at the github repository link below
include(FetchContent)
FetchContent_Declare(ztd.cmake
	GIT_REPOSITORY https://github.com/soasis/cmake
	GIT_TAG main)
FetchContent_MakeAvailable(ztd.cmake)
set(CMAKE_PROJECT_INCLUDE ${ZTD_CMAKE_PROJECT_PRELUDE})

# # Project declaration
# informs about the project, gives a description, version and MOST IMPORTANTLY
# the languages the project is going to use. Required.
project(ztd.text
	VERSION 0.0.0
	DESCRIPTION "A spicy text library."
	HOMEPAGE_URL "https://ztdtext.readthedocs.io/en/latest/"
	LANGUAGES C CXX)

if(ZTD_TEXT_READTHEDOCS)
	# ReadTheDocs seems unable to handle the include at the project level: something must be going wrong?
	include(CheckCXXCompilerFlag)
	include(CheckCCompilerFlag)
	include(CheckIPOSupported)
	include(CMakePackageConfigHelpers)
	include(CMakeDependentOption)
	include(CMakePrintHelpers)
	include(GNUInstallDirs)
	include(FeatureSummary)
	include(FetchContent)
	include(CTest)
endif()

# # # Top-Level Directories
# Check if this is the top-level project or not
if(CMAKE_SOURCE_DIR STREQUAL PROJECT_SOURCE_DIR)
	set(ZTD_TEXT_IS_TOP_LEVEL_PROJECT YES)
else()
	set(ZTD_TEXT_IS_TOP_LEVEL_PROJECT NO)
endif()

# Modify bad flags / change defaults if we are the top level
if(ZTD_TEXT_IS_TOP_LEVEL_PROJECT)
	if(CMAKE_SIZEOF_VOID_P EQUAL 4)
		set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}/x86/${CMAKE_BUILD_TYPE}/lib")
		set(CMAKE_LIBRARY_OUTPUT_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}/x86/${CMAKE_BUILD_TYPE}/bin")
		set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}/x86/${CMAKE_BUILD_TYPE}/bin")
	else()
		set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}/x64/${CMAKE_BUILD_TYPE}/lib")
		set(CMAKE_LIBRARY_OUTPUT_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}/x64/${CMAKE_BUILD_TYPE}/bin")
		set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}/x64/${CMAKE_BUILD_TYPE}/bin")
	endif()

	if(NOT DEFINED CMAKE_CXX_STANDARD)
		set(CMAKE_CXX_STANDARD 17)
	endif()

	if(NOT DEFINED CMAKE_C_STANDARD)
		set(CMAKE_C_STANDARD 11)
	endif()

	if(ZTD_TEXT_BENCHMARKS OR ZTD_TEXT_EXAMPLES OR ZTD_TEXT_TESTS OR ZTD_TEXT_SCRATCH)
		# normal flags
		check_compiler_flag(disable-permissive MSVC /permissive- GCC -pedantic)

		# Warning flags
		check_compiler_flag(warn-pedantic MSVC /permissive- GCC -pedantic)
		check_compiler_flag(warn-all MSVC /W4 GCC -Wall)
		check_compiler_flag(warn-errors MSVC /WX GCC -Werror)
		check_compiler_flag(warn-extra GCC -Wextra Clang -Wextra)
		check_compiler_flag(utf8-literal-encoding MSVC /execution-charset:utf-8 GCC -fexec-charset=utf-8)
		check_compiler_flag(utf8-source-encoding MSVC /source-charset:utf-8 GCC -finput-charset=utf-8)
		check_compiler_flag(extra-constexpr-depth MSVC /constexpr:depth2147483647 GCC -fconstexpr-depth=2147483647 Clang -fconstexpr-depth=2147483647)
		check_compiler_flag(extra-constexpr-steps MSVC /constexpr:steps2147483647 GCC -fconstexpr-ops-limit=2147483647 Clang -fconstexpr-steps=2147483647)
		check_compiler_flag(template-debugging-mode GCC -ftemplate-backtrace-limit=0)
	endif()
endif()

# # Options
option(ZTD_TEXT_CI "Whether or not we are in continuous integration mode" OFF)
option(ZTD_TEXT_READTHEDOCS "Whether or not we are building inside of ReadTheDocs" OFF)
option(ZTD_TEXT_TESTS "Enable build of tests" OFF)
option(ZTD_TEXT_DOCUMENTATION "Enable build of documentation" OFF)
option(ZTD_TEXT_DOCUMENTATION_NO_SPHINX "Turn off Sphinx usage (useful for ReadTheDocs builds)" OFF)
option(ZTD_TEXT_EXAMPLES "Enable build of examples" OFF)
option(ZTD_TEXT_BENCHMARKS "Enable build of benchmarks" OFF)
option(ZTD_TEXT_GENERATE_SINGLE "Enable generation of a single header and its target" OFF)
option(ZTD_TEXT_USE_CUNEICODE "Use ztd.cuneicode" OFF)

# # Dependencies
# ztd.idk
FetchContent_Declare(ztd.idk
	GIT_REPOSITORY https://github.com/soasis/idk.git
	GIT_TAG main)
FetchContent_MakeAvailable(ztd.idk)

# ztd.static_containers
FetchContent_Declare(ztd.static_containers
	GIT_REPOSITORY https://github.com/soasis/static_containers.git
	GIT_TAG main)
FetchContent_MakeAvailable(ztd.static_containers)

# ztd.cuneicode
set(ZTD_CUNEICODE_SIMDUTF_SHARED ${ZTD_TEXT_BENCHMARKS})
FetchContent_Declare(ztd.cuneicode
	GIT_REPOSITORY https://github.com/soasis/cuneicode.git
	GIT_TAG main)
FetchContent_MakeAvailable(ztd.cuneicode)

# ztd.platform
FetchContent_Declare(ztd.platform
	GIT_REPOSITORY https://github.com/soasis/platform.git
	GIT_TAG main)
FetchContent_MakeAvailable(ztd.platform)

if (ZTD_TEXT_BENCHAMRKS OR ZTD_TEXT_GENERATE_SINGLE OR ZTD_TEXT_DOCUMENTATION)
	find_package(Python3 REQUIRED COMPONENTS Interpreter)
endif()

# Main library declarations
file(GLOB_RECURSE ztd.text.includes CONFIGURE_DEPENDS include/*.hpp)

add_library(ztd.text INTERFACE)
add_library(ztd::text ALIAS ztd.text)
target_include_directories(ztd.text
	INTERFACE
	$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
	$<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>)
target_sources(ztd.text INTERFACE ${ztd.text.includes})
target_link_libraries(ztd.text
	INTERFACE
	ztd::idk
	ztd::static_containers
	ztd::platform
	ztd::cuneicode)
install(DIRECTORY include/
	DESTINATION ${CMAKE_INSTALL_INCLUDEDIR})

# # Config / Version packaging
# Version configurations
configure_package_config_file(
	cmake/ztd.text-config.cmake.in
	"${CMAKE_CURRENT_BINARY_DIR}/cmake/ztd.text/ztd.text-config.cmake"
	INSTALL_DESTINATION lib/cmake/ztd.text
	NO_CHECK_REQUIRED_COMPONENTS_MACRO)

write_basic_package_version_file(
	"${CMAKE_CURRENT_BINARY_DIR}/cmake/ztd.text/ztd.text-config-version.cmake"
	COMPATIBILITY SameMajorVersion)

export(TARGETS ztd.text
	FILE
	"${CMAKE_CURRENT_BINARY_DIR}/cmake/ztd.text/ztd.text-targets.cmake")

if(ZTD_TEXT_GENERATE_SINGLE)
	add_subdirectory(single)
endif()

# # Benchmarks, Tests, Examples and Documentation
# Example and/or benchmark dependencies
if (ZTD_TEXT_EXAMPLES OR ZTD_TEXT_BENCHMARKS)
	# # Fetch dependencies for either benchmarks and/or examples
	function(simdutf_dependency_jail)
		# simdutf
		set(SIMDUTF_BENCHMARKS OFF CACHE INTERNAL "")
		set(BUILD_TESTING OFF CACHE INTERNAL "")
		FetchContent_Declare(
			simdutf
			GIT_REPOSITORY https://github.com/simdutf/simdutf
		)
		FetchContent_MakeAvailable(simdutf)
	endfunction()
	simdutf_dependency_jail()

	function(boost_text_dependency_jail)
		# boost.text
		set(BOOST_TEXT_PERF OFF CACHE INTERNAL "")
		set(BOOST_TEXT_EXAMPLES OFF CACHE INTERNAL "")
		set(BOOST_TEXT_TESTS OFF CACHE INTERNAL "")
		set(BOOST_TEXT_FETCH_BOOST ON CACHE INTERNAL "")
		set(BOOST_TEXT_INSTALL OFF CACHE INTERNAL "")
		set(BUILD_TESTING OFF CACHE INTERNAL "")
		FetchContent_Declare(
			boost.text
			GIT_REPOSITORY https://github.com/ThePhD/text
		)
		FetchContent_MakeAvailable(boost.text)
	endfunction()
	boost_text_dependency_jail()
endif()

if(ZTD_TEXT_TESTS)
	include(CTest)
	add_subdirectory(tests)
endif()

if(ZTD_TEXT_DOCUMENTATION)
	add_subdirectory(documentation)
endif()

if(ZTD_TEXT_BENCHMARKS)
	add_subdirectory(benchmarks)
endif()

if(ZTD_TEXT_EXAMPLES)
	add_subdirectory(examples)
endif()

# For quick debugging and development only: don't peek! 🙈
mark_as_advanced(ZTD_TEXT_SCRATCH)

if(ZTD_TEXT_SCRATCH)
	add_executable(scratch main.cpp)
	target_link_libraries(scratch PRIVATE ztd::text)
	target_include_directories(scratch PRIVATE tests/shared/include)
	target_compile_options(scratch
		PRIVATE
		${--template-debugging-mode}
		${--disable-permissive}
		${--warn-pedantic}
		${--warn-all}
		${--warn-extra}
		${--warn-errors})
endif()
