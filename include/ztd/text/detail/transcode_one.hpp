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

#pragma once

#ifndef ZTD_TEXT_DETAIL_TRANSCODE_ONE_HPP
#define ZTD_TEXT_DETAIL_TRANSCODE_ONE_HPP

#include <ztd/text/version.hpp>

#include <ztd/text/encode_result.hpp>
#include <ztd/text/decode_result.hpp>
#include <ztd/text/transcode_result.hpp>
#include <ztd/text/validate_result.hpp>
#include <ztd/text/count_result.hpp>
#include <ztd/text/error_handler.hpp>
#include <ztd/text/state.hpp>
#include <ztd/text/code_point.hpp>
#include <ztd/text/code_unit.hpp>
#include <ztd/text/unbounded.hpp>
#include <ztd/text/subrange.hpp>
#include <ztd/text/tag.hpp>

#include <ztd/text/detail/is_lossless.hpp>
#include <ztd/text/detail/blackhole_iterator.hpp>
#include <ztd/text/detail/encoding_iterator_storage.hpp>
#include <ztd/text/detail/adl.hpp>
#include <ztd/text/detail/encoding_range.hpp>
#include <ztd/text/detail/span.hpp>
#include <ztd/text/detail/algorithm.hpp>

#include <array>
#include <algorithm>

#include <ztd/text/detail/prologue.hpp>

namespace ztd { namespace text {
	ZTD_TEXT_INLINE_ABI_NAMESPACE_OPEN_I_

	namespace __txt_detail {

		enum class __consume : unsigned char { __no = 0, __embrace_the_void = 1 };
		enum class __transaction : unsigned char { __encode = 0, __decode = 1 };

		template <__consume _ConsumeIntoTheNothingness, typename _Encoding, typename _Input, typename _Output,
			typename _ErrorHandler, typename _State>
		constexpr auto __basic_decode_one(_Input&& __input, _Encoding&& __encoding, _Output&& __output,
			_ErrorHandler& __error_handler, _State& __state) {
			using _UInput         = __remove_cvref_t<_Input>;
			using _UOutput        = __remove_cvref_t<_Output>;
			using _UEncoding      = __remove_cvref_t<_Encoding>;
			using _ValueType      = __range_value_type_t<_UOutput>;
			using _OutputIterator = __range_iterator_t<_UOutput>;
			using _Blackhole      = unbounded_view<__blackhole_iterator>;

			if constexpr (__is_iterator_output_iterator_v<__range_iterator_t<_UInput>>) {
				return __encoding.decode_one(
					::std::forward<_Input>(__input), ::std::forward<_Output>(__output), __error_handler, __state);
			}
			else if constexpr (_ConsumeIntoTheNothingness == __consume::__embrace_the_void) {
				constexpr ::std::size_t _MaxValues = max_code_units_v<_UEncoding>;
				using _Sentinel                    = __range_sentinel_t<_UOutput>;
				using _UnboundedSpan               = unbounded_view<_ValueType*>;
				using _Span                        = ::ztd::text::span<_ValueType>;
				using _OutRange                    = subrange<_OutputIterator, _Sentinel>;
				if constexpr (__is_decode_one_callable_v<_UEncoding, _Input, _Blackhole, _ErrorHandler, _State>) {
					(void)__output;
					_Blackhole __output_range(__blackhole_iterator {});
					return __encoding.decode_one(
						::std::forward<_Input>(__input), __output_range, __error_handler, __state);
				}
				else if constexpr (__is_decode_one_callable_v<_UEncoding, _Input, _UnboundedSpan, _ErrorHandler,
					                   _State>) {
					_ValueType __fake[_MaxValues] {};
					_ValueType* __first = __fake;
					_UnboundedSpan __output_range(__first);
					return __encoding.decode_one(
						::std::forward<_Input>(__input), __output_range, __error_handler, __state);
				}
				else if constexpr (__is_decode_one_callable_v<_UEncoding, _Input, _Span, _ErrorHandler, _State>) {
					_ValueType __fake[_MaxValues] {};
					_ValueType* __first = __fake;
					_ValueType* __last  = __first + _MaxValues;
					_Span __output_range(__first, __last);
					return __encoding.decode_one(
						::std::forward<_Input>(__input), __output_range, __error_handler, __state);
				}
				else if constexpr (__is_decode_one_callable_v<_UEncoding, _Input, _Output, _ErrorHandler, _State>) {
					return __encoding.decode_one(::std::forward<_Input>(__input),
						::std::forward<_Output>(__output), __error_handler, __state);
				}
				else {
					auto __first = __adl::__adl_begin(::std::forward<_Output>(__output));
					auto __last  = __adl::__adl_end(::std::forward<_Output>(__output));
					_OutRange __output_range(::std::move(__first), ::std::move(__last));
					return __encoding.decode_one(
						::std::forward<_Input>(__input), ::std::move(__output_range), __error_handler, __state);
				}
			}
			else {
				using _Sentinel  = __range_sentinel_t<_UOutput>;
				using _Unbounded = unbounded_view<_OutputIterator>;
				using _Span      = ::ztd::text::span<_ValueType>;
				using _OutRange  = subrange<_OutputIterator, _Sentinel>;
				if constexpr (__is_decode_one_callable_v<_UEncoding, _Input, _Output, _ErrorHandler, _State>) {
					return __encoding.decode_one(::std::forward<_Input>(__input),
						::std::forward<_Output>(__output), __error_handler, __state);
				}
				else if constexpr (__is_decode_one_callable_v<_UEncoding, _Input, _Unbounded, _ErrorHandler,
					                   _State>) {
					auto __first = __adl::__adl_begin(::std::forward<_Output>(__output));
					_Unbounded __output_range(::std::move(__first));
					return __encoding.decode_one(
						::std::forward<_Input>(__input), ::std::move(__output_range), __error_handler, __state);
				}
				else if constexpr (__is_decode_one_callable_v<_UEncoding, _Input, _Unbounded, _ErrorHandler,
					                   _State>) {
					auto __first = __adl::__adl_begin(::std::forward<_Output>(__output));
					_Unbounded __output_range(::std::move(__first));
					return __encoding.decode_one(
						::std::forward<_Input>(__input), ::std::move(__output_range), __error_handler, __state);
				}
				else if constexpr (
					__is_iterator_concept_or_better_v<contiguous_iterator_tag,
					     _OutputIterator> && __is_decode_one_callable_v<_UEncoding, _Input, _Span, _ErrorHandler, _State>) {
					auto __first = __adl::__adl_begin(::std::forward<_Output>(__output));
					auto __last  = __adl::__adl_end(::std::forward<_Output>(__output));
					_Span __output_range(::std::move(__first), ::std::move(__last));
					return __encoding.decode_one(
						::std::forward<_Input>(__input), __output_range, __error_handler, __state);
				}
				else {
					auto __first = __adl::__adl_begin(::std::forward<_Output>(__output));
					auto __last  = __adl::__adl_end(::std::forward<_Output>(__output));
					_OutRange __output_range(::std::move(__first), ::std::move(__last));
					return __encoding.decode_one(
						::std::forward<_Input>(__input), ::std::move(__output_range), __error_handler, __state);
				}
			}
		}

