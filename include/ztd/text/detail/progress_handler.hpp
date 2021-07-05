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

#ifndef ZTD_TEXT_DETAIL_PROGRESS_HANDLER_HPP
#define ZTD_TEXT_DETAIL_PROGRESS_HANDLER_HPP

#include <ztd/text/version.hpp>

#include <ztd/text/decode_result.hpp>
#include <ztd/text/encode_result.hpp>
#include <ztd/text/code_point.hpp>
#include <ztd/text/code_unit.hpp>

#include <ztd/ranges/adl.hpp>

#include <array>
#include <type_traits>

#include <ztd/prologue.hpp>

namespace ztd { namespace text {
	ZTD_TEXT_INLINE_ABI_NAMESPACE_OPEN_I_
	namespace __txt_detail {

		template <bool _AssumeValid, typename _DesiredEncoding>
		class __progress_handler {
		public:
			using assume_valid = ::std::integral_constant<bool, _AssumeValid>;

			::std::array<code_point_t<_DesiredEncoding>, max_code_points_v<_DesiredEncoding>> _M_code_points;
			::std::size_t _M_code_points_size;
			::std::array<code_unit_t<_DesiredEncoding>, max_code_units_v<_DesiredEncoding>> _M_code_units;
			::std::size_t _M_code_units_size;

			constexpr __progress_handler() noexcept
			: _M_code_points(), _M_code_points_size(), _M_code_units(), _M_code_units_size() {
			}

			template <typename _Encoding, typename _InputRange, typename _OutputRange, typename _State,
				typename _Progress>
			constexpr auto operator()(const _Encoding&, encode_result<_InputRange, _OutputRange, _State> __result,
				const _Progress& __progress) noexcept {
				_M_code_points_size = ranges::ranges_adl::adl_size(__progress);
				// avoid needing potentially non-constexpr ::std::copy
#ifdef __cpp_lib_constexpr_algorithms
				::std::copy_n(ranges::ranges_adl::adl_cbegin(__progress), this->_M_code_points_size,
					ranges::ranges_adl::adl_begin(this->_M_code_points));
#else
				for (::std::size_t __index = 0; __index < _M_code_points_size; ++__index) {
					_M_code_points[__index] = __progress[__index];
				}
#endif
				return __result;
			}

			template <typename _Encoding, typename _InputRange, typename _OutputRange, typename _State,
				typename _Progress>
			constexpr auto operator()(const _Encoding&, decode_result<_InputRange, _OutputRange, _State> __result,
				const _Progress& __progress) noexcept {
				_M_code_units_size = ranges::ranges_adl::adl_size(__progress);
#ifdef __cpp_lib_constexpr_algorithms
				::std::copy_n(ranges::ranges_adl::adl_cbegin(__progress), this->_M_code_units_size,
					ranges::ranges_adl::adl_begin(this->_M_code_units));
#else
				// avoid needing potentially non-constexpr ::std::copy
				auto __first = ranges::ranges_adl::adl_cbegin(__progress);
				for (::std::size_t __index = 0; __index < _M_code_units_size; (void)++__index, (void)++__first) {
					_M_code_units[__index] = *__first;
				}
#endif
				return __result;
			}
		};
	} // namespace __txt_detail
	ZTD_TEXT_INLINE_ABI_NAMESPACE_CLOSE_I_
}} // namespace ztd::text


#include <ztd/epilogue.hpp>

#endif // ZTD_TEXT_DETAIL_PROGRESS_HANDLER_HPP
