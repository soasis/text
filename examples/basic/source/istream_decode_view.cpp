// =============================================================================
//
// ztd.text
// Copyright © 2022-2023 JeanHeyd "ThePhD" Meneide and Shepherd's Oasis, LLC
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
// https://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
//
// ============================================================================ //

#include <ztd/text/examples/utf8_startup.hpp>

#include <ztd/text/decode_view.hpp>
#include <ztd/text/encoding.hpp>

#include <iterator>
#include <fstream>
#include <iostream>

int main(int, char*[]) {
	std::string input_file_name = "utf16-le.hex";
	// open the desired file
	std::ifstream input_file(input_file_name, std::ios_base::binary);
	// if opening the file failed, or setting the
	// "do not skip whitespace" option failed,
	// then leave with an exit code of 1
	if (!input_file || !(input_file >> std::noskipws)) {
		std::cerr << "Could not initialize the input stream with the file " << input_file_name << std::endl;
		return 1;
	}

	std::string expected_file_name = "utf32-be.hex";
	// open the desired file
	std::ifstream expected_file(expected_file_name, std::ios_base::binary);
	// if opening the file failed, or setting the
	// "do not skip whitespace" option failed,
	// then leave with an exit code of 2
	if (!expected_file || !(expected_file >> std::noskipws)) {
		std::cerr << "Could not initialize the input stream with the file " << expected_file_name << std::endl;
		return 2;
	}

	// We create a subrange of "istream iterators", which allow us to walk
	// over a sequence of data as if it was just a normal, regular range
	using istream_view = ztd::ranges::subrange<std::istream_iterator<char>, std::istream_iterator<char>>;
	istream_view input_file_view(std::istream_iterator<char> { input_file }, std::istream_iterator<char> {});
	istream_view expected_file_view(std::istream_iterator<char> { expected_file }, std::istream_iterator<char> {});

	// View the desired input file as UTF-16, in Little Endian format,
	// where the source is given to us as `char`
	ztd::text::decode_view<ztd::text::basic_utf16_le<char>, istream_view> input_decoding_view(input_file_view);
	// View the desired expected file as UTF-32, in Big Endian format, with the source given to us as `char`
	ztd::text::decode_view<ztd::text::basic_utf32_be<char>, istream_view> expected_decoding_view(expected_file_view);

	// Iterate over all the code points
	// And print them out!
	std::cout << "Code Points:" << std::endl;

	auto expected_it   = expected_decoding_view.begin();
	auto expected_last = expected_decoding_view.end();
	for (const auto code_point : input_decoding_view) {
		std::cout << std::hex << (int)code_point << " " << std::flush;
		if (code_point == U'\n') {
			std::cout << std::endl;
		}
		const auto expected_code_point = *expected_it;
		if (code_point != expected_code_point) {
			std::cerr << "Input file " << input_file_name << " was not identical to expected file "
			          << expected_file_name << std::endl;
			return 3;
		}
		++expected_it;
	}
	if (expected_it != expected_last) {
		std::cerr << "Input file converted to UTF-32" << input_file_name << " was not identical expected file "
		          << expected_file_name << std::endl;
		return 4;
	}


	std::cout << std::endl;

	return 0;
}
