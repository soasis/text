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

#ifndef ZTD_TEXT_BENCHMARKS_ZTD_TRANSCODE_SPEED_HPP
#define ZTD_TEXT_BENCHMARKS_ZTD_TRANSCODE_SPEED_HPP

#include <ztd/text/extension.hpp>

#include <ztd/idk/span.hpp>
#include <ztd/idk/endian.hpp>
#include <ztd/ranges/unbounded.hpp>

#include <simdutf.h>

#define TEXT_TRANSCODE_EXTENSION_POINTS(FROM_N, TO_N, FROM_BIG_SUFFIX, FROM_LIL_SUFFIX, TO_BIG_SUFFIX, TO_LIL_SUFFIX)  \
	template <typename FromErrorHandler, typename ToErrorHandler, typename FromState, typename ToState,               \
	     typename PivotRange>                                                                                         \
	auto text_transcode(::ztd::tag<ztd::text::utf##FROM_N##_t, ztd::text::utf##TO_N##_t>,                             \
	     ztd::span<const ztd_char##FROM_N##_t> input, const ztd::text::utf##FROM_N##_t& from,                         \
	     ztd::ranges::unbounded_view<ztd_char##TO_N##_t*> output, const ztd::text::utf##TO_N##_t& to,                 \
	     FromErrorHandler&& from_error_handler, ToErrorHandler&& to_error_handler, FromState& from_state,             \
	     ToState& to_state, ztd::text::pivot<PivotRange>& __pivot) {                                                  \
		using from_char_t       = std::conditional_t<FROM_N == 8, char, ztd_char##FROM_N##_t>;                       \
		using to_char_t         = std::conditional_t<TO_N == 8, char, ztd_char##TO_N##_t>;                           \
		using UFromErrorHandler = ztd::remove_cvref_t<FromErrorHandler>;                                             \
		using UToErrorHandler   = ztd::remove_cvref_t<ToErrorHandler>;                                               \
		if constexpr (ztd::text::is_ignorable_error_handler_v<UFromErrorHandler> /* cf */                            \
		     && ztd::text::is_ignorable_error_handler_v<UToErrorHandler>) {                                          \
			const std::size_t written_chars = (ztd::endian::native == ztd::endian::big                              \
			          ? simdutf::convert_valid_utf##FROM_N##FROM_BIG_SUFFIX##_to_utf##TO_N##TO_BIG_SUFFIX           \
			          : simdutf::convert_valid_utf##FROM_N##FROM_LIL_SUFFIX##_to_utf##TO_N##TO_LIL_SUFFIX)(         \
			     (const from_char_t*)input.data(), input.size(), (to_char_t*)output.begin());                       \
			return ztd::text::transcode_result<decltype(input), decltype(output), FromState, ToState>(              \
			     input.subspan(input.size()), ztd::ranges::unbounded_view(output.begin() + written_chars),          \
			     from_state, to_state, ztd::text::encoding_error::ok);                                              \
		}                                                                                                            \
		else {                                                                                                       \
			const simdutf::result res = (ztd::endian::native == ztd::endian::big                                    \
			          ? simdutf::convert_utf##FROM_N##FROM_BIG_SUFFIX##_to_utf##TO_N##TO_BIG_SUFFIX##_with_errors   \
			          : simdutf::convert_utf##FROM_N##FROM_LIL_SUFFIX##_to_utf##TO_N##TO_LIL_SUFFIX##_with_errors)( \
			     (const from_char_t*)input.data(), input.size(), (to_char_t*)output.begin());                       \
			if (res.error == simdutf::error_code::SUCCESS) {                                                        \
				return ztd::text::transcode_result<decltype(input), decltype(output), FromState, ToState>(         \
				     input.subspan(input.size()), ztd::ranges::unbounded_view(output.begin() + res.count),         \
				     from_state, to_state, ztd::text::encoding_error::ok);                                         \
			}                                                                                                       \
			return ztd::text::basic_transcode_into(input, from, output, to,                                         \
			     ::std::forward<FromErrorHandler>(from_error_handler),                                              \
			     ::std::forward<ToErrorHandler>(to_error_handler), from_state, to_state, __pivot);                  \
		}                                                                                                            \
	}                                                                                                                 \
                                                                                                                       \
	template <typename FromErrorHandler, typename ToErrorHandler, typename FromState, typename ToState,               \
	     typename PivotRange>                                                                                         \
	auto text_transcode(::ztd::tag<ztd::text::utf##FROM_N##_t, ztd::text::utf##TO_N##_t>,                             \
	     ztd::span<const ztd_char##FROM_N##_t> input, const ztd::text::utf##FROM_N##_t& from,                         \
	     ztd::span<ztd_char##TO_N##_t> output, const ztd::text::utf##TO_N##_t& to,                                    \
	     FromErrorHandler&& from_error_handler, ToErrorHandler&& to_error_handler, FromState& from_state,             \
	     ToState& to_state, ztd::text::pivot<PivotRange>& __pivot) {                                                  \
		using from_char_t       = std::conditional_t<FROM_N == 8, char, ztd_char##FROM_N##_t>;                       \
		using to_char_t         = std::conditional_t<TO_N == 8, char, ztd_char##TO_N##_t>;                           \
		using UFromErrorHandler = ztd::remove_cvref_t<FromErrorHandler>;                                             \
		using UToErrorHandler   = ztd::remove_cvref_t<ToErrorHandler>;                                               \
		if constexpr (!ztd::text::is_ignorable_error_handler_v<UFromErrorHandler> /* cf */                           \
		     || !ztd::text::is_ignorable_error_handler_v<UToErrorHandler>) {                                         \
			const simdutf::result validate_res = (ztd::endian::native == ztd::endian::big                           \
			          ? simdutf::validate_utf##FROM_N##FROM_BIG_SUFFIX##_with_errors                                \
			          : simdutf::validate_utf##FROM_N##FROM_LIL_SUFFIX##_with_errors)(                              \
			     reinterpret_cast<const from_char_t*>(input.data()), input.size());                                 \
			if (validate_res.error != simdutf::error_code::SUCCESS) {                                               \
				return ztd::text::basic_transcode_into(input, from, output, to,                                    \
				     ::std::forward<FromErrorHandler>(from_error_handler),                                         \
				     ::std::forward<ToErrorHandler>(to_error_handler), from_state, to_state, __pivot);             \
			}                                                                                                       \
		}                                                                                                            \
		const size_t output_count = (ztd::endian::native == ztd::endian::big                                         \
		          ? simdutf::utf##TO_N##_length_from_utf##FROM_N##FROM_BIG_SUFFIX                                    \
		          : simdutf::utf##TO_N##_length_from_utf##FROM_N##FROM_LIL_SUFFIX)(                                  \
		     reinterpret_cast<const from_char_t*>(input.data()), input.size());                                      \
		if (output_count <= output.size()) {                                                                         \
			std::size_t written_count = (ztd::endian::native == ztd::endian::big                                    \
			          ? simdutf::convert_valid_utf##FROM_N##FROM_BIG_SUFFIX##_to_utf##TO_N##TO_BIG_SUFFIX           \
			          : simdutf::convert_valid_utf##FROM_N##FROM_LIL_SUFFIX##_to_utf##TO_N##TO_LIL_SUFFIX)(         \
			     reinterpret_cast<const from_char_t*>(input.data()), input.size(),                                  \
			     reinterpret_cast<to_char_t*>(output.data()));                                                      \
			return ztd::text::transcode_result<decltype(input), decltype(output), FromState, ToState>(              \
			     input.subspan(input.size()), output.subspan(written_count), from_state, to_state,                  \
			     ztd::text::encoding_error::ok);                                                                    \
		}                                                                                                            \
		return ztd::text::basic_transcode_into(input, from, output, to,                                              \
		     ::std::forward<FromErrorHandler>(from_error_handler), ::std::forward<ToErrorHandler>(to_error_handler), \
		     from_state, to_state, __pivot);                                                                         \
	}                                                                                                                 \
	static_assert(true, "")

