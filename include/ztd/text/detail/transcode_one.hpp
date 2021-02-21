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
#include <ztd/text/detail/is_lossless.hpp>

#include <ztd/text/detail/blackhole_iterator.hpp>
#include <ztd/text/detail/encoding_iterator_storage.hpp>
#include <ztd/text/detail/adl.hpp>
#include <ztd/text/detail/encoding_range.hpp>
#include <ztd/text/detail/span.hpp>

#include <array>
#include <algorithm>

namespace ztd { namespace text {
	ZTD_TEXT_INLINE_ABI_NAMESPACE_OPEN_I_
	namespace __detail {

		enum class __consume : unsigned char { __no = 0, __embrace_the_void = 1 };
		enum class __transaction : unsigned char { __encode = 0, __decode = 1 };

		template <typename _Iterator0, typename _Sentinel0, typename _Iterator1, typename _Sentinel1>
		constexpr bool __ce_equal(_Iterator0 __first0, _Sentinel0 __last0, _Iterator1 __first1, _Sentinel1 __last1) {
#if ZTD_TEXT_IS_ON(ZTD_TEXT_STD_LIBRARY_CONSTEXPR_ALGORITHMS_I_)
			return ::std::equal(
				::std::move(__first0), ::std::move(__last0), ::std::move(__first1), ::std::move(__last1));
#else
			if (__first0 == __last0) {
				if (__first1 == __last1) {
					return true;
				}
				return false;
			}
			for (; __first0 != __last0; (void)++__first0, ++__first1) {
				if (__first1 == __last1) {
					return false;
				}
				if (*__first0 != *__first1) {
					return false;
				}
			}

			return __first1 == __last1;
#endif
		}

		template <__consume _ConsumeIntoTheNothingness, typename _Encoding, typename _Input, typename _Output,
			typename _ErrorHandler, typename _State>
		constexpr auto __basic_decode_one(_Input&& __input, _Encoding&& __encoding, _Output&& __output,
			_ErrorHandler& __error_handler, _State& __state) {
			using _UInput    = __remove_cvref_t<_Input>;
			using _UOutput   = __remove_cvref_t<_Output>;
			using _UEncoding = __remove_cvref_t<_Encoding>;
			using _ValueType = __range_value_type_t<_UOutput>;
			using _Iterator  = __range_iterator_t<_UOutput>;
			using _Blackhole = unbounded_view<__blackhole_iterator>;

			if constexpr (_ConsumeIntoTheNothingness == __consume::__embrace_the_void
				&& __is_decode_one_callable_v<_UEncoding, _UInput, _Blackhole, _ErrorHandler, _State>) {
				(void)__output;
				_Blackhole __output_range(__blackhole_iterator {});
				return __encoding.decode_one(
					::std::forward<_Input>(__input), __output_range, __error_handler, __state);
			}
			else if constexpr (::std::is_void_v<_ValueType>) {
				// TODO: fix check to be "is output iterator"
				return __encoding.decode_one(
					::std::forward<_Input>(__input), ::std::forward<_Output>(__output), __error_handler, __state);
			}
			else {
				using _Sentinel                    = __range_sentinel_t<_UOutput>;
				using _Unbounded                   = unbounded_view<_Iterator>;
				using _UnboundedSpan               = unbounded_view<_ValueType*>;
				using _Span                        = ::ztd::text::span<_ValueType>;
				using _OutRange                    = subrange<_Iterator, _Sentinel>;
				constexpr ::std::size_t _MaxValues = max_code_units_v<_UEncoding>;
				if constexpr (_ConsumeIntoTheNothingness == __consume::__embrace_the_void
					&& __is_decode_one_callable_v<_UEncoding, _UInput, _UnboundedSpan, _ErrorHandler, _State>) {
					_ValueType __fake[_MaxValues] {};
					_ValueType* __first = __fake;
					_UnboundedSpan __output_range(__first);
					return __encoding.decode_one(
						::std::forward<_Input>(__input), __output_range, __error_handler, __state);
				}
				else if constexpr (__is_decode_one_callable_v<_UEncoding, _UInput, _Unbounded, _ErrorHandler,
					                   _State>) {
					auto __first = __adl::__adl_begin(::std::forward<_Output>(__output));
					_Unbounded __output_range(::std::move(__first));
					return __encoding.decode_one(
						::std::forward<_Input>(__input), __output_range, __error_handler, __state);
				}
				else if constexpr (_ConsumeIntoTheNothingness == __consume::__embrace_the_void
					&& __is_decode_one_callable_v<_UEncoding, _UInput, _Span, _ErrorHandler, _State>) {
					_ValueType __fake[_MaxValues] {};
					_ValueType* __first = __fake;
					_ValueType* __last  = __first + _MaxValues;
					_Span __output_range(__first, __last);
					return __encoding.decode_one(
						::std::forward<_Input>(__input), __output_range, __error_handler, __state);
				}
				else if constexpr (
					__is_iterator_concept_or_better_v<contiguous_iterator_tag,
					     _Iterator> && __is_decode_one_callable_v<_UEncoding, _UInput, _Span, _ErrorHandler, _State>) {
					auto __first = __adl::__adl_begin(::std::forward<_Output>(__output));
					auto __last  = __adl::__adl_end(::std::forward<_Output>(__output));
					_Span __output_range(::std::move(__first), ::std::move(__last));
					return __encoding.decode_one(
						::std::forward<_Input>(__input), __output_range, __error_handler, __state);
				}
				else if constexpr (__is_decode_one_callable_v<_UEncoding, _UInput, _Span, _ErrorHandler, _State>) {
					return __encoding.decode_one(::std::forward<_Input>(__input),
						::std::forward<_Output>(__output), __error_handler, __state);
				}
				else {
					auto __first = __adl::__adl_begin(::std::forward<_Output>(__output));
					auto __last  = __adl::__adl_end(::std::forward<_Output>(__output));
					_OutRange __output_range(::std::move(__first), ::std::move(__last));
					return __encoding.decode_one(
						::std::forward<_Input>(__input), __output_range, __error_handler, __state);
				}
			}
		}

