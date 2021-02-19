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

#ifndef ZTD_TEXT_ENCODING_SCHEME_HPP
#define ZTD_TEXT_ENCODING_SCHEME_HPP

#include <ztd/text/version.hpp>

#include <ztd/text/state.hpp>
#include <ztd/text/code_point.hpp>
#include <ztd/text/code_unit.hpp>
#include <ztd/text/is_code_points_replaceable.hpp>
#include <ztd/text/is_code_units_replaceable.hpp>
#include <ztd/text/is_ignorable_error_handler.hpp>
#include <ztd/text/is_bidirectional_encoding.hpp>
#include <ztd/text/is_full_range_representable.hpp>
#include <ztd/text/subrange.hpp>
#include <ztd/text/encode_result.hpp>
#include <ztd/text/decode_result.hpp>
#include <ztd/text/encoding_scheme.hpp>
#include <ztd/text/endian.hpp>

#include <ztd/text/detail/word_iterator.hpp>
#include <ztd/text/detail/ebco.hpp>
#include <ztd/text/detail/span.hpp>

#include <optional>
#include <cstddef>

namespace ztd { namespace text {
	ZTD_TEXT_INLINE_ABI_NAMESPACE_OPEN_I_

	namespace __detail {
		template <typename _Byte, typename _UInputRange, typename _UOutputRange, typename _ErrorHandler>
		class __scheme_decode_handler {
		private:
			::std::reference_wrapper<_ErrorHandler> _M_handler;

		public:
			constexpr __scheme_decode_handler(_ErrorHandler& __handler) noexcept : _M_handler(__handler) {
			}

			template <typename _Encoding, typename _Input, typename _Output, typename _State, typename _Progress>
			constexpr auto operator()(const _Encoding& __encoding, decode_result<_Input, _Output, _State> __result,
				const _Progress& __progress) const noexcept(::std::is_nothrow_invocable_v<_ErrorHandler&,
				const _Encoding&, decode_result<_Input, _Output, _State>, const _Progress&>) {
				if constexpr (::std::is_invocable_v<_ErrorHandler&, const _Encoding,
					              decode_result<_Input, _Output, _State>, _Progress>) {
					return this->_M_handler.get()(__encoding, ::std::move(__result), __progress);
				}
				else {
					using _ProgressPointer = __detail::__range_pointer_t<_Progress>;
					using _ProgressWord    = __detail::__range_value_type_t<_Progress>;
					_Byte* __byte_progress_data
						= reinterpret_cast<_Byte*>(const_cast<_ProgressPointer>(__progress.data()));
					auto __byte_progress_size
						= (__detail::__adl::__adl_size(__progress) * sizeof(_ProgressWord)) / (sizeof(_Byte));
					::std::span<_Byte> __byte_progress(__byte_progress_data, __byte_progress_size);
					return this->_M_handler.get()(__encoding, ::std::move(__result), __byte_progress);
				}
			}
		};

		template <typename _Super, bool = is_code_units_replaceable_v<typename _Super::encoding_type>>
		class __replacement_code_units { };

		template <typename _Super>
		class __replacement_code_units<_Super, true> {
		private:
			const _Super& _M_super() const noexcept {
				return static_cast<const _Super&>(*this);
			}

		public:
			constexpr auto replacement_code_units() const noexcept {
				using _OriginalCodeUnit   = code_unit_of_t<typename _Super::encoding_type>;
				using _CodeUnit           = typename _Super::code_unit;
				decltype(auto) __original = this->_M_super().base().replacement_code_units();
				if constexpr (std::is_same_v<_OriginalCodeUnit, _CodeUnit>) {
					return __original;
				}
				else {
					using _OriginalSpan    = ::std::span<const _OriginalCodeUnit>;
					using _TransformedSpan = ::std::span<const _CodeUnit>;
					_OriginalSpan __guaranteed_code_unit_view(__original);
					// transform into proper type...
					auto __transformed_ptr
						= reinterpret_cast<const _CodeUnit*>(__guaranteed_code_unit_view.data());
					auto __transformed_size = (__guaranteed_code_unit_view.size() * sizeof(_OriginalCodeUnit))
						/ sizeof(const _CodeUnit);
					return _TransformedSpan(__transformed_ptr, __transformed_size);
				}
			}
		};

		template <typename _Super, bool = is_code_points_replaceable_v<typename _Super::encoding_type>>
		class __replacement_code_points { };