TEXT_TRANSCODE_EXTENSION_POINTS(8, 16, , , be, le);
TEXT_TRANSCODE_EXTENSION_POINTS(16, 8, be, le, , );
TEXT_TRANSCODE_EXTENSION_POINTS(32, 16, , , be, le);
TEXT_TRANSCODE_EXTENSION_POINTS(16, 32, be, le, , );
TEXT_TRANSCODE_EXTENSION_POINTS(32, 8, , , , );
/// This one is expanded below, as an example, with commentary!
// TEXT_TRANSCODE_EXTENSION_POINTS(8, 32, , , , );
// Macro hygiene!
#undef TEXT_TRANSCODE_EXTENSION_POINTS

template <typename FromErrorHandler, typename ToErrorHandler, typename FromState, typename ToState, typename PivotRange>
auto text_transcode(::ztd::tag<ztd::text::utf8_t, ztd::text::utf32_t>, ztd::span<const ztd_char8_t> input,
     const ztd::text::utf8_t& from, ztd::ranges::unbounded_view<ztd_char32_t*> output, const ztd::text::utf32_t& to,
     FromErrorHandler&& from_error_handler, ToErrorHandler&& to_error_handler, FromState& from_state, ToState& to_state,
     ztd::text::pivot<PivotRange>& __pivot) {
	using UFromErrorHandler          = ztd::remove_cvref_t<FromErrorHandler>;
	using UToErrorHandler            = ztd::remove_cvref_t<ToErrorHandler>;
	constexpr bool no_error_handling = ztd::text::is_ignorable_error_handler_v<UFromErrorHandler> // cf
	     && ztd::text::is_ignorable_error_handler_v<UToErrorHandler>;
	if constexpr (no_error_handling) {
		// UTF-8 to UTF-32, with the assumption that the input is valid UTF-8 and does not need to be checked for
		// validity.
		const std::size_t written_count
		     = simdutf::convert_valid_utf8_to_utf32((const char*)input.data(), input.size(), output.begin());
		return ztd::text::transcode_result<decltype(input), decltype(output), FromState, ToState>(
		     input.subspan(input.size()), ztd::ranges::unbounded_view(output.begin() + written_count), from_state,
		     to_state);
	}
	else {
		// UTF-8 to UTF-32, but we cannot assume the error handler is ignorable in these cases. So, we have to check
		// everything and convert.
		const simdutf::result res
		     = simdutf::convert_utf8_to_utf32_with_errors((const char*)input.data(), input.size(), output.begin());
		if (res.error == simdutf::error_code::SUCCESS) {
			return ztd::text::transcode_result<decltype(input), decltype(output), FromState, ToState>(
			     input.subspan(input.size()), ztd::ranges::unbounded_view(output.begin() + res.count), from_state,
			     to_state, ztd::text::encoding_error::ok);
		}
		// If it fails, we have to simply bail and go to the base case, which will find precisely where we failed and
		// do the appropriate callbacks and work for us. Note that this is due to what information SIMDUTF returns to
		// us. On failure, the simdutf::result type gives us a
		// `.count` variable. That variable is the # of output written on success, or the # of input read and
		// converted on failure. In the first case, it's fine: we get all the information we need because we know the
		// entire input has been consumed. In the second case, it's terrible because we have NO idea how many code
		// units were succesfully written! So we know where the error happened in the input, but cannot take advantage
		// of any work already done by the algorithm. Which REALLY sucks!!
		return ztd::text::basic_transcode_into(input, from, output, to,
		     ::std::forward<FromErrorHandler>(from_error_handler), ::std::forward<ToErrorHandler>(to_error_handler),
		     from_state, to_state, __pivot);
	}
}

