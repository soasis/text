# =============================================================================
#
# ztd.text
# Copyright © 2022-2023 JeanHeyd "ThePhD" Meneide and Shepherd's Oasis, LLC
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

#[[
# === snippet-project-declaration-start
project(my_app
	VERSION 1.0.0
	DESCRIPTION "My application."
	HOMEPAGE_URL "https://ztdtext.readthedocs.io/en/latest/quick.html"
	LANGUAGES C CPP
)

include(FetchContent)

FetchContent_Declare(ztd.text
	GIT_REPOSITORY https://github.com/soasis/text.git
	GIT_SHALLOW    ON
	GIT_TAG        main)
FetchContent_MakeAvailable(ztd.text)
# === snippet-project-declaration-end
#]]

# === snippet-library-start
# …

file(GLOB_RECURSE my_app_sources
	LIST_DIRECTORIES OFF
	CONFIGURE_DEPENDS
	source/*.cpp
)

add_executable(my_app ${my_app_sources})

target_link_libraries(my_app PRIVATE ztd::text)
# === snippet-library-end

if (ZTD_TEXT_TESTS)
	add_test(NAME ztd.text.examples.documentation.quick.my_app COMMAND my_app)
endif()
