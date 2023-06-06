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

#pragma once

#ifndef ZTD_TEXT_PETSCII_HPP
#define ZTD_TEXT_PETSCII_HPP

#include <ztd/text/version.hpp>

#include <ztd/text/unicode_code_point.hpp>
#include <ztd/text/impl/single_byte_lookup_encoding.hpp>

#include <ztd/encoding_tables/petscii.tables.hpp>

#include <ztd/prologue.hpp>

namespace ztd { namespace text {
	ZTD_TEXT_INLINE_ABI_NAMESPACE_OPEN_I_

	//////
	/// @brief The current shift state of a PETSCII encoding object and it's associated state during an encoding
	/// operation.
	enum class petscii_shift {
		//////
		/// @brief The SHIFT button is not pressed, and character codes should be interpreted as not being "shifted".
		unshifted = 0,
		//////
		/// @brief The SHIFT button is pressed, and character codes should be interpreted as being "shifted".
		shifted = 1
	};

	//////
	/// @brief The state associated with an ongoing PETSCII encoding operation.
	class petscii_state {
	public:
		//////
		/// @brief The current 'shift' button state of a PETSCII encoding conversion.
		petscii_shift shift;
	};

	//////
	/// @brief The encoding that matches the PETSCII (CBM ASCII) encoding specification, for shifted characters (when
	/// the SHIFT button was pressed on a PET/CBM device).
	///
	/// @tparam _CodeUnit The default code unit type to use when outputting encoded units.
	/// @tparam _CodePoint The code point type to use when outputting decoded units.
	template <typename _CodeUnit = char, typename _CodePoint = unicode_code_point>
	class basic_petscii {
		//////
		/// @brief The individual units that result from an encode operation or are used as input to a decode
		/// operation.
		using code_unit = _CodeUnit;

		//////
		/// @brief The individual units that result from a decode operation or as used as input to an encode
		/// operation. For most encodings, this is going to be a Unicode Code Point or a Unicode Scalar Value.
		using code_point = _CodePoint;

		//////
		/// @brief The state that can be used between calls to the encoder and decoder.
		///
		/// @remarks It is an empty struct because there is no shift state to preserve between complete units of
		/// encoded information. It is also only `state` and not separately `decode_state` and `encode_state`
		/// because one type suffices for both.
		using state = petscii_state;

		//////
		/// @brief Whether or not the decode operation can process all forms of input into code point values.
		using is_decode_injective = ::std::true_type;

		//////
		/// @brief Whether or not the encode operation can process all forms of input into code unit values.
		using is_encode_injective = ::std::false_type;

		//////
		/// @brief The maximum code units a single complete operation of encoding can produce.
		inline static constexpr const ::std::size_t max_code_units = 1;

		//////
		/// @brief The maximum number of code points a single complete operation of decoding can produce.
		inline static constexpr const ::std::size_t max_code_points = 1;

		//////
		/// @brief A range of code units representing the values to use when a replacement happen; for PETSCII, this
		/// is '?'.
		static constexpr ::ztd::span<const code_unit, 1> replacement_code_units() noexcept {
			return __txt_detail::__question_mark_replacement_units<code_unit>;
		}

		//////
		/// @brief Decodes a single complete unit of information as code points and produces a result with the
		/// input and output ranges moved past what was successfully read and written; or, produces an error and
		/// returns the input and output ranges untouched.
		///
		/// @param[in] __input The input view to read code uunits from.
		/// @param[in] __output The output view to write code points into.
		/// @param[in] __error_handler The error handler to invoke if encoding fails.
		/// @param[in, out] __s The necessary state information. For this encoding, the state is empty and means
		/// very little.
		///
		/// @returns A ztd::text::decode_result object that contains the input range, output range, error handler, and
		/// a reference to the passed-in state\.
		///
		/// @remarks To the best ability of the implementation, the iterators will be returned untouched (e.g.,
		/// the input models at least a view and a forward_range). If it is not possible, returned ranges may be
		/// incremented even if an error occurs due to the semantics of any view that models an input_range.
		template <typename _Input, typename _Output, typename _ErrorHandler>
		static constexpr auto decode_one(
			_Input&& __input, _Output&& __output, _ErrorHandler&& __error_handler, state& __s) {
			if (__s.shift == petscii_shift::unshifted) {
				return __txt_impl::__single_byte_lookup_encoding<basic_petscii<_CodeUnit, _CodePoint>,
					&::ztd::et::petscii_unshifted_index_to_code_point,
					&::ztd::et::petscii_unshifted_code_point_to_index, code_unit, code_point,
					state>::decode_one(::std::forward<_Input>(__input), ::std::forward<_Output>(__output),
					::std::forward<_ErrorHandler>(__error_handler), __s);
			}
			else {
				return __txt_impl::__single_byte_lookup_encoding<basic_petscii<_CodeUnit, _CodePoint>,
					&::ztd::et::petscii_shifted_index_to_code_point,
					&::ztd::et::petscii_shifted_code_point_to_index, code_unit, code_point,
					state>::decode_one(::std::forward<_Input>(__input), ::std::forward<_Output>(__output),
					::std::forward<_ErrorHandler>(__error_handler), __s);
			}
		}

		//////
		/// @brief Encodes a single complete unit of information as code units and produces a result with the
		/// input and output ranges moved past what was successfully read and written; or, produces an error and
		/// returns the input and output ranges untouched.
		///
		/// @param[in] __input The input view to read code points from.
		/// @param[in] __output The output view to write code units into.
		/// @param[in] __error_handler The error handler to invoke if encoding fails.
		/// @param[in, out] __s The necessary state information. For this encoding, the state is empty and means
		/// very little.
		///
		/// @returns A ztd::text::encode_result object that contains the input range, output range, error handler, and
		/// a reference to the passed-in state\.
		///
		/// @remarks To the best ability of the implementation, the iterators will be returned untouched (e.g.,
		/// the input models at least a view and a forward_range). If it is not possible, returned ranges may be
		/// incremented even if an error occurs due to the semantics of any view that models an input_range.
		template <typename _Input, typename _Output, typename _ErrorHandler>
		static constexpr auto encode_one(
			_Input&& __input, _Output&& __output, _ErrorHandler&& __error_handler, state& __s) {
			if (__s.shift == petscii_shift::unshifted) {
				return __txt_impl::__single_byte_lookup_encoding<basic_petscii<_CodeUnit, _CodePoint>,
					&::ztd::et::petscii_unshifted_index_to_code_point,
					&::ztd::et::petscii_unshifted_code_point_to_index, code_unit, code_point,
					state>::encode_one(::std::forward<_Input>(__input), ::std::forward<_Output>(__output),
					::std::forward<_ErrorHandler>(__error_handler), __s);
			}
			else {
				return __txt_impl::__single_byte_lookup_encoding<basic_petscii<_CodeUnit, _CodePoint>,
					&::ztd::et::petscii_shifted_index_to_code_point,
					&::ztd::et::petscii_shifted_code_point_to_index, code_unit, code_point,
					state>::encode_one(::std::forward<_Input>(__input), ::std::forward<_Output>(__output),
					::std::forward<_ErrorHandler>(__error_handler), __s);
			}
		}
	};

	//////
	/// @brief An instance of basic_petscii for ease of use.
	inline constexpr basic_petscii<char> petscii = {};

	ZTD_TEXT_INLINE_ABI_NAMESPACE_CLOSE_I_
}} // namespace ztd::text


#include <ztd/epilogue.hpp>

#endif
