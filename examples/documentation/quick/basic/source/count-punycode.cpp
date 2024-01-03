// =============================================================================
//
// ztd.text
// Copyright © JeanHeyd "ThePhD" Meneide and Shepherd's Oasis, LLC
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

#include <ztd/text.hpp>

#include <iostream>

int main(int, char*[]) {
	const char input[]
	     = " OSSL   s  s  RFC ss   "
	       "-cqj0qgheba6zgdehhb85bfc31d5m2evf4423k0a7nd6abq3flcampfa17ac5froq64c0"
	       "a2a7nbcyjnb1b7yp96t0e31nkf95i";
	std::vector<char32_t> output(256);
	auto counting_result
	     = ztd::text::count_as_decoded(input, ztd::text::punycode);
	ZTD_TEXT_ASSERT(counting_result.error_code == ztd::text::encoding_error::ok);
	ZTD_TEXT_ASSERT(!counting_result.errors_were_handled());
	if (counting_result.count > 256) {
		std::cerr << "The input punycode exceeeds the IDNA limited size buffer: "
		             "change parameters to allocate a larger one!"
		          << std::endl;
		return 1;
	}
	output.resize(counting_result.count);
	auto decoding_result = ztd::text::decode_into_raw(
	     input, ztd::text::punycode, ztd::span<char32_t>(output));
	std::size_t decoding_result_count
	     = decoding_result.output.data() - output.data();
	ZTD_TEXT_ASSERT(decoding_result.error_code == ztd::text::encoding_error::ok);
	ZTD_TEXT_ASSERT(!decoding_result.errors_were_handled());
	ZTD_TEXT_ASSERT(ztd::ranges::empty(decoding_result.input));
	ZTD_TEXT_ASSERT(decoding_result_count == counting_result.count);

	// Show decoded punycode (translate to UTF-8 to print to console)
	std::cout << "Decoded punycode code points:\n\t";
	for (const auto& code_point : output) {
		std::cout << std::hex << std::showbase
		          << static_cast<uint_least32_t>(code_point) << " ";
	}
	std::cout << "\n" << std::endl;

	std::cout << "Decoded punycode as UTF-8:\n\t";
	ztd::text::encode_view<ztd::text::utf8_t> print_view(
	     std::u32string_view(output.data(), decoding_result_count));
	for (auto u8_code_unit : print_view) {
		std::cout.write(reinterpret_cast<const char*>(&u8_code_unit), 1);
	}
	std::cout << std::endl;

	return 0;
}
