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
#		http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.
#
# ============================================================================>

file(GLOB_RECURSE ztd.text.tests.tests.compile_fails.sources
	LIST_DIRECTORIES false 
	RELATIVE ${CMAKE_CURRENT_SOURCE_DIR}/source CONFIGURE_DEPENDS 
	source/*.cpp)

foreach (test_name ${ztd.text.tests.tests.compile_fails.sources})
	set(test_file source/${test_name})
	set(test_target ztd.text.tests.compile_fails.${test_name})
	add_executable(${test_target} ${test_file})
	target_compile_options(${test_target}
		PRIVATE
		${--utf8-literal-encoding}
		${--utf8-source-encoding}
		${--disable-permissive}
		${--warn-pedantic}
		${--warn-all}
		${--warn-extra}
		${--warn-errors})
	target_link_libraries(${test_target}
		PRIVATE
		ztd::text
		${CMAKE_DL_LIBS})
	set_target_properties(${test_target}
		PROPERTIES
		EXCLUDE_FROM_ALL TRUE
		EXCLUDE_FROM_DEFAULT_BUILD TRUE)
	add_test(NAME ${test_target}
		COMMAND cmake --build . --target ${test_target}
		WORKING_DIRECTORY ${CMAKE_BINARY_DIR})
	set_tests_properties(${test_target}
		PROPERTIES 
		WILL_FAIL true)
endforeach()