		template <__consume _ConsumeIntoTheNothingness, typename _Encoding, typename _Input, typename _Output,
			typename _ErrorHandler, typename _State>
		constexpr auto __basic_encode_one(_Input&& __input, _Encoding&& __encoding, _Output&& __output,
			_ErrorHandler& __error_handler, _State& __state) {
			using _UOutput        = __remove_cvref_t<_Output>;
			using _UEncoding      = __remove_cvref_t<_Encoding>;
			using _OutputIterator = __range_iterator_t<_UOutput>;
			using _Blackhole      = unbounded_view<__blackhole_iterator>;

			if constexpr (__is_iterator_output_iterator_v<_OutputIterator>) {
				return __encoding.encode_one(
					::std::forward<_Input>(__input), ::std::forward<_Output>(__output), __error_handler, __state);
			}
			else if constexpr (_ConsumeIntoTheNothingness == __consume::__embrace_the_void) {
				using _ValueType                   = __range_value_type_t<_UOutput>;
				using _Sentinel                    = __range_sentinel_t<_UOutput>;
				using _Unbounded                   = unbounded_view<_OutputIterator>;
				using _UnboundedSpan               = unbounded_view<_ValueType*>;
				using _Span                        = ::ztd::text::span<_ValueType>;
				using _OutRange                    = subrange<_OutputIterator, _Sentinel>;
				constexpr ::std::size_t _MaxValues = max_code_units_v<_UEncoding>;

				if constexpr (
					__is_encode_error_handler_callable_v<_UEncoding, _Input, _Blackhole, _ErrorHandler,
					     _State> && __is_encode_one_callable_v<_UEncoding, _Input, _Blackhole, _ErrorHandler, _State>) {
					(void)__output;
					_Blackhole __output_range(__blackhole_iterator {});
					return __encoding.encode_one(
						::std::forward<_Input>(__input), __output_range, __error_handler, __state);
				}
				else if constexpr (
					__is_encode_error_handler_callable_v<_UEncoding, _Input, _UnboundedSpan, _ErrorHandler,
					     _State> && __is_encode_one_callable_v<_UEncoding, _Input, _UnboundedSpan, _ErrorHandler, _State>) {
					_ValueType __fake[_MaxValues] {};
					_ValueType* __first = __fake;
					_UnboundedSpan __output_range(__first);
					return __encoding.encode_one(
						::std::forward<_Input>(__input), __output_range, __error_handler, __state);
				}
				else if constexpr (
					__is_encode_error_handler_callable_v<_UEncoding, _Input, _Unbounded, _ErrorHandler,
					     _State> && __is_encode_one_callable_v<_UEncoding, _Input, _Unbounded, _ErrorHandler, _State>) {
					auto __first = __adl::__adl_begin(::std::forward<_Output>(__output));
					_Unbounded __output_range(::std::move(__first));
					return __encoding.encode_one(
						::std::forward<_Input>(__input), __output_range, __error_handler, __state);
				}
				else if constexpr (
					__is_encode_error_handler_callable_v<_UEncoding, _Input, _Span, _ErrorHandler,
					     _State> && __is_encode_one_callable_v<_UEncoding, _Input, _Span, _ErrorHandler, _State>) {
					_ValueType __fake[_MaxValues] {};
					_ValueType* __first = __fake;
					_ValueType* __last  = __first + _MaxValues;
					_Span __output_range(__first, __last);
					return __encoding.encode_one(
						::std::forward<_Input>(__input), __output_range, __error_handler, __state);
				}
				else if constexpr (
					__is_iterator_concept_or_better_v<contiguous_iterator_tag,
					     _OutputIterator> && __is_encode_error_handler_callable_v<_UEncoding, _Input, _Span, _ErrorHandler, _State> && __is_encode_one_callable_v<_UEncoding, _Input, _Span, _ErrorHandler, _State>) {
					auto __first = __adl::__adl_begin(::std::forward<_Output>(__output));
					auto __last  = __adl::__adl_end(::std::forward<_Output>(__output));
					_Span __output_range(::std::move(__first), ::std::move(__last));
					return __encoding.encode_one(
						::std::forward<_Input>(__input), __output_range, __error_handler, __state);
				}
				else {
					auto __first = __adl::__adl_begin(::std::forward<_Output>(__output));
					auto __last  = __adl::__adl_end(::std::forward<_Output>(__output));
					_OutRange __output_range(::std::move(__first), ::std::move(__last));
					return __encoding.encode_one(
						::std::forward<_Input>(__input), __output_range, __error_handler, __state);
				}
			}
			else {
				using _ValueType = __range_value_type_t<_UOutput>;
				using _Sentinel  = __range_sentinel_t<_UOutput>;
				using _Unbounded = unbounded_view<_OutputIterator>;
				using _Span      = ::ztd::text::span<_ValueType>;
				using _OutRange  = subrange<_OutputIterator, _Sentinel>;
				if constexpr (
					__is_encode_error_handler_callable_v<_UEncoding, _Input, _Output, _ErrorHandler,
					     _State> && __is_encode_one_callable_v<_UEncoding, _Input, _Output, _ErrorHandler, _State>) {
					return __encoding.encode_one(::std::forward<_Input>(__input),
						::std::forward<_Output>(__output), __error_handler, __state);
				}
				else if constexpr (
					__is_encode_error_handler_callable_v<_UEncoding, _Input, _Unbounded, _ErrorHandler,
					     _State> && __is_encode_one_callable_v<_UEncoding, _Input, _Unbounded, _ErrorHandler, _State>) {
					auto __first = __adl::__adl_begin(::std::forward<_Output>(__output));
					_Unbounded __output_range(::std::move(__first));
					return __encoding.encode_one(
						::std::forward<_Input>(__input), __output_range, __error_handler, __state);
				}
				else if constexpr (
					__is_iterator_concept_or_better_v<contiguous_iterator_tag,
					     _OutputIterator> && __is_encode_error_handler_callable_v<_UEncoding, _Input, _Span, _ErrorHandler, _State> && __is_encode_one_callable_v<_UEncoding, _Input, _Span, _ErrorHandler, _State>) {
					auto __first = __adl::__adl_begin(::std::forward<_Output>(__output));
					auto __last  = __adl::__adl_end(::std::forward<_Output>(__output));
					_Span __output_range(::std::move(__first), ::std::move(__last));
					return __encoding.encode_one(
						::std::forward<_Input>(__input), __output_range, __error_handler, __state);
				}
				else {
					auto __first = __adl::__adl_begin(::std::forward<_Output>(__output));
					auto __last  = __adl::__adl_end(::std::forward<_Output>(__output));
					_OutRange __output_range(::std::move(__first), ::std::move(__last));
					return __encoding.encode_one(
						::std::forward<_Input>(__input), __output_range, __error_handler, __state);
				}
			}
		}

