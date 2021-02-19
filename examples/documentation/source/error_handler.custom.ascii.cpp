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

#include <ztd/text/encode.hpp>
#include <ztd/text/encoding.hpp>

#include <ztd/text/examples/assert.hpp>

#include <iostream>

template <typename Output>
using ascii_encode_result = ztd::text::encode_result<
     // input range type
     std::u32string_view,
     // output range type; figured out from function call
     Output,
     // the state type for encode operations
     ztd::text::encode_state_of_t<ztd::text::ascii>>;

struct my_printing_handler {
	template <typename Output>
	ascii_encode_result<Output> operator()(const ztd::text::ascii& encoding, ascii_encode_result<Output> result,
	     std::span<const char32_t> unused_read_characters) const noexcept {
		// just printing some information
		std::cout << "An error occurred.\n"
		          << "\tError code value: " << ztd::text::to_name(result.error_code) << "\n"
		          << "\t# of unused characters: " << unused_read_characters.size() << "\n"
		          << "\tInput units left: " << result.input.size() << "\n";
		// setting the error to "ok"
		// tells the algorithm to keep spinning,
		// even if nothing gets written to the output
		result.error_code = ztd::text::encoding_error::ok;
		return result;
	}
};

int main(int, char*[]) {
	std::string my_ascii_string = ztd::text::encode(
	     // input
	     U"안녕",
	     // to this encoding
	     ztd::text::ascii {},
	     // handled with our type
	     my_printing_handler {});

	example_assert(my_ascii_string == "");

	return 0;
}