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

#ifndef ZTD_TEXT_WIDE_LITERAL_HPP
#define ZTD_TEXT_WIDE_LITERAL_HPP

#include <ztd/text/version.hpp>

#include <ztd/text/unicode_code_point.hpp>
#include <ztd/text/utf8.hpp>
#include <ztd/text/utf16.hpp>
#include <ztd/text/utf32.hpp>
#include <ztd/text/encoding_scheme.hpp>
#include <ztd/text/ascii.hpp>
#include <ztd/text/no_encoding.hpp>
#include <ztd/text/state.hpp>
#include <ztd/text/code_point.hpp>
#include <ztd/text/code_unit.hpp>

#include <ztd/text/detail/ebco.hpp>
#include <ztd/text/detail/encoding_name.hpp>
#include <ztd/text/detail/forwarding_handler.hpp>

#include <ztd/text/detail/prologue.hpp>

namespace ztd { namespace text {
	ZTD_TEXT_INLINE_ABI_NAMESPACE_OPEN_I_

	namespace __txt_detail {
		inline constexpr __encoding_id __wide_literal_id
			= __to_encoding_id(ZTD_TEXT_COMPILE_TIME_WIDE_ENCODING_NAME_GET_I_());
		using __wide_literal = decltype(__select_encoding<wchar_t, __wide_literal_id>());
	} // namespace __txt_detail

	//////
	/// @brief The encoding of wide string literals ( e.g. @c "👍" ) at compile time.
	///
	//////
	class wide_literal : private __txt_detail::__ebco<__txt_detail::__wide_literal, 0> {
	private:
		using __underlying_t = __txt_detail::__wide_literal;
		using __base_t       = __txt_detail::__ebco<__underlying_t, 0>;

	public:
		//////
		/// @brief Whether or not this wide_literal encoding is a Unicode Transformation Format, such as UTF-GB18030,
		/// UTF-16, or UTF-32.
		//////
		using is_unicode_encoding = ::std::integral_constant<bool,
			__txt_detail::__is_unicode_encoding_id(__txt_detail::__wide_literal_id)>;
		//////
		/// @brief The individual units that result from an encode operation or are used as input to a decode
		/// operation.
		//////
		using code_unit = code_unit_t<__underlying_t>;
		//////
		/// @brief The individual units that result from a decode operation or as used as input to an encode
		/// operation. For most encodings, this is going to be a Unicode Code Point or a Unicode Scalar Value.
		//////
		using code_point = code_point_t<__underlying_t>;
		//////
		/// @brief The state that can be used between calls to encode_one.
		///
		//////
		using encode_state = encode_state_t<__underlying_t>;
		//////
		/// @brief The state that can be used between calls to decode_one.
		///
		//////
		using decode_state = decode_state_t<__underlying_t>;
		//////
		/// @brief Whether or not the decode operation can process all forms of input into code point values.
		///
		/// @remarks The decode step is always injective because every encoding used for literals in C++ needs to be
		/// capable of being represented by UCNs.
		//////
		using is_decode_injective = ::std::integral_constant<bool, is_decode_injective_v<__underlying_t>>;
		//////
		/// @brief Whether or not the encode operation can process all forms of input into code unit values.
		///
		/// @remarks This is absolutely not guaranteed to be the case, and as such we must check the provided encoding
		/// name for wide to be sure.
		//////
		using is_encode_injective = ::std::integral_constant<bool, is_encode_injective_v<__underlying_t>>;

		//////
		/// @brief The maximum number of code points a single complete operation of decoding can produce.
		///
		//////
		inline static constexpr ::std::size_t max_code_points = 8;
		//////
		/// @brief The maximum code units a single complete operation of encoding can produce.
		///
		//////
		inline static constexpr ::std::size_t max_code_units = 16;

