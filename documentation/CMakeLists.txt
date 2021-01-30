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

find_package(Doxygen REQUIRED)
find_package(Python3 REQUIRED)
if (NOT ZTD_TEXT_DOCUMENTATION_NO_SPHINX)
	find_package(Sphinx REQUIRED)
endif()

# Find all the public headers
file(GLOB_RECURSE ztd_text_public_headers LIST_DIRECTORIES false CONFIGURE_DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/../include/**.*)
# make a list copy for use with dependency tracking later down below
list(LENGTH ztd_text_public_headers ztd_text_public_headers_size)
list(SUBLIST ztd_text_public_headers 0 ${ztd_text_public_headers_size} ztd_text_doxygen_public_headers)
# Remove every single public header
list(FILTER ztd_text_doxygen_public_headers EXCLUDE REGEX /detail/*)
# separate text with spaces and surround them with quotes for Doxygen to understand them all
list(JOIN ztd_text_doxygen_public_headers "\" \"" ztd_text_doxygen_public_headers)
set(ztd_text_doxygen_public_headers \"${ztd_text_doxygen_public_headers}\")

set(ZTD_TEXT_DOXYGEN_PROJECT_NAME ${ZTD_TEXT_PROJECT_NAME})
set(ZTD_TEXT_DOXYGEN_PROJECT_VERSION ${ZTD_TEXT_PROJECT_VERSION})
set(ZTD_TEXT_DOXYGEN_PROJECT_DESCRIPTION ${ZTD_TEXT_PROJECT_DESCRIPTION})
set(ZTD_TEXT_DOXYGEN_INPUT ${ztd_text_doxygen_public_headers})
set(ZTD_TEXT_DOXYGEN_OUTPUT_DIR ${CMAKE_CURRENT_BINARY_DIR}/doxygen)
set(ZTD_TEXT_DOXYGEN_XML_OUTPUT_DIR ${ZTD_TEXT_DOXYGEN_OUTPUT_DIR}/xml)
set(ZTD_TEXT_DOXYGEN_HTML_OUTPUT_DIR ${ZTD_TEXT_DOXYGEN_OUTPUT_DIR}/html)
set(ZTD_TEXT_DOXYGEN_INDEX_FILE ${ZTD_TEXT_DOXYGEN_XML_OUTPUT_DIR}/index.xml)
set(ZTD_TEXT_DOXYFILE_IN ${CMAKE_CURRENT_SOURCE_DIR}/Doxyfile.in)
set(ZTD_TEXT_DOXYFILE_OUT ${CMAKE_CURRENT_BINARY_DIR}/Doxyfile)
set(ZTD_TEXT_SPHINX_SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/source)
set(ZTD_TEXT_SPHINX_BUILD_DIR ${CMAKE_CURRENT_BINARY_DIR}/sphinx)

# Get all Sphinx source files
file(GLOB_RECURSE ztd_text_sphinx_sources LIST_DIRECTORIES false CONFIGURE_DEPENDS ${ZTD_TEXT_SPHINX_SOURCE_DIR}/**.*)

#Replace variables inside @@ with the current values
configure_file(${ZTD_TEXT_DOXYFILE_IN} ${ZTD_TEXT_DOXYFILE_OUT} @ONLY)

# Ensure Doxygen won't crash because it doesn't create directories for us
file(MAKE_DIRECTORY ${ZTD_TEXT_DOXYGEN_OUTPUT_DIR})
file(MAKE_DIRECTORY ${ZTD_TEXT_DOXYGEN_XML_OUTPUT_DIR})
file(MAKE_DIRECTORY ${ZTD_TEXT_DOXYGEN_HTML_OUTPUT_DIR})

add_custom_command(OUTPUT ${ZTD_TEXT_DOXYGEN_INDEX_FILE}
	COMMAND ${DOXYGEN_EXECUTABLE} ${ZTD_TEXT_DOXYFILE_OUT}
	MAIN_DEPENDENCY ${ZTD_TEXT_DOXYFILE_IN} ${ZTD_TEXT_DOXYFILE_OUT}
	DEPENDS ${ztd_text_public_headers}
	COMMENT "[ztd.text/documentation] Generating Doxygen XML..."
)
add_custom_target(ztd.text.documentation.doxygen ALL DEPENDS ${ZTD_TEXT_DOXYGEN_INDEX_FILE})

if (NOT ZTD_TEXT_DOCUMENTATION_NO_SPHINX)
	add_custom_target(ztd.text.documentation.sphinx ALL
		COMMAND ${SPHINX_EXECUTABLE} -b html ${ZTD_TEXT_SPHINX_SOURCE_DIR} ${ZTD_TEXT_SPHINX_BUILD_DIR}
			-Dbreathe_projects.ztd.text=${ZTD_TEXT_DOXYGEN_XML_OUTPUT_DIR}
		WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
		DEPENDS ${ZTD_TEXT_DOXYGEN_INDEX_FILE} ${ztd_text_sphinx_sources}
		COMMENT "[ztd.text/documentation] Generating Sphinx documentation..."
	)

	add_dependencies(ztd.text.documentation.sphinx ztd.text.documentation.doxygen)
endif()

# For the install target
include(GNUInstallDirs)
install(DIRECTORY ${ZTD_TEXT_SPHINX_BUILD_DIR}
	DESTINATION ${CMAKE_INSTALL_DOCDIR}
)