		template <typename _Super>
		class __replacement_code_points<_Super, true> {
		private:
			const _Super& _M_super() const noexcept {
				return static_cast<const _Super&>(*this);
			}

		public:
			constexpr auto replacement_code_points() const noexcept {
				using _OriginalCodePoint  = code_point_of_t<typename _Super::encoding_type>;
				using _CodePoint          = typename _Super::code_point;
				decltype(auto) __original = this->_M_super().base().replacement_code_points();
				if constexpr (std::is_same_v<_OriginalCodePoint, _CodePoint>) {
					return __original;
				}
				else {
					using _OriginalSpan    = ::std::span<const _OriginalCodePoint>;
					using _TransformedSpan = ::std::span<const _CodePoint>;
					_OriginalSpan __guaranteed_code_point_view(__original);
					// transform into proper type...
					auto __transformed_ptr
						= reinterpret_cast<const _CodePoint*>(__guaranteed_code_point_view.data());
					auto __transformed_size = (__guaranteed_code_point_view.size() * sizeof(_OriginalCodePoint))
						/ sizeof(const _CodePoint);
					return _TransformedSpan(__transformed_ptr, __transformed_size);
				}
			}
		};

		template <typename _Super, bool = is_code_units_maybe_replaceable_v<typename _Super::encoding_type>>
		class __maybe_replacement_code_units { };

		template <typename _Super>
		class __maybe_replacement_code_units<_Super, true> {
		private:
			const _Super& _M_super() const noexcept {
				return static_cast<const _Super&>(*this);
			}

		public:
			constexpr auto maybe_replacement_code_units() const noexcept {
				using _OriginalCodeUnit         = code_unit_of_t<typename _Super::encoding_type>;
				using _CodeUnit                 = typename _Super::code_unit;
				decltype(auto) __maybe_original = this->_M_super().base().maybe_replacement_code_units();
				if constexpr (std::is_same_v<_OriginalCodeUnit, _CodeUnit>) {
					return __maybe_original;
				}
				else {
					using _OriginalSpan         = ::std::span<const _OriginalCodeUnit>;
					using _TransformedSpan      = ::std::span<const _CodeUnit>;
					using _MaybeTransformedSpan = ::std::optional<_TransformedSpan>;
					if (!__maybe_original) {
						return _MaybeTransformedSpan(::std::nullopt);
					}
					decltype(auto) __original = *::std::forward<decltype(__maybe_original)>(__maybe_original);
					_OriginalSpan __guaranteed_code_unit_view(__original);
					// transform into proper type...
					auto __transformed_ptr
						= reinterpret_cast<const _CodeUnit*>(__guaranteed_code_unit_view.data());
					auto __transformed_size = (__guaranteed_code_unit_view.size() * sizeof(_OriginalCodeUnit))
						/ sizeof(const _CodeUnit);
					return _MaybeTransformedSpan(std::in_place, __transformed_ptr, __transformed_size);
				}
			}
		};

		template <typename _Super, bool = is_code_points_maybe_replaceable_v<typename _Super::encoding_type>>
		class __maybe_replacement_code_points { };

		template <typename _Super>
		class __maybe_replacement_code_points<_Super, true> {
		private:
			const _Super& _M_super() const noexcept {
				return static_cast<const _Super&>(*this);
			}

		public:
			constexpr auto maybe_replacement_code_points() const noexcept {
				using _OriginalCodePoint        = code_point_of_t<typename _Super::encoding_type>;
				using _CodePoint                = typename _Super::code_point;
				decltype(auto) __maybe_original = this->_M_super().base().maybe_replacement_code_points();
				if constexpr (std::is_same_v<_OriginalCodePoint, _CodePoint>) {
					return __maybe_original;
				}
				else {
					using _OriginalSpan         = ::std::span<const _OriginalCodePoint>;
					using _TransformedSpan      = ::std::span<const _CodePoint>;
					using _MaybeTransformedSpan = ::std::optional<_TransformedSpan>;
					if (!__maybe_original) {
						return _MaybeTransformedSpan(::std::nullopt);
					}
					decltype(auto) __original = *::std::forward<decltype(__maybe_original)>(__maybe_original);
					_OriginalSpan __guaranteed_code_point_view(__original);
					// transform into proper type...
					auto __transformed_ptr
						= reinterpret_cast<const _CodePoint*>(__guaranteed_code_point_view.data());
					auto __transformed_size = (__guaranteed_code_point_view.size() * sizeof(_OriginalCodePoint))
						/ sizeof(const _CodePoint);
					return _MaybeTransformedSpan(std::in_place, __transformed_ptr, __transformed_size);
				}
			}
		};
	} // namespace __detail

