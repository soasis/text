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

#ifndef ZTD_TEXT_NO_NORMALIZATION_HPP
#define ZTD_TEXT_NO_NORMALIZATION_HPP

#include <ztd/text/version.hpp>

#include <ztd/text/normalization_result.hpp>

#include <ztd/ranges/adl.hpp>
#include <ztd/ranges/reconstruct.hpp>

#include <cstddef>

#include <ztd/prologue.hpp>

namespace ztd { namespace text {
	ZTD_TEXT_INLINE_ABI_NAMESPACE_OPEN_I_

	//////
	/// @addtogroup ztd_text_normalization Normalization
	///
	/// @{

	//////
	/// @brief A normalization form that does nothing.
	class no_normalization {
	public:
		//////
		/// @brief An optional `static constexpr` hinting that a fixed-size, maximum-output container can be used.
		static inline constexpr const ::std::size_t max_output = 1;

		//////
		/// @brief Performs the associated normalization operation on the given input, putting data in the
		/// provided output.
		///
		/// @param[in] __input The input range to read from.
		/// @param[in] __output The output range to write to.
		///
		/// @returns A ztd::text::normalization_result representing the normalized output.
		template <typename _Input, typename _Output>
		constexpr auto operator()(_Input&& __input, _Output&& __output) noexcept {
			using _UInput  = remove_cvref_t<_Input>;
			using _UOutput = remove_cvref_t<_Output>;
			using _Result
				= normalization_result<ztd::ranges::reconstruct_t<_UInput>, ztd::ranges::reconstruct_t<_UOutput>>;

			auto __in_it   = ::ztd::ranges::ranges_adl::adl_begin(__input);
			auto __in_last = ::ztd::ranges::ranges_adl::adl_end(__input);
			auto __out_it  = ::ztd::ranges::ranges_adl::adl_begin(__output);
			auto __outlast = ::ztd::ranges::ranges_adl::adl_end(__output);
			if (__in_it == __in_last) {
				return _Result(::ztd::ranges::reconstruct(
					               ::std::in_place_type<_UInput>, ::std::move(__in_it), ::std::move(__in_last)),
					::ztd::ranges::reconstruct(
					     ::std::in_place_type<_UOutput>, ::std::move(__out_it), ::std::move(__outlast)),
					normalization_error::ok);
			}
			if (__out_it == __outlast) {
				return _Result(::ztd::ranges::reconstruct(
					               ::std::in_place_type<_UInput>, ::std::move(__in_it), ::std::move(__in_last)),
					::ztd::ranges::reconstruct(
					     ::std::in_place_type<_UOutput>, ::std::move(__out_it), ::std::move(__outlast)),
					normalization_error::insufficient_output);
			}

			*__out_it = *__in_it;
			++__in_it;
			++__out_it;
			return _Result(::ztd::ranges::reconstruct(
				               ::std::in_place_type<_UInput>, ::std::move(__in_it), ::std::move(__in_last)),
				::ztd::ranges::reconstruct(
				     ::std::in_place_type<_UOutput>, ::std::move(__out_it), ::std::move(__outlast)),
				normalization_error::ok);
		}
	};

	//////
	/// @}

	ZTD_TEXT_INLINE_ABI_NAMESPACE_CLOSE_I_
}} // namespace ztd::text

#include <ztd/epilogue.hpp>

#endif // ZTD_TEXT_NO_NORMALIZATION_HPP
