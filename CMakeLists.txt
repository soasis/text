# =============================================================================
#
# ztd.text
# Copyright © 2021 JeanHeyd "ThePhD" Meneide and Shepherd's Oasis, LLC
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
#		http:#www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.
#
# ============================================================================>

cmake_minimum_required(VERSION 3.15.0)
cmake_policy(VERSION 3.15)
# # Project declaration
# informs about the project, gives a description, version and MOST IMPORTANTLY
# the languages the project is going to use. Required.

project(ztd.text
  VERSION 0.0.0
  DESCRIPTION "A spicy text library."
  HOMEPAGE_URL "https://ztdtext.readthedocs.io/en/latest/"
  LANGUAGES C CXX)

# # Modules
# Include modules useful to the project, whether locally made in our own cmake DIRECTORY
# our from the standard cmake libraries
# Add home-rolled modules path to front of module path list
list(APPEND CMAKE_MODULE_PATH "${PROJECT_SOURCE_DIR}/cmake")

# # # Top-Level Directories
# Check if this is the top-level project or not
if(CMAKE_SOURCE_DIR STREQUAL PROJECT_SOURCE_DIR)
	set(ZTD_TEXT_IS_TOP_LEVEL_PROJECT YES)
else()
	set(ZTD_TEXT_IS_TOP_LEVEL_PROJECT NO)
endif()

# Modify bad flags / change defaults if we are the top level
if (ZTD_TEXT_IS_TOP_LEVEL_PROJECT)
	if (CMAKE_SIZEOF_VOID_P EQUAL 4)
		set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}/x86/${CMAKE_BUILD_TYPE}/${CFG_INT_DIR}/lib")
		set(CMAKE_LIBRARY_OUTPUT_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}/x86/${CMAKE_BUILD_TYPE}/${CFG_INT_DIR}/bin")
		set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}/x86/${CMAKE_BUILD_TYPE}/${CFG_INT_DIR}/bin")
	else()
		set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}/x64/${CMAKE_BUILD_TYPE}/${CFG_INT_DIR}/lib")
		set(CMAKE_LIBRARY_OUTPUT_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}/x64/${CMAKE_BUILD_TYPE}/${CFG_INT_DIR}/bin")
		set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}/x64/${CMAKE_BUILD_TYPE}/${CFG_INT_DIR}/bin")
	endif()
endif()

# # Include standard modules
include(CMakePackageConfigHelpers)
include(CheckCXXCompilerFlag)
include(CMakeDependentOption)
include(GNUInstallDirs)
include(FetchContent)

# # Options
option(ZTD_TEXT_CI "Whether or not we are in continuous integration mode" OFF)
option(ZTD_TEXT_TESTS "Enable build of tests" OFF)
option(ZTD_TEXT_DOCUMENTATION "Enable build of documentation" OFF)
option(ZTD_TEXT_DOCUMENTATION_NO_SPHINX "Turn off Sphinx usage (useful for ReadTheDocs builds)" OFF)
option(ZTD_TEXT_EXAMPLES "Enable build of examples" OFF)
option(ZTD_TEXT_BENCHMARKS "Enable build of benchmarks" OFF)
option(ZTD_TEXT_GENERATE_SINGLE "Enable generation of a single header and its target" OFF)
option(ZTD_TEXT_USE_CUNEICODE "Enable generation of a single header and its target" OFF)

if (NOT CMAKE_CXX_STANDARD GREATER_EQUAL 20)
	set(CMAKE_CXX_STANDARD 20)
endif()

# # Dependencies
# # Dependencies (test-specific)
if (EXISTS ${CMAKE_CURRENT_SOURCE_DIR}/vendor/cuneicode)
	set(ztd_text_has_cuneicode ON)
	add_subdirectory(vendor/cuneicode)
else()
	set(ztd_text_has_cuneicode OFF)
endif()

