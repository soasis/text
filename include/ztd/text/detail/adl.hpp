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
// =============================================================================

#pragma once

#ifndef ZTD_TEXT_DETAIL_ADL_HPP
#define ZTD_TEXT_DETAIL_ADL_HPP

#include <ztd/text/version.hpp>

#include <iterator>

namespace ztd { namespace text {
	ZTD_TEXT_INLINE_ABI_NAMESPACE_OPEN_I_
	namespace __detail {
		// Blessed Overload Overlord Xeo,
		// may his name be praised.
		namespace __adl {
			// using ::std::cdata;
			using ::std::data;
			using ::std::empty;
			using ::std::size;

			using ::std::begin;
			using ::std::cbegin;
			using ::std::crbegin;
			using ::std::rbegin;

			using ::std::cend;
			using ::std::crend;
			using ::std::end;
			using ::std::rend;

			template <typename _Range>
			constexpr auto __adl_begin(_Range&& __range) noexcept(noexcept(begin(::std::forward<_Range>(__range))))
				-> decltype(begin(::std::forward<_Range>(__range))) {
				return begin(::std::forward<_Range>(__range));
			}

			template <typename _Range>
			constexpr auto __adl_cbegin(_Range&& __range) noexcept(noexcept(cbegin(::std::forward<_Range>(__range))))
				-> decltype(cbegin(::std::forward<_Range>(__range))) {
				return cbegin(::std::forward<_Range>(__range));
			}

			template <typename _Range>
			constexpr auto __adl_rbegin(_Range&& __range) noexcept(noexcept(rbegin(::std::forward<_Range>(__range))))
				-> decltype(rbegin(::std::forward<_Range>(__range))) {
				return rbegin(::std::forward<_Range>(__range));
			}

			template <typename _Range>
			constexpr auto __adl_crbegin(_Range&& __range) noexcept(noexcept(
				crbegin(::std::forward<_Range>(__range)))) -> decltype(crbegin(::std::forward<_Range>(__range))) {
				return crbegin(::std::forward<_Range>(__range));
			}

			template <typename _Range>
			constexpr auto __adl_end(_Range&& __range) noexcept(noexcept(end(::std::forward<_Range>(__range))))
				-> decltype(end(__range)) {
				return end(::std::forward<_Range>(__range));
			}

			template <typename _Range>
			constexpr auto __adl_cend(_Range&& __range) noexcept(noexcept(cend(::std::forward<_Range>(__range))))
				-> decltype(cend(::std::forward<_Range>(__range))) {
				return cend(::std::forward<_Range>(__range));
			}

			template <typename _Range>
			constexpr auto __adl_rend(_Range&& __range) noexcept(noexcept(rend(::std::forward<_Range>(__range))))
				-> decltype(rend(::std::forward<_Range>(__range))) {
				return rend(::std::forward<_Range>(__range));
			}

			template <typename _Range>
			constexpr auto __adl_crend(_Range&& __range) noexcept(noexcept(crend(::std::forward<_Range>(__range))))
				-> decltype(crend(::std::forward<_Range>(__range))) {
				return crend(::std::forward<_Range>(__range));
			}

			template <typename _Range>
			constexpr auto __adl_data(_Range&& __range) noexcept(noexcept(data(::std::forward<_Range>(__range))))
				-> decltype(data(::std::forward<_Range>(__range))) {
				return data(::std::forward<_Range>(__range));
			}

			template <typename _Range>
			constexpr auto __adl_size(_Range&& __range) noexcept(noexcept(size(::std::forward<_Range>(__range))))
				-> decltype(size(::std::forward<_Range>(__range))) {
				return size(::std::forward<_Range>(__range));
			}

			template <typename _Range>
			constexpr auto __adl_empty(_Range&& __range) noexcept(noexcept(empty(::std::forward<_Range>(__range))))
				-> decltype(empty(::std::forward<_Range>(__range))) {
				return empty(::std::forward<_Range>(__range));
			}
		} // namespace __adl

		template <typename _Range>
		using __detect_adl_size
			= decltype(__adl::__adl_size(::std::declval<::std::add_lvalue_reference_t<_Range>>()));

		template <typename _Range>
		using __detect_adl_empty
			= decltype(__adl::__adl_empty(::std::declval<::std::add_lvalue_reference_t<_Range>>()));
	} // namespace __detail
	ZTD_TEXT_INLINE_ABI_NAMESPACE_CLOSE_I_
}} // namespace ztd::text

#endif // ZTD_TEXT_DETAIL_ADL_HPP
