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

set(ztd-text-examples-shift_jis-data-source ${CMAKE_CURRENT_SOURCE_DIR}/data)
set(ztd-text-examples-shift_jis-data-destination ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/data)

file(GLOB_RECURSE ztd-text-examples-shift_jis-data-source-files LIST_DIRECTORIES false 
	RELATIVE ${ztd-text-examples-shift_jis-data-source} CONFIGURE_DEPENDS 
	./data/*.*)

list(SUBLIST ztd-text-examples-shift_jis-data-source-files 0 -1 ztd-text-examples-shift_jis-data-destination-files)
list(TRANSFORM ztd-text-examples-shift_jis-data-source-files PREPEND "${ztd-text-examples-shift_jis-data-source}/")
list(TRANSFORM ztd-text-examples-shift_jis-data-destination-files PREPEND "${ztd-text-examples-shift_jis-data-destination}/")

add_custom_command(OUTPUT ${ztd-text-examples-shift_jis-data-destination-files}
	COMMAND ${CMAKE_COMMAND} -E copy_directory ${ztd-text-examples-shift_jis-data-source} ${ztd-text-examples-shift_jis-data-destination}
	COMMENT "[ztd.text/examples] Copying data files from \"${ztd-text-examples-shift_jis-data-source}\" used in examples/shift_jis over to \"${ztd-text-examples-shift_jis-data-destination}\"..."
	DEPENDS ${ztd-text-examples-shift_jis-data-source-files}
)

add_custom_target(ztd.text.examples.shift_jis.data
	DEPENDS ${ztd-text-examples-shift_jis-data-destination-files}
)

file(GLOB_RECURSE ztd.text.examples.shift_jis.sources LIST_DIRECTORIES false 
	RELATIVE ${CMAKE_CURRENT_SOURCE_DIR} CONFIGURE_DEPENDS 
	source/*.cpp)

add_executable(ztd.text.examples.shift_jis ${ztd.text.examples.shift_jis.sources})
if (MSVC)
	target_compile_options(ztd.text.examples.shift_jis
		PRIVATE /std:c++latest /utf-8 /permissive-)
else()
	target_compile_options(ztd.text.examples.shift_jis
		PRIVATE -std=c++2a -Wall -Werror -Wpedantic)
endif()
target_include_directories(ztd.text.examples.shift_jis
	PRIVATE
	../include
	include)
target_link_libraries(ztd.text.examples.shift_jis
	PRIVATE
	ztd::text
	${CMAKE_DL_LIBS}
)

add_dependencies(ztd.text.examples.shift_jis ztd.text.examples.shift_jis.data)
if (ZTD_TEXT_TESTS)
	add_test(NAME ztd.text.examples.shift_jis COMMAND ztd.text.examples.shift_jis WORKING_DIRECTORY ${ztd-text-examples-shift_jis-data-destination})
endif()
