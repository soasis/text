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

#pragma once

#ifndef ZTD_TEXT_REPLACEMENT_ERROR_HANDLER_HPP
#define ZTD_TEXT_REPLACEMENT_ERROR_HANDLER_HPP

#include <ztd/text/version.hpp>

#include <ztd/text/code_point.hpp>
#include <ztd/text/code_unit.hpp>
#include <ztd/text/state.hpp>
#include <ztd/text/encode_result.hpp>
#include <ztd/text/decode_result.hpp>
#include <ztd/text/encoding_error.hpp>
#include <ztd/text/is_code_points_replaceable.hpp>
#include <ztd/text/is_code_units_replaceable.hpp>
#include <ztd/text/is_unicode_code_point.hpp>
#include <ztd/text/skip_input_error.hpp>
#include <ztd/text/detail/transcode_routines.hpp>
#include <ztd/text/detail/pass_through_handler.hpp>

#include <ztd/ranges/range.hpp>
#include <ztd/ranges/reconstruct.hpp>
#include <ztd/idk/type_traits.hpp>
#include <ztd/idk/detail/unicode.hpp>

#include <climits>
#include <cstddef>
#include <utility>

#include <ztd/prologue.hpp>

namespace ztd { namespace text {
	ZTD_TEXT_INLINE_ABI_NAMESPACE_OPEN_I_

	namespace __txt_detail {

		template <typename _Encoding, typename _Input, typename _Result>
		constexpr _Result&& __write_direct(const _Encoding&, _Input&& __input, _Result&& __result) noexcept {
			using _UOutputRange = remove_cvref_t<decltype(__result.output)>;

			auto __out_it  = ::ztd::ranges::begin(__result.output);
			auto __outlast = ::ztd::ranges::end(__result.output);
			if (__out_it == __outlast) {
				// BAIL
				return ::std::forward<_Result>(__result);
			}

			if (::ztd::ranges::empty(__input)) {
				// empty range, everything is okay
				__result.error_code = encoding_error::ok;
				return ::std::forward<_Result>(__result);
			}

			for (const auto& __element : ::std::forward<_Input>(__input)) {
				if (__out_it == __outlast) {
					__result.output = ranges::reconstruct(
						::std::in_place_type<_UOutputRange>, ::std::move(__out_it), ::std::move(__outlast));
					return ::std::forward<_Result>(__result);
				}
				(*__out_it) = __element;
				++__out_it;
			}

			__result.output = ranges::reconstruct(
				::std::in_place_type<_UOutputRange>, ::std::move(__out_it), ::std::move(__outlast));
			__result.error_code = encoding_error::ok;
			return ::std::forward<_Result>(__result);
		}

		template <typename _Encoding, typename _Result>
		constexpr _Result&& __write_static_code_points_direct(
			const _Encoding& __encoding, _Result&& __result) noexcept {
			using _InputCodePoint = code_point_t<_Encoding>;
			if constexpr (is_code_points_replaceable_v<_Encoding, decltype(__result.state)&>) {
				return __txt_detail::__write_direct(__encoding, __encoding.replacement_code_points(__result.state),
					::std::forward<_Result>(__result));
			}
			else if constexpr (is_code_points_replaceable_v<_Encoding>) {
				return __txt_detail::__write_direct(
					__encoding, __encoding.replacement_code_points(), ::std::forward<_Result>(__result));
			}
			else if constexpr (is_code_points_maybe_replaceable_v<_Encoding, decltype(__result.state)&>) {
				decltype(auto) __maybe_code_points = __encoding.maybe_replacement_code_points(__result.state);
				if (__maybe_code_points) {
					return __txt_detail::__write_direct(__encoding,
						*::std::forward<decltype(__maybe_code_points)>(__maybe_code_points),
						::std::forward<_Result>(__result));
				}
				else {
					return ::std::forward<_Result>(__result);
				}
			}
			else if constexpr (is_code_points_maybe_replaceable_v<_Encoding>) {
				decltype(auto) __maybe_code_points = __encoding.maybe_replacement_code_points();
				if (__maybe_code_points) {
					return __txt_detail::__write_direct(__encoding,
						*::std::forward<decltype(__maybe_code_points)>(__maybe_code_points),
						::std::forward<_Result>(__result));
				}
				else {
					return ::std::forward<_Result>(__result);
				}
			}
			else if constexpr (is_unicode_code_point_v<_InputCodePoint>) {
				constexpr _InputCodePoint __replacements[1]
					= { static_cast<_InputCodePoint>(__ztd_idk_detail_replacement) };
				return __txt_detail::__write_direct(__encoding, __replacements, ::std::forward<_Result>(__result));
			}
			else {
				static_assert(always_false_v<_Encoding>,
					"There is no logical replacement code points to insert into the stream on failure for the "
					"specified encoding type.");
			}
		}

