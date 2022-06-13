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

#include "shift_jis.hpp"
#include "detail/shift_jis_index.hpp"

shift_jis::sjis_encode_result shift_jis::encode_one(ztd::span<const code_point> input, ztd::span<code_unit> output,
     sjis_encode_error_handler error_handler, state& current_state) const {
	using input_span  = ztd::span<const code_point>;
	using output_span = ztd::span<code_unit>;
	// Do we have anything to read?
	if (input.empty()) {
		// we don't need more, so we can just say the empty state is a-okay.
		return sjis_encode_result(std::move(input), std::move(output), current_state, ztd::text::encoding_error::ok);
	}
	auto in_it    = input.begin();
	auto in_last  = input.end();
	auto out_it   = output.begin();
	auto out_last = output.end();

	code_point code = *in_it;
	++in_it;

	if (code <= 0x80) {
		if (out_it == out_last) {
			// output is empty :(
			return error_handler(*this,
			     sjis_encode_result(std::move(input), std::move(output), current_state,
			          ztd::text::encoding_error::insufficient_output_space),
			     input_span(), output_span());
		}
		*out_it = static_cast<code_unit>(code);
		++out_it;
		return sjis_encode_result(input_span(std::move(in_it), std::move(in_last)),
		     output_span(std::move(out_it), std::move(out_last)), current_state, ztd::text::encoding_error::ok);
	}
	else if (code == 0x00A5) {
		if (out_it == out_last) {
			// output is empty :(
			return error_handler(*this,
			     sjis_encode_result(std::move(input), std::move(output), current_state,
			          ztd::text::encoding_error::insufficient_output_space),
			     input_span(), output_span());
		}
		*out_it = static_cast<code_unit>(0x5C);
		++out_it;
		return sjis_encode_result(input_span(std::move(in_it), std::move(in_last)),
		     output_span(std::move(out_it), std::move(out_last)), current_state, ztd::text::encoding_error::ok);
	}
	else if (code == 0x203E) {
		if (out_it == out_last) {
			// output is empty :(
			return error_handler(*this,
			     sjis_encode_result(std::move(input), std::move(output), current_state,
			          ztd::text::encoding_error::insufficient_output_space),
			     input_span(), output_span());
		}
		*out_it = static_cast<code_unit>(0x7E);
		++out_it;
		return sjis_encode_result(input_span(std::move(in_it), std::move(in_last)),
		     output_span(std::move(out_it), std::move(out_last)), current_state, ztd::text::encoding_error::ok);
	}
	else if (code >= 0xFF61 && code <= 0xFF9F) {
		if (out_it == out_last) {
			// output is empty :(
			return error_handler(*this,
			     sjis_encode_result(std::move(input), std::move(output), current_state,
			          ztd::text::encoding_error::insufficient_output_space),
			     input_span(), output_span());
		}
		code_point intermediate = (code - 0xFF61);
		*out_it                 = static_cast<code_unit>(intermediate + 0xA1);
		++out_it;
		return sjis_encode_result(input_span(std::move(in_it), std::move(in_last)),
		     output_span(std::move(out_it), std::move(out_last)), current_state, ztd::text::encoding_error::ok);
	}

	if (code == 0x2212) {
		code = 0xFF0D;
	}

	std::optional<std::size_t> maybe_index = detail::shift_jis_code_point_to_index(code);
	if (maybe_index) {
		std::size_t index         = *maybe_index;
		std::size_t first         = index / 188;
		std::size_t first_offset  = first < 0x1F ? 0x81 : 0xC1;
		std::size_t second        = index % 188;
		std::size_t second_offset = second < 0x3F ? 0x40 : 0x41;

		auto out_first = out_it;
		if (out_it == out_last) {
			// output is empty :(
			return error_handler(*this,
			     sjis_encode_result(std::move(input), std::move(output), current_state,
			          ztd::text::encoding_error::insufficient_output_space),
			     input_span(), output_span());
		}
		++out_it;
		if (out_it == out_last) {
			// output is empty :(
			return error_handler(*this,
			     sjis_encode_result(std::move(input), std::move(output), current_state,
			          ztd::text::encoding_error::insufficient_output_space),
			     input_span(), output_span());
		}
		*out_first = static_cast<code_unit>(first + first_offset);
		*out_it    = static_cast<code_unit>(second + second_offset);
		++out_it;
		return sjis_encode_result(input_span(std::move(in_it), std::move(in_last)),
		     output_span(std::move(out_it), std::move(out_last)), current_state, ztd::text::encoding_error::ok);
	}

	return error_handler(*this,
	     sjis_encode_result(std::move(input), std::move(output), current_state,
	          ztd::text::encoding_error::insufficient_output_space),
	     input_span(), output_span());
}

