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

#ifndef ZTD_TEXT_IS_TRANSCODING_COMPATIBLE_HPP
#define ZTD_TEXT_IS_TRANSCODING_COMPATIBLE_HPP

#include <ztd/text/version.hpp>

#include <ztd/text/is_unicode_code_point.hpp>
#include <ztd/text/code_point.hpp>
#include <ztd/text/code_unit.hpp>
#include <ztd/text/unicode_code_point.hpp>
#include <ztd/text/unicode_scalar_value.hpp>
#include <ztd/text/forward.hpp>

#include <ztd/text/detail/type_traits.hpp>

namespace ztd { namespace text {
	ZTD_TEXT_INLINE_ABI_NAMESPACE_OPEN_I_

	namespace __detail {

		// clang-format off
		template <typename _From, typename _To>
		struct __is_bitwise_transcoding_compatible : ::std::integral_constant<bool,
			::std::is_same_v<__remove_cvref_t<_To>, __remove_cvref_t<_From>>
			// if the To is from ASCII, then it's bitwise-compatible with all UTF-8 implementations,
			// provided those UTF-8 implementations have a sufficiently sized/aligned char that can be bit-copied.
			|| (::std::is_same_v<__remove_cvref_t<_From>, ascii>
				&& (::std::is_same_v<__remove_cvref_t<_To>, utf8>
					|| ::std::is_base_of_v<__impl::__utf8_tag, __remove_cvref_t<_To>>)
				&& ((sizeof(code_unit_of_t<__remove_cvref_t<_To>>) == sizeof(char))
					&& (alignof(code_unit_of_t<__remove_cvref_t<_To>>) == alignof(char)))
			)
		> { };
		// clang-format on

		template <typename _From, typename _To>
		inline constexpr bool __is_bitwise_transcoding_compatible_v
			= __is_bitwise_transcoding_compatible<_From, _To>::value;

	} // namespace __detail

	//////
	/// @brief Checks whether or not the specified @p _From encoding can be transcoded to the @p _To encoding without
	/// by form of bit copying.
	///
	/// @tparam _From The encoding that is going to decode the input code units into the intermediate code points.
	/// @tparam _To The encoding that is going to encode the intermediate code points into the final code units.
	//////
	template <typename _From, typename _To>
	class is_bitwise_transcoding_compatible
	: public ::std::integral_constant<bool, __detail::__is_bitwise_transcoding_compatible_v<_From, _To>> { };

	//////
	/// @brief A @c "::value" alias for ztd::text::is_transcoding_compatible.
	///
	//////
	template <typename _From, typename _To>
	constexpr bool is_bitwise_transcoding_compatible_v = is_bitwise_transcoding_compatible<_From, _To>::value;

	//////
	/// @brief Checks whether or not the specified @p _From encoding can be transcoded to the @p _To encoding without
	/// invoking a lossy conversion when using the intermediate code points.
	///
	/// @tparam _From The encoding that is going to decode the input code units into the intermediate code points.
	/// @tparam _To The encoding that is going to encode the intermediate code points into the final code units.
	///
	/// @remarks First, it checks if the encodings are bitwise compatible with one another (e.g., transcoding ASCII to
	/// UTF-8). If that is not the case, then it checks if the two encodings are just identical. Finally, it checks if
	/// the code point types are the same or if it's putting unicode scalar values into unicode code points (which is
	/// valid one way, but not the other way since scalar values do not allow surrogates). If none of these are true,
	/// then, the intermediate code point likely cannot convert between the two losslessly.
	//////
	// clang-format off
	template <typename _From, typename _To>
	class is_transcoding_compatible
	: public ::std::integral_constant<bool,
		__detail::__is_bitwise_transcoding_compatible_v<_From,_To>
		|| ::std::is_same_v<__detail::__remove_cvref_t<_From>, __detail::__remove_cvref_t<_To>>
		|| ::std::is_same_v<code_point_of_t<_From>, code_point_of_t<_To>>
		|| (is_unicode_scalar_value_v<code_point_of_t<_From>>
			? (is_unicode_code_point_v<code_point_of_t<_To>>)
			: (is_unicode_code_point_v<_From> && !is_unicode_scalar_value_v<_To>))
	> {
	};
	// clang-format on

	//////
	/// @brief A @c "::value" alias for ztd::text::is_transcoding_compatible.
	///
	//////
	template <typename _From, typename _To>
	constexpr bool is_transcoding_compatible_v = is_transcoding_compatible<_To, _From>::value;

	ZTD_TEXT_INLINE_ABI_NAMESPACE_CLOSE_I_
}} // namespace ztd::text

#endif // ZTD_TEXT_IS_TRANSCODING_COMPATIBLE_HPP