		template <typename _Encoding, typename _Result>
		constexpr _Result&& __write_static_code_units_direct(
			const _Encoding& __encoding, _Result&& __result) noexcept {
			using _InputCodeUnit = code_unit_t<_Encoding>;
			if constexpr (is_code_units_replaceable_v<_Encoding, decltype(__result.state)&>) {
				return __txt_detail::__write_direct(__encoding, __encoding.replacement_code_units(__result.state),
					::std::forward<_Result>(__result));
			}
			else if constexpr (is_code_units_replaceable_v<_Encoding>) {
				return __txt_detail::__write_direct(
					__encoding, __encoding.replacement_code_units(), ::std::forward<_Result>(__result));
			}
			else if constexpr (is_code_units_maybe_replaceable_v<_Encoding, decltype(__result.state)&>) {
				decltype(auto) __maybe_code_points = __encoding.maybe_replacement_code_units(__result.state);
				if (__maybe_code_points) {
					return __txt_detail::__write_direct(__encoding,
						*::std::forward<decltype(__maybe_code_points)>(__maybe_code_points),
						::std::forward<_Result>(__result));
				}
				else {
					return ::std::forward<_Result>(__result);
				}
			}
			else if constexpr (is_code_units_maybe_replaceable_v<_Encoding>) {
				decltype(auto) __maybe_code_points = __encoding.maybe_replacement_code_units();
				if (__maybe_code_points) {
					return __txt_detail::__write_direct(__encoding,
						*::std::forward<decltype(__maybe_code_points)>(__maybe_code_points),
						::std::forward<_Result>(__result));
				}
				else {
					return ::std::forward<_Result>(__result);
				}
			}
			else if constexpr (sizeof(_InputCodeUnit) >= sizeof(char)) {
				constexpr _InputCodeUnit __replacements[1]
					= { static_cast<_InputCodeUnit>(__ztd_idk_detail_ascii_replacement) };
				return __txt_detail::__write_direct(__encoding, __replacements, ::std::forward<_Result>(__result));
			}
			else {
				static_assert(always_false_v<_Encoding>,
					"There is no logical replacement code units to insert into the stream on failure for the "
					"specified encoding type.");
			}
		}

		template <typename _Encoding, typename _State>
		constexpr ::std::size_t __fill_replacement_code_point_static(const _Encoding& __encoding, _State& __state,
			code_point_t<_Encoding> (&__replacement_code_points)[max_code_points_v<_Encoding>]) {
			using _InputCodePoint = code_point_t<_Encoding>;
			if constexpr (is_code_points_replaceable_v<_Encoding, _State&>) {
				::std::size_t __replacement_index = 0;
				for (const auto& __element : __encoding.replacement_code_points(__state)) {
					__replacement_code_points[__replacement_index] = static_cast<_InputCodePoint>(__element);
					++__replacement_index;
				}
				return __replacement_index;
			}
			else if constexpr (is_code_points_replaceable_v<_Encoding>) {
				::std::size_t __replacement_index = 0;
				for (const auto& __element : __encoding.replacement_code_points()) {
					__replacement_code_points[__replacement_index] = static_cast<_InputCodePoint>(__element);
					++__replacement_index;
				}
				return __replacement_index;
			}
			else if constexpr (is_code_points_maybe_replaceable_v<_Encoding, _State&>) {
				::std::size_t __replacement_index  = 0;
				decltype(auto) __maybe_code_points = __encoding.maybe_replacement_code_points(__state);
				if (__maybe_code_points) {
					decltype(auto) __code_points
						= *::std::forward<decltype(__maybe_code_points)>(__maybe_code_points);
					for (const auto& __element : __code_points) {
						__replacement_code_points[__replacement_index] = static_cast<_InputCodePoint>(__element);
						++__replacement_index;
					}
				}
				return __replacement_index;
			}
			else if constexpr (is_code_points_maybe_replaceable_v<_Encoding>) {
				::std::size_t __replacement_index  = 0;
				decltype(auto) __maybe_code_points = __encoding.maybe_replacement_code_points();
				if (__maybe_code_points) {
					decltype(auto) __code_points
						= *::std::forward<decltype(__maybe_code_points)>(__maybe_code_points);
					for (const auto& __element : __code_points) {
						__replacement_code_points[__replacement_index] = static_cast<_InputCodePoint>(__element);
						++__replacement_index;
					}
				}
				return __replacement_index;
			}
			else if constexpr (is_unicode_code_point_v<_InputCodePoint>) {
				__replacement_code_points[0] = static_cast<_InputCodePoint>(__ztd_idk_detail_replacement);
				return 1;
			}
			else {
				static_assert(always_false_v<_Encoding>,
					"There is no logical replacement code points to insert into the stream on failure for the "
					"specified encoding type.");
			}
		}