		template <__consume _ConsumeIntoTheNothingness, __transaction __encode_or_decode, typename _Encoding,
			typename _Input, typename _OutputContainer, typename _ErrorHandler, typename _State>
		constexpr auto __basic_encode_or_decode_one(_Input&& __input, _Encoding&& __encoding,
			_OutputContainer& __output, _ErrorHandler& __error_handler, _State& __state) {
			if constexpr (__encode_or_decode == __transaction::__decode) {
				return __basic_decode_one<_ConsumeIntoTheNothingness>(::std::forward<_Input>(__input),
					::std::forward<_Encoding>(__encoding), __output, __error_handler, __state);
			}
			else {
				return __basic_encode_one<_ConsumeIntoTheNothingness>(::std::forward<_Input>(__input),
					::std::forward<_Encoding>(__encoding), __output, __error_handler, __state);
			}
		}

		template <__consume _ConsumeIntoTheNothingness, typename _Input, typename _FromEncoding, typename _Output,
			typename _ToEncoding, typename _FromErrorHandler, typename _ToErrorHandler, typename _FromState,
			typename _ToState, typename _Intermediate>
		constexpr auto __super_basic_transcode_one(_Input&& __input, _FromEncoding& __from_encoding,
			_Output&& __output, _ToEncoding& __to_encoding, _FromErrorHandler& __from_error_handler,
			_ToErrorHandler& __to_error_handler, _FromState& __from_state, _ToState& __to_state,
			_Intermediate& __intermediate) {
			using _InitialIntermediate = __span_or_reconstruct_t<_Intermediate>;
			using _DecodeResult
				= decltype(__basic_decode_one<_ConsumeIntoTheNothingness>(::std::forward<_Input>(__input),
				     __from_encoding, ::std::declval<_InitialIntermediate>(), __from_error_handler, __from_state));
			using _InputView           = decltype(::std::declval<_DecodeResult>().input);
			using _WorkingIntermediate = decltype(::std::declval<_DecodeResult>().output);
			using _OutputView
				= decltype(__basic_encode_one<_ConsumeIntoTheNothingness>(::std::declval<_WorkingIntermediate>(),
				     __to_encoding, ::std::forward<_Output>(__output), __to_error_handler, __to_state)
				                .output);
			using _Result = transcode_result<_InputView, _OutputView, _FromState, _ToState>;

			static_assert(__txt_detail::__is_decode_lossless_or_deliberate_v<__remove_cvref_t<_FromEncoding>,
				              __remove_cvref_t<_FromErrorHandler>>,
				"The decode (input) portion of this transcode is a lossy, non-injective operation. This means "
				"you "
				"may lose data that you did not intend to lose; specify an 'in_handler' error handler "
				"parameter to "
				"transcode[_to](in, in_encoding, out_encoding, in_handler, ...) or transcode_into(in, "
				"in_encoding, "
				"out, "
				"out_encoding, in_handler, ...) explicitly in order to bypass this.");
			static_assert(__txt_detail::__is_encode_lossless_or_deliberate_v<__remove_cvref_t<_ToEncoding>,
				              __remove_cvref_t<_ToErrorHandler>>,
				"The encode (output) portion of this transcode is a lossy, non-injective operation. This "
				"means you "
				"may lose data that you did not intend to lose; specify an 'out_handler' error handler "
				"parameter "
				"to transcode[_to](in, in_encoding, out_encoding, in_handler, out_handler, ...) or "
				"transcode_into(in, "
				"in_encoding, out, out_encoding, in_handler, out_handler, ...) explicitly in order to bypass "
				"this.");
			// TODO: is_range_convertible_to_v<DecodeCodePoint, EncodeCodePoint> ?
			// static_assert(::std::is_convertible_v<>, "The intermediary codepoint types are not trivially
			// convertible to one another");


			_WorkingIntermediate __working_output = __span_or_reconstruct(__intermediate);
			auto __intermediate_result = __basic_decode_one<__consume::__no>(::std::forward<_Input>(__input),
				__from_encoding, __working_output, __from_error_handler, __from_state);
			if (__intermediate_result.error_code != encoding_error::ok) {
#if 0
				if constexpr (::std::is_same_v<__range_iterator_t<_OutputView>, __blackhole_iterator>) {
					return _Result(::std::move(__intermediate_result.input), _OutputView {},
						__intermediate_result.state, __to_state, __intermediate_result.error_code,
						__intermediate_result.handled_errors);
				}
				else if constexpr (__is_specialization_of_v<_OutputView, unbounded_view>) {
					return _Result(::std::move(__intermediate_result.input),
						__reconstruct(::std::in_place_type<_OutputView>, __adl::__adl_begin(__output),
						     __range_sentinel_t<_OutputView> {}),
						__intermediate_result.state, __to_state, __intermediate_result.error_code,
						__intermediate_result.handled_errors);
				}
				else {
#endif
				return _Result(::std::move(__intermediate_result.input),
					__reconstruct(::std::in_place_type<_OutputView>, ::std::forward<_Output>(__output)),
					__intermediate_result.state, __to_state, __intermediate_result.error_code,
					__intermediate_result.handled_errors);
#if 0
				}
#endif
			}
			_WorkingIntermediate __working_input = __reconstruct(::std::in_place_type<_WorkingIntermediate>,
				__adl::__adl_begin(__working_output), __adl::__adl_begin(__intermediate_result.output));
			auto __end_result = __basic_encode_one<_ConsumeIntoTheNothingness>(::std::move(__working_input),
				__to_encoding, ::std::forward<_Output>(__output), __to_error_handler, __to_state);

			return _Result(::std::move(__intermediate_result.input), ::std::move(__end_result.output),
				__intermediate_result.state, __end_result.state, __end_result.error_code,
				__intermediate_result.handled_errors + __end_result.handled_errors);
		}

