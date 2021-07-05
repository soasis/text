// =============================================================================
//
// ztd.text
// Copyright © 2021 JeanHeyd "ThePhD" Meneide and Shepherd's Oasis, LLC
// Contact: opensource@soasis.org
//
// Commercial License Usage
// Licensees holding valid commercial ztd.text licenses may use this file
// in accordance with the commercial license agreement provided with the
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

#include <ztd/text.hpp>

struct my_error_handler {
	// Helper definitions
	template <typename Encoding>
	using code_point_span
	     = ztd::ranges::span<const ztd::text::code_point_t<Encoding>>;
	template <typename Encoding>
	using code_unit_span
	     = ztd::ranges::span<const ztd::text::code_unit_t<Encoding>>;

	// Function call operator that returns a "deduced" (auto) type
	// Specifically, this one is called for encode failures
	template <typename Encoding, typename Input, typename Output,
	     typename State>
	auto operator()(
	     // First Parameter
	     const Encoding& encoding,
	     // Second Parameter, encode-specific
	     ztd::text::encode_result<Input, Output, State> result,
	     // Third Parameter
	     code_point_span<Encoding> progress) const noexcept {
		// ... implementation here!
		(void)encoding;
		(void)progress;
		return result;
	}

	// Function call operator that returns a "deduced" (auto) type
	// Specifically, this one is called for decode failures
	template <typename Encoding, typename Input, typename Output,
	     typename State>
	auto operator()(
	     // First Parameter
	     const Encoding& encoding,
	     // Second Parameter, decode-specific
	     ztd::text::decode_result<Input, Output, State> result,
	     // Third Parameter
	     code_unit_span<Encoding> progress) const noexcept {
		// ... implementation here!
		(void)encoding;
		(void)progress;
		return result;
	}
};

int main(int, char* argv[]) {

	// convert from execution encoding to utf8 encoding,
	// using our new handler
	std::string utf8_string = ztd::text::transcode(
	     std::string_view(argv[0]), ztd::text::execution {},
	     ztd::text::basic_utf8<char> {}, my_error_handler {});

	return 0;
}
