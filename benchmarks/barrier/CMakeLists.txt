# =============================================================================
#
# ztd.text
# Copyright © JeanHeyd "ThePhD" Meneide and Shepherd's Oasis, LLC
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

# # Benchmark dependecy data
function(unicode_lipsum_dependency_jail)
	# unicode_lipsum check
	FetchContent_Declare(
		unicode_lipsum
		GIT_REPOSITORY https://github.com/lemire/unicode_lipsum
		GIT_SHALLOW    ON
		GIT_TAG        main
		EXCLUDE_FROM_ALL)
	FetchContent_MakeAvailable(unicode_lipsum)
endfunction()
unicode_lipsum_dependency_jail()

# # Benchmarks data, compiled behind a shared data barrier
file(GLOB_RECURSE ztd.text.benchmarks.barrier.sources
	LIST_DIRECTORIES FALSE
	CONFIGURE_DEPENDS
	source/*.c source/*.cpp)

add_library(ztd.text.benchmarks.barrier
	SHARED
	${ztd.text.benchmarks.barrier.sources}
)
add_library(ztd::text::benchmarks::barrier
	ALIAS
	ztd.text.benchmarks.barrier
)
target_compile_definitions(ztd.text.benchmarks.barrier
	PRIVATE
		ZTD_TEXT_BENCHMARKS_BARRIER_BUILD=1
		ZTD_TEXT_BENCHMARKS_BARRIER_WIKIPEDIA_SOURCE_DIR="${UNICODE_LIPSUM_SOURCE_DIR}/wikipedia_mars/"
		ZTD_TEXT_BENCHMARKS_BARRIER_WIKIPEDIA_SOURCE_LIST="${UNICODE_LIPSUM_SOURCE_DIR}/wikipedia_mars/"
		ZTD_TEXT_BENCHMARKS_BARRIER_LIPSUM_SOURCE_DIR="${UNICODE_LIPSUM_SOURCE_DIR}/lipsum/"
		ZTD_TEXT_BENCHMARKS_BARRIER_LIPSUM_SOURCE_DIR="${UNICODE_LIPSUM_SOURCE_DIR}/lipsum/"
	PUBLIC
		ZTD_TEXT_BENCHMARKS_BARRIER_DLL=1
)
target_compile_options(ztd.text.benchmarks.barrier
	PRIVATE
	${--utf8-literal-encoding}
	${--utf8-source-encoding}
	${--disable-permissive}
	${--updated-cpp-version-flag}
	${--warn-pedantic}
	${--warn-all}
	${--warn-extra}
	${--warn-errors}
	${--allow-alignas-extra-padding}
	${--allow-stringop-overflow} ${--allow-stringop-overread}
	${--allow-array-bounds}
)
target_link_libraries(ztd.text.benchmarks.barrier
	PRIVATE
	ztd::cuneicode
	${CMAKE_DL_LIBS}
)
target_include_directories(ztd.text.benchmarks.barrier
	PUBLIC
	include
)
