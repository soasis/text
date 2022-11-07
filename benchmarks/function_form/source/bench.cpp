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

#include <benchmark/benchmark.h>

#include <ztd/cuneicode.h>

#include <ztd/tests/basic_unicode_strings.h>
#include <ztd/idk/size.h>

#include <barrier/convert.h>
#include <barrier/barrier.h>

#include <array>

static const ztd_char32_t* expected_input     = c_span_char32_t_data(u32_data);
static const std::size_t expected_input_size  = c_span_char32_t_size(u32_data);
static const ztd_char8_t* expected_output     = c_span_char8_t_data(u8_data);
static const std::size_t expected_output_size = c_span_char8_t_size(u8_data);

static void noop(benchmark::State& state) {
	for (auto _ : state) { }
}

static void memcpy_(benchmark::State& state) {
	using to_char_t = ztd_char8_t;
	std::vector<to_char_t> output_data(expected_output, expected_output + expected_output_size);
	for (auto _ : state) {
		std::memcpy(output_data.data(), expected_output, expected_output_size * sizeof(ztd_char8_t));
		state.PauseTiming();
		const bool is_equal = std::equal(output_data.data(), output_data.data() + expected_output_size,
		     expected_output, expected_output + expected_output_size);
		if (!is_equal) {
			state.SkipWithError("Data did not encode correctly.");
			return;
		}
		state.ResumeTiming();
	}
}

static void err_ptrptr_ptrsize_ptrptr_ptrsize(benchmark::State& state) {
	std::vector<ztd_char8_t> output_buffer;
	output_buffer.resize(expected_output_size * 2);
	for (auto _ : state) {
		const ztd_char32_t* original_input = expected_input;
		const ztd_char32_t* input          = original_input;
		ztd_char8_t* output                = output_buffer.data();
		size_t input_size_left             = expected_input_size;
		size_t output_size_left            = output_buffer.size();
		for (;;) {
			cnc_mcerror err = err_pptr_psize(&output, &output_size_left, &input, &input_size_left);
			if (err != CNC_MCERROR_OK) {
				state.SkipWithError("Data did not encode correctly.");
				return;
			}
			if (input_size_left == 0) {
				break;
			}
		}
		state.PauseTiming();
		const bool is_equal         = std::equal(output_buffer.data(), output_buffer.data() + expected_output_size,
		             expected_output, expected_output + expected_output_size);
		size_t expected_leftover    = output_buffer.size() - expected_output_size;
		const bool is_expected_size = expected_leftover == output_size_left;
		if (!is_equal || !is_expected_size) {
			state.SkipWithError("Data did not encode correctly.");
			return;
		}
		state.ResumeTiming();
	}
}

static void err_ptrptr_ptr_ptrptr_ptr(benchmark::State& state) {
	std::vector<ztd_char8_t> output_buffer;
	output_buffer.resize(expected_output_size * 2);
	for (auto _ : state) {
		const ztd_char32_t* original_input = expected_input;
		const ztd_char32_t* input          = original_input;
		const ztd_char32_t* input_last     = original_input + expected_input_size;
		ztd_char8_t* output                = output_buffer.data();
		ztd_char8_t* output_last           = output_buffer.data() + output_buffer.size();
		for (;;) {
			cnc_mcerror err = err_pptr_ptr(&output, output_last, &input, input_last);
			if (err != CNC_MCERROR_OK) {
				state.SkipWithError("Data did not encode correctly.");
				return;
			}
			if (input == input_last) {
				break;
			}
		}
		state.PauseTiming();
		size_t output_size_left     = output_last - output;
		const bool is_equal         = std::equal(output_buffer.data(), output_buffer.data() + expected_output_size,
		             expected_output, expected_output + expected_output_size);
		size_t expected_leftover    = output_buffer.size() - expected_output_size;
		const bool is_expected_size = expected_leftover == output_size_left;
		if (!is_equal || !is_expected_size) {
			state.SkipWithError("Data did not encode correctly.");
			return;
		}
		state.ResumeTiming();
	}
}

static void structsize_ptr_ptr_ptr_ptr(benchmark::State& state) {
	std::vector<ztd_char8_t> output_buffer;
	output_buffer.resize(expected_output_size * 2);
	for (auto _ : state) {
		const ztd_char32_t* original_input = expected_input;
		const ztd_char32_t* input          = original_input;
		const ztd_char32_t* input_last     = original_input + expected_input_size;
		ztd_char8_t* output                = output_buffer.data();
		ztd_char8_t* output_last           = output_buffer.data() + output_buffer.size();
		for (;;) {
			error_size_size err = size_struct_ptr_ptr(output, output_last, input, input_last);
			if (err.error != CNC_MCERROR_OK) {
				state.SkipWithError("Data did not encode correctly.");
				return;
			}
			input += err.input_read;
			output += err.output_written;
			if (input == input_last) {
				break;
			}
		}
		state.PauseTiming();
		size_t output_size_left     = output_last - output;
		const bool is_equal         = std::equal(output_buffer.data(), output_buffer.data() + expected_output_size,
		             expected_output, expected_output + expected_output_size);
		size_t expected_leftover    = output_buffer.size() - expected_output_size;
		const bool is_expected_size = expected_leftover == output_size_left;
		if (!is_equal || !is_expected_size) {
			state.SkipWithError("Data did not encode correctly.");
			return;
		}
		state.ResumeTiming();
	}
}

