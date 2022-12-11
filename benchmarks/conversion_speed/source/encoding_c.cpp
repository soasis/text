// ============================================================================
//
// ztd.text
// Copyright © 2022-2022 JeanHeyd "ThePhD" Meneide and Shepherd's Oasis, LLC
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
// Version 2.0 (the "License"); you may not use this file except in compliance
// with the License. You may obtain a copy of the License at
//
// 		http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
//
// ========================================================================= //

#include <ztd/text/benchmarks/version.hpp>

#if ZTD_IS_ON(ZTD_TEXT_BENCHMARKS_CONVERSION_SPEED_ENCODING_C_BENCHMARKS)

#include <ztd/platform/version.hpp>

#if ZTD_IS_ON(ZTD_PLATFORM_ENCODING_C)

#include <benchmark/benchmark.h>

#include <barrier/barrier.h>
#include <ztd/idk/span.hpp>
#include <ztd/idk/endian.hpp>
#include <ztd/platform/encoding_c/encoding_rs_cpp.hpp>

#include <vector>
#include <string_view>
#include <algorithm>

#define UTF8_INTERMEDIATE_BUFFER_SIZE 4096
#define UTF16_INTERMEDIATE_BUFFER_SIZE 2048

bool encoding_c_convert_through_utf8(encoding_rs::Decoder* decoder, encoding_rs::Encoder* encoder,
     ztd::span<const unsigned char> input, ztd::span<unsigned char> output) {
	const auto native_utf16_encoding = ztd::endian::native == ztd::endian::big ? UTF_16BE_ENCODING : UTF_16LE_ENCODING;
	if (decoder->encoding() == UTF_8_ENCODING) {
		// we can skip one half, we are already have data in UTF-8
		uint32_t encoder_result                                 = INPUT_EMPTY;
		size_t encoder_read                                     = 0;
		size_t encoder_written                                  = 0;
		std::tie(encoder_result, encoder_read, encoder_written) = encoder->encode_from_utf8_without_replacement(
		     std::string_view((const char*)input.data(), input.size()), output, true);
		if (encoder_result != INPUT_EMPTY) {
			return false;
		}
		return true;
	}
	else if (decoder->encoding() == native_utf16_encoding) {
		// we can skip one half, we are already have data in UTF-16
		uint32_t encoder_result                                 = INPUT_EMPTY;
		size_t encoder_read                                     = 0;
		size_t encoder_written                                  = 0;
		std::tie(encoder_result, encoder_read, encoder_written) = encoder->encode_from_utf16_without_replacement(
		     std::u16string_view((const char16_t*)input.data(), input.size() / sizeof(char16_t)), output, true);
		if (encoder_result != INPUT_EMPTY) {
			return false;
		}
		return true;
	}
	unsigned char pivot_buffer[UTF8_INTERMEDIATE_BUFFER_SIZE];
	size_t decoder_offset = 0;
	size_t encoder_offset = 0;
	for (;;) {
		uint32_t decoder_result = INPUT_EMPTY;
		size_t decoder_read     = 0;
		size_t decoder_written  = 0;
		std::tie(decoder_result, decoder_read, decoder_written)
		     = decoder->decode_to_utf8_without_replacement(input.subspan(decoder_offset), pivot_buffer, false);
		if (decoder_result != OUTPUT_FULL || decoder_result != INPUT_EMPTY) {
			return false;
		}
		decoder_offset += decoder_read;
		uint32_t encoder_result                                 = INPUT_EMPTY;
		size_t encoder_read                                     = 0;
		size_t encoder_written                                  = 0;
		std::tie(encoder_result, encoder_read, encoder_written) = encoder->encode_from_utf8_without_replacement(
		     std::string_view((const char*)input.data(), input.size()), output, false);
		if (encoder_result != INPUT_EMPTY) {
			return false;
		}
		encoder_offset += encoder_written;
		if (output.size() < encoder_offset) {
			return false;
		}
		if (decoder_result == INPUT_EMPTY) {
			return true;
		}
		if (input.size() < decoder_offset) {
			return false;
		}
	}
	return false;
}

