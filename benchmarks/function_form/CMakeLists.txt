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

# # Benchmarks
file(GLOB_RECURSE ztd.text.benchmarks.function_form.sources
	LIST_DIRECTORIES FALSE
	CONFIGURE_DEPENDS
	source/*.c source/*.cpp)

add_executable(ztd.text.benchmarks.function_form ${ztd.text.benchmarks.function_form.sources})
target_compile_options(ztd.text.benchmarks.function_form
	PRIVATE
	${--utf8-literal-encoding}
	${--utf8-source-encoding}
	${--disable-permissive}
	${--updated-cpp-version-flag}
	${--warn-pedantic}
	${--warn-all}
	${--warn-extra}
	${--allow-alignas-extra-padding}
	# MSVC is BROKEN
	# ${--warn-errors}
)
target_compile_definitions(ztd.text.benchmarks.function_form
	PRIVATE
	CATCH_CONFIG_ENABLE_BENCHMARKING=1
)
target_link_libraries(ztd.text.benchmarks.function_form
	PRIVATE
	ztd::cuneicode::benchmarks::barrier
	ztd::cuneicode
	benchmark::benchmark
	${CMAKE_DL_LIBS}
)
target_include_directories(ztd.text.benchmarks.function_form
	PRIVATE
	include
	../include
)
#generate_config_and_manifest_for(ztd.text.benchmarks.function_form)
configure_graph_benchmark_targets(
	NAME
		function_form
	CONFIG
		"${CMAKE_CURRENT_SOURCE_DIR}/graph_config.json"
	TARGETS
		ztd.text.benchmarks.function_form
	REPETITIONS
		${ZTD_TEXT_BENCHMARKS_REPETITIONS}
)