		template <__consume _ConsumeIntoTheNothingness, typename _Input, typename _FromEncoding, typename _Output,
			typename _ToEncoding, typename _FromErrorHandler, typename _ToErrorHandler, typename _FromState,
			typename _ToState>
		constexpr auto __super_basic_transcode_one(_Input&& __input, _FromEncoding& __from_encoding,
			_Output&& __output, _ToEncoding& __to_encoding, _FromErrorHandler& __from_error_handler,
			_ToErrorHandler& __to_error_handler, _FromState& __from_state, _ToState& __to_state) {
			using _IntermediateCodePoint                       = code_point_t<_FromEncoding>;
			constexpr ::std::size_t _IntermediateMaxCodePoints = max_code_points_v<_FromEncoding>;

			_IntermediateCodePoint __intermediate_storage[_IntermediateMaxCodePoints] {};
			::ztd::text::span<_IntermediateCodePoint, _IntermediateMaxCodePoints> __intermediate(
				__intermediate_storage);
			return __super_basic_transcode_one<_ConsumeIntoTheNothingness>(::std::forward<_Input>(__input),
				__from_encoding, ::std::forward<_Output>(__output), __to_encoding, __from_error_handler,
				__to_error_handler, __from_state, __to_state, __intermediate);
		}

		template <__consume _ConsumeIntoTheNothingness, typename _Input, typename _FromEncoding, typename _Output,
			typename _ToEncoding, typename _FromErrorHandler, typename _ToErrorHandler, typename _FromState,
			typename _ToState, typename _Intermediate>
		constexpr auto __basic_transcode_one(_Input&& __input, _FromEncoding& __from_encoding, _Output&& __output,
			_ToEncoding& __to_encoding, _FromErrorHandler& __from_error_handler, _ToErrorHandler& __to_error_handler,
			_FromState& __from_state, _ToState& __to_state, _Intermediate& __intermediate) {
			using _UOutput        = __remove_cvref_t<_Output>;
			using _OutputIterator = __range_iterator_t<_UOutput>;
			using _Blackhole      = unbounded_view<__blackhole_iterator>;
			if constexpr (__is_iterator_output_iterator_v<_OutputIterator>) {
				if constexpr (__is_detected_v<__detect_adl_text_transcode_one, _Input, _FromEncoding, _Output,
					              _ToEncoding, _FromErrorHandler, _ToErrorHandler, _FromState, _ToState>) {
					(void)__intermediate;
					return text_transcode_one(
						tag<__remove_cvref_t<_FromEncoding>, __remove_cvref_t<_ToEncoding>> {},
						::std::forward<_Input>(__input), __from_encoding, ::std::forward<_Output>(__output),
						__to_encoding, __from_error_handler, __to_error_handler, __from_state, __to_state);
				}
				else if constexpr (__is_detected_v<__detect_adl_internal_text_transcode_one, _Input, _FromEncoding,
					                   _Output, _ToEncoding, _FromErrorHandler, _ToErrorHandler, _FromState,
					                   _ToState>) {
					(void)__intermediate;
					return __text_transcode_one(
						tag<__remove_cvref_t<_FromEncoding>, __remove_cvref_t<_ToEncoding>> {},
						::std::forward<_Input>(__input), __from_encoding, ::std::forward<_Output>(__output),
						__to_encoding, __from_error_handler, __to_error_handler, __from_state, __to_state);
				}
				else {
					return __super_basic_transcode_one<_ConsumeIntoTheNothingness>(::std::forward<_Input>(__input),
						__from_encoding, ::std::forward<_Output>(__output), __to_encoding, __from_error_handler,
						__to_error_handler, __from_state, __to_state, __intermediate);
				}
			}
			else if constexpr (_ConsumeIntoTheNothingness == __consume::__embrace_the_void) {
				if constexpr (__is_detected_v<__detect_adl_text_transcode_one, _Input, _FromEncoding, _Blackhole,
					              _ToEncoding, _FromErrorHandler, _ToErrorHandler, _FromState, _ToState>) {
					(void)__intermediate;
					_Blackhole __output_range(__blackhole_iterator {});
					return text_transcode_one(
						tag<__remove_cvref_t<_FromEncoding>, __remove_cvref_t<_ToEncoding>> {},
						::std::forward<_Input>(__input), __from_encoding, ::std::move(__output_range),
						__to_encoding, __from_error_handler, __to_error_handler, __from_state, __to_state);
				}
				else if constexpr (__is_detected_v<__detect_adl_internal_text_transcode_one, _Input, _FromEncoding,
					                   _Blackhole, _ToEncoding, _FromErrorHandler, _ToErrorHandler, _FromState,
					                   _ToState>) {
					(void)__intermediate;
					_Blackhole __output_range(__blackhole_iterator {});
					return __text_transcode_one(
						tag<__remove_cvref_t<_FromEncoding>, __remove_cvref_t<_ToEncoding>> {},
						::std::forward<_Input>(__input), __from_encoding, ::std::move(__output_range),
						__to_encoding, __from_error_handler, __to_error_handler, __from_state, __to_state);
				}
				else {
					return __super_basic_transcode_one<_ConsumeIntoTheNothingness>(::std::forward<_Input>(__input),
						__from_encoding, __intermediate, ::std::forward<_Output>(__output), __to_encoding,
						__from_error_handler, __to_error_handler, __from_state, __to_state);
				}
			}
			else {
				using _ValueType   = __range_value_type_t<_UOutput>;
				using _ToUnbounded = unbounded_view<_OutputIterator>;
				using _ToSpan      = ::ztd::text::span<_ValueType>;
				if constexpr (__is_detected_v<__detect_adl_text_transcode_one, _Input, _FromEncoding, _ToUnbounded,
					              _ToEncoding, _FromErrorHandler, _ToErrorHandler, _FromState, _ToState>) {
					(void)__intermediate;
					_OutputIterator __first = __adl::__adl_begin(::std::forward<_Output>(__output));
					_ToUnbounded __output_range(::std::move(__first));
					return text_transcode_one(
						tag<__remove_cvref_t<_FromEncoding>, __remove_cvref_t<_ToEncoding>> {},
						::std::forward<_Input>(__input), __from_encoding, ::std::move(__output_range),
						__to_encoding, __from_error_handler, __to_error_handler, __from_state, __to_state);
				}
				else if constexpr (
					__is_iterator_concept_or_better_v<contiguous_iterator_tag,
					     _OutputIterator> && __is_detected_v<__detect_adl_text_transcode_one, _Input, _FromEncoding, _ToSpan, _ToEncoding, _FromErrorHandler, _ToErrorHandler, _FromState, _ToState>) {
					(void)__intermediate;
					_ToSpan __output_range(::std::forward<_Output>(__output));
					return text_transcode_one(
						tag<__remove_cvref_t<_FromEncoding>, __remove_cvref_t<_ToEncoding>> {},
						::std::forward<_Input>(__input), __from_encoding, ::std::move(__output_range),
						__to_encoding, __from_error_handler, __to_error_handler, __from_state, __to_state);
				}
				else if constexpr (__is_detected_v<__detect_adl_internal_text_transcode_one, _Input, _FromEncoding,
					                   _ToUnbounded, _ToEncoding, _FromErrorHandler, _ToErrorHandler, _FromState,
					                   _ToState>) {
					(void)__intermediate;
					_OutputIterator __first = __adl::__adl_begin(::std::forward<_Output>(__output));
					_ToUnbounded __output_range(::std::move(__first));
					return __text_transcode_one(
						tag<__remove_cvref_t<_FromEncoding>, __remove_cvref_t<_ToEncoding>> {},
						::std::forward<_Input>(__input), __from_encoding, ::std::move(__output_range),
						__to_encoding, __from_error_handler, __to_error_handler, __from_state, __to_state);
				}
				else if constexpr (
					__is_iterator_concept_or_better_v<contiguous_iterator_tag,
					     _OutputIterator> && __is_detected_v<__detect_adl_internal_text_transcode_one, _Input, _FromEncoding, _ToSpan, _ToEncoding, _FromErrorHandler, _ToErrorHandler, _FromState, _ToState>) {
					(void)__intermediate;
					_ToSpan __output_range(::std::forward<_Output>(__output));
					return __text_transcode_one(
						tag<__remove_cvref_t<_FromEncoding>, __remove_cvref_t<_ToEncoding>> {},
						::std::forward<_Input>(__input), __from_encoding, ::std::move(__output_range),
						__to_encoding, __from_error_handler, __to_error_handler, __from_state, __to_state);
				}
				else {
					return __super_basic_transcode_one<_ConsumeIntoTheNothingness>(::std::forward<_Input>(__input),
						__from_encoding, ::std::forward<_Output>(__output), __to_encoding, __from_error_handler,
						__to_error_handler, __from_state, __to_state, __intermediate);
				}
			}
		}