template <typename FromErrorHandler, typename ToErrorHandler, typename FromState, typename ToState, typename PivotRange>
auto text_transcode(::ztd::tag<ztd::text::utf8_t, ztd::text::utf32_t>, ztd::span<const ztd_char8_t> input,
     const ztd::text::utf8_t& from, ztd::span<ztd_char32_t> output, const ztd::text::utf32_t& to,
     FromErrorHandler&& from_error_handler, ToErrorHandler&& to_error_handler, FromState& from_state, ToState& to_state,
     ztd::text::pivot<PivotRange>& __pivot) {
	// UTF-16 to UTF-8, but we cannot assume the error handler is ignorable in these cases. So, we have to
	// check everything and convert. we use the validation function as a way to also check the count.
	using UFromErrorHandler = ztd::remove_cvref_t<FromErrorHandler>;
	using UToErrorHandler   = ztd::remove_cvref_t<ToErrorHandler>;
	if constexpr (!ztd::text::is_ignorable_error_handler_v<UFromErrorHandler> // cf
	     || !ztd::text::is_ignorable_error_handler_v<UToErrorHandler>) {
		const simdutf::result validate_res
		     = simdutf::validate_utf8_with_errors(reinterpret_cast<const char*>(input.data()), input.size());
		if (validate_res.error != simdutf::error_code::SUCCESS) {
			// It is not vlaid, we must bail here.
			return ztd::text::basic_transcode_into(input, from, output, to,
			     ::std::forward<FromErrorHandler>(from_error_handler),
			     ::std::forward<ToErrorHandler>(to_error_handler), from_state, to_state, __pivot);
		}
	}
	// Now, we check to make sure the size is appropriate so we do not overflow the output buffer.
	// Not we don't have to do this if we have an unbounded_view type.
	const size_t output_count
	     = simdutf::utf32_length_from_utf8(reinterpret_cast<const char*>(input.data()), input.size());
	if (output_count <= output.size()) {
		// we can successfully convert the data: do so!
		std::size_t written_count = simdutf::convert_valid_utf8_to_utf32(
		     reinterpret_cast<const char*>(input.data()), input.size(), output.data());
		return ztd::text::transcode_result<decltype(input), decltype(output), FromState, ToState>(
		     input.subspan(input.size()), output.subspan(written_count), from_state, to_state,
		     ztd::text::encoding_error::ok);
	}
	// If it fails, we have to simply bail and go to the base case, which will find precisely where we failed
	// and do the appropriate callbacks and work for us. Note that this is due to what information SIMDUTF
	// returns to us. On failure, the simdutf::result type gives us a
	// `.count` variable. That variable is the # of output written on success, or the # of input read and
	// converted on failure. In the first case, it's fine: we get all the information we need because we know
	// the entire input has been consumed. In the second case, it's terrible because we have NO idea how many
	// code units were succesfully written! So we know where the error happened in the input, but cannot take
	// advantage of any work already done by the algorithm. Which REALLY sucks!!
	return ztd::text::basic_transcode_into(input, from, output, to,
	     ::std::forward<FromErrorHandler>(from_error_handler), ::std::forward<ToErrorHandler>(to_error_handler),
	     from_state, to_state, __pivot);
}

#endif
