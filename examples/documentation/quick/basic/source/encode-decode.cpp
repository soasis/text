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

#include <ztd/text.hpp>

#include <string>

int main(int, char*[]) {
	const char input[]
	     = "\xbe\xc8\xb3\xe7\x2c\x20\xbf\xc0\xb4\xc3\xc0\xba\x20\xc1\xc1\xc0\xba"
	       "\x20\xb3\xaf\xc0\xcc\xbf\xa1\xbf\xe4\x21";

	// Decode, with result to check!
	auto korean_decoded_output_result
	     = ztd::text::decode_to(input, ztd::text::euc_kr);
	ZTD_TEXT_ASSERT(korean_decoded_output_result.error_code
	     == ztd::text::encoding_error::ok);
	ZTD_TEXT_ASSERT(!korean_decoded_output_result.errors_were_handled());
	ZTD_TEXT_ASSERT(ztd::ranges::empty(korean_decoded_output_result.input));
	const std::u32string& korean_decoded_output
	     = korean_decoded_output_result.output;

	// Take decoded Unicode code points and encode it into UTF-8
	auto korean_utf8_output_result
	     = ztd::text::encode_to(korean_decoded_output, ztd::text::compat_utf8);
	ZTD_TEXT_ASSERT(
	     korean_utf8_output_result.error_code == ztd::text::encoding_error::ok);
	ZTD_TEXT_ASSERT(!korean_utf8_output_result.errors_were_handled());
	ZTD_TEXT_ASSERT(ztd::ranges::empty(korean_utf8_output_result.input));
	const std::string& korean_utf8_output = korean_utf8_output_result.output;
	// verify that what we got out in UTF-8 would be the same if we converted
	// it back to EUC-KR.
	ZTD_TEXT_ASSERT(ztd::ranges::equal(std::string_view(input),
	     ztd::text::transcode(korean_utf8_output, ztd::text::compat_utf8,
	          ztd::text::euc_kr, ztd::text::pass_handler)));
	// A korean greeting!
	std::cout.write(korean_utf8_output.data(), korean_utf8_output.size());
	std::cout << std::endl;

	return 0;
}