		template <__consume _ConsumeIntoTheNothingness, typename _Encoding, typename _Input, typename _Output,
			typename _ErrorHandler, typename _State>
		constexpr auto __basic_encode_one(_Input&& __input, _Encoding&& __encoding, _Output&& __output,
			_ErrorHandler& __error_handler, _State& __state) {
			using _UInput    = __remove_cvref_t<_Input>;
			using _UOutput   = __remove_cvref_t<_Output>;
			using _UEncoding = __remove_cvref_t<_Encoding>;
			using _ValueType = __range_value_type_t<_UOutput>;
			using _Iterator  = __range_iterator_t<_UOutput>;
			using _Blackhole = unbounded_view<__blackhole_iterator>;

			if constexpr (_ConsumeIntoTheNothingness == __consume::__embrace_the_void
				&& __is_encode_one_callable_v<_UEncoding, _UInput, _Blackhole, _ErrorHandler, _State>) {
				(void)__output;
				_Blackhole __output_range(__blackhole_iterator {});
				return __encoding.encode_one(
					::std::forward<_Input>(__input), __output_range, __error_handler, __state);
			}
			else if constexpr (::std::is_void_v<_ValueType>) {
				// TODO: fix check to be "is output iterator"
				return __encoding.encode_one(
					::std::forward<_Input>(__input), ::std::forward<_Output>(__output), __error_handler, __state);
			}
			else {
				using _Sentinel                    = __range_sentinel_t<_UOutput>;
				using _Unbounded                   = unbounded_view<_Iterator>;
				using _UnboundedSpan               = unbounded_view<_ValueType*>;
				using _Span                        = ::ztd::text::span<_ValueType>;
				using _OutRange                    = subrange<_Iterator, _Sentinel>;
				constexpr ::std::size_t _MaxValues = max_code_units_v<_UEncoding>;
				if constexpr (_ConsumeIntoTheNothingness == __consume::__embrace_the_void
					&& __is_encode_one_callable_v<_UEncoding, _UInput, _UnboundedSpan, _ErrorHandler, _State>) {
					_ValueType __fake[_MaxValues] {};
					_ValueType* __first = __fake;
					_UnboundedSpan __output_range(__first);
					return __encoding.encode_one(
						::std::forward<_Input>(__input), __output_range, __error_handler, __state);
				}
				else if constexpr (__is_encode_one_callable_v<_UEncoding, _UInput, _Unbounded, _ErrorHandler,
					                   _State>) {
					auto __first = __adl::__adl_begin(::std::forward<_Output>(__output));
					_Unbounded __output_range(::std::move(__first));
					return __encoding.encode_one(
						::std::forward<_Input>(__input), __output_range, __error_handler, __state);
				}
				else if constexpr (_ConsumeIntoTheNothingness == __consume::__embrace_the_void
					&& __is_encode_one_callable_v<_UEncoding, _UInput, _Span, _ErrorHandler, _State>) {
					_ValueType __fake[_MaxValues] {};
					_ValueType* __first = __fake;
					_ValueType* __last  = __first + _MaxValues;
					_Span __output_range(__first, __last);
					return __encoding.encode_one(
						::std::forward<_Input>(__input), __output_range, __error_handler, __state);
				}
				else if constexpr (
					__is_iterator_concept_or_better_v<contiguous_iterator_tag,
					     _Iterator> && __is_encode_one_callable_v<_UEncoding, _UInput, _Span, _ErrorHandler, _State>) {
					auto __first = __adl::__adl_begin(::std::forward<_Output>(__output));
					auto __last  = __adl::__adl_end(::std::forward<_Output>(__output));
					_Span __output_range(::std::move(__first), ::std::move(__last));
					return __encoding.encode_one(
						::std::forward<_Input>(__input), __output_range, __error_handler, __state);
				}
				else if constexpr (__is_encode_one_callable_v<_UEncoding, _UInput, _UOutput, _ErrorHandler,
					                   _State>) {
					return __encoding.encode_one(::std::forward<_Input>(__input),
						::std::forward<_Output>(__output), __error_handler, __state);
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

		template <__consume _ConsumeIntoTheNothingness, typename _Input, typename _FromEncoding,
			typename _IntermediateContainer, typename _Output, typename _ToEncoding, typename _FromErrorHandler,
			typename _ToErrorHandler, typename _FromState, typename _ToState>
		constexpr auto __super_basic_transcode_one(_Input&& __input, _FromEncoding& __from_encoding,
			_IntermediateContainer& __intermediate, _Output&& __output, _ToEncoding& __to_encoding,
			_FromErrorHandler& __from_error_handler, _ToErrorHandler& __to_error_handler, _FromState& __from_state,
			_ToState& __to_state) {
			using _UInput = __remove_cvref_t<_Input>;
			using _WorkingIntermediate
				= ::std::conditional_t<__is_iterator_concept_or_better_v<contiguous_iterator_tag,
				                            __range_iterator_t<_IntermediateContainer>>,
				     ::ztd::text::span<__range_value_type_t<_IntermediateContainer>>,
				     subrange<__range_iterator_t<_IntermediateContainer>>>;
			using _OutputView
				= decltype(__basic_encode_one<_ConsumeIntoTheNothingness>(::std::declval<_WorkingIntermediate>(),
				     __to_encoding, ::std::forward<_Output>(__output), __to_error_handler, __to_state)
				                .output);
			using _Result = transcode_result<__reconstruct_t<_UInput>, _OutputView, _FromState, _ToState>;

			static_assert(__detail::__is_decode_lossless_or_deliberate_v<__remove_cvref_t<_FromEncoding>,
				              __remove_cvref_t<_FromErrorHandler>>,
				"The decode (input) portion of this transcode is a lossy, non-injective operation. This means "
				"you "
				"may lose data that you did not intend to lose; specify an 'in_handler' error handler "
				"parameter to "
				"transcode[_to](in, in_encoding, out_encoding, in_handler, ...) or transcode_into(in, in_encoding, "
				"out, "
				"out_encoding, in_handler, ...) explicitly in order to bypass this.");
			static_assert(__detail::__is_encode_lossless_or_deliberate_v<__remove_cvref_t<_ToEncoding>,
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

			auto __intermediate_result = __basic_decode_one<__consume::__no>(::std::forward<_Input>(__input),
				__from_encoding, __intermediate, __from_error_handler, __from_state);
			if (__intermediate_result.error_code != encoding_error::ok) {
				if constexpr (::std::is_same_v<__range_iterator_t<_OutputView>, __blackhole_iterator>) {
					return _Result(::std::move(__intermediate_result.input), _OutputView {},
						__intermediate_result.state, __to_state, __intermediate_result.error_code,
						__intermediate_result.handled_error);
				}
				else if constexpr (__is_specialization_of_v<_OutputView, unbounded_view>) {
					return _Result(::std::move(__intermediate_result.input),
						__reconstruct(::std::in_place_type<_OutputView>, __adl::__adl_begin(__output),
						     __range_sentinel_t<_OutputView> {}),
						__intermediate_result.state, __to_state, __intermediate_result.error_code,
						__intermediate_result.handled_error);
				}
				else {
					return _Result(::std::move(__intermediate_result.input),
						__reconstruct(::std::in_place_type<_OutputView>, ::std::forward<_Output>(__output)),
						__intermediate_result.state, __to_state, __intermediate_result.error_code,
						__intermediate_result.handled_error);
				}
			}
			_WorkingIntermediate __intermediate_view = __reconstruct(::std::in_place_type<_WorkingIntermediate>,
				__adl::__adl_begin(__intermediate), __adl::__adl_begin(__intermediate_result.output));
			auto __end_result = __basic_encode_one<_ConsumeIntoTheNothingness>(__intermediate_view, __to_encoding,
				::std::forward<_Output>(__output), __to_error_handler, __to_state);

			return _Result(::std::move(__intermediate_result.input), ::std::move(__end_result.output),
				__intermediate_result.state, __end_result.state, __end_result.error_code,
				__end_result.handled_error);
		}

		template <__consume _ConsumeIntoTheNothingness, typename _Input, typename _FromEncoding, typename _Output,
			typename _ToEncoding, typename _FromErrorHandler, typename _ToErrorHandler, typename _FromState,
			typename _ToState>
		constexpr auto __super_basic_transcode_one(_Input&& __input, _FromEncoding& __from_encoding,
			_Output&& __output, _ToEncoding& __to_encoding, _FromErrorHandler& __from_error_handler,
			_ToErrorHandler& __to_error_handler, _FromState& __from_state, _ToState& __to_state) {
			using _IntermediateCodePoint                       = code_point_t<_FromEncoding>;
			constexpr ::std::size_t _IntermediateMaxCodePoints = max_code_points_v<_FromEncoding>;

			_IntermediateCodePoint __intermediate_buffer[_IntermediateMaxCodePoints] {};
			return __super_basic_transcode_one<_ConsumeIntoTheNothingness>(::std::forward<_Input>(__input),
				__from_encoding, __intermediate_buffer, ::std::forward<_Output>(__output), __to_encoding,
				__from_error_handler, __to_error_handler, __from_state, __to_state);
		}

		template <__consume _ConsumeIntoTheNothingness, typename _Input, typename _FromEncoding,
			typename _IntermediateContainer, typename _Output, typename _ToEncoding, typename _FromErrorHandler,
			typename _ToErrorHandler, typename _FromState, typename _ToState>
		constexpr auto __basic_transcode_one(_Input&& __input, _FromEncoding& __from_encoding,
			_IntermediateContainer& __intermediate, _Output&& __output, _ToEncoding& __to_encoding,
			_FromErrorHandler& __from_error_handler, _ToErrorHandler& __to_error_handler, _FromState& __from_state,
			_ToState& __to_state) {
			using _UOutput   = __remove_cvref_t<_Output>;
			using _ValueType = __range_value_type_t<_UOutput>;
			using _Iterator  = __range_iterator_t<_UOutput>;
			using _Blackhole = unbounded_view<__blackhole_iterator>;
			if constexpr (__is_detected_v<__detect_adl_text_transcode_one, _Input, _FromEncoding, _Output,
				              _ToEncoding, _FromErrorHandler, _ToErrorHandler, _FromState, _ToState>) {
				(void)__intermediate;
				return text_transcode_one(::std::forward<_Input>(__input), __from_encoding,
					::std::forward<_Output>(__output), __to_encoding, __from_error_handler, __to_error_handler,
					__from_state, __to_state);
			}
			else if constexpr (_ConsumeIntoTheNothingness == __consume::__embrace_the_void
				&& __is_detected_v<__detect_adl_text_transcode_one, _Input, _FromEncoding, _Blackhole, _ToEncoding,
				     _FromErrorHandler, _ToErrorHandler, _FromState, _ToState>) {
				(void)__intermediate;
				_Blackhole __output_range(__blackhole_iterator {});
				return text_transcode_one(::std::forward<_Input>(__input), __from_encoding,
					::std::move(__output_range), __to_encoding, __from_error_handler, __to_error_handler,
					__from_state, __to_state);
			}
			else if constexpr (::std::is_void_v<_ValueType>) {
				return __super_basic_transcode_one<_ConsumeIntoTheNothingness>(::std::forward<_Input>(__input),
					__from_encoding, __intermediate, ::std::forward<_Output>(__output), __to_encoding,
					__from_error_handler, __to_error_handler, __from_state, __to_state);
			}
			else {
				using _ToUnbounded = unbounded_view<_Iterator>;
				using _ToSpan      = ::ztd::text::span<_ValueType>;
				if constexpr (__is_detected_v<__detect_adl_text_transcode_one, _Input, _FromEncoding, _ToUnbounded,
					              _ToEncoding, _FromErrorHandler, _ToErrorHandler, _FromState, _ToState>) {
					(void)__intermediate;
					_Iterator __first = __adl::__adl_begin(::std::forward<_Output>(__output));
					_ToUnbounded __output_range(::std::move(__first));
					return text_transcode_one(::std::forward<_Input>(__input), __from_encoding,
						::std::move(__output_range), __to_encoding, __from_error_handler, __to_error_handler,
						__from_state, __to_state);
				}
				else if constexpr (
					__is_iterator_concept_or_better_v<contiguous_iterator_tag,
					     _Iterator> && __is_detected_v<__detect_adl_text_transcode_one, _Input, _FromEncoding, _ToSpan, _ToEncoding, _FromErrorHandler, _ToErrorHandler, _FromState, _ToState>) {
					(void)__intermediate;
					_ToSpan __output_range(::std::forward<_Output>(__output));
					return text_transcode_one(::std::forward<_Input>(__input), __from_encoding,
						::std::move(__output_range), __to_encoding, __from_error_handler, __to_error_handler,
						__from_state, __to_state);
				}
				else {
					return __super_basic_transcode_one<_ConsumeIntoTheNothingness>(::std::forward<_Input>(__input),
						__from_encoding, __intermediate, ::std::forward<_Output>(__output), __to_encoding,
						__from_error_handler, __to_error_handler, __from_state, __to_state);
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

			_IntermediateCodePoint __intermediate[_IntermediateMaxCodePoints] {};
			return __basic_transcode_one<_ConsumeIntoTheNothingness>(::std::forward<_Input>(__input),
				__from_encoding, __intermediate, ::std::forward<_Output>(__output), __to_encoding,
				__from_error_handler, __to_error_handler, __from_state, __to_state);
		}

		template <typename _Input, typename _Encoding, typename _CodeUnitContainer, typename _DecodeState,
			typename _EncodeState>
		constexpr auto __basic_validate_code_units_one(_Input&& __input, _Encoding&& __encoding,
			_CodeUnitContainer& __code_unit_output, _DecodeState& __decode_state, _EncodeState& __encode_state) {

			using _UInput      = __remove_cvref_t<_Input>;
			using _UOutput     = __remove_cvref_t<_CodeUnitContainer>;
			using _Result      = validate_result<__reconstruct_t<_UInput>, _DecodeState>;
			using _InSubRange  = subrange<__range_iterator_t<_UInput>, __range_sentinel_t<_UInput>>;
			using _OutSubRange = subrange<__range_iterator_t<_UOutput>, __range_sentinel_t<_UOutput>>;

			_InSubRange __working_input(::std::forward<_Input>(__input));
			_OutSubRange __working_output(__code_unit_output);
			__pass_through_handler __error_handler {};

			auto __transcode_result
				= __basic_transcode_one<__consume::__no>(::std::move(__working_input), __encoding, __working_output,
				     __encoding, __error_handler, __error_handler, __decode_state, __encode_state);
			if (__transcode_result.error_code != encoding_error::ok) {
				return _Result(__reconstruct(::std::in_place_type<_UInput>, ::std::move(__transcode_result.input)),
					false, __decode_state);
			}

			const bool __is_equal_transcode
				= __ce_equal(__adl::__adl_cbegin(__working_input), __adl::__adl_cbegin(__transcode_result.input),
				     __adl::__adl_cbegin(__working_output), __adl::__adl_cbegin(__transcode_result.output));
			if (!__is_equal_transcode) {
				return _Result(__reconstruct(::std::in_place_type<_UInput>, ::std::move(__transcode_result.input)),
					false, __decode_state);
			}
			return _Result(__reconstruct(::std::in_place_type<_UInput>, ::std::move(__transcode_result.input)), true,
				__decode_state);
		}

		template <typename _Input, typename _Encoding, typename _DecodeState, typename _EncodeState>
		constexpr auto __basic_validate_code_units_one(
			_Input&& __input, _Encoding&& __encoding, _DecodeState& __decode_state, _EncodeState& __encode_state) {
			using _UEncoding = __remove_cvref_t<_Encoding>;
			using _CodeUnit  = code_unit_t<_UEncoding>;

			_CodeUnit __code_unit_buf[max_code_units_v<_UEncoding>] {};
			::ztd::text::span<_CodeUnit, max_code_units_v<_UEncoding>> __code_unit_view(__code_unit_buf);
			return __basic_validate_code_units_one(::std::forward<_Input>(__input),
				::std::forward<_Encoding>(__encoding), __code_unit_view, __decode_state, __encode_state);
		}

		template <typename _Input, typename _Encoding, typename _CodePointContainer, typename _CodeUnitContainer,
			typename _EncodeState, typename _DecodeState>
		constexpr auto __basic_validate_code_points_one(_Input&& __input, _Encoding&& __encoding,
			_CodePointContainer& __code_point_output, _CodeUnitContainer& __code_unit_output,
			_EncodeState& __encode_state, _DecodeState& __decode_state) {

			using _UInput      = __remove_cvref_t<_Input>;
			using _UOutput     = __remove_cvref_t<_CodePointContainer>;
			using _Result      = validate_result<__reconstruct_t<_UInput>, _EncodeState>;
			using _InSubRange  = subrange<__range_iterator_t<_UInput>, __range_sentinel_t<_UInput>>;
			using _OutSubRange = subrange<__range_iterator_t<_UOutput>, __range_sentinel_t<_UOutput>>;

			__pass_through_handler __error_handler {};
			_InSubRange __working_input(::std::forward<_Input>(__input));

			auto __encode_result = __basic_encode_one<__consume::__no>(
				__working_input, __encoding, __code_unit_output, __error_handler, __encode_state);
			if (__encode_result.error_code != encoding_error::ok) {
				return _Result(__reconstruct(::std::in_place_type<_UInput>, ::std::move(__encode_result.input)),
					false, __encode_state);
			}

			_OutSubRange __working_output(__code_point_output);

			auto __decode_result = __basic_decode_one<__consume::__no>(
				__code_unit_output, __encoding, __working_output, __error_handler, __decode_state);
			if (__decode_result.error_code != encoding_error::ok) {
				return _Result(__reconstruct(::std::in_place_type<_UInput>, ::std::move(__encode_result.input)),
					false, __encode_state);
			}

			const bool __is_equal_transcode
				= __ce_equal(__adl::__adl_cbegin(__working_input), __adl::__adl_cbegin(__encode_result.input),
				     __adl::__adl_cbegin(__working_output), __adl::__adl_cbegin(__decode_result.output));
			if (!__is_equal_transcode) {
				return _Result(__reconstruct(::std::in_place_type<_UInput>, ::std::move(__encode_result.input)),
					false, __encode_state);
			}
			return _Result(__reconstruct(::std::in_place_type<_UInput>, ::std::move(__encode_result.input)), true,
				__encode_state);
		}

		template <typename _Input, typename _Encoding, typename _EncodeState, typename _DecodeState>
		constexpr auto __basic_validate_code_points_one(
			_Input&& __input, _Encoding&& __encoding, _EncodeState& __encode_state, _DecodeState& __decode_state) {
			using _UEncoding = __remove_cvref_t<_Encoding>;
			using _CodePoint = code_point_t<_UEncoding>;
			using _CodeUnit  = code_unit_t<_UEncoding>;

			_CodePoint __code_point_buf[max_code_points_v<_UEncoding>] {};
			_CodeUnit __code_unit_buf[max_code_units_v<_UEncoding>] {};
			::ztd::text::span<_CodePoint, max_code_points_v<_UEncoding>> __code_point_view(__code_point_buf);
			::ztd::text::span<_CodeUnit, max_code_units_v<_UEncoding>> __code_unit_view(__code_unit_buf);
			return __basic_validate_code_points_one(::std::forward<_Input>(__input),
				::std::forward<_Encoding>(__encoding), __code_point_view, __code_unit_view, __encode_state,
				__decode_state);
		}

		template <typename _Input, typename _Encoding, typename _OutputContainer, typename _ErrorHandler,
			typename _State>
		auto __basic_count_code_points_one(_Input&& __input, _Encoding&& __encoding, _OutputContainer& __output,
			_ErrorHandler&& __error_handler, _State& __state) {
			using _UInput = __remove_cvref_t<_Input>;
			using _Result = count_result<__reconstruct_t<_UInput>, _State>;

			auto __intermediate_result = __basic_decode_one<__consume::__no>(::std::forward<_Input>(__input),
				::std::forward<_Encoding>(__encoding), __output, ::std::forward<_ErrorHandler>(__error_handler),
				__state);
			::std::size_t __written
				= __adl::__adl_cbegin(__intermediate_result.output) - __adl::__adl_cbegin(__output);

			return _Result(::std::move(__intermediate_result.input), __written, __intermediate_result.state);
		}

		template <typename _Input, typename _Encoding, typename _ErrorHandler, typename _State>
		auto __basic_count_code_points_one(
			_Input&& __input, _Encoding&& __encoding, _ErrorHandler&& __error_handler, _State& __state) {
			using _UEncoding                                   = __remove_cvref_t<_Encoding>;
			using _IntermediateCodePoint                       = code_point_t<_UEncoding>;
			constexpr ::std::size_t _IntermediateMaxCodePoints = max_code_points_v<_UEncoding>;

			_IntermediateCodePoint __intermediate_buffer[_IntermediateMaxCodePoints] {};
			return __basic_count_code_points_one(::std::forward<_Input>(__input),
				::std::forward<_Encoding>(__encoding), __intermediate_buffer,
				::std::forward<_ErrorHandler>(__error_handler), __state);
		}

		template <typename _Input, typename _Encoding, typename _OutputContainer, typename _ErrorHandler,
			typename _State>
		auto __basic_count_code_units_one(_Input&& __input, _Encoding&& __encoding, _OutputContainer& __output,
			_ErrorHandler&& __error_handler, _State& __state) {
			using _UInput = __remove_cvref_t<_Input>;
			using _Result = count_result<__reconstruct_t<_UInput>, _State>;

			auto __intermediate_result = __basic_encode_one<__consume::__no>(::std::forward<_Input>(__input),
				::std::forward<_Encoding>(__encoding), __output, ::std::forward<_ErrorHandler>(__error_handler),
				__state);
			::std::size_t __written
				= __adl::__adl_cbegin(__intermediate_result.output) - __adl::__adl_cbegin(__output);

			return _Result(::std::move(__intermediate_result.input), __written, __intermediate_result.state);
		}

		template <typename _Input, typename _Encoding, typename _ErrorHandler, typename _State>
		auto __basic_count_code_units_one(
			_Input&& __input, _Encoding&& __encoding, _ErrorHandler&& __error_handler, _State& __state) {
			using _UEncoding                                  = __remove_cvref_t<_Encoding>;
			using _IntermediateCodeUnit                       = code_unit_t<_UEncoding>;
			constexpr ::std::size_t _IntermediateMaxCodeUnits = max_code_units_v<_UEncoding>;

			_IntermediateCodeUnit __intermediate_buffer[_IntermediateMaxCodeUnits] {};
			return __basic_count_code_units_one(::std::forward<_Input>(__input),
				::std::forward<_Encoding>(__encoding), __intermediate_buffer,
				::std::forward<_ErrorHandler>(__error_handler), __state);
		}
	} // namespace __detail
	ZTD_TEXT_INLINE_ABI_NAMESPACE_CLOSE_I_
}} // namespace ztd::text

#endif // ZTD_TEXT_DETAIL_TRANSCODE_ONE_HPP
