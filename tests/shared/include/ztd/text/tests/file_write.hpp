// =============================================================================
//
// ztd.text
// Copyright © 2021 JeanHeyd "ThePhD" Meneide and Shepherd's Oasis, LLC
// Contact: opensource@soasis.org
//
// Commercial License Usage
// Licensees holding valid commercial ztd.text licenses may use this file in
// accordance with the commercial license agreement provided with the
// Software or, alternatively, in accordance with the terms contained in
// a written agreement between you and Shepherd's Oasis, LLC.
// For licensing terms and conditions see your agreement. For
// further information contact opensource@soasis.org.
//
// Apache License Version 2 Usage
// Alternatively, this file may be used under the terms of Apache License
// Version 2.0 (the "License") for non-commercial use; you may not use this
// file except in compliance with the License. You may obtain a copy of the
// License at
//
//		http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
//
// ============================================================================>

#pragma once

#ifndef ZTD_TEXT_TESTS_BASIC_UNICODE_STRINGS_HPP
#define ZTD_TEXT_TESTS_BASIC_UNICODE_STRINGS_HPP

#include <ztd/text/version.hpp>

#include <ztd/ranges/span.hpp>

#include <fstream>
#include <string>


namespace ztd { namespace text { namespace tests {

	template <typename Name, typename Data>
	void write_file_attempt(Name&& filename, Data&& data) {
		bool success = false;
		for (std::size_t i = 0; i < 20; ++i) {
			try {
				std::ofstream file(std::forward<Name>(filename), std::ios::out);
				file.exceptions(std::ios_base::badbit | std::ios_base::failbit);
				file.write(data.data(), data.size());
				file.flush();
				file.close();
			}
			catch (const std::exception&) {
				std::this_thread::sleep_for(std::chrono::milliseconds(10));
				continue;
			}
			success = true;
			break;
		}
		if (!success) {
			throw std::runtime_error("cannot open file or write out");
		}
	}

	struct write_and_cleanup_file {
		std::string file_name;

		write_and_cleanup_file(std::string file_name_, ztd::ranges::span<unsigned char> data_)
		: file_name(file_name_) {
			write_file_attempt(file_name, data_);
		}

		~write_and_cleanup_file() {
			std::remove(file_name.c_str());
		}
	}

}}} // namespace ztd::text::tests