shift_jis::sjis_decode_result shift_jis::decode_one(ztd::span<const shift_jis::code_unit> input,
     ztd::span<code_point> output, sjis_decode_error_handler error_handler, state& current_state) const {
	using input_span  = ztd::span<const code_unit>;
	using output_span = ztd::span<code_point>;
	// Do we have anything to read?
	if (input.empty()) {
		// we don't need more, so we can just say the empty state is a-okay.
		return sjis_decode_result(std::move(input), std::move(output), current_state, ztd::text::encoding_error::ok);
	}
	auto in_it    = input.begin();
	auto in_last  = input.end();
	auto out_it   = output.begin();
	auto out_last = output.end();

	unsigned char first_byte = static_cast<unsigned char>(*in_it);
	++in_it;

	if (first_byte <= 0x80) {
		// Top-Level case 0: it's an ASCII byte
		code_point code = static_cast<code_point>(first_byte);
		if (out_it == out_last) {
			// output is empty :(
			return error_handler(*this,
			     sjis_decode_result(std::move(input), std::move(output), current_state,
			          ztd::text::encoding_error::insufficient_output_space),
			     input_span(), output_span());
		}
		*out_it = code;
		++out_it;
		return sjis_decode_result(input_span(std::move(in_it), std::move(in_last)),
		     output_span(std::move(out_it), std::move(out_last)), current_state, ztd::text::encoding_error::ok);
	}
	else if (first_byte <= 0xDF && first_byte >= 0xA1) {
		// Top-Level case 1: it's non-ASCII single-byte
		code_point code = static_cast<code_point>((0xFF61 - 0xA1) + first_byte);
		if (out_it == out_last) {
			return error_handler(*this,
			     sjis_decode_result(std::move(input), std::move(output), current_state,
			          ztd::text::encoding_error::insufficient_output_space),
			     input_span(), output_span());
		}
		*out_it = code;
		++out_it;
		return sjis_decode_result(input_span(std::move(in_it), std::move(in_last)),
		     output_span(std::move(out_it), std::move(out_last)), current_state, ztd::text::encoding_error::ok);
	}
	else if ((first_byte <= 0x9F && first_byte >= 0x81) || (first_byte <= 0xFC && first_byte >= 0xE0)) {
		// Top-Level case 2: this is a double-byte sequence!
		unsigned char second_byte = static_cast<unsigned char>(*in_it);
		++in_it;
		unsigned char lookup_offset      = second_byte < 0x7F ? 0x40 : 0x41;
		unsigned char lead_lookup_offset = first_byte < 0xA0 ? 0x81 : 0xC1;
		std::size_t lookup_index         = 0;
		if ((second_byte <= 0x7E && second_byte >= 0x40) || (second_byte <= 0xFC && second_byte >= 0x80)) {
			lookup_index = (((first_byte - lead_lookup_offset) * 188) + second_byte) - lookup_offset;
			if (lookup_index <= 10715 && lookup_index >= 8836) {
				code_point code = static_cast<char32_t>(0xE000 - 8836 + lookup_index);
				*out_it         = code;
				++out_it;
				return sjis_decode_result(input_span(std::move(in_it), std::move(in_last)),
				     output_span(std::move(out_it), std::move(out_last)), current_state,
				     ztd::text::encoding_error::ok);
			}
			else {
				std::optional<code_point> maybe_code = detail::shift_jis_index_to_code_point(lookup_index);
				if (maybe_code) {
					const code_point& code = *maybe_code;
					*out_it                = code;
					++out_it;
					return sjis_decode_result(input_span(std::move(in_it), std::move(in_last)),
					     output_span(std::move(out_it), std::move(out_last)), current_state,
					     ztd::text::encoding_error::ok);
				}
			}
		}
	}

	// Top-Level case 3 (default): unrecognized byte sequence!!
	return error_handler(*this,
	     sjis_decode_result(std::move(input), std::move(output), current_state,
	          ztd::text::encoding_error::insufficient_output_space),
	     input_span(), output_span());
}