		template <__consume _ConsumeIntoTheNothingness, typename _Input, typename _FromEncoding, typename _Output,
			typename _ToEncoding, typename _FromErrorHandler, typename _ToErrorHandler, typename _FromState,
			typename _ToState>
		constexpr auto __basic_transcode_one(_Input&& __input, _FromEncoding& __from_encoding, _Output&& __output,
			_ToEncoding& __to_encoding, _FromErrorHandler& __from_error_handler, _ToErrorHandler& __to_error_handler,
			_FromState& __from_state, _ToState& __to_state) {
			using _IntermediateCodePoint                       = code_point_t<_FromEncoding>;
			constexpr ::std::size_t _IntermediateMaxCodePoints = max_code_points_v<_FromEncoding>;

			_IntermediateCodePoint __intermediate_storage[_IntermediateMaxCodePoints] {};
			::ztd::text::span<_IntermediateCodePoint, _IntermediateMaxCodePoints> __intermediate(
				__intermediate_storage);
			return __basic_transcode_one<_ConsumeIntoTheNothingness>(::std::forward<_Input>(__input),
				__from_encoding, ::std::forward<_Output>(__output), __to_encoding, __from_error_handler,
				__to_error_handler, __from_state, __to_state, __intermediate);
		}

		template <typename _Input, typename _Encoding, typename _Output, typename _DecodeState, typename _EncodeState,
			typename _Intermediate>
		constexpr auto __basic_validate_decodable_as_one(_Input&& __input, _Encoding&& __encoding, _Output& __output,
			_DecodeState& __decode_state, _EncodeState& __encode_state, _Intermediate& __intermediate) {

			using _UInput      = __remove_cvref_t<_Input>;
			using _UOutput     = __remove_cvref_t<_Output>;
			using _InSubRange  = subrange<__range_iterator_t<_UInput>, __range_sentinel_t<_UInput>>;
			using _OutSubRange = subrange<__range_iterator_t<_UOutput>, __range_sentinel_t<_UOutput>>;

			_InSubRange __working_input(::std::forward<_Input>(__input));
			_OutSubRange __working_output(__output);
			__pass_through_handler __error_handler {};

			using _Result = validate_result<__range_reconstruct_t<_InSubRange, _UInput>, _DecodeState>;

			auto __transcode_result
				= __basic_transcode_one<__consume::__no>(__working_input, __encoding, __working_output, __encoding,
				     __error_handler, __error_handler, __decode_state, __encode_state, __intermediate);
			if (__transcode_result.error_code != encoding_error::ok) {
				return _Result(__reconstruct(::std::in_place_type<_UInput>, ::std::move(__transcode_result.input)),
					false, __decode_state);
			}

			const bool __is_equal_transcode
				= __equal(__adl::__adl_begin(__working_input), __adl::__adl_begin(__transcode_result.input),
				     __adl::__adl_begin(__working_output), __adl::__adl_begin(__transcode_result.output));
			if (!__is_equal_transcode) {
				return _Result(__reconstruct(::std::in_place_type<_UInput>, ::std::move(__transcode_result.input)),
					false, __decode_state);
			}
			return _Result(__reconstruct(::std::in_place_type<_UInput>, ::std::move(__transcode_result.input)), true,
				__decode_state);
		}

