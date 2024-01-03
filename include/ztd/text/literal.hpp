// =============================================================================
//
// ztd.text
// Copyright © JeanHeyd "ThePhD" Meneide and Shepherd's Oasis, LLC
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

#pragma once

#ifndef ZTD_TEXT_LITERAL_HPP
#define ZTD_TEXT_LITERAL_HPP

#include <ztd/text/version.hpp>

#include <ztd/text/unicode_code_point.hpp>

#include <ztd/text/state.hpp>
#include <ztd/text/code_point.hpp>
#include <ztd/text/code_unit.hpp>
#include <ztd/text/is_unicode_encoding.hpp>
#include <ztd/text/is_input_error_skippable.hpp>
#include <ztd/text/detail/encoding_name.hpp>
#include <ztd/text/detail/forwarding_handler.hpp>


#include <ztd/prologue.hpp>

namespace ztd { namespace text {
	ZTD_TEXT_INLINE_ABI_NAMESPACE_OPEN_I_

	namespace __txt_detail {
		inline constexpr text_encoding_id __literal_id
			= __idk_detail::__to_encoding_id(ZTD_CXX_COMPILE_TIME_ENCODING_NAME_GET_I_());
		using __literal = decltype(__select_compile_time_encoding<char, __literal_id>());
	} // namespace __txt_detail

	//////
	/// @brief The encoding of string literal_ts ( e.g. @c "👍" ) at compile time.
	class literal_t : public __txt_detail::__literal {
	private:
		using __base_t = __txt_detail::__literal;

	public:
		//////
		/// @brief Whether or not this literal_t encoding is a Unicode Transformation Format, such as UTF-8,
		/// UTF-EBCDIC, or GB18030.
		using is_unicode_encoding
			= ::std::integral_constant<bool, is_unicode_encoding_id(__txt_detail::__literal_id)>;
		//////
		/// @brief The individual units that result from an encode operation or are used as input to a decode
		/// operation.
		using code_unit = code_unit_t<__base_t>;
		//////
		/// @brief The individual units that result from a decode operation or as used as input to an encode
		/// operation. For most encodings, this is going to be a Unicode Code Point or a Unicode Scalar Value.
		using code_point = code_point_t<__base_t>;
		//////
		/// @brief The state that can be used between calls to encode_one.
		using encode_state = encode_state_t<__base_t>;
		//////
		/// @brief The state that can be used between calls to decode_one.
		using decode_state = decode_state_t<__base_t>;
		//////
		/// @brief Whether or not the decode operation can process all forms of input into code point values.
		///
		/// @remarks The decode step should always be injective because every encoding used for literal_ts in C++
		/// needs to be capable of being represented by UCNs. Whether or not a platform is a jerk, who knows?
		using is_decode_injective = ::std::integral_constant<bool, is_decode_injective_v<__base_t>>;
		//////
		/// @brief Whether or not the encode operation can process all forms of input into code unit values.
		///
		/// @remarks This is absolutely not guaranteed to be the case, and as such we must check the provided encoding
		/// name for us to be sure.
		using is_encode_injective = ::std::integral_constant<bool, is_encode_injective_v<__base_t>>;

		//////
		/// @brief The id representing the decoded text.
		inline static constexpr const ::ztd::text_encoding_id decoded_id = decoded_id_v<__base_t>;
		//////
		/// @brief The id representing the encoded text.
		inline static constexpr const ::ztd::text_encoding_id encoded_id = encoded_id_v<__base_t>;

		//////
		/// @brief The maximum number of code points a single complete operation of decoding can produce.
		inline static constexpr ::std::size_t max_code_points = 16;
		//////
		/// @brief The maximum code units a single complete operation of encoding can produce.
		inline static constexpr ::std::size_t max_code_units = 32;

		//////
		/// @brief Default constructs a ztd::text::literal.
		constexpr literal_t() noexcept = default;
		//////
		/// @brief Copy constructs a ztd::text::literal.
		constexpr literal_t(const literal_t&) noexcept = default;
		//////
		/// @brief Move constructs a ztd::text::literal.
		constexpr literal_t(literal_t&&) noexcept = default;
		//////
		/// @brief Copy assigns into a ztd::text::literal_t object.
		constexpr literal_t& operator=(const literal_t&) noexcept = default;
		//////
		/// @brief Move assigns into a ztd::text::literal_t object.
		constexpr literal_t& operator=(literal_t&&) noexcept = default;