file(GLOB_RECURSE ztd.text.includes CONFIGURE_DEPENDS include/*.hpp)

# Check environment/prepare generator expressions
check_cxx_compiler_flag(/permissive- ZTD_TEXT_MSVC_DISABLE_PERMISSIVE)

check_cxx_compiler_flag(-Wpedantic ZTD_TEXT_DIAGNOSTIC_PEDANTIC)
check_cxx_compiler_flag(-Werror ZTD_TEXT_DIAGNOSTIC_ERRORS)
check_cxx_compiler_flag(-Wall ZTD_TEXT_DIAGNOSTIC_DEFAULTS)
check_cxx_compiler_flag(-W4 ZTD_TEXT_MSVC_DIAGNOSTIC_DEFAULTS)

string(CONCAT ztd-text-is-top-level $<STREQUAL:${PROJECT_NAME},${CMAKE_PROJECT_NAME}>)
string(CONCAT --disable-permissive $<
	$<AND:
		$<BOOL:${ZTD_TEXT_MSVC_DISABLE_PERMISSIVE}>,
		${ztd-text-is-top-level}
	>:/permissive-
>)
string(CONCAT --utf8-literal-encoding
	$<IF:$<BOOL:${MSVC}>,
		/execution-charset:utf-8,
		-fexec-charset=utf-8
	>
)
string(CONCAT --utf8-source-encoding
	$<IF:$<BOOL:${MSVC}>,
		/source-charset:utf-8,
		-finput-charset=utf-8
	>
)
string(CONCAT --extra-constexpr-power
	$<IF:$<BOOL:${MSVC}>,
		/constexpr:steps2147483647,
		$<IF:$<BOOL:${CLANG}>,
			-fconstexpr-steps=2147483647,
			-fconstexpr-depth=2147483647
		>
	>
)
string(CONCAT ztd-use-cuneicode $<
	$<AND:
		$<BOOL:${ZTD_TEXT_USE_CUNEICODE}>,
		$<BOOL:${ztd_text_has_cuneicode}>
	>:$<TARGET_NAME_IF_EXISTS:ztd::cuneicode>
>)

string(CONCAT --warn-pedantic $<$<BOOL:${ZTD_TEXT_DIAGNOSTIC_PEDANTIC}>:-Wpedantic>)
string(CONCAT --warn-default $<
	$<AND:
		$<BOOL:${ZTD_TEXT_DIAGNOSTIC_DEFAULTS}>,
		$<NOT:$<BOOL:MSVC>>
	>:-Wall
>)
string(CONCAT --warn-default $<
	$<AND:
		$<BOOL:${ZTD_TEXT_MSVC_DIAGNOSTIC_DEFAULTS}>,
		$<BOOL:MSVC>
	>:-W4
>)
string(CONCAT --deny-errors $<$<BOOL:${ZTD_TEXT_DIAGNOSTIC_ERRORS}>:-Werror>)


# Main library declarations
add_library(ztd.text INTERFACE)
add_library(ztd::text ALIAS ztd.text)
target_include_directories(ztd.text
	INTERFACE
		$<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/include>
		$<INSTALL_INTERFACE:include>)
target_compile_features(ztd.text INTERFACE $<${ztd-text-is-top-level}:cxx_std_20>)
target_compile_options(ztd.text INTERFACE ${--disable-permissive})
target_sources(ztd.text INTERFACE ${ztd.text.includes})
target_link_libraries(ztd.text INTERFACE ${ztd-use-cuneicode})
install(DIRECTORY include/
	DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
)

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


if (ZTD_TEXT_GENERATE_SINGLE)
	add_subdirectory(single)
endif()

# # Benchmarks, Tests, Examples
if (ZTD_TEXT_TESTS)
	include(CTest)
	add_subdirectory(tests)
endif()

if (ZTD_TEXT_DOCUMENTATION)
	add_subdirectory(documentation)
endif()
if (ZTD_TEXT_BENCHMARKS)
	add_subdirectory(benchmarks)
endif()
if (ZTD_TEXT_EXAMPLES)
	add_subdirectory(examples)
endif()

if (ZTD_TEXT_SCRATCH)
	add_executable(scratch main.cpp)
	target_link_libraries(scratch PRIVATE ztd::text)
	target_compile_options(scratch
		PRIVATE
		${--warn-pedantic}
		${--warn-default}
		${--deny-errors})
	target_compile_features(scratch PRIVATE cxx_std_20)
endif()