		template <typename _Input, typename _Encoding, typename _DecodeState, typename _EncodeState>
		constexpr auto __basic_validate_decodable_as_one(
			_Input&& __input, _Encoding&& __encoding, _DecodeState& __decode_state, _EncodeState& __encode_state) {
			using _UEncoding = __remove_cvref_t<_Encoding>;
			using _CodeUnit  = code_unit_t<_UEncoding>;
			using _CodePoint = code_point_t<_UEncoding>;

			_CodePoint __intermediate_storage[max_code_points_v<_UEncoding>] {};
			::ztd::text::span<_CodePoint, max_code_points_v<_UEncoding>> __intermediate(__intermediate_storage);
			_CodeUnit __output_storage[max_code_units_v<_UEncoding>] {};
			::ztd::text::span<_CodeUnit, max_code_units_v<_UEncoding>> __output(__output_storage);
			return __basic_validate_decodable_as_one(::std::forward<_Input>(__input),
				::std::forward<_Encoding>(__encoding), __output, __decode_state, __encode_state, __intermediate);
		}

		template <typename _Input, typename _Encoding, typename _CodePointContainer, typename _CodeUnitContainer,
			typename _EncodeState, typename _DecodeState>
		constexpr auto __basic_validate_encodable_as_one(_Input&& __input, _Encoding&& __encoding,
			_CodePointContainer& __output, _CodeUnitContainer& __intermediate, _EncodeState& __encode_state,
			_DecodeState& __decode_state) {

			using _UInput      = __remove_cvref_t<_Input>;
			using _UOutput     = __remove_cvref_t<_CodePointContainer>;
			using _InSubRange  = subrange<__range_iterator_t<_UInput>, __range_sentinel_t<_UInput>>;
			using _OutSubRange = subrange<__range_iterator_t<_UOutput>, __range_sentinel_t<_UOutput>>;
			using _Result      = validate_result<__range_reconstruct_t<_InSubRange, _UInput>, _EncodeState>;

			__pass_through_handler __error_handler {};
			_InSubRange __working_input(::std::forward<_Input>(__input));

			auto __encode_result = __basic_encode_one<__consume::__no>(
				__working_input, __encoding, __intermediate, __error_handler, __encode_state);
			if (__encode_result.error_code != encoding_error::ok) {
				return _Result(__reconstruct(::std::in_place_type<_UInput>, ::std::move(__encode_result.input)),
					false, __encode_state);
			}

			_OutSubRange __working_output(__output);

			auto __decode_result = __basic_decode_one<__consume::__no>(
				__intermediate, __encoding, __working_output, __error_handler, __decode_state);
			if (__decode_result.error_code != encoding_error::ok) {
				return _Result(__reconstruct(::std::in_place_type<_UInput>, ::std::move(__encode_result.input)),
					false, __encode_state);
			}

			const bool __is_equal_transcode
				= __equal(__adl::__adl_begin(__working_input), __adl::__adl_begin(__encode_result.input),
				     __adl::__adl_begin(__working_output), __adl::__adl_begin(__decode_result.output));
			if (!__is_equal_transcode) {
				return _Result(__reconstruct(::std::in_place_type<_UInput>, ::std::move(__encode_result.input)),
					false, __encode_state);
			}
			return _Result(__reconstruct(::std::in_place_type<_UInput>, ::std::move(__encode_result.input)), true,
				__encode_state);
		}