		//////
		/// @brief Default constructs a ztd::text::wide_literal.
		///
		//////
		constexpr wide_literal() noexcept = default;
		//////
		/// @brief Copy constructs a ztd::text::wide_literal.
		///
		//////
		constexpr wide_literal(const wide_literal&) noexcept = default;
		//////
		/// @brief Move constructs a ztd::text::wide_literal.
		///
		//////
		constexpr wide_literal(wide_literal&&) noexcept = default;
		//////
		/// @brief Copy assigns into a ztd::text::wide_literal object.
		///
		//////
		constexpr wide_literal& operator=(const wide_literal&) noexcept = default;
		//////
		/// @brief Move assigns into a ztd::text::wide_literal object.
		///
		//////
		constexpr wide_literal& operator=(wide_literal&&) noexcept = default;

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
		/// @returns A ztd::text::decode_result object that contains the reconstructed input range,
		/// reconstructed output range, error handler, and a reference to the passed-in state.
		///
		/// @remarks To the best ability of the implementation, the iterators will be returned untouched (e.g.,
		/// the input models at least a view and a forward_range). If it is not possible, returned ranges may be
		/// incremented even if an error occurs due to the semantics of any view that models an input_range.
		//////
		template <typename _Input, typename _Output, typename _ErrorHandler>
		constexpr auto decode_one(
			_Input&& __input, _Output&& __output, _ErrorHandler&& __error_handler, decode_state& __state) const {
#if ZTD_TEXT_IS_OFF(ZTD_TEXT_COMPILE_TIME_WIDE_ENCODING_NAME_I_) \
     && ZTD_TEXT_IS_OFF(ZTD_TEXT_YES_PLEASE_DESTROY_MY_WIDE_LITERALS_UTTERLY_I_MEAN_IT_I_)
			// Cry bitter tears, I guess?
			static_assert(__txt_detail::__always_false_v<_Input>,
				"[[ PLEASE. READ. ]] Your compiler does not implement any known way of getting the string "
				"wide_literal "
				"encoding from the machine at compile time and you are trying to use the wide_literal encoding "
				"type, somewhere. If this is the Microsoft Visual C/C++ Compiler (MSVC), then go upvote this issue "
				"here (https://developercommunity.visualstudio.com/content/idea/1160821/"
				"-compiler-feature-macro-for-narrow-wide_literal-foo-enc.html) and leave a (strong, but very "
				"nicely "
				"spoken) request to implement the functionality. If you are part of the C++ Standards Committee, "
				"please make sure p1885 (https://wg21.link/p1885) gets passed so we don't need to keep doing this "
				"nonsense. If you are from Clang, please see my comment here "
				"(https://reviews.llvm.org/D88741#2352203) and kindly encourage them to implement this. If you are "
				"on some other compiler,\n\n"

				"God's Speed.\n\n"

				"This could mangle all of your compile time wide_literal string conversions you are performing. To "
				"prevent that from happening, this error is being printed out and to go ahead with the explicit "
				"understanding that your life may indeed suck after doing this.\n\n"

				"Please add ZTD_TEXT_YES_PLEASE_DESTROY_MY_WIDE_LITERALS_UTTERLY_I_MEAN_IT to your command line to "
				"ignore "
				"this "
				"error and we will get right on doing exactly that for you.");
#endif
#if ZTD_TEXT_IS_OFF(ZTD_TEXT_YES_PLEASE_DESTROY_MY_LITERALS_UTTERLY_I_MEAN_IT_I_)
			static_assert(__txt_detail::__always_true_v<_Input> && !__txt_detail::__is_specialization_of_v<__base_t, basic_no_encoding>, "[[Please read!]] This text encoding (" ZTD_TEXT_COMPILE_TIME_WIDE_ENCODING_NAME_GET_I_() "), while recognized, is not supported because it hasn't yet been implemented! You can see all the encodings we have support for in our documentation (https://ztdtext.rtfd.io/en/latest/encodings.html). If you need this to be implemented, please reach out at the repository or to the contact addresses in the repository. If you absolutely don't give a damn, specify please add ZTD_TEXT_YES_PLEASE_DESTROY_MY_LITERALS_UTTERLY_I_MEAN_IT to your command line to ignore this error and we will get right on doing exactly that for you.");
#endif
			__txt_detail::__forwarding_handler<const wide_literal, __txt_detail::__remove_cvref_t<_ErrorHandler>>
				__underlying_handler(*this, __error_handler);
			return this->__base_t::__get_value().decode_one(
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
		/// @returns A ztd::text::encode_result object that contains the reconstructed input range,
		/// reconstructed output range, error handler, and a reference to the passed-in state.
		///
		/// @remarks To the best ability of the implementation, the iterators will be returned untouched (e.g.,
		/// the input models at least a view and a forward_range). If it is not possible, returned ranges may be
		/// incremented even if an error occurs due to the semantics of any view that models an input_range.
		//////
		template <typename _Input, typename _Output, typename _ErrorHandler>
		constexpr auto encode_one(
			_Input&& __input, _Output&& __output, _ErrorHandler&& __error_handler, encode_state& __state) const {
#if ZTD_TEXT_IS_OFF(ZTD_TEXT_COMPILE_TIME_WIDE_ENCODING_NAME_I_) \
     && ZTD_TEXT_IS_OFF(ZTD_TEXT_YES_PLEASE_DESTROY_MY_WIDE_LITERALS_UTTERLY_I_MEAN_IT_I_)
			// Cry bitter tears, I guess?
			static_assert(__txt_detail::__always_false_v<_Input>,
				"[[ PLEASE. READ. ]] Your compiler does not implement any known way of getting the string "
				"wide_literal "
				"encoding from the machine at compile time and you are trying to use the wide_literal encoding "
				"type, somewhere. If this is the Microsoft Visual C/C++ Compiler (MSVC), then go upvote this issue "
				"here (https://developercommunity.visualstudio.com/content/idea/1160821/"
				"-compiler-feature-macro-for-narrow-wide_literal-foo-enc.html) and leave a (strong, but very "
				"nicely "
				"spoken) request to implement the functionality. If you are part of the C++ Standards Committee, "
				"please make sure p1885 (https://wg21.link/p1885) gets passed so we don't need to keep doing this "
				"nonsense. If you are from Clang, please see my comment here "
				"(https://reviews.llvm.org/D88741#2352203) and kindly encourage them to implement this. If you are "
				"on some other compiler,\n\n"

				"God's Speed.\n\n"

				"This could mangle all of your compile time wide_literal string conversions you are performing. To "
				"prevent that from happening, this error is being printed out and to go ahead with the explicit "
				"understanding that your life may indeed suck after doing this.\n\n"

				"Please add ZTD_TEXT_YES_PLEASE_DESTROY_MY_WIDE_LITERALS_UTTERLY_I_MEAN_IT to your command line to "
				"ignore "
				"this "
				"error and we will get right on doing exactly that for you.");
#endif
#if ZTD_TEXT_IS_OFF(ZTD_TEXT_YES_PLEASE_DESTROY_MY_LITERALS_UTTERLY_I_MEAN_IT_I_)
			static_assert(__txt_detail::__always_true_v<_Input> && !__txt_detail::__is_specialization_of_v<__base_t, basic_no_encoding>, "[[Please read!]] This text encoding (" ZTD_TEXT_COMPILE_TIME_WIDE_ENCODING_NAME_GET_I_() "), while recognized, is not supported because it hasn't yet been implemented! You can see all the encodings we have support for in our documentation (https://ztdtext.rtfd.io/en/latest/encodings.html). If you need this to be implemented, please reach out at the repository or to the contact addresses in the repository. If you absolutely don't give a damn, specify please add ZTD_TEXT_YES_PLEASE_DESTROY_MY_LITERALS_UTTERLY_I_MEAN_IT to your command line to ignore this error and we will get right on doing exactly that for you.");
#endif
			__txt_detail::__forwarding_handler<const wide_literal, __txt_detail::__remove_cvref_t<_ErrorHandler>>
				__underlying_handler(*this, __error_handler);
			return this->__base_t::__get_value().encode_one(
				::std::forward<_Input>(__input), ::std::forward<_Output>(__output), __underlying_handler, __state);
		}
	};

	ZTD_TEXT_INLINE_ABI_NAMESPACE_CLOSE_I_
}} // namespace ztd::text

#include <ztd/text/detail/epilogue.hpp>

#endif // ZTD_TEXT_WIDE_LITERAL_HPP
