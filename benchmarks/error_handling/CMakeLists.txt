# =============================================================================
#
# ztd.text
# Copyright © JeanHeyd "ThePhD" Meneide and Shepherd's Oasis, LLC
# Contact: opensource@soasis.org
#
# Commercial License Usage
# Licensees holding valid commercial ztd.idk licenses may use this file in
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

# all sources
file(GLOB_RECURSE ztd.text.benchmarks.error_handling.sources
	CONFIGURE_DEPENDS
	LIST_DIRECTORIES NO
	source/**)
set(ztd.text.benchmarks.error_handling.sources.no_warnings ${ztd.text.benchmarks.error_handling.sources})
list(FILTER ztd.text.benchmarks.error_handling.sources.no_warnings INCLUDE REGEX ".*\.no_warnings\..*")

function (generate_converion_speed_benchmark_targets name data_name title)
	add_executable(ztd.text.benchmarks.error_handling.${name} ${ztd.text.benchmarks.error_handling.sources})
	target_include_directories(ztd.text.benchmarks.error_handling.${name}
		PRIVATE
			include
			../include
	)
	target_link_libraries(ztd.text.benchmarks.error_handling.${name}
		PRIVATE
			ztd::platform
			ztd::cuneicode
			ztd::cuneicode::shared
			ztd::text::benchmarks::barrier
			ztd::cuneicode::shared::simdutf
			ztd::text
			ctre::ctre
			utf8cpp
			$<$<BOOL:${ZTD_TEXT_BOOST.TEXT}>:Boost::text>
			benchmark::benchmark
			${CMAKE_DL_LIBS}
	)
	# boost.text is a landmine of bad warning-vomiting code
	if (ztd.text.benchmarks.error_handling.sources.no_warnings)
		set_source_files_properties(${ztd.text.benchmarks.error_handling.sources.no_warnings}
			PROPERTIES
				COMPILE_OPTIONS ${--warn-none}
		)
	endif()
	target_compile_options(ztd.text.benchmarks.error_handling.${name}
		PRIVATE
			${--utf8-literal-encoding}
			${--utf8-source-encoding}
			${--disable-permissive}
			${--updated-cpp-version-flag}
			${--warn-pedantic}
			${--warn-default}
			${--warn-extra}
			${--warn-errors}
			${--allow-alignas-extra-padding}
			${--allow-stringop-overflow} ${--allow-stringop-overread}
			${--allow-array-bounds}
	)
	target_compile_definitions(ztd.text.benchmarks.error_handling.${name}
		PRIVATE
			u8_data=u8_${data_name}_data
			u16_data=u16_${data_name}_data
			u32_data=u32_${data_name}_data
			ZTD_TEXT_BENCHMARKS_ERROR_HANDLING_BOOST_TEXT_BENCHMARKS=$<IF:$<BOOL:${ZTD_TEXT_BOOST.TEXT}>,1,0>
			ZTD_TEXT_BENCHMARKS_ERROR_HANDLING_ZTD_TEXT_BENCHMARKS=1
			ZTD_TEXT_BENCHMARKS_ERROR_HANDLING_CUNEICODE_BENCHMARKS=1
			ZTD_TEXT_BENCHMARKS_ERROR_HANDLING_ICU_BENCHMARKS=1
			ZTD_TEXT_BENCHMARKS_ERROR_HANDLING_ICONV_BENCHMARKS=1
			ZTD_TEXT_BENCHMARKS_ERROR_HANDLING_ENCODING_C_BENCHMARKS=1
			ZTD_TEXT_BENCHMARKS_ERROR_HANDLING_WINDOWS_BENCHMARKS=1
			_SILENCE_CXX17_CODECVT_HEADER_DEPRECATION_WARNING=1
			_SILENCE_CXX20_CODECVT_FACETS_DEPRECATION_WARNING=1
			_SILENCE_CXX23_CODECVT_FACETS_DEPRECATION_WARNING=1
			_SILENCE_CXX26_CODECVT_FACETS_DEPRECATION_WARNING=1
	)
	generate_target_manifest(ztd.text.benchmarks.error_handling.${name})
	set(ZTD_TEXT_BENCHMARKS_ERROR_HANDLING_TITLE ${title})
	configure_file(${CMAKE_CURRENT_SOURCE_DIR}/graph_config.in.json
		${CMAKE_CURRENT_BINARY_DIR}/graph_config.${name}.json
		@ONLY
	)
	
	ztd_tools_add_benchmark_grapher(
		NAMES
			error_handling.${name}
		CONFIGS
			"${CMAKE_CURRENT_BINARY_DIR}/graph_config.${name}.json"
		TARGETS
			ztd.text.benchmarks.error_handling.${name}
		REPETITIONS
			${ZTD_TEXT_BENCHMARKS_REPETITIONS}
	)
endfunction()

generate_converion_speed_benchmark_targets(large unicode "All Unicode Scalar Values with Erorrs Conversion")
generate_converion_speed_benchmark_targets(small basic_source "C Character Set with Errors Conversion")