static void structsize_ptr_size_ptr_size(benchmark::State& state) {
	std::vector<ztd_char8_t> output_buffer;
	output_buffer.resize(expected_output_size * 2);
	for (auto _ : state) {
		const ztd_char32_t* original_input = expected_input;
		const ztd_char32_t* input          = original_input;
		ztd_char8_t* output                = output_buffer.data();
		size_t input_size                  = expected_input_size;
		size_t output_size                 = output_buffer.size();
		for (;;) {
			error_size_size err = size_struct_ptr_size(output, output_size, input, input_size);
			if (err.error != CNC_MCERROR_OK) {
				state.SkipWithError("Data did not encode correctly.");
				return;
			}
			input += err.input_read;
			output += err.output_written;
			input_size -= err.input_read;
			output_size -= err.output_written;
			if (input_size == 0) {
				break;
			}
		}
		state.PauseTiming();
		size_t output_size_left     = output_size;
		const bool is_equal         = std::equal(output_buffer.data(), output_buffer.data() + expected_output_size,
		             expected_output, expected_output + expected_output_size);
		size_t expected_leftover    = output_buffer.size() - expected_output_size;
		const bool is_expected_size = expected_leftover == output_size_left;
		if (!is_equal || !is_expected_size) {
			state.SkipWithError("Data did not encode correctly.");
			return;
		}
		state.ResumeTiming();
	}
}

static void structptr_ptr_ptr_ptr_ptr(benchmark::State& state) {
	std::vector<ztd_char8_t> output_buffer;
	output_buffer.resize(expected_output_size * 2);
	for (auto _ : state) {
		const ztd_char32_t* original_input = expected_input;
		const ztd_char32_t* input          = original_input;
		const ztd_char32_t* input_last     = original_input + expected_input_size;
		ztd_char8_t* output                = output_buffer.data();
		ztd_char8_t* output_last           = output_buffer.data() + output_buffer.size();
		for (;;) {
			error_ptr_ptr err = ptr_struct_ptr_ptr(output, output_last, input, input_last);
			if (err.error != CNC_MCERROR_OK) {
				state.SkipWithError("Data did not encode correctly.");
				return;
			}
			input  = err.input;
			output = err.output;
			if (input == input_last) {
				break;
			}
		}
		state.PauseTiming();
		size_t output_size_left     = output_last - output;
		const bool is_equal         = std::equal(output_buffer.data(), output_buffer.data() + expected_output_size,
		             expected_output, expected_output + expected_output_size);
		size_t expected_leftover    = output_buffer.size() - expected_output_size;
		const bool is_expected_size = expected_leftover == output_size_left;
		if (!is_equal || !is_expected_size) {
			state.SkipWithError("Data did not encode correctly.");
			return;
		}
		state.ResumeTiming();
	}
}

static void structptr_ptr_size_ptr_size(benchmark::State& state) {
	std::vector<ztd_char8_t> output_buffer;
	output_buffer.resize(expected_output_size * 2);
	for (auto _ : state) {
		const ztd_char32_t* original_input = expected_input;
		const ztd_char32_t* input          = original_input;
		const ztd_char32_t* input_last     = original_input + expected_input_size;
		ztd_char8_t* output                = output_buffer.data();
		ztd_char8_t* output_last           = output_buffer.data() + output_buffer.size();
		size_t input_size                  = expected_input_size;
		size_t output_size                 = output_buffer.size();
		for (;;) {
			error_ptr_ptr err = ptr_struct_ptr_size(output, output_size, input, input_size);
			if (err.error != CNC_MCERROR_OK) {
				state.SkipWithError("Data did not encode correctly.");
				return;
			}
			input  = err.input;
			output = err.output;
			if (input == input_last) {
				break;
			}
		}
		state.PauseTiming();
		size_t output_size_left     = output_last - output;
		const bool is_equal         = std::equal(output_buffer.data(), output_buffer.data() + expected_output_size,
		             expected_output, expected_output + expected_output_size);
		size_t expected_leftover    = output_buffer.size() - expected_output_size;
		const bool is_expected_size = expected_leftover == output_size_left;
		if (!is_equal || !is_expected_size) {
			state.SkipWithError("Data did not encode correctly.");
			return;
		}
		state.ResumeTiming();
	}
}

BENCHMARK(noop);
BENCHMARK(memcpy_);
BENCHMARK(err_ptrptr_ptrsize_ptrptr_ptrsize);
BENCHMARK(err_ptrptr_ptr_ptrptr_ptr);
BENCHMARK(structsize_ptr_ptr_ptr_ptr);
BENCHMARK(structsize_ptr_size_ptr_size);
BENCHMARK(structptr_ptr_ptr_ptr_ptr);
BENCHMARK(structptr_ptr_size_ptr_size);