		template <typename _Encoding, typename _State>
		constexpr ::std::size_t __fill_replacement_code_unit_static(const _Encoding& __encoding, _State& __state,
			code_unit_t<_Encoding> (&__replacement_code_units)[max_code_units_v<_Encoding>]) {
			using _InputCodeUnit = code_unit_t<_Encoding>;
			if constexpr (is_code_units_replaceable_v<_Encoding, _State&>) {
				::std::size_t __replacement_index = 0;
				for (const auto& __element : __encoding.replacement_code_units(__state)) {
					__replacement_code_units[__replacement_index] = static_cast<_InputCodeUnit>(__element);
					++__replacement_index;
				}
				return __replacement_index;
			}
			else if constexpr (is_code_units_replaceable_v<_Encoding>) {
				::std::size_t __replacement_index = 0;
				for (const auto& __element : __encoding.replacement_code_units()) {
					__replacement_code_units[__replacement_index] = static_cast<_InputCodeUnit>(__element);
					++__replacement_index;
				}
				return __replacement_index;
			}
			else if constexpr (is_code_units_maybe_replaceable_v<_Encoding, _State&>) {
				::std::size_t __replacement_index = 0;
				decltype(auto) __maybe_code_units = __encoding.maybe_replacement_code_units(__state);
				if (__maybe_code_units) {
					decltype(auto) __code_units
						= *::std::forward<decltype(__maybe_code_units)>(__maybe_code_units);
					for (const auto& __element : __code_units) {
						__replacement_code_units[__replacement_index] = static_cast<_InputCodeUnit>(__element);
						++__replacement_index;
					}
				}
				return __replacement_index;
			}
			else if constexpr (is_code_units_maybe_replaceable_v<_Encoding>) {
				::std::size_t __replacement_index = 0;
				decltype(auto) __maybe_code_units = __encoding.maybe_replacement_code_units();
				if (__maybe_code_units) {
					decltype(auto) __code_units
						= *::std::forward<decltype(__maybe_code_units)>(__maybe_code_units);
					for (const auto& __element : __code_units) {
						__replacement_code_units[__replacement_index] = static_cast<_InputCodeUnit>(__element);
						++__replacement_index;
					}
				}
				return __replacement_index;
			}
			else if constexpr (is_unicode_code_point_v<_InputCodeUnit>) {
				__replacement_code_units[0] = static_cast<_InputCodeUnit>(__ztd_idk_detail_replacement);
				return 1;
			}
			else if constexpr (sizeof(_InputCodeUnit) >= sizeof(char)) {
				__replacement_code_units[0] = static_cast<_InputCodeUnit>(__ztd_idk_detail_ascii_replacement);
				return 1;
			}
			else {
				static_assert(always_false_v<_Encoding>,
					"There is no logical replacement code units to insert into the stream on failure for the "
					"specified encoding type.");
			}
		}
	} // namespace __txt_detail

