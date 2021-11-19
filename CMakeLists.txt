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
#		http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.
#
# ============================================================================>

cmake_minimum_required(VERSION 3.16.0)
cmake_policy(VERSION 3.16)
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

if (ZTD_TEXT_READTHEDOCS)
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
if (ZTD_TEXT_IS_TOP_LEVEL_PROJECT)
	if (CMAKE_SIZEOF_VOID_P EQUAL 4)
		set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}/x86/${CMAKE_BUILD_TYPE}/lib")
		set(CMAKE_LIBRARY_OUTPUT_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}/x86/${CMAKE_BUILD_TYPE}/bin")
		set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}/x86/${CMAKE_BUILD_TYPE}/bin")
	else()
		set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}/x64/${CMAKE_BUILD_TYPE}/lib")
		set(CMAKE_LIBRARY_OUTPUT_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}/x64/${CMAKE_BUILD_TYPE}/bin")
		set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}/x64/${CMAKE_BUILD_TYPE}/bin")
	endif()

	if (NOT DEFINED CMAKE_CXX_STANDARD)
		set(CMAKE_CXX_STANDARD 17)
	endif()

	if (NOT DEFINED CMAKE_C_STANDARD)
		set(CMAKE_C_STANDARD 11)
	endif()

	if (ZTD_TEXT_BENCHMARKS OR ZTD_TEXT_EXAMPLES OR ZTD_TEXT_TESTS OR ZTD_TEXT_SCRATCH)
		# normal flags
		check_compiler_flag(disable-permissive MSVC /permissive- GCC -pedantic)
		# Warning flags
		check_compiler_flag(warn-pedantic MSVC /permissive- GCC -pedantic)
		check_compiler_flag(warn-all MSVC /W4 GCC -Wall)
		check_compiler_flag(warn-errors MSVC /WX GCC -Werror)
		check_compiler_flag(warn-extra GCC -Wextra CLANG -Wextra)
		check_compiler_flag(utf8-literal-encoding MSVC /execution-charset:utf-8 GCC -fexec-charset=utf-8)
		check_compiler_flag(utf8-source-encoding MSVC /source-charset:utf-8 GCC -finput-charset=utf-8)
		check_compiler_flag(extra-constexpr-depth MSVC /constexpr:depth2147483647 GCC -fconstexpr-depth=2147483647 CLANG -fconstexpr-depth=2147483647)
		check_compiler_flag(extra-constexpr-steps MSVC /constexpr:steps2147483647 GCC -fconstexpr-ops-limit=2147483647 CLANG -fconstexpr-steps=2147483647)
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
option(ZTD_TEXT_USE_LIBICONV "Use libiconv" OFF)
option(ZTD_TEXT_USE_STATIC_LIBICONV "Use libiconv and link against it even if it's a static library" OFF)

# # Dependencies
# ztd.idk
FetchContent_Declare(ztd.idk
	GIT_REPOSITORY https://github.com/soasis/idk.git
	GIT_TAG main)
FetchContent_MakeAvailable(ztd.idk)

# ztd.cuneicode
if (ZTD_TEXT_USE_CUNEICODE)
	FetchContent_Declare(ztd.cuneicode
		GIT_REPOSITORY https://github.com/soasis/cuneicode.git
		GIT_TAG main)
	FetchContent_MakeAvailable(ztd.cuneicode)
	set(ztd-text-cuneicode ztd::cuneicode)
endif()

# iconv - static and dynamic
if (NOT TARGET Iconv::Iconv AND ZTD_TEXT_USE_LIBICONV)
	# because of iconv's license, using it as a static dependency
	# is tricky: therefore, it's only enabled for folks who explicitly asked for it,
	# regardless of whether it is available or not
	find_package(Iconv)
endif()
# define generator expressions for each moment
string(CONCAT ztd-text-libiconv-define
	$<IF:$<BOOL:${ZTD_TEXT_USE_LIBICONV}>,
		ZTD_LIBICONV=1,
		ZTD_LIBICONV=0
	>
)
string(CONCAT ztd-text-libiconv-load-define
	$<IF:$<AND:$<BOOL:${Iconv_FOUND}>,$<BOOL:${ZTD_TEXT_USE_LIBICONV}>>,
		ZTD_LIBICONV_LOAD=0,
		ZTD_LIBICONV_LOAD=1
	>
)
string(CONCAT ztd-text-libiconv-header-define
	$<IF:$<AND:$<BOOL:${Iconv_FOUND}>,$<BOOL:${ZTD_TEXT_USE_LIBICONV}>>,
		ZTD_ICONV_H=1,
		ZTD_ICONV_H=0
	>
)
string(CONCAT ztd-text-libiconv
	$<TARGET_NAME_IF_EXISTS:Iconv::Iconv>
)
string(CONCAT ztd-text-libiconv-dl
	$<$<BOOL:ztd-text-libiconv>:${CMAKE_DL_LIBS}>
)
string(CONCAT ztd-text-static-libiconv-define
	$<IF:
		$<AND:$<BOOL:${Iconv_FOUND}>, $<BOOL:${ZTD_TEXT_USE_STATIC_LIBICONV}>,
			$<STREQUAL:
				$<$<BOOL:$<TARGET_NAME_IF_EXISTS:Iconv::Iconv>>:$<TARGET_PROPERTY:Iconv::Iconv,TYPE>>,
				STATIC_LIBRARY
			>
		>,
		ZTD_LIBICONV_STATIC=1,
		ZTD_LIBICONV_STATIC=0
	>
)
string(CONCAT ztd-text-dynamic-libiconv-define
	$<IF:
		$<AND:$<BOOL:Iconv_FOUND>, $<BOOL:ZTD_TEXT_USE_LIBICONV>,
			$<STREQUAL:
				$<$<BOOL:$<TARGET_NAME_IF_EXISTS:Iconv::Iconv>>:$<TARGET_PROPERTY:Iconv::Iconv,TYPE>>,
				SHARED_LIBRARY
			>
		>,
		ZTD_LIBICONV_DYNAMIC=1,
		ZTD_LIBICONV_DYNAMIC=0
	>
)

# Main library declarations
file(GLOB_RECURSE ztd.text.includes CONFIGURE_DEPENDS include/*.hpp)

add_library(ztd.text INTERFACE)
add_library(ztd::text ALIAS ztd.text)
target_include_directories(ztd.text
	INTERFACE
		$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
		$<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>)
target_sources(ztd.text INTERFACE ${ztd.text.includes})
target_compile_definitions(ztd.text
	INTERFACE
	${ztd-text-libiconv-define}
	${ztd-text-libiconv-load-define}
	${ztd-text-libiconv-header-define}
	${ztd-text-static-libiconv-define}
	${ztd-text-dynamic-libiconv-define})
target_link_libraries(ztd.text
	INTERFACE
	${ztd-text-libiconv-dl}
	ztd::idk
	${ztd-text-libiconv}
	${ztd-text-cuneicode})
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


if (ZTD_TEXT_GENERATE_SINGLE)
	add_subdirectory(single)
endif()

# # Benchmarks, Tests, Examples and Documentation
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

# For quick debugging and development only: don't peek! 🙈
mark_as_advanced(ZTD_TEXT_SCRATCH)
if (ZTD_TEXT_SCRATCH)
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
