// =============================================================================
//
// ztd.text
// Copyright © 2022 JeanHeyd "ThePhD" Meneide and Shepherd's Oasis, LLC
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
// ============================================================================ //

#include <ztd/text/examples/utf8_startup.hpp>

#include <ztd/text/any_encoding.hpp>
#include <ztd/text/encode.hpp>
#include <ztd/text/decode.hpp>
#include <ztd/text/transcode.hpp>

#include <iostream>
#include <fstream>

int main(int, char*[]) {

	ztd::text::any_encoding encoding(ztd::text::compat_utf8);

	const char32_t source_data[] = U"\U0001F408\U0001F431\U0001F408\n\x3A\x33";
	std::string storage;
	storage.resize(std::size(source_data) * sizeof(char32_t));

	ztd::span<char> normal_output(storage);
	ztd::span<std::byte> output = ztd::as_writable_bytes(normal_output);
	ztd::text::encode_state_t<ztd::text::any_encoding> state(encoding);
	auto result = ztd::text::encode_into(source_data, encoding, output, ztd::text::replacement_handler, state);
	std::size_t result_written = output.size() - result.output.size();
	storage.resize(result_written);

	return 0;
}