	//////
	/// @brief An error handler that replaces bad code points and code units with a chosen code point / code unit
	/// sequence.
	///
	/// @remarks This class hooks into the encodings passed as the first parameter to the error handling functions to
	/// see if they define either `replacement_code_points()` or `replacement_code_units()` function. If so, they
	/// will call them and use the returned contiguous range to isnert code points or code units into the function. If
	/// neither of these exist, then it checks for a definition of a `maybe_replacement_code_points()` or a @c
	/// maybe_replacement_code_units() function. If either is present, they are expected to return a `std::optional`
	/// of a contiguous range. If it is engaged (the `std::optional` is filled) it will be used. Otherwise, if it is
	/// not engaged, then it will explicitly fall back to attempt to insert the default replacement character `U`+FFFD
	/// (<tt>U'�'</tt>) or <tt>?</tt> character. If the output is out of room for the desired object, then nothing will
	/// be inserted at all.
	class replacement_handler_t {
	private:
		template <typename _Encoding, typename _Input, typename _Output, typename _State>
		static constexpr auto _S_encode_replace(
			const _Encoding& __encoding, encode_result<_Input, _Output, _State> __result) noexcept {
			using _CodeUnit = code_unit_t<_Encoding>;
			if constexpr (is_code_units_replaceable_v<_Encoding, _State&>) {
				return __txt_detail::__write_direct(
					__encoding, __encoding.replacement_code_units(__result.state), ::std::move(__result));
			}
			else if constexpr (is_code_units_replaceable_v<_Encoding>) {
				return __txt_detail::__write_direct(
					__encoding, __encoding.replacement_code_units(), ::std::move(__result));
			}
			else if constexpr (
				!(is_code_points_maybe_replaceable_v<_Encoding,
				       _State&> || is_code_points_maybe_replaceable_v<_Encoding>)&&is_unicode_code_point_v<_CodeUnit>) {
				constexpr _CodeUnit __replacements[1] = { static_cast<_CodeUnit>(__ztd_idk_detail_replacement) };
				return __txt_detail::__write_direct(__encoding, __replacements, ::std::move(__result));
			}
			else {
				if constexpr (is_code_units_maybe_replaceable_v<_Encoding, _State&>) {
					auto __maybe_direct_replacement = __encoding.maybe_replacement_code_units(__result.state);
					if (__maybe_direct_replacement) {
						const auto& __direct_replacement = *__maybe_direct_replacement;
						return __txt_detail::__write_direct(
							__encoding, __direct_replacement, ::std::move(__result));
					}
				}
				else if constexpr (is_code_units_maybe_replaceable_v<_Encoding>) {
					auto __maybe_direct_replacement = __encoding.maybe_replacement_code_units();
					if (__maybe_direct_replacement) {
						const auto& __direct_replacement = *__maybe_direct_replacement;
						return __txt_detail::__write_direct(
							__encoding, __direct_replacement, ::std::move(__result));
					}
				}

				using _InputCodePoint = code_point_t<_Encoding>;

				_InputCodePoint __replacement[max_code_points_v<_Encoding>] {};
				::std::size_t __replacement_size = 0;

				if constexpr (is_code_points_replaceable_v<_Encoding, _State&>) {
					auto __replacement_code_units = __encoding.replacement_code_points(__result.state);
					for (const auto& __element : __replacement_code_units) {
						__replacement[__replacement_size] = __element;
						++__replacement_size;
					}
				}
				else if constexpr (is_code_points_replaceable_v<_Encoding>) {
					auto __replacement_code_units = __encoding.replacement_code_points();
					for (const auto& __element : __replacement_code_units) {
						__replacement[__replacement_size] = __element;
						++__replacement_size;
					}
				}
				else {
					__replacement_size = __txt_detail::__fill_replacement_code_point_static(
						__encoding, __result.state, __replacement);
				}

				const ::ztd::span<const _InputCodePoint> __replacement_range(__replacement, __replacement_size);

				__txt_detail::__pass_through_handler __handler {};
				encode_state_t<_Encoding> __state = copy_encode_state_with(__encoding, __result.state);
				auto __encresult                  = __txt_detail::__basic_encode_one<__txt_detail::__consume::__no>(
                         __replacement_range, __encoding, ::std::move(__result.output), __handler, __state);
				__result.output = ::std::move(__encresult.output);
				if (__encresult.error_code != encoding_error::ok) {
					// we can't even encode a single code unit
					// into the stream... report error and bail
					return __result;
				}

				__result.error_code = encoding_error::ok;

				return __result;
			}
		}

