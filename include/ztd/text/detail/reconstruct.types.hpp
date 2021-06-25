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

#ifndef ZTD_TEXT_DETAIL_RECONSTRUCT_TYPES_HPP
#define ZTD_TEXT_DETAIL_RECONSTRUCT_TYPES_HPP

#include <ztd/text/version.hpp>

#include <ztd/text/subrange.hpp>
#include <ztd/text/detail/span.hpp>
#include <ztd/text/detail/memory.hpp>
#include <ztd/text/detail/empty_string.hpp>

#include <iterator>
#include <utility>
#include <type_traits>
#include <string_view>

#include <ztd/text/detail/prologue.hpp>

namespace ztd { namespace text {
	ZTD_TEXT_INLINE_ABI_NAMESPACE_OPEN_I_

	namespace __txt_detail {

		template <typename _Ty, decltype(::ztd::text::dynamic_extent) _Extent, typename _It, typename _Sen,
			::std::enable_if_t<
			     __is_iterator_contiguous_iterator_v<
			          _It> && ((::std::is_const_v<::std::remove_reference_t<__iterator_reference_t<_It>>>) ? std::is_const_v<_Ty> : true)>* = nullptr>
		constexpr ::ztd::text::span<_Ty> reconstruct(
			::std::in_place_type_t<::ztd::text::span<_Ty, _Extent>>, _It __iterator, _Sen __sentinel) noexcept {
#if ZTD_TEXT_IS_ON(ZTD_TEXT_STD_LIBRARY_SPAN_I_)
			return ::ztd::text::span<_Ty>(__iterator, __sentinel);
#else
			if constexpr (!::std::is_integral_v<_Sen>) {
				auto __iterator_address = __adl::__adl_to_address(__iterator);
				auto __sentinel_address = __adl::__adl_to_address(__sentinel);
				return ::ztd::text::span<_Ty>(__iterator_address, __sentinel_address - __iterator_address);
			}
			else {
				auto __iterator_address = __adl::__adl_to_address(__iterator);
				return ::ztd::text::span<_Ty>(__iterator_address, __sentinel);
			}
#endif
		}

		template <typename _Ty, typename _Traits, typename _It, typename _Sen,
			::std::enable_if_t<__is_iterator_contiguous_iterator_v<_It>>* = nullptr>
		constexpr ::std::basic_string_view<_Ty, _Traits> reconstruct(
			::std::in_place_type_t<::std::basic_string_view<_Ty, _Traits>>, _It __iterator,
			_Sen __sentinel) noexcept {
			using _SizeType = typename ::std::basic_string_view<_Ty, _Traits>::size_type;
			if constexpr (!::std::is_integral_v<_Sen>) {
				_SizeType __ptr_size = static_cast<_SizeType>(__sentinel - __iterator);
#if defined(_ITERATOR_DEBUG_LEVEL) && _ITERATOR_DEBUG_LEVEL >= 1
				if (__ptr_size == static_cast<_SizeType>(0)) {
					const auto& __empty_str = __empty_string<_Ty>();
					return ::std::basic_string_view<_Ty, _Traits>(__empty_str + 0, 0);
				}
#endif
				return ::std::basic_string_view<_Ty, _Traits>(::std::addressof(*__iterator), __ptr_size);
			}
			else {
#if defined(_ITERATOR_DEBUG_LEVEL) && _ITERATOR_DEBUG_LEVEL >= 1
				if (static_cast<_SizeType>(__sentinel) == static_cast<_SizeType>(0)) {
					const auto& __empty_str = __empty_string<_Ty>();
					return ::std::basic_string_view<_Ty, _Traits>(__empty_str + 0, 0);
				}
#endif
				return ::std::basic_string_view<_Ty, _Traits>(
					::std::addressof(*__iterator), static_cast<_SizeType>(__sentinel));
			}
		}
	} // namespace __txt_detail

	ZTD_TEXT_INLINE_ABI_NAMESPACE_CLOSE_I_
}} // namespace ztd::text

#endif // ZTD_TEXT_DETAIL_RECONSTRUCT_TYPES_HPP
