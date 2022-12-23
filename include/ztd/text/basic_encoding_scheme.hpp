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

#ifndef ZTD_TEXT_BASIC_ENCODING_SCHEME_HPP
#define ZTD_TEXT_BASIC_ENCODING_SCHEME_HPP

#include <ztd/text/version.hpp>

#include <ztd/text/state.hpp>
#include <ztd/text/code_point.hpp>
#include <ztd/text/code_unit.hpp>
#include <ztd/text/skip_input_error.hpp>
#include <ztd/text/detail/constant_encoding_traits.hpp>
#include <ztd/text/detail/basic_encoding_scheme_includes.hpp>

#include <optional>
#include <cstddef>

#include <ztd/prologue.hpp>

namespace ztd { namespace text {
	ZTD_TEXT_INLINE_ABI_NAMESPACE_OPEN_I_

	//////
	/// @addtogroup ztd_text_encodings Encodings
	///
	/// @{

	//////
	/// @brief Decomposes the provided Encoding type into a specific endianness (big, little, or native) to allow
	/// for a single encoding type to be viewed in different ways.
	///
	/// @tparam _Encoding The encoding type.
	/// @tparam _Endian The endianess to use. Defaults to ztd::endian::native.
	/// @tparam _Byte The byte type to use. Defaults to ``std::byte``.
	///
	/// @remarks For example, this can be used to construct a Big Endian UTF-16 by using
	/// ``encoding_scheme<ztd::text::utf16_t, ztd::endian::big>``. It can be made interopable with ``unsigned
	/// char`` buffers rather than ``std::byte`` buffers by doing:
	/// ``ztd::text::encoding_scheme<ztd::text::utf32_t, ztd::endian::native, unsigned char>``.
	template <typename _Encoding, endian _Endian = endian::native, typename _Byte = ::std::byte>
	class encoding_scheme : public __txt_detail::__is_unicode_encoding_es<encoding_scheme<_Encoding, _Endian, _Byte>,
		                        unwrap_remove_cvref_t<_Encoding>>,
		                   private ebco<_Encoding> {
	private:
		using __base_t       = ebco<_Encoding>;
		using _UBaseEncoding = unwrap_remove_cvref_t<_Encoding>;
		using _BaseCodeUnit  = code_unit_t<_UBaseEncoding>;

	public:
		//////
		/// @brief The encoding type.
		//////
		/// @brief The encoding type that this scheme wraps.
		using encoding_type = _Encoding;
		//////
		/// @brief The individual units that result from a decode operation or as used as input to an encode
		/// operation. For most encodings, this is going to be a Unicode Code Point or a Unicode Scalar Value.
		using code_point = code_point_t<_UBaseEncoding>;
		///////
		/// @brief The individual units that result from an encode operation or are used as input to a decode
		/// operation.
		///
		/// @remarks Typically, this type is usually always some kind of byte type (unsigned char or std::byte or
		/// other ``sizeof(obj) == 1`` type)./
		using code_unit = _Byte;
		//////
		/// @brief The state that can be used between calls to the decode function.
		///
		/// @remarks Even if the underlying encoding only has a single `state` type, we need to separate the two
		/// out in order to generically handle all encodings. Therefore, the encoding_scheme will always have
		/// both `encode_state` and `decode_state.`
		using decode_state = decode_state_t<_UBaseEncoding>;
		//////
		/// @brief The state that can be used between calls to the encode function.
		///
		/// @remarks Even if the underlying encoding only has a single `state` type, we need to separate the two
		/// out in order to generically handle all encodings. Therefore, the encoding_scheme will always have
		/// both `encode_state` and `decode_state.`
		using encode_state = encode_state_t<_UBaseEncoding>;
		//////
		/// @brief Whether or not the encode operation can process all forms of input into code point values.
		///
		/// @remarks Defers to what the underlying `encoding_type` does.
		using is_encode_injective = ::std::integral_constant<bool, is_encode_injective_v<_UBaseEncoding>>;
		//////
		/// @brief Whether or not the decode operation can process all forms of input into code point values.
		///
		/// @remarks Defers to what the underlying `encoding_type` does.
		using is_decode_injective = ::std::integral_constant<bool, is_decode_injective_v<_UBaseEncoding>>;
		//////
		/// @brief The maximum number of code points a single complete operation of decoding can produce. This is
		/// 1 for all Unicode Transformation Format (UTF) encodings.
		inline static constexpr const ::std::size_t max_code_points = max_code_points_v<_UBaseEncoding>;
		//////
		/// @brief The maximum code units a single complete operation of encoding can produce.
		inline static constexpr const ::std::size_t max_code_units
			= (max_code_units_v<_UBaseEncoding> * sizeof(_BaseCodeUnit)) / (sizeof(_Byte));
		//////
		/// @brief The id representing the decoded text.
		inline static constexpr const ::ztd::text_encoding_id decoded_id = decoded_id_v<_UBaseEncoding>;
		//////
		/// @brief The id representing the encoded text.
		inline static constexpr const ::ztd::text_encoding_id encoded_id = ::ztd::to_byte_text_encoding_id(
			encoded_id_v<_UBaseEncoding>, _Endian, sizeof(code_unit_t<_UBaseEncoding>));

		//////
		/// @brief Default constructs a ztd::text::encoding_scheme.
		encoding_scheme() = default;

		//////
		/// @brief Constructs a ztd::text::encoding_scheme with the encoding object and any additional arguments.
		///
		/// @param[in] __arg0 The first argument used to construct the stored encoding.
		/// @param[in] __args Any additional arguments used to construct the stored encoding.
		template <typename _Arg0, typename... _Args,
			::std::enable_if_t<!::std::is_same_v<remove_cvref_t<_Arg0>, encoding_scheme> // cf
			     && !::std::is_same_v<remove_cvref_t<_Arg0>, ::std::in_place_t>>* = nullptr>
		constexpr encoding_scheme(_Arg0&& __arg0, _Args&&... __args) noexcept(
			::std::is_nothrow_constructible_v<_UBaseEncoding, _Arg0, _Args...>)
		: __base_t(::std::forward<_Arg0>(__arg0), ::std::forward<_Args>(__args)...) {
		}

		//////
		/// @brief Constructs a ztd::text::encoding_scheme with the encoding object and any additional arguments.
		///
		/// @param[in] __args Any additional arguments used to construct the encoding in the erased storage.
		///
		/// @remarks If the provided encoding does not have a byte code_unit type, it is wrapped in an
		/// ztd::text::encoding_scheme first.
		template <typename... _Args>
		constexpr encoding_scheme(::std::in_place_t, _Args&&... __args) noexcept(
			::std::is_nothrow_constructible_v<_UBaseEncoding, _Args...>)
		: __base_t(::std::forward<_Args>(__args)...) {
		}

		//////
		/// @brief Cannot copy-construct a ztd::text::encoding_scheme object.
		encoding_scheme(const encoding_scheme&) = default;

		//////
		/// @brief Cannot copy-assign a ztd::text::encoding_scheme object.
		encoding_scheme& operator=(const encoding_scheme&) = default;

		//////
		/// @brief Move-constructs a ztd::text::encoding_scheme from the provided r-value reference.
		///
		/// @remarks This leaves the passed-in r-value reference without an encoding object. Calling any function
		/// on a moved-fron ztd::text::encoding_scheme, except for destruction, is a violation and invokes
		/// Undefined Behavior (generally, a crash).
		encoding_scheme(encoding_scheme&&) = default;

		//////
		/// @brief Move-assigns a ztd::text::encoding_scheme from the provided r-value reference.
		///
		/// @remarks This leaves the passed-in r-value reference without an encoding object. Calling any function
		/// on a moved-fron ztd::text::encoding_scheme, except for destruction, is a violation and may invoke
		/// Undefined Behavior (generally, a crash).
		encoding_scheme& operator=(encoding_scheme&&) = default;

		//////
		/// @brief Retrives the underlying encoding object.
		///
		/// @returns An l-value reference to the encoding object.
		constexpr encoding_type& base() & noexcept {
			return this->__base_t::get_value();
		}

		//////
		/// @brief Retrives the underlying encoding object.
		///
		/// @returns An l-value reference to the encoding object.
		constexpr const encoding_type& base() const& noexcept {
			return this->__base_t::get_value();
		}

		//////
		/// @brief Retrives the underlying encoding object.
		///
		/// @returns An l-value reference to the encoding object.
		constexpr encoding_type&& base() && noexcept {
			return this->__base_t::get_value();
		}

		//////
		/// @brief Returns, the desired replacement code units to use.
		///
		/// @remarks This is only callable if the function call exists on the wrapped encoding. It is broken down
		/// into a contiguous view type formulated from bytes if the wrapped code unit types do not match.
		template <typename _Unused                                     = encoding_type,
			::std::enable_if_t<is_code_units_replaceable_v<_Unused>>* = nullptr>
		constexpr decltype(auto) replacement_code_units() const noexcept {
			using _OriginalCodeUnit = code_unit_t<encoding_type>;

			decltype(auto) __original = this->base().replacement_code_units();
			if constexpr (::std::is_same_v<_OriginalCodeUnit, code_unit>) {
				return __original;
			}
			else {
				using _OriginalSpan    = ::ztd::span<const _OriginalCodeUnit>;
				using _TransformedSpan = ::ztd::span<const code_unit>;
				_OriginalSpan __guaranteed_code_unit_view(__original);
				// transform into proper type...
				auto __transformed_ptr = reinterpret_cast<const code_unit*>(__guaranteed_code_unit_view.data());
				auto __transformed_size
					= (__guaranteed_code_unit_view.size() * sizeof(_OriginalCodeUnit)) / sizeof(const code_unit);
				return _TransformedSpan(__transformed_ptr, __transformed_size);
			}
		}

		//////
		/// @brief Returns the desired replacement code points to use.
		///
		/// @remarks Is only callable if the function call exists on the wrapped encoding.
		template <typename _Unused                                      = encoding_type,
			::std::enable_if_t<is_code_points_replaceable_v<_Unused>>* = nullptr>
		constexpr decltype(auto) replacement_code_points() const noexcept {
			return this->base().replacement_code_points();
		}

		//////
		/// @brief Returns the desired replacement code units to use, or an empty optional-like type if there is
		/// nothing present.
		///
		/// @remarks This is only callable if the function call exists on the wrapped encoding. It is broken down
		/// into a contiguous view type formulated from bytes if the wrapped code unit types do not match.
		template <typename _Unused                                           = encoding_type,
			::std::enable_if_t<is_code_units_maybe_replaceable_v<_Unused>>* = nullptr>
		constexpr decltype(auto) maybe_replacement_code_units() const noexcept {
			using _OriginalCodeUnit = code_unit_t<encoding_type>;

			decltype(auto) __maybe_original = this->base().maybe_replacement_code_units();
			if constexpr (::std::is_same_v<_OriginalCodeUnit, code_unit>) {
				return __maybe_original;
			}
			else {
				using _OriginalSpan    = ::ztd::span<const _OriginalCodeUnit>;
				using _TransformedSpan = ::ztd::span<const code_unit>;
				if (!__maybe_original) {
					return ::std::optional<_TransformedSpan>(::std::nullopt);
				}
				decltype(auto) __original = *__maybe_original;
				_OriginalSpan __guaranteed_code_unit_view(__original);
				// transform into proper type...
				auto __transformed_ptr = reinterpret_cast<const code_unit*>(__guaranteed_code_unit_view.data());
				auto __transformed_size
					= (__guaranteed_code_unit_view.size() * sizeof(_OriginalCodeUnit)) / sizeof(const code_unit);
				return _TransformedSpan(__transformed_ptr, __transformed_size);
			}
		}

		//////
		/// @brief Returns the desired replacement code units to use.
		///
		/// @remarks This Is only callable if the function call exists on the wrapped encoding.
		template <typename _Unused                                            = encoding_type,
			::std::enable_if_t<is_code_points_maybe_replaceable_v<_Unused>>* = nullptr>
		constexpr decltype(auto) maybe_replacement_code_points() const noexcept {
			return this->base().maybe_replacement_code_points();
		}

		//////
		/// @brief Whether or not this encoding is some form of Unicode encoding.
		constexpr bool contains_unicode_encoding() const noexcept {
			return ::ztd::text::contains_unicode_encoding(this->base());
		}

		//////
		/// @brief Skips any consecutive input errors in the encoded input, where possible.
		///
		/// @remarks This Is only callable if the function call exists on the wrapped encoding.
		template <typename _Result,
			::std::enable_if_t<is_input_error_skippable_v<const encoding_type&, _Result>>* = nullptr>
		constexpr decltype(auto) skip_input_error(_Result&& __result) const noexcept(noexcept(
			::ztd::text::skip_input_error(::std::declval<const encoding_type&>(), ::std::declval<_Result>()))) {
			return ::ztd::text::skip_input_error(this->base(), ::std::forward<_Result>(__result));
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
		/// @returns A ztd::text::decode_result object that contains the reconstructed input range,
		/// reconstructed output range, error handler, and a reference to the passed-in state.
		///
		/// @remarks To the best ability of the implementation, the iterators will be returned untouched (e.g.,
		/// the input models at least a view and a forward_range). If it is not possible, returned ranges may be
		/// incremented even if an error occurs due to the semantics of any view that models an input_range.
		template <typename _Input, typename _Output, typename _ErrorHandler>
		constexpr auto decode_one(
			_Input&& __input, _Output&& __output, _ErrorHandler&& __error_handler, decode_state& __s) const {
			using _UInputRange    = remove_cvref_t<_Input>;
			using _UOutputRange   = remove_cvref_t<_Output>;
			using _CVErrorHandler = ::std::remove_reference_t<_ErrorHandler>;
			using _SubInput       = ztd::ranges::subrange_for_t<::std::remove_reference_t<_Input>>;
			using _SubOutput      = ztd::ranges::subrange_for_t<::std::remove_reference_t<_Output>>;
			using _Result         = decode_result<_SubInput, _SubOutput, decode_state>;

			using _InByteIt  = ranges::word_iterator<_BaseCodeUnit, _UInputRange, _Endian>;
			using _InByteSen = ranges::word_sentinel;
			::ztd::ranges::subrange<_InByteIt, _InByteSen> __inbytes(
				_InByteIt(::std::in_place, ::std::forward<_Input>(__input)), _InByteSen());
			__txt_detail::__scheme_handler<_Byte, _UInputRange, _UOutputRange, _CVErrorHandler>
				__intermediate_handler(__error_handler);
			auto __result = this->base().decode_one(
				::std::move(__inbytes), ::std::forward<_Output>(__output), __intermediate_handler, __s);
			return _Result(::std::move(__result.input).begin().range(), ::std::move(__result.output), __s,
				__result.error_code, __result.error_count);
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
		/// @returns A ztd::text::encode_result object that contains the reconstructed input range,
		/// reconstructed output range, error handler, and a reference to the passed-in state.
		///
		/// @remarks To the best ability of the implementation, the iterators will be returned untouched (e.g.,
		/// the input models at least a view and a forward_range). If it is not possible, returned ranges may be
		/// incremented even if an error occurs due to the semantics of any view that models an input_range.
		template <typename _Input, typename _Output, typename _ErrorHandler>
		constexpr auto encode_one(
			_Input&& __input, _Output&& __output, _ErrorHandler&& __error_handler, encode_state& __s) const {
			using _UInputRange    = remove_cvref_t<_Input>;
			using _UOutputRange   = remove_cvref_t<_Output>;
			using _CVErrorHandler = ::std::remove_reference_t<_ErrorHandler>;
			using _SubInput       = ztd::ranges::subrange_for_t<::std::remove_reference_t<_Input>>;
			using _SubOutput      = ztd::ranges::subrange_for_t<::std::remove_reference_t<_Output>>;
			using _Result         = encode_result<_SubInput, _SubOutput, encode_state>;
			using _OutByteIt      = ranges::word_iterator<_BaseCodeUnit, _UOutputRange, _Endian>;
			using _OutByteSen     = ranges::word_sentinel;

			ranges::subrange<_OutByteIt, _OutByteSen> __outwords(
				_OutByteIt(::std::forward<_Output>(__output)), _OutByteSen());
			__txt_detail::__scheme_handler<_Byte, _UInputRange, _UOutputRange, _CVErrorHandler>
				__intermediate_handler(__error_handler);
			auto __result
				= this->base().encode_one(::std::forward<_Input>(__input), __outwords, __intermediate_handler, __s);
			return _Result(::std::move(__result.input), ::std::move(__result.output).begin().range(), __s,
				__result.error_code, __result.error_count);
		}
	};

	//////
	/// @}

	ZTD_TEXT_INLINE_ABI_NAMESPACE_CLOSE_I_
}} // namespace ztd::text

#include <ztd/epilogue.hpp>

#endif