		template <typename _Encoding, typename _Input, typename _Output, typename _State>
		static constexpr auto _S_decode_replace(
			const _Encoding& __encoding, decode_result<_Input, _Output, _State> __result) noexcept {
			using _CodePoint = code_point_t<_Encoding>;

			if constexpr (is_code_points_replaceable_v<_Encoding, _State&>) {
				return __txt_detail::__write_direct(
					__encoding, __encoding.replacement_code_points(__result.state), ::std::move(__result));
			}
			else if constexpr (is_code_points_replaceable_v<_Encoding>) {
				return __txt_detail::__write_direct(
					__encoding, __encoding.replacement_code_points(), ::std::move(__result));
			}
			else if constexpr (
				!(is_code_points_maybe_replaceable_v<_Encoding,
				       _State&> || is_code_points_maybe_replaceable_v<_Encoding>)&&is_unicode_code_point_v<_CodePoint>) {
				constexpr _CodePoint __replacements[1] = { static_cast<_CodePoint>(__ztd_idk_detail_replacement) };
				return __txt_detail::__write_direct(__encoding, __replacements, ::std::move(__result));
			}
			else {
				if constexpr (is_code_points_maybe_replaceable_v<_Encoding, _State&>) {
					auto __maybe_direct_replacement = __encoding.maybe_replacement_code_points(__result.state);
					if (__maybe_direct_replacement) {
						const auto& __direct_replacement = *__maybe_direct_replacement;
						return __txt_detail::__write_direct(
							__encoding, __direct_replacement, ::std::move(__result));
					}
				}
				else if constexpr (is_code_points_maybe_replaceable_v<_Encoding>) {
					auto __maybe_direct_replacement = __encoding.maybe_replacement_code_points();
					if (__maybe_direct_replacement) {
						const auto& __direct_replacement = *__maybe_direct_replacement;
						return __txt_detail::__write_direct(
							__encoding, __direct_replacement, ::std::move(__result));
					}
				}
				using _InputCodeUnit = code_unit_t<_Encoding>;
				_InputCodeUnit __replacement[max_code_units_v<_Encoding>] {};
				::std::size_t __replacement_size = 0;
				if constexpr (is_code_units_replaceable_v<_Encoding, _State&>) {
					auto __replacement_code_units = __encoding.replacement_code_units(__result.state);
					for (const auto& __element : __replacement_code_units) {
						__replacement[__replacement_size] = __element;
						++__replacement_size;
					}
				}
				else if constexpr (is_code_units_replaceable_v<_Encoding>) {
					auto __replacement_code_units = __encoding.replacement_code_units();
					for (const auto& __element : __replacement_code_units) {
						__replacement[__replacement_size] = __element;
						++__replacement_size;
					}
				}
				else {
					__replacement_size = __txt_detail::__fill_replacement_code_unit_static(
						__encoding, __result.state, __replacement);
				}

				const ::ztd::span<const _InputCodeUnit> __replacement_range(__replacement, __replacement_size);

				__txt_detail::__pass_through_handler __handler {};
				decode_state_t<_Encoding> __state = copy_decode_state_with(__encoding, __result.state);
				auto __decresult                  = __txt_detail::__basic_decode_one<__txt_detail::__consume::__no>(
                         __replacement_range, __encoding, ::std::move(__result.output), __handler, __state);
				__result.output = ::std::move(__decresult.output);
				if (__decresult.error_code != encoding_error::ok) {
					// we can't even decode a single code unit
					// into the stream... report error and bail
					return __result;
				}

				__result.error_code = encoding_error::ok;
				return __result;
			}
		}

	public:
		//////
		/// @brief The function call for inserting replacement code units at the point of failure, before returning
		/// flow back to the caller of the encode operation.
		///
		/// @param[in] __encoding The Encoding that experienced the error.
		/// @param[in] __result The current state of the encode operation.
		template <typename _Encoding, typename _Input, typename _Output, typename _State, typename _InputProgress,
			typename _OutputProgress>
		constexpr auto operator()(const _Encoding& __encoding, encode_result<_Input, _Output, _State> __result,
			const _InputProgress&, const _OutputProgress&) const noexcept {
			if (__result.error_code == encoding_error::insufficient_output_space) {
				// BAIL
				return __result;
			}

			return ::ztd::text::skip_input_error(
				__encoding, this->_S_encode_replace(__encoding, ::std::move(__result)));
		}

		//////
		/// @brief The function call for inserting replacement code points at the point of failure, before returning
		/// flow back to the caller of the decode operation.
		///
		/// @param[in] __encoding The Encoding that experienced the error.
		/// @param[in] __result The current state of the encode operation.
		template <typename _Encoding, typename _Input, typename _Output, typename _State, typename _InputProgress,
			typename _OutputProgress>
		constexpr auto operator()(const _Encoding& __encoding, decode_result<_Input, _Output, _State> __result,
			const _InputProgress&, const _OutputProgress&) const noexcept {
			if (__result.error_code == encoding_error::insufficient_output_space) {
				// BAIL
				return __result;
			}

			return ::ztd::text::skip_input_error(
				__encoding, this->_S_decode_replace(__encoding, ::std::move(__result)));
		}
	};

	//////
	/// @brief A convenience variable for passing the replacement_handler_t handler to functions.
	inline constexpr replacement_handler_t replacement_handler = {};

	ZTD_TEXT_INLINE_ABI_NAMESPACE_CLOSE_I_
}} // namespace ztd::text

#endif
