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
function (generate_function_form_benchmark_targets name data_name title)
	file(GLOB_RECURSE sources
		LIST_DIRECTORIES FALSE
		CONFIGURE_DEPENDS
		source/*.c source/*.cpp)

	add_executable(ztd.text.benchmarks.function_form.${name} ${sources})
	target_compile_options(ztd.text.benchmarks.function_form.${name}
		PRIVATE
		${--utf8-literal-encoding}
		${--utf8-source-encoding}
		${--disable-permissive}
		${--updated-cpp-version-flag}
		${--warn-pedantic}
		${--warn-all}
		${--warn-extra}
		${--allow-alignas-extra-padding}
		${--allow-stringop-overflow}
		# MSVC is BROKEN
		# ${--warn-errors}
	)
	target_compile_definitions(ztd.text.benchmarks.function_form.${name}
		PRIVATE
		CATCH_CONFIG_ENABLE_BENCHMARKING=1
		u8_data=u8_${data_name}_data
		u16_data=u16_${data_name}_data
		u32_data=u32_${data_name}_data
	)
	target_link_libraries(ztd.text.benchmarks.function_form.${name}
		PRIVATE
		ztd::text::benchmarks::barrier
		ztd::cuneicode
		benchmark::benchmark
		${CMAKE_DL_LIBS}
	)
	target_include_directories(ztd.text.benchmarks.function_form.${name}
		PRIVATE
		include
		../include
	)
	generate_target_manifest(ztd.text.benchmarks.function_form.${name})
	set(ZTD_TEXT_BENCHMARKS_FUNCTION_FORM_TITLE ${title})
	configure_file(${CMAKE_CURRENT_SOURCE_DIR}/graph_config.in.json
		${CMAKE_CURRENT_BINARY_DIR}/graph_config.${name}.json
		@ONLY
	)
	ztd_tools_add_benchmark_grapher(
		NAME
			function_form.${name}
		CONFIG
			"${CMAKE_CURRENT_BINARY_DIR}/graph_config.${name}.json"
		TARGETS
			ztd.text.benchmarks.function_form.${name}
		REPETITIONS
			${ZTD_TEXT_BENCHMARKS_REPETITIONS}
	)
endfunction()

generate_function_form_benchmark_targets(large unicode "All Unicode Scalar Values")
generate_function_form_benchmark_targets(small basic_source "C Character Set")