		//////
		/// @brief Decodes a single complete unit of information as code points and produces a result with the
		/// input and output ranges moved past what was successfully read and written; or, produces an error and
		/// returns the input and output ranges untouched.
		///
		/// @param[in] __input The input view to read code uunits from.
		/// @param[in] __output The output view to write code points into.
		/// @param[in] __error_handler The error handler to invoke if encoding fails.
		/// @param[in, out] __state The necessary state information. For this encoding, the state is empty and means
		/// very little.
		///
		/// @returns A ztd::text::decode_result object that contains the input range, output range, error handler, and
		/// a reference to the passed-in state\.
		///
		/// @remarks To the best ability of the implementation, the iterators will be returned untouched (e.g.,
		/// the input models at least a view and a forward_range). If it is not possible, returned ranges may be
		/// incremented even if an error occurs due to the semantics of any view that models an input_range.
		template <typename _Input, typename _Output, typename _ErrorHandler>
		constexpr auto decode_one(
			_Input&& __input, _Output&& __output, _ErrorHandler&& __error_handler, decode_state& __state) const {
#if ZTD_IS_OFF(ZTD_CXX_COMPILE_TIME_ENCODING_NAME) \
     && ZTD_IS_OFF(ZTD_TEXT_YES_PLEASE_DESTROY_MY_LITERALS_UTTERLY_I_MEAN_IT)
			static_assert(always_false_v<_Input>,
				ZTD_TEXT_UNKNOWN_LITERAL_ENCODING_MESSAGE_I_("string literal", "literal_t", ""));
#endif
#if ZTD_IS_OFF(ZTD_TEXT_YES_PLEASE_DESTROY_MY_LITERALS_UTTERLY_I_MEAN_IT)
			static_assert(always_true_v<_Input> && !is_specialization_of_v<__base_t, ::ztd::text::basic_no_encoding>,
				ZTD_TEXT_UNIMPLEMENTED_LITERAL_ENCODING_MESSAGE_I_(
				     ZTD_CXX_COMPILE_TIME_ENCODING_NAME_DESCRIPTION_I_(), ""));
#endif
			__txt_detail::__forwarding_handler<const literal_t, ::std::remove_reference_t<_ErrorHandler>>
				__underlying_handler(*this, __error_handler);
			return this->__base_t::decode_one(
				::std::forward<_Input>(__input), ::std::forward<_Output>(__output), __underlying_handler, __state);
		}

		//////
		/// @brief Encodes a single complete unit of information as code units and produces a result with the
		/// input and output ranges moved past what was successfully read and written; or, produces an error and
		/// returns the input and output ranges untouched.
		///
		/// @param[in] __input The input view to read code points from.
		/// @param[in] __output The output view to write code units into.
		/// @param[in] __error_handler The error handler to invoke if encoding fails.
		/// @param[in, out] __state The necessary state information. For this encoding, the state is empty and means
		/// very little.
		///
		/// @returns A ztd::text::encode_result object that contains the input range, output range, error handler, and
		/// a reference to the passed-in state\.
		///
		/// @remarks To the best ability of the implementation, the iterators will be returned untouched (e.g.,
		/// the input models at least a view and a forward_range). If it is not possible, returned ranges may be
		/// incremented even if an error occurs due to the semantics of any view that models an input_range.
		template <typename _Input, typename _Output, typename _ErrorHandler>
		constexpr auto encode_one(
			_Input&& __input, _Output&& __output, _ErrorHandler&& __error_handler, encode_state& __state) const {
#if ZTD_IS_OFF(ZTD_CXX_COMPILE_TIME_ENCODING_NAME) \
     && ZTD_IS_OFF(ZTD_TEXT_YES_PLEASE_DESTROY_MY_LITERALS_UTTERLY_I_MEAN_IT)
			static_assert(always_false_v<_Input>,
				ZTD_TEXT_UNKNOWN_LITERAL_ENCODING_MESSAGE_I_("string literal", "literal_t", ""));
#endif
#if ZTD_IS_OFF(ZTD_TEXT_YES_PLEASE_DESTROY_MY_LITERALS_UTTERLY_I_MEAN_IT)
			static_assert(always_true_v<_Input> && !is_specialization_of_v<__base_t, ::ztd::text::basic_no_encoding>,
				ZTD_TEXT_UNIMPLEMENTED_LITERAL_ENCODING_MESSAGE_I_(
				     ZTD_CXX_COMPILE_TIME_ENCODING_NAME_DESCRIPTION_I_(), ""));
#endif
			__txt_detail::__forwarding_handler<const literal_t, ::std::remove_reference_t<_ErrorHandler>>
				__underlying_handler(*this, __error_handler);
			return this->__base_t::encode_one(
				::std::forward<_Input>(__input), ::std::forward<_Output>(__output), __underlying_handler, __state);
		}

		template <typename _Result, typename _InputProgress, typename _OutputProgress,
			::std::enable_if_t<::ztd::text::is_input_error_skippable_v<const __base_t&, _Result,
			     const _InputProgress&, const _OutputProgress&>>* = nullptr>
		constexpr auto skip_input_error(_Result&& __result, const _InputProgress& __input_progress,
			const _OutputProgress& __output_progress) const
			noexcept(::ztd::text::is_nothrow_skip_input_error_v<const __base_t&, _Result, const _InputProgress&,
			     const _OutputProgress&>) {
			return ::ztd::text::skip_input_error(static_cast<const __base_t&>(*this),
				::std::forward<_Result>(__result), __input_progress, __output_progress);
		}
	};

	//////
	/// @brief An instance of the literal_t type for ease of use.
	inline constexpr literal_t literal = {};

	ZTD_TEXT_INLINE_ABI_NAMESPACE_CLOSE_I_
}} // namespace ztd::text

#include <ztd/epilogue.hpp>

#endif