		template <typename _Input, typename _Encoding, typename _EncodeState, typename _DecodeState>
		constexpr auto __basic_validate_encodable_as_one(
			_Input&& __input, _Encoding&& __encoding, _EncodeState& __encode_state, _DecodeState& __decode_state) {
			using _UEncoding = __remove_cvref_t<_Encoding>;
			using _CodePoint = code_point_t<_UEncoding>;
			using _CodeUnit  = code_unit_t<_UEncoding>;

			_CodeUnit __intermediate_storage[max_code_units_v<_UEncoding>] {};
			::ztd::text::span<_CodeUnit, max_code_units_v<_UEncoding>> __intermediate(__intermediate_storage);
			_CodePoint __output_storage[max_code_points_v<_UEncoding>] {};
			::ztd::text::span<_CodePoint, max_code_points_v<_UEncoding>> __output(__output_storage);
			return __basic_validate_encodable_as_one(::std::forward<_Input>(__input),
				::std::forward<_Encoding>(__encoding), __output, __intermediate, __encode_state, __decode_state);
		}

		template <typename _Input, typename _Encoding, typename _OutputContainer, typename _ErrorHandler,
			typename _State>
		constexpr auto __basic_count_encodable_one(_Input&& __input, _Encoding&& __encoding,
			_OutputContainer& __output, _ErrorHandler&& __error_handler, _State& __state) {
			using _Result = count_result<__reconstruct_t<_Input>, _State>;

			auto __intermediate_result = __basic_encode_one<__consume::__no>(::std::forward<_Input>(__input),
				::std::forward<_Encoding>(__encoding), __output, ::std::forward<_ErrorHandler>(__error_handler),
				__state);
			::std::size_t __written    = static_cast<::std::size_t>(
                    __adl::__adl_data(__intermediate_result.output) - __adl::__adl_data(__output));

			return _Result(::std::move(__intermediate_result.input), __written, __intermediate_result.state,
				__intermediate_result.error_code, __intermediate_result.handled_errors);
		}

