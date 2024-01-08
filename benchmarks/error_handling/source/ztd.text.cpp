// ============================================================================
//
// ztd.text
// Copyright © JeanHeyd "ThePhD" Meneide and Shepherd's Oasis, LLC
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
// https://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
//
// ========================================================================= //

#include <ztd/text/benchmarks/version.hpp>

#if ZTD_IS_ON(ZTD_TEXT_BENCHMARKS_ERROR_HANDLING_ZTD_TEXT_BENCHMARKS)

#include <benchmark/benchmark.h>

#include <ztd/text/benchmarks/fast_transcode.ztd.text.hpp>

#include <ztd/text.hpp>
#include <barrier/barrier.h>

#include <vector>
#include <string_view>

using u8string_view = std::basic_string_view<ztd_char8_t>;

static void skip_input_unicode_replacement_ztd_text(benchmark::State& state) {
	using from_char_t = ztd_char8_t;
	using to_char_t   = ztd_char16_t;
	const std::vector<from_char_t> input_data(
	     c_span_char8_t_data(u8_data), c_span_char8_t_data(u8_data) + c_span_char8_t_size(u8_data));
	std::vector<to_char_t> output_data(c_span_char16_t_size(u16_data));
	bool result = true;
	for (auto _ : state) {
		size_t input_size        = input_data.size();
		const from_char_t* input = input_data.data();
		size_t output_size       = output_data.size();
		to_char_t* output        = output_data.data();
		auto err                 = ztd::text::transcode_into_raw(ztd::span(input, input_size), ztd::text::utf8,
		                     ztd::span(output, output_size), ztd::text::utf16, ztd::text::replacement_handler,
		                     ztd::text::replacement_handler);
		if (err.error_code != ztd::text::encoding_error::ok) {
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
	}
}

static void skip_input_unicode_replacement_ztd_text_unbounded(benchmark::State& state) {
	using from_char_t = ztd_char8_t;
	using to_char_t   = ztd_char16_t;
	const std::vector<from_char_t> input_data(
	     c_span_char8_t_data(u8_data), c_span_char8_t_data(u8_data) + c_span_char8_t_size(u8_data));
	std::vector<to_char_t> output_data(c_span_char16_t_size(u16_data));
	bool result = true;
	for (auto _ : state) {
		size_t input_size        = input_data.size();
		const from_char_t* input = input_data.data();
		to_char_t* output        = output_data.data();
		auto err                 = ztd::text::transcode_into(ztd::span(input, input_size), ztd::text::utf8,
		                     ztd::ranges::unbounded_view(output), ztd::text::utf16, ztd::text::replacement_handler,
		                     ztd::text::replacement_handler);
		if (err.error_code != ztd::text::encoding_error::ok) {
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
	}
}

static void skip_input_unicode_replacement_ztd_text_view(benchmark::State& state) {
	using from_char_t = ztd_char8_t;
	using to_char_t   = ztd_char16_t;
	const std::vector<from_char_t> input_data(
	     c_span_char8_t_data(u8_data), c_span_char8_t_data(u8_data) + c_span_char8_t_size(u8_data));
	std::vector<to_char_t> output_data(c_span_char16_t_size(u16_data));
	const ::std::size_t output_size = output_data.size();
	bool result                     = true;
	for (auto _ : state) {
		ztd::text::transcode_view<ztd::text::utf8_t, ztd::text::utf16_t, ::ztd::span<const from_char_t>,
		     ztd::text::replacement_handler_t, ztd::text::replacement_handler_t>
		     view(input_data);
		auto output = output_data.data();
		for (const to_char_t value : view) {
			*output = value;
			++output;
		}
		if (output != (output_data.data() + output_size)) {
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
	}
}

static void skip_input_ascii_replacement_ztd_text(benchmark::State& state) {
	using from_char_t = ztd_char8_t;
	using to_char_t   = ztd_char16_t;
	const std::vector<from_char_t> input_data(
	     c_span_char8_t_data(u8_data), c_span_char8_t_data(u8_data) + c_span_char8_t_size(u8_data));
	std::vector<to_char_t> output_data(c_span_char16_t_size(u16_data));
	ztd::text::replacement_of_handler u8handler { u8string_view(reinterpret_cast<const ztd_char8_t*>(u8"?")),
		std::u32string_view(U"?") };
	ztd::text::replacement_of_handler u16handler { std::u16string_view(u"?"), std::u32string_view(U"?") };
	bool result = true;
	for (auto _ : state) {
		size_t input_size        = input_data.size();
		const from_char_t* input = input_data.data();
		size_t output_size       = output_data.size();
		to_char_t* output        = output_data.data();
		auto err                 = ztd::text::transcode_into_raw(ztd::span(input, input_size), ztd::text::utf8,
		                     ztd::span(output, output_size), ztd::text::utf16, u8handler, u16handler);
		if (err.error_code != ztd::text::encoding_error::ok) {
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
	}
}

static void skip_input_ascii_replacement_ztd_text_unbounded(benchmark::State& state) {
	using from_char_t = ztd_char8_t;
	using to_char_t   = ztd_char16_t;
	const std::vector<from_char_t> input_data(
	     c_span_char8_t_data(u8_data), c_span_char8_t_data(u8_data) + c_span_char8_t_size(u8_data));
	std::vector<to_char_t> output_data(c_span_char16_t_size(u16_data));
	bool result = true;
	ztd::text::replacement_of_handler u8handler { u8string_view(reinterpret_cast<const ztd_char8_t*>(u8"?")),
		std::u32string_view(U"?") };
	ztd::text::replacement_of_handler u16handler { std::u16string_view(u"?"), std::u32string_view(U"?") };
	for (auto _ : state) {
		size_t input_size        = input_data.size();
		const from_char_t* input = input_data.data();
		to_char_t* output        = output_data.data();
		auto err                 = ztd::text::transcode_into(ztd::span(input, input_size), ztd::text::utf8,
		                     ztd::ranges::unbounded_view(output), ztd::text::utf16, u8handler, u16handler);
		if (err.error_code != ztd::text::encoding_error::ok) {
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
	}
}

static void skip_input_ascii_replacement_ztd_text_view(benchmark::State& state) {
	using from_char_t = ztd_char8_t;
	using to_char_t   = ztd_char16_t;
	const std::vector<from_char_t> input_data(
	     c_span_char8_t_data(u8_data), c_span_char8_t_data(u8_data) + c_span_char8_t_size(u8_data));
	std::vector<to_char_t> output_data(c_span_char16_t_size(u16_data));
	const ::std::size_t output_size = output_data.size();
	bool result                     = true;
	ztd::text::replacement_of_handler u8handler { u8string_view(reinterpret_cast<const ztd_char8_t*>(u8"?")),
		std::u32string_view(U"?") };
	ztd::text::replacement_of_handler u16handler { std::u16string_view(u"?"), std::u32string_view(U"?") };
	for (auto _ : state) {
		ztd::text::transcode_view<ztd::text::utf8_t, ztd::text::utf16_t, ::ztd::span<const from_char_t>,
		     decltype(u8handler), decltype(u16handler)>
		     view(input_data, ztd::text::utf8, ztd::text::utf16, u8handler, u16handler);
		auto output = output_data.data();
		for (const to_char_t value : view) {
			*output = value;
			++output;
		}
		if (output != (output_data.data() + output_size)) {
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
	}
}

static void skip_input_no_replacement_ztd_text(benchmark::State& state) {
	using from_char_t = ztd_char8_t;
	using to_char_t   = ztd_char16_t;
	const std::vector<from_char_t> input_data(
	     c_span_char8_t_data(u8_data), c_span_char8_t_data(u8_data) + c_span_char8_t_size(u8_data));
	std::vector<to_char_t> output_data(c_span_char16_t_size(u16_data));
	bool result = true;
	for (auto _ : state) {
		size_t input_size        = input_data.size();
		const from_char_t* input = input_data.data();
		size_t output_size       = output_data.size();
		to_char_t* output        = output_data.data();
		auto err                 = ztd::text::transcode_into_raw(ztd::span(input, input_size), ztd::text::utf8,
		                     ztd::span(output, output_size), ztd::text::utf16, ztd::text::skip_handler, ztd::text::skip_handler);
		if (err.error_code != ztd::text::encoding_error::ok) {
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
	}
}

static void skip_input_no_replacement_ztd_text_unbounded(benchmark::State& state) {
	using from_char_t = ztd_char8_t;
	using to_char_t   = ztd_char16_t;
	const std::vector<from_char_t> input_data(
	     c_span_char8_t_data(u8_data), c_span_char8_t_data(u8_data) + c_span_char8_t_size(u8_data));
	std::vector<to_char_t> output_data(c_span_char16_t_size(u16_data));
	bool result = true;
	for (auto _ : state) {
		size_t input_size        = input_data.size();
		const from_char_t* input = input_data.data();
		to_char_t* output        = output_data.data();
		auto err                 = ztd::text::transcode_into(ztd::span(input, input_size), ztd::text::utf8,
		                     ztd::ranges::unbounded_view(output), ztd::text::utf16, ztd::text::skip_handler, ztd::text::skip_handler);
		if (err.error_code != ztd::text::encoding_error::ok) {
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
	}
}

static void skip_input_no_replacement_ztd_text_view(benchmark::State& state) {
	using from_char_t = ztd_char8_t;
	using to_char_t   = ztd_char16_t;
	const std::vector<from_char_t> input_data(
	     c_span_char8_t_data(u8_data), c_span_char8_t_data(u8_data) + c_span_char8_t_size(u8_data));
	std::vector<to_char_t> output_data(c_span_char16_t_size(u16_data));
	const ::std::size_t output_size = output_data.size();
	bool result                     = true;
	for (auto _ : state) {
		ztd::text::transcode_view<ztd::text::utf8_t, ztd::text::utf16_t, ::ztd::span<const from_char_t>,
		     ztd::text::skip_handler_t, ztd::text::skip_handler_t>
		     view(input_data);
		auto output = output_data.data();
		for (const to_char_t value : view) {
			*output = value;
			++output;
		}
		if (output != (output_data.data() + output_size)) {
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
	}
}

BENCHMARK(skip_input_unicode_replacement_ztd_text);
BENCHMARK(skip_input_unicode_replacement_ztd_text_unbounded);
BENCHMARK(skip_input_unicode_replacement_ztd_text_view);

BENCHMARK(skip_input_ascii_replacement_ztd_text);
BENCHMARK(skip_input_ascii_replacement_ztd_text_unbounded);
BENCHMARK(skip_input_ascii_replacement_ztd_text_view);

BENCHMARK(skip_input_no_replacement_ztd_text);
BENCHMARK(skip_input_no_replacement_ztd_text_unbounded);
BENCHMARK(skip_input_no_replacement_ztd_text_view);
#endif
