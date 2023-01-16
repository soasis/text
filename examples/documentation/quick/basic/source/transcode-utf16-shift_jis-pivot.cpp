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
#include <iterator>

int main(int, char*[]) {
	constexpr const char16_t u16_data[]
	     = u"この国の歴史は世界がまだ未完成で、神様すらいなかったところから始まる"
	       u"。現在の日本は『神様の住む天界』『人間の住む地上』『死者の住む冥界』"
	       u"の三層に分かれているけれど、その頃はまだ気体と固体すら分かれていなく"
	       u"て、カオス状態の世界がどこまでもどこまでも広がっているだけだった。そ"
	       u"れから長ーーい長ーーーい時間が経ったある日、ふいに天と地が分かれた。"
	       u"すると、どこからともなく天に1人の神様が、なりなりと生まれてきた。";

	// must provide all arguments to get to the "pivot" part.
	// decode and encode states to use
	auto utf16_decode_state = ztd::text::make_decode_state(ztd::text::utf16);
	auto shift_jis_encode_state
	     = ztd::text::make_encode_state(ztd::text::shift_jis_x0208);
	// the output we're going to serialize into! We're using a std::back_inserter
	// to just fill up our desired container (in this case, a std::string)
	std::string shift_jis_string;
	auto output_view
	     = ztd::ranges::unbounded_view(std::back_inserter(shift_jis_string));
	// we're going to use a static buffer, but anything
	// would work just fine, really, as the "pivot"
	char32_t my_intermediate_buffer[256];
	ztd::span<char32_t> pivot(my_intermediate_buffer);

	// Perform the conversion!
	auto shift_jis_result = ztd::text::transcode_into(u16_data, ztd::text::utf16,
	     output_view, ztd::text::shift_jis_x0208, ztd::text::replacement_handler,
	     ztd::text::replacement_handler, utf16_decode_state,
	     shift_jis_encode_state, pivot);


	// Verify everything is in a state we expect it to be in!
	// A Shift-JIS encoded character string.
	constexpr const char expected_shift_jis_string[]
	     = "\x82\xb1\x82\xcc\x8d\x91\x82\xcc\x97\xf0\x8e\x6a\x82\xcd\x90\xa2\x8a"
	       "\x45\x82\xaa\x82\xdc\x82\xbe\x96\xa2\x8a\xae\x90\xac\x82\xc5\x81\x41"
	       "\x90\x5f\x97\x6c\x82\xb7\x82\xe7\x82\xa2\x82\xc8\x82\xa9\x82\xc1\x82"
	       "\xbd\x82\xc6\x82\xb1\x82\xeb\x82\xa9\x82\xe7\x8e\x6e\x82\xdc\x82\xe9"
	       "\x81\x42\x8c\xbb\x8d\xdd\x82\xcc\x93\xfa\x96\x7b\x82\xcd\x81\x77\x90"
	       "\x5f\x97\x6c\x82\xcc\x8f\x5a\x82\xde\x93\x56\x8a\x45\x81\x78\x81\x77"
	       "\x90\x6c\x8a\xd4\x82\xcc\x8f\x5a\x82\xde\x92\x6e\x8f\xe3\x81\x78\x81"
	       "\x77\x8e\x80\x8e\xd2\x82\xcc\x8f\x5a\x82\xde\x96\xbb\x8a\x45\x81\x78"
	       "\x82\xcc\x8e\x4f\x91\x77\x82\xc9\x95\xaa\x82\xa9\x82\xea\x82\xc4\x82"
	       "\xa2\x82\xe9\x82\xaf\x82\xea\x82\xc7\x81\x41\x82\xbb\x82\xcc\x8d\xa0"
	       "\x82\xcd\x82\xdc\x82\xbe\x8b\x43\x91\xcc\x82\xc6\x8c\xc5\x91\xcc\x82"
	       "\xb7\x82\xe7\x95\xaa\x82\xa9\x82\xea\x82\xc4\x82\xa2\x82\xc8\x82\xad"
	       "\x82\xc4\x81\x41\x83\x4a\x83\x49\x83\x58\x8f\xf3\x91\xd4\x82\xcc\x90"
	       "\xa2\x8a\x45\x82\xaa\x82\xc7\x82\xb1\x82\xdc\x82\xc5\x82\xe0\x82\xc7"
	       "\x82\xb1\x82\xdc\x82\xc5\x82\xe0\x8d\x4c\x82\xaa\x82\xc1\x82\xc4\x82"
	       "\xa2\x82\xe9\x82\xbe\x82\xaf\x82\xbe\x82\xc1\x82\xbd\x81\x42\x82\xbb"
	       "\x82\xea\x82\xa9\x82\xe7\x92\xb7\x81\x5b\x81\x5b\x82\xa2\x92\xb7\x81"
	       "\x5b\x81\x5b\x81\x5b\x82\xa2\x8e\x9e\x8a\xd4\x82\xaa\x8c\x6f\x82\xc1"
	       "\x82\xbd\x82\xa0\x82\xe9\x93\xfa\x81\x41\x82\xd3\x82\xa2\x82\xc9\x93"
	       "\x56\x82\xc6\x92\x6e\x82\xaa\x95\xaa\x82\xa9\x82\xea\x82\xbd\x81\x42"
	       "\x82\xb7\x82\xe9\x82\xc6\x81\x41\x82\xc7\x82\xb1\x82\xa9\x82\xe7\x82"
	       "\xc6\x82\xe0\x82\xc8\x82\xad\x93\x56\x82\xc9\x31\x90\x6c\x82\xcc\x90"
	       "\x5f\x97\x6c\x82\xaa\x81\x41\x82\xc8\x82\xe8\x82\xc8\x82\xe8\x82\xc6"
	       "\x90\xb6\x82\xdc\x82\xea\x82\xc4\x82\xab\x82\xbd\x81\x42";

	ZTD_TEXT_ASSERT(shift_jis_result.error_code == ztd::text::encoding_error::ok);
	ZTD_TEXT_ASSERT(!shift_jis_result.errors_were_handled());
	ZTD_TEXT_ASSERT(
	     shift_jis_result.pivot_error_code == ztd::text::encoding_error::ok);
	ZTD_TEXT_ASSERT(shift_jis_result.pivot_error_count == 0);
	ZTD_TEXT_ASSERT(ztd::ranges::empty(shift_jis_result.input));
	ZTD_TEXT_ASSERT(shift_jis_string == expected_shift_jis_string);
	return 0;
}