		template <typename _Input, typename _Encoding, typename _ErrorHandler, typename _State>
		constexpr auto __basic_count_encodable_one(
			_Input&& __input, _Encoding&& __encoding, _ErrorHandler&& __error_handler, _State& __state) {
			using _UEncoding                                  = __remove_cvref_t<_Encoding>;
			using _IntermediateCodeUnit                       = code_unit_t<_UEncoding>;
			constexpr ::std::size_t _IntermediateMaxCodeUnits = max_code_units_v<_UEncoding>;

			_IntermediateCodeUnit __intermediate_storage[_IntermediateMaxCodeUnits] {};
			::ztd::text::span<_IntermediateCodeUnit, _IntermediateMaxCodeUnits> __intermediate(
				__intermediate_storage);
			return __basic_count_encodable_one(::std::forward<_Input>(__input),
				::std::forward<_Encoding>(__encoding), __intermediate,
				::std::forward<_ErrorHandler>(__error_handler), __state);
		}

		template <typename _Input, typename _Encoding, typename _OutputCodePointContainer, typename _ErrorHandler,
			typename _State>
		constexpr auto __basic_count_decodable_one(_Input&& __input, _Encoding&& __encoding,
			_OutputCodePointContainer& __output, _ErrorHandler&& __error_handler, _State& __state) {
			using _Result = count_result<__reconstruct_t<_Input>, _State>;

			auto __intermediate_result = __basic_decode_one<__consume::__no>(::std::forward<_Input>(__input),
				::std::forward<_Encoding>(__encoding), __output, ::std::forward<_ErrorHandler>(__error_handler),
				__state);
			::std::size_t __written    = static_cast<::std::size_t>(
                    __adl::__adl_data(__intermediate_result.output) - __adl::__adl_data(__output));

			return _Result(::std::move(__intermediate_result.input), __written, __intermediate_result.state,
				__intermediate_result.error_code, __intermediate_result.handled_errors);
		}

		template <typename _Input, typename _Encoding, typename _ErrorHandler, typename _State>
		constexpr auto __basic_count_decodable_one(
			_Input&& __input, _Encoding&& __encoding, _ErrorHandler&& __error_handler, _State& __state) {
			using _UEncoding                                   = __remove_cvref_t<_Encoding>;
			using _IntermediateCodePoint                       = code_point_t<_UEncoding>;
			constexpr ::std::size_t _IntermediateMaxCodePoints = max_code_points_v<_UEncoding>;


			_IntermediateCodePoint __intermediate_storage[_IntermediateMaxCodePoints] {};
			::ztd::text::span<_IntermediateCodePoint, _IntermediateMaxCodePoints> __intermediate(
				__intermediate_storage);
			return __basic_count_decodable_one(::std::forward<_Input>(__input),
				::std::forward<_Encoding>(__encoding), __intermediate,
				::std::forward<_ErrorHandler>(__error_handler), __state);
		}
	} // namespace __txt_detail

	ZTD_TEXT_INLINE_ABI_NAMESPACE_CLOSE_I_
}} // namespace ztd::text

#include <ztd/text/detail/epilogue.hpp>

#endif // ZTD_TEXT_DETAIL_TRANSCODE_ONE_HPP
