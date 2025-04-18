# =============================================================================
#
# ztd.text
# Copyright © JeanHeyd "ThePhD" Meneide and Shepherd's Oasis, LLC
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
# https://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.
#
# ============================================================================>

# # # # ztd.text, single
# # # Required minimum version statement
cmake_minimum_required(VERSION 3.31.0)

# # # ztd.text single generation header
# generate the single header if we can find the python3 interpreter
if (NOT Python3_Interpreter_FOUND)
	message(FATAL_ERROR "ztd.text single header cannot be generated as python 3 has not been found: install or set the python 3 interpreter to generate the single header")
endif()

# to generate, need all of the existing header files
file(GLOB_RECURSE ztd.text.generated_header_sources LIST_DIRECTORIES false CONFIGURE_DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/../include/**/*.*)
add_custom_command(
	COMMAND ${CMAKE_COMMAND} -E make_directory "${CMAKE_CURRENT_BINARY_DIR}/include/ztd/text"
	COMMAND Python3::Interpreter "${CMAKE_CURRENT_SOURCE_DIR}/single.py" --input "${CMAKE_CURRENT_SOURCE_DIR}/../include" --output "${CMAKE_CURRENT_BINARY_DIR}/include/ztd/text.hpp"
	DEPENDS "${CMAKE_CURRENT_SOURCE_DIR}/single.py" "${ztd.text.generated_header_sources}"
	OUTPUT "${CMAKE_CURRENT_BINARY_DIR}/include/ztd/text.hpp" "${CMAKE_CURRENT_BINARY_DIR}/include/ztd/text/forward.hpp")
add_custom_target(ztd.text.single_header_generator ALL
	DEPENDS "${CMAKE_CURRENT_SOURCE_DIR}/single.py"
	"${CMAKE_CURRENT_BINARY_DIR}/include/ztd/text.hpp" "${CMAKE_CURRENT_BINARY_DIR}/include/ztd/text/forward.hpp"
	"${ztd.text.generated_header_sources}")

# # # ztd.text generated single header library
add_library(ztd.text.single INTERFACE)
add_library(ztd::text::single ALIAS ztd.text.single)
set_target_properties(ztd.text.single
	PROPERTIES
	EXPORT_NAME ztd::text::single)
target_include_directories(ztd.text.single
	INTERFACE
		$<BUILD_INTERFACE:${CMAKE_CURRENT_BINARY_DIR}/include>
		$<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}/single>)
add_dependencies(ztd.text.single ztd.text.single_header_generator)

install(FILES
	"${CMAKE_CURRENT_BINARY_DIR}/include/ztd/text.hpp"
	DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/single/ztd)
install(FILES
	"${CMAKE_CURRENT_BINARY_DIR}/include/ztd/text/forward.hpp"
	DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/single/ztd/text)