#define UTF_CONVERSION_BENCHMARK(FROM_N, TO_N, FROM_BIG_SUFFIX, FROM_LIL_SUFFIX, TO_BIG_SUFFIX, TO_LIL_SUFFIX)       \
	static void utf##FROM_N##_to_utf##TO_N##_well_formed_encoding_c(benchmark::State& state) {                      \
		if (TO_N == 16 && FROM_N == 8) {                                                                           \
			state.SkipWithError("unsupported");                                                                   \
			return;                                                                                               \
		}                                                                                                          \
		const std::vector<ztd_char##FROM_N##_t> input_data(c_span_char##FROM_N##_t_data(u##FROM_N##_data),         \
		     c_span_char##FROM_N##_t_data(u##FROM_N##_data) + c_span_char##FROM_N##_t_size(u##FROM_N##_data));     \
		std::vector<ztd_char##TO_N##_t> output_data(c_span_char##TO_N##_t_size(u##TO_N##_data));                   \
		std::unique_ptr<encoding_rs::Decoder> decoder = ztd::endian::native == ztd::endian::big                    \
		     ? UTF_##FROM_N##FROM_BIG_SUFFIX##_ENCODING->new_decoder_without_bom_handling()                        \
		     : UTF_##FROM_N##FROM_LIL_SUFFIX##_ENCODING->new_decoder_without_bom_handling();                       \
		std::unique_ptr<encoding_rs::Encoder> encoder = ztd::endian::native == ztd::endian::big                    \
		     ? UTF_##TO_N##TO_BIG_SUFFIX##_ENCODING->new_encoder()                                                 \
		     : UTF_##TO_N##TO_LIL_SUFFIX##_ENCODING->new_encoder();                                                \
		bool result                                   = true;                                                      \
		for (auto _ : state) {                                                                                     \
			bool successful = encoding_c_convert_through_utf8(decoder.get(), encoder.get(),                       \
			     ztd::span<const unsigned char>(                                                                  \
			          (const unsigned char*)input_data.data(), input_data.size() * sizeof(ztd_char##FROM_N##_t)), \
			     ztd::span<unsigned char>(                                                                        \
			          (unsigned char*)output_data.data(), output_data.size() * sizeof(ztd_char##TO_N##_t)));      \
			if (!successful) {                                                                                    \
				result = false;                                                                                  \
			}                                                                                                     \
		}                                                                                                          \
		const bool is_equal                                                                                        \
		     = std::equal(output_data.cbegin(), output_data.cend(), c_span_char##TO_N##_t_data(u##TO_N##_data),    \
		          c_span_char##TO_N##_t_data(u##TO_N##_data) + c_span_char##TO_N##_t_size(u##TO_N##_data));        \
		if (!result) {                                                                                             \
			if (TO_N == 16 && FROM_N == 8) {                                                                      \
				state.SkipWithError("unsupported");                                                              \
			}                                                                                                     \
			else {                                                                                                \
				state.SkipWithError("conversion failed with an error");                                          \
			}                                                                                                     \
		}                                                                                                          \
		else if (!is_equal) {                                                                                      \
			if (TO_N == 16 && FROM_N == 8) {                                                                      \
				state.SkipWithError("unsupported");                                                              \
			}                                                                                                     \
			else {                                                                                                \
				state.SkipWithError("conversion succeeded but produced illegitimate data");                      \
			}                                                                                                     \
			return;                                                                                               \
		}                                                                                                          \
	}                                                                                                               \
                                                                                                                     \
	static void utf##FROM_N##_to_utf##TO_N##_init_well_formed_encoding_c(benchmark::State& state) {                 \
		if (TO_N == 16 && FROM_N == 8) {                                                                           \
			state.SkipWithError("unsupported");                                                                   \
			return;                                                                                               \
		}                                                                                                          \
		const std::vector<ztd_char##FROM_N##_t> input_data(c_span_char##FROM_N##_t_data(u##FROM_N##_data),         \
		     c_span_char##FROM_N##_t_data(u##FROM_N##_data) + c_span_char##FROM_N##_t_size(u##FROM_N##_data));     \
		std::vector<ztd_char##TO_N##_t> output_data(c_span_char##TO_N##_t_size(u##TO_N##_data));                   \
		bool result = true;                                                                                        \
		for (auto _ : state) {                                                                                     \
			std::unique_ptr<encoding_rs::Decoder> decoder = ztd::endian::native == ztd::endian::big               \
			     ? UTF_##FROM_N##FROM_BIG_SUFFIX##_ENCODING->new_decoder_without_bom_handling()                   \
			     : UTF_##FROM_N##FROM_LIL_SUFFIX##_ENCODING->new_decoder_without_bom_handling();                  \
			std::unique_ptr<encoding_rs::Encoder> encoder = ztd::endian::native == ztd::endian::big               \
			     ? UTF_##TO_N##TO_BIG_SUFFIX##_ENCODING->new_encoder()                                            \
			     : UTF_##TO_N##TO_LIL_SUFFIX##_ENCODING->new_encoder();                                           \
			bool successful = encoding_c_convert_through_utf8(decoder.get(), encoder.get(),                       \
			     ztd::span<const unsigned char>(                                                                  \
			          (const unsigned char*)input_data.data(), input_data.size() * sizeof(ztd_char##FROM_N##_t)), \
			     ztd::span<unsigned char>(                                                                        \
			          (unsigned char*)output_data.data(), output_data.size() * sizeof(ztd_char##TO_N##_t)));      \
			if (!successful) {                                                                                    \
				result = false;                                                                                  \
			}                                                                                                     \
		}                                                                                                          \
		const bool is_equal                                                                                        \
		     = std::equal(output_data.cbegin(), output_data.cend(), c_span_char##TO_N##_t_data(u##TO_N##_data),    \
		          c_span_char##TO_N##_t_data(u##TO_N##_data) + c_span_char##TO_N##_t_size(u##TO_N##_data));        \
		if (!result) {                                                                                             \
			if (TO_N == 16 && FROM_N == 8) {                                                                      \
				state.SkipWithError("unsupported");                                                              \
			}                                                                                                     \
			else {                                                                                                \
				state.SkipWithError("conversion failed with an error");                                          \
			}                                                                                                     \
		}                                                                                                          \
		else if (!is_equal) {                                                                                      \
			if (TO_N == 16 && FROM_N == 8) {                                                                      \
				state.SkipWithError("unsupported");                                                              \
			}                                                                                                     \
			else {                                                                                                \
				state.SkipWithError("conversion succeeded but produced illegitimate data");                      \
			}                                                                                                     \
			return;                                                                                               \
		}                                                                                                          \
	}                                                                                                               \
	static_assert(true, "")

UTF_CONVERSION_BENCHMARK(8, 16, , , BE, LE);
UTF_CONVERSION_BENCHMARK(16, 8, BE, LE, , );

#undef UTF_CONVERSION_BENCHMARK

static void utf8_to_utf16_well_formed_encoding_c_direct(benchmark::State& state) {
	const std::vector<ztd_char8_t> input_data(
	     c_span_char8_t_data(u8_data), c_span_char8_t_data(u8_data) + c_span_char8_t_size(u8_data));
	std::vector<ztd_char16_t> output_data(c_span_char16_t_size(u16_data));
	bool result                                   = true;
	std::unique_ptr<encoding_rs::Decoder> decoder = UTF_8_ENCODING->new_decoder_without_bom_handling();
	for (auto _ : state) {
		// we can skip one half, we are already have data in UTF-8
		uint32_t decoder_result                                 = INPUT_EMPTY;
		size_t decoder_read                                     = 0;
		size_t decoder_written                                  = 0;
		std::tie(decoder_result, decoder_read, decoder_written) = decoder->decode_to_utf16_without_replacement(
		     ztd::span<const unsigned char>((const unsigned char*)input_data.data(), input_data.size()), output_data,
		     false);
		if (decoder_result != INPUT_EMPTY) {
			result = false;
		}
	}
	const bool is_equal = std::equal(output_data.cbegin(), output_data.cend(), c_span_char16_t_data(u16_data),
	     c_span_char16_t_data(u16_data) + c_span_char16_t_size(u16_data));
	if (!result) {
		state.SkipWithError("conversion failed with an error");
	}
	else if (!is_equal) {
		state.SkipWithError("conversion succeeded but produced illegitimate data");
		return;
	}
}

static void utf8_to_utf16_init_well_formed_encoding_c_direct(benchmark::State& state) {
	const std::vector<ztd_char8_t> input_data(
	     c_span_char8_t_data(u8_data), c_span_char8_t_data(u8_data) + c_span_char8_t_size(u8_data));
	std::vector<ztd_char16_t> output_data(c_span_char16_t_size(u16_data));
	bool result = true;
	for (auto _ : state) {
		// we can skip one half, we are already have data in UTF-8
		std::unique_ptr<encoding_rs::Decoder> decoder           = UTF_8_ENCODING->new_decoder_without_bom_handling();
		uint32_t decoder_result                                 = INPUT_EMPTY;
		size_t decoder_read                                     = 0;
		size_t decoder_written                                  = 0;
		std::tie(decoder_result, decoder_read, decoder_written) = decoder->decode_to_utf16_without_replacement(
		     ztd::span<const unsigned char>((const unsigned char*)input_data.data(), input_data.size()), output_data,
		     false);
		if (decoder_result != INPUT_EMPTY) {
			result = false;
		}
	}
	const bool is_equal = std::equal(output_data.cbegin(), output_data.cend(), c_span_char16_t_data(u16_data),
	     c_span_char16_t_data(u16_data) + c_span_char16_t_size(u16_data));
	if (!result) {
		state.SkipWithError("conversion failed with an error");
	}
	else if (!is_equal) {
		state.SkipWithError("conversion succeeded but produced illegitimate data");
		return;
	}
}

// This is intentionally broken, as per the WHATWG spec.
// Don't ask me why anyone thought it was a good idea,
// from the spec writers to the implementers.
BENCHMARK(utf8_to_utf16_well_formed_encoding_c);
BENCHMARK(utf8_to_utf16_well_formed_encoding_c_direct);
BENCHMARK(utf16_to_utf8_well_formed_encoding_c);

BENCHMARK(utf8_to_utf16_init_well_formed_encoding_c);
BENCHMARK(utf8_to_utf16_init_well_formed_encoding_c_direct);
BENCHMARK(utf16_to_utf8_init_well_formed_encoding_c);

#endif

#endif
