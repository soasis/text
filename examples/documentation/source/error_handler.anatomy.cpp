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

#include <ztd/text.hpp>

namespace txt = ztd::text;

struct my_error_handler {

	// For encode_one/encode failures
	template <typename Encoding, typename Input, typename Output,
	     typename State>
	auto operator()(                                        // Function call operator
	     const Encoding& encoding,                          // First Parameter
	     txt::encode_result<Input, Output, State> result,   // Second Parameter, encode-specific
	     std::span<txt::code_point_of_t<Encoding>> progress // Third Parameter
	) const {
		// ... implementation here!
		return result;
	}

	// For decode_one/decode failures
	template <typename Encoding, typename Input, typename Output,
	     typename State>
	auto operator()(               // Function call operator that returns a "deduced" (auto) type
	     const Encoding& encoding, // First Parameter
	     txt::decode_result<Input, Output, State> result,   // Second Parameter, decode-specific
	     std::span<txt::code_point_of_t<Encoding>> progress // Third Parameter
	) const {
		// ... implementation here!
		return result;
	}
};

int main() {

	return 0;
}