	//////
	/// @addtogroup ztd_text_encodings Encodings
	/// @{
	//////

	//////
	/// @brief Decomposes the provided Encoding type into a specific endianness (big, little, or native) to allow for a
	/// single encoding type to be viewed in different ways.
	///
	/// @tparam _Encoding The encoding type.
	/// @tparam _Endian The endianess to use. Defaults to ztd::text::endian::native.
	/// @tparam _Byte The byte type to use. Defaults to ``std::byte``.
	///
	/// @remarks For example, this can be used to construct a Big Endian UTF-16 by using
	/// ``encoding_scheme<ztd::text::utf16, ztd::text::endian::big>``. It can be made interopable with ``unsigned
	/// char`` buffers rather than ``std::byte`` buffers by doing: ``ztd::text::encoding_scheme<ztd::text::utf32,
	/// ztd::text::endian::native, unsigned char>``.
	//////
	template <typename _Encoding, endian _Endian = endian::native, typename _Byte = ::std::byte>
	class encoding_scheme
	: private __detail::__ebco<_Encoding>,
	  private __detail::__replacement_code_units<encoding_scheme<_Encoding, _Endian, _Byte>,
		  is_code_points_replaceable_v<__detail::__remove_cvref_t<__detail::__unwrap_t<_Encoding>>>>,
	  public __detail::__replacement_code_points<encoding_scheme<_Encoding, _Endian, _Byte>,
		  is_code_units_replaceable_v<__detail::__remove_cvref_t<__detail::__unwrap_t<_Encoding>>>>,
	  public __detail::__maybe_replacement_code_units<encoding_scheme<_Encoding, _Endian, _Byte>,
		  is_code_points_maybe_replaceable_v<__detail::__remove_cvref_t<__detail::__unwrap_t<_Encoding>>>>,
	  public __detail::__maybe_replacement_code_points<encoding_scheme<_Encoding, _Endian, _Byte>,
		  is_code_units_maybe_replaceable_v<__detail::__remove_cvref_t<__detail::__unwrap_t<_Encoding>>>> {
	private:
		using __base_t       = __detail::__ebco<_Encoding>;
		using _UBaseEncoding = __detail::__remove_cvref_t<__detail::__unwrap_t<_Encoding>>;
		using _BaseCodeUnit  = code_unit_of_t<_UBaseEncoding>;

	public:
		///////
		/// @brief The encoding type that this scheme wraps.
		///
		///////
		using encoding_type = _Encoding;
		//////
		/// @brief The individual units that result from a decode operation or as used as input to an encode
		/// operation. For most encodings, this is going to be a Unicode Code Point or a Unicode Scalar Value.
		//////
		using code_point = code_point_of_t<_UBaseEncoding>;
		///////
		/// @brief The individual units that result from an encode operation or are used as input to a decode
		/// operation.
		///
		/// @remarks Typically, this type is usually always some kind of byte type (unsigned char or std::byte or
		/// other ``sizeof(obj) == 1`` type).
		///////
		using code_unit = _Byte;
		//////
		/// @brief The state that can be used between calls to the decode function.
		///
		/// @remarks Even if the underlying encoding only has a single @c state type, we need to separate the two out
		/// in order to generically handle all encodings. Therefore, the encoding_scheme will always have both
		/// @c encode_state and @c decode_state.
		//////
		using decode_state = decode_state_of_t<_UBaseEncoding>;
		//////
		/// @brief The state that can be used between calls to the encode function.
		///
		/// @remarks Even if the underlying encoding only has a single @c state type, we need to separate the two out
		/// in order to generically handle all encodings. Therefore, the encoding_scheme will always have both
		/// @c encode_state and @c decode_state.
		//////
		using encode_state = encode_state_of_t<_UBaseEncoding>;
		//////
		/// @brief Whether or not the encode operation can process all forms of input into code point values.
		///
		/// @remarks Defers to what the underlying @c encoding_type does.
		//////
		using is_encode_injective = ::std::integral_constant<bool, is_encode_injective_v<_UBaseEncoding>>;
		//////
		/// @brief Whether or not the decode operation can process all forms of input into code point values.
		///
		/// @remarks Defers to what the underlying @c encoding_type does.
		//////
		using is_decode_injective = ::std::integral_constant<bool, is_decode_injective_v<_UBaseEncoding>>;
		//////
		/// @brief The maximum number of code points a single complete operation of decoding can produce. This is
		/// 1 for all Unicode Transformation Format (UTF) encodings.
		//////
		inline static constexpr const ::std::size_t max_code_points = max_code_points_v<_UBaseEncoding>;
		//////
		/// @brief The maximum code units a single complete operation of encoding can produce.
		///
		//////
		inline static constexpr const ::std::size_t max_code_units
			= (max_code_units_v<_UBaseEncoding> * sizeof(_BaseCodeUnit)) / (sizeof(_Byte));

		//////
		/// @brief Constructs a ztd::text::encoding_scheme with the given arguments.
		///
		//////
		using __base_t::__base_t;

		//////
		/// @brief Retrives the underlying encoding object.
		///
		/// @returns An l-value reference to the encoding object.
		//////
		constexpr encoding_type& base() & noexcept {
			return this->__base_t::get_value();
		}

		//////
		/// @brief Retrives the underlying encoding object.
		///
		/// @returns An l-value reference to the encoding object.
		//////
		constexpr const encoding_type& base() const& noexcept {
			return this->__base_t::get_value();
		}

		//////
		/// @brief Retrives the underlying encoding object.
		///
		/// @returns An l-value reference to the encoding object.
		//////
		constexpr encoding_type&& base() && noexcept {
			return this->__base_t::get_value();
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
		/// @returns A @ref ztd::text::decode_result object that contains the reconstructed input range,
		/// reconstructed output range, error handler, and a reference to the passed-in state.
		///
		/// @remarks To the best ability of the implementation, the iterators will be returned untouched (e.g.,
		/// the input models at least a view and a forward_range). If it is not possible, returned ranges may be
		/// incremented even if an error occurs due to the semantics of any view that models an input_range.
		//////
		template <typename _InputRange, typename _OutputRange, typename _ErrorHandler>
		constexpr auto decode_one(_InputRange&& __input, _OutputRange&& __output, _ErrorHandler&& __error_handler,
			decode_state& __s) const {
			using _UInputRange   = __detail::__remove_cvref_t<_InputRange>;
			using _UOutputRange  = __detail::__remove_cvref_t<_OutputRange>;
			using _UErrorHandler = __detail::__remove_cvref_t<_ErrorHandler>;
			using _Result = __detail::__reconstruct_decode_result_t<_UInputRange, _UOutputRange, decode_state>;
			using _InByteIt
				= __detail::__word_iterator<_BaseCodeUnit, __detail::__range_iterator_t<_UInputRange>, _Endian>;
			using _InByteSen = __detail::__word_sentinel<__detail::__range_sentinel_t<_UInputRange>>;

			auto __init   = __detail::__adl::__adl_cbegin(__input);
			auto __inlast = __detail::__adl::__adl_cend(__input);
			subrange<_InByteIt, _InByteSen> __inbytes(
				_InByteIt(::std::move(__init)), _InByteSen(::std::move(__inlast)));
			__detail::__scheme_decode_handler<_Byte, _UInputRange, _UOutputRange, _UErrorHandler> __scheme_handler(
				__error_handler);
			auto __result
				= this->base().decode_one(__inbytes, ::std::forward<_OutputRange>(__output), __scheme_handler, __s);
			return _Result(__detail::__reconstruct(::std::in_place_type<_UInputRange>, __result.input.begin().base(),
				               __result.input.end().base()),
				__detail::__reconstruct(::std::in_place_type<_UOutputRange>, ::std::move(__result.output)), __s,
				__result.error_code, __result.handled_error);
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
		/// @returns A @ref ztd::text::encode_result object that contains the reconstructed input range,
		/// reconstructed output range, error handler, and a reference to the passed-in state.
		///
		/// @remarks To the best ability of the implementation, the iterators will be returned untouched (e.g.,
		/// the input models at least a view and a forward_range). If it is not possible, returned ranges may be
		/// incremented even if an error occurs due to the semantics of any view that models an input_range.
		//////
		template <typename _InputRange, typename _OutputRange, typename _ErrorHandler>
		constexpr auto encode_one(_InputRange&& __input, _OutputRange&& __output, _ErrorHandler&& __error_handler,
			encode_state& __s) const {
			using _UInputRange  = __detail::__remove_cvref_t<_InputRange>;
			using _UOutputRange = __detail::__remove_cvref_t<_OutputRange>;
			using _Result       = __detail::__reconstruct_encode_result_t<_UInputRange, _UOutputRange, encode_state>;
			using _OutByteIt
				= __detail::__word_iterator<_BaseCodeUnit, __detail::__range_iterator_t<_UOutputRange>, _Endian>;
			using _OutByteSen = __detail::__word_sentinel<__detail::__range_sentinel_t<_UOutputRange>>;

			auto __outit   = __detail::__adl::__adl_begin(__output);
			auto __outlast = __detail::__adl::__adl_end(__output);
			subrange<_OutByteIt, _OutByteSen> __outwords(
				_OutByteIt(::std::move(__outit)), _OutByteSen(::std::move(__outlast)));
			auto __result = this->base().encode_one(::std::forward<_InputRange>(__input), __outwords,
				::std::forward<_ErrorHandler>(__error_handler), __s);
			return _Result(__detail::__reconstruct(::std::in_place_type<_UInputRange>, __result.input),
				__detail::__reconstruct(::std::in_place_type<_UOutputRange>, __result.output.begin().base(),
				     __result.output.end().base()),
				__s, __result.error_code, __result.handled_error);
		}
	};

	//////
	/// @brief A UTF-16 encoding, in Little Endian format, with inputs as a sequence of bytes.
	///
	/// @tparam _Byte The byte type to use. Typically, this is @c std::byte or @c unsigned char.
	//////
	template <typename _Byte>
	using basic_utf16_le = encoding_scheme<utf16, endian::little, _Byte>;

	//////
	/// @brief A UTF-16 encoding, in Big Endian format, with inputs as a sequence of bytes.
	///
	/// @tparam _Byte The byte type to use. Typically, this is @c std::byte or @c "unsigned char" .
	//////
	template <typename _Byte>
	using basic_utf16_be = encoding_scheme<utf16, endian::big, _Byte>;

	//////
	/// @brief A UTF-16 encoding, in Native Endian format, with inputs as a sequence of bytes.
	///
	/// @tparam _Byte The byte type to use. Typically, this is @c std::byte or @c "unsigned char" .
	//////
	template <typename _Byte>
	using basic_utf16_ne = encoding_scheme<utf16, endian::native, _Byte>;

	//////
	/// @brief A UTF-16 encoding, in Little Endian format, with inputs as a sequence of bytes.
	///
	//////
	using utf16_le = basic_utf16_le<::std::byte>;

	//////
	/// @brief A UTF-16 encoding, in Big Endian format, with inputs as a sequence of bytes.
	///
	//////
	using utf16_be = basic_utf16_be<::std::byte>;

	//////
	/// @brief A UTF-16 encoding, in Native Endian format, with inputs as a sequence of bytes.
	///
	//////
	using utf16_ne = basic_utf16_ne<::std::byte>;

	//////
	/// @brief A UTF-32 encoding, in Little Endian format, with inputs as a sequence of bytes.
	///
	/// @tparam _Byte The byte type to use. Typically, this is @c std::byte or @c "unsigned char" .
	//////
	template <typename _Byte>
	using basic_utf32_le = encoding_scheme<utf32, endian::little, _Byte>;

	//////
	/// @brief A UTF-32 encoding, in Big Endian format, with inputs as a sequence of bytes.
	///
	/// @tparam _Byte The byte type to use. Typically, this is @c std::byte or @c "unsigned char" .
	//////
	template <typename _Byte>
	using basic_utf32_be = encoding_scheme<utf32, endian::big, _Byte>;

	//////
	/// @brief A UTF-32 encoding, in Native Endian format, with inputs as a sequence of bytes.
	///
	/// @tparam _Byte The byte type to use. Typically, this is @c std::byte or @c "unsigned char" .
	//////
	template <typename _Byte>
	using basic_utf32_ne = encoding_scheme<utf32, endian::native, _Byte>;

	//////
	/// @brief A UTF-32 encoding, in Little Endian format, with inputs as a sequence of bytes.
	///
	//////
	using utf32_le = basic_utf32_le<::std::byte>;

	//////
	/// @brief A UTF-32 encoding, in Big Endian format, with inputs as a sequence of bytes.
	///
	//////
	using utf32_be = basic_utf32_be<::std::byte>;

	//////
	/// @brief A UTF-32 encoding, in Big Endian format, with inputs as a sequence of bytes.
	///
	//////
	using utf32_ne = basic_utf32_ne<::std::byte>;

	//////
	/// @}
	//////

	ZTD_TEXT_INLINE_ABI_NAMESPACE_CLOSE_I_
}} // namespace ztd::text

#endif // ZTD_TEXT_ENCODING_SCHEME_HPP
