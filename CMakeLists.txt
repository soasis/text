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
# =============================================================================

cmake_minimum_required(VERSION 3.13.0)

# # Project declaration
# informs about the project, gives a description, version and MOST IMPORTANTLY
# the languages the project is going to use. Required.
set(ZTD_TEXT_PROJECT_NAME ztd.text)
set(ZTD_TEXT_PROJECT_VERSION 0.0.0)
set(ZTD_TEXT_PROJECT_DESCRIPTION "A spicy text library.")
project(${ZTD_TEXT_PROJECT_NAME} VERSION ${ZTD_TEXT_PROJECT_VERSION} DESCRIPTION ${ZTD_TEXT_PROJECT_DESCRIPTION} LANGUAGES C CXX)

# # Modules
# Include modules useful to the project, whether locally made in our own cmake DIRECTORY
# our from the standard cmake libraries
# Add home-rolled modules path to front of module path list
set(CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake" "${CMAKE_MODULE_PATH}")

# # # Top-Level Directories
# Check if this is the top-level project or not
if(CMAKE_SOURCE_DIR STREQUAL PROJECT_SOURCE_DIR)
    set(ZTD_TEXT_IS_TOP_LEVEL_PROJECT true)
else()
    set(ZTD_TEXT_IS_TOP_LEVEL_PROJECT false)
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

	if (MSVC)
		string(REGEX REPLACE "/W[0-4]" "" CMAKE_C_FLAGS ${CMAKE_C_FLAGS})
		string(REGEX REPLACE "/W[0-4]" "" CMAKE_CXX_FLAGS ${CMAKE_CXX_FLAGS})
	endif()
endif()

# # Include standard modules
include(CMakeDependentOption)
include(CMakePackageConfigHelpers)
include(GNUInstallDirs)

# # Options
option(ZTD_TEXT_CI "Whether or not we are in continuous integration mode" OFF)
option(ZTD_TEXT_TESTS "Enable build of tests" OFF)
option(ZTD_TEXT_DOCUMENTATION "Enable build of documentation" OFF)
option(ZTD_TEXT_DOCUMENTATION_NO_SPHINX "Turn off Sphinx usage (useful for ReadTheDocs builds)" OFF)
option(ZTD_TEXT_EXAMPLES "Enable build of examples" OFF)
option(ZTD_TEXT_BENCHMARKS "Enable build of benchmarks" OFF)
option(ZTD_TEXT_GENERATE_SINGLE "Enable generation of a single header and its target" OFF)
option(ZTD_TEXT_USE_CUNEICODE "Enable generation of a single header and its target" OFF)

# # Dependencies
# # Dependencies (test-specific)
if (EXISTS ${CMAKE_CURRENT_SOURCE_DIR}/vendor/Catch2)
	set(ztd_text_has_catch2 ON)
	add_subdirectory(vendor/Catch2)
else()
	set(ztd_text_has_catch2 OFF)
endif()
if (EXISTS ${CMAKE_CURRENT_SOURCE_DIR}/vendor/cuneicode)
	set(ztd_text_has_cuneicode ON)
	add_subdirectory(vendor/cuneicode)
else()
	set(ztd_text_has_cuneicode OFF)
endif()

file(GLOB ztd.text.includes CONFIGURE_DEPENDS include/*.hpp)

add_library(ztd.text INTERFACE)
add_library(ztd::text ALIAS ztd.text)
# Not quite yet
# target_link_libraries(ztd.text INTERFACE ztd::cuneicode)
target_include_directories(ztd.text INTERFACE 
	$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
	$<INSTALL_INTERFACE:include>
)
if (ZTD_TEXT_IS_TOP_LEVEL_PROJECT)
	if (MSVC)
		target_compile_options(ztd.text INTERFACE
			/std:c++latest
			/permissive-
		)
	else()
		target_compile_options(ztd.text INTERFACE
			-std=c++2a
		)
	endif()
endif()
target_sources(ztd.text INTERFACE ${ztd.text.includes})
#set_target_properties(ztd.text
#	PROPERTIES
#	EXPORT_NAME ztd::text
#)

if (ZTD_TEXT_GENERATE_SINGLE)
	add_subdirectory(single)
endif(ZTD_TEXT_GENERATE_SINGLE)

if (ZTD_TEXT_SCRATCH)
	add_executable(scratch main.cpp)
	target_link_libraries(scratch PRIVATE ztd::text)
	if (MSVC)
		target_compile_options(scratch
			PUBLIC
			/std:c++latest
			/permissive-
		)
	else()
		target_compile_options(scratch
			PUBLIC
			-std=c++2a -Wall -Werror -Wpedantic
		)
	endif()
endif()

# # Benchmarks, Tests, Examples
if (ZTD_TEXT_TESTS)
	enable_testing()
endif()
if (ZTD_TEXT_TESTS AND ztd_text_has_catch2)
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

# # Config / Version packaging
# Version configurations
#configure_package_config_file(
#	cmake/ztd-config.cmake.in
#	"${CMAKE_CURRENT_BINARY_DIR}/cmake/ztd.text/ztd.text-config.cmake"
#	INSTALL_DESTINATION lib/cmake/ztd.text
#	NO_CHECK_REQUIRED_COMPONENTS_MACRO)

#write_basic_package_version_file(
#	"${CMAKE_CURRENT_BINARY_DIR}/cmake/ztd.text/ztd.text-config-version.cmake"
#	COMPATIBILITY AnyNewerVersion)

#export(TARGETS ztd.text FILE "${CMAKE_CURRENT_BINARY_DIR}/cmake/ztd.text/ztd.text-targets.cmake")
