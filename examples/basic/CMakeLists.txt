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

set(ztd-text-examples-data-source ${CMAKE_CURRENT_SOURCE_DIR}/data)
set(ztd-text-examples-data-destination ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/data)

file(GLOB_RECURSE ztd-text-examples-data-source-files LIST_DIRECTORIES false 
	RELATIVE ${ztd-text-examples-data-source} CONFIGURE_DEPENDS 
	./data/*.*)

list(SUBLIST ztd-text-examples-data-source-files 0 -1 ztd-text-examples-data-destination-files)
list(TRANSFORM ztd-text-examples-data-source-files PREPEND "${ztd-text-examples-data-source}/")
list(TRANSFORM ztd-text-examples-data-destination-files PREPEND "${ztd-text-examples-data-destination}/")

add_custom_command(OUTPUT ${ztd-text-examples-data-destination-files}
	COMMAND ${CMAKE_COMMAND} -E copy_directory ${ztd-text-examples-data-source} ${ztd-text-examples-data-destination}
	COMMENT "[ztd.text/examples] Copying data files from \"${ztd-text-examples-data-source}\" used in examples over to \"${ztd-text-examples-data-destination}\"..."
	DEPENDS ${ztd-text-examples-data-source-files}
)

add_custom_target(ztd.text.examples.basic.data
	DEPENDS ${ztd-text-examples-data-destination-files}
)

file(GLOB_RECURSE ztd.text.examples.basic.sources LIST_DIRECTORIES false 
	RELATIVE ${CMAKE_CURRENT_SOURCE_DIR}/source CONFIGURE_DEPENDS 
	source/*.cpp)

foreach (example_source_name ${ztd.text.examples.basic.sources})
	set(example_source_file source/${example_source_name})
	set(example_target ztd.text.examples.basic.${example_source_name})
	add_executable(${example_target} ${example_source_file})
	if (MSVC)
		target_compile_options(${example_target}
			PRIVATE /std:c++latest /utf-8 /permissive-)
	else()
		target_compile_options(${example_target}
			PRIVATE -std=c++2a -Wall -Werror -Wpedantic)
	endif()
	target_link_libraries(${example_target}
		PRIVATE
		ztd::text
		${CMAKE_DL_LIBS}
	)
	add_dependencies(${example_target} ztd.text.examples.basic.data)
	if (ZTD_TEXT_TESTS)
		add_test(NAME ${example_target} COMMAND ${example_target} WORKING_DIRECTORY ${ztd-text-examples-data-destination})
	endif()
endforeach()
