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

#ifndef ZTD_TEXT_DETAIL_CONSTANT_ENCODING_TRAITS_HPP
#define ZTD_TEXT_DETAIL_CONSTANT_ENCODING_TRAITS_HPP

#include <ztd/text/version.hpp>

#include <ztd/text/state.hpp>
#include <ztd/text/code_point.hpp>
#include <ztd/text/code_unit.hpp>
#include <ztd/text/is_code_points_replaceable.hpp>
#include <ztd/text/is_code_units_replaceable.hpp>
#include <ztd/text/is_ignorable_error_handler.hpp>
#include <ztd/text/is_bidirectional_encoding.hpp>
#include <ztd/text/is_full_range_representable.hpp>
#include <ztd/text/is_unicode_encoding.hpp>
#include <ztd/text/encode_result.hpp>
#include <ztd/text/decode_result.hpp>
#include <ztd/text/detail/scheme_handler.hpp>

#include <ztd/idk/ebco.hpp>
#include <ztd/idk/span.hpp>

#include <optional>
#include <cstddef>

#include <ztd/prologue.hpp>

namespace ztd { namespace text {

	ZTD_TEXT_INLINE_ABI_NAMESPACE_OPEN_I_

	namespace __txt_detail {
		template <typename _Super, bool = is_code_units_maybe_replaceable_v<typename _Super::encoding_type>>
		class __maybe_replacement_code_units_es { };

		template <typename _Super>
		class __maybe_replacement_code_units_es<_Super, true> {
		private:
			const _Super& _M_super() const noexcept {
				return static_cast<const _Super&>(*this);
			}

		public:
			constexpr auto maybe_replacement_code_units() const noexcept {
				using _OriginalCodeUnit         = code_unit_t<typename _Super::encoding_type>;
				using _CodeUnit                 = typename _Super::code_unit;
				decltype(auto) __maybe_original = this->_M_super().base().maybe_replacement_code_units();
				if constexpr (::std::is_same_v<_OriginalCodeUnit, _CodeUnit>) {
					return __maybe_original;
				}
				else {
					using _OriginalSpan         = ::ztd::span<const _OriginalCodeUnit>;
					using _TransformedSpan      = ::ztd::span<const _CodeUnit>;
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
					return _MaybeTransformedSpan(::std::in_place, __transformed_ptr, __transformed_size);
				}
			}
		};

		template <typename _Super, bool = is_code_points_maybe_replaceable_v<typename _Super::encoding_type>>
		class __maybe_replacement_code_points_es { };

		template <typename _Super>
		class __maybe_replacement_code_points_es<_Super, true> {
		private:
			const _Super& _M_super() const noexcept {
				return static_cast<const _Super&>(*this);
			}

		public:
			constexpr auto maybe_replacement_code_points() const noexcept {
				using _OriginalCodePoint        = code_point_t<typename _Super::encoding_type>;
				using _CodePoint                = typename _Super::code_point;
				decltype(auto) __maybe_original = this->_M_super().base().maybe_replacement_code_points();
				if constexpr (::std::is_same_v<_OriginalCodePoint, _CodePoint>) {
					return __maybe_original;
				}
				else {
					using _OriginalSpan         = ::ztd::span<const _OriginalCodePoint>;
					using _TransformedSpan      = ::ztd::span<const _CodePoint>;
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
					return _MaybeTransformedSpan(::std::in_place, __transformed_ptr, __transformed_size);
				}
			}
		};

		template <typename _Super, typename _Encoding, typename = void>
		class __is_unicode_encoding_es { };

		template <typename _Super, typename _Encoding>
		class __is_unicode_encoding_es<_Super, _Encoding,
			::std::enable_if_t<is_detected_v<__detect_is_unicode_encoding, _Encoding>>> {
		public:
			using is_unicode_encoding = ::std::integral_constant<bool, is_unicode_encoding_v<_Encoding>>;
		};
	} // namespace __txt_detail

	ZTD_TEXT_INLINE_ABI_NAMESPACE_CLOSE_I_
}} // namespace ztd::text

#include <ztd/epilogue.hpp>

#endif // ZTD_TEXT_DETAIL_CONSTANT_ENCODING_TRAITS_HPP
