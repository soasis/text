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

#ifndef ZTD_TEXT_ENCODE_ITERATOR_HPP
#define ZTD_TEXT_ENCODE_ITERATOR_HPP

#include <ztd/text/version.hpp>

#include <ztd/text/error_handler.hpp>
#include <ztd/text/state.hpp>

#include <ztd/text/detail/encoding_iterator.hpp>

namespace ztd { namespace text {
	ZTD_TEXT_INLINE_ABI_NAMESPACE_OPEN_I_

	//////
	/// @brief The encode sentinel to use as the @c end value for a ztd::text::encode_iterator.
	///
	//////
	class encode_sentinel : public __detail::__encoding_sentinel { };

	//////
	/// @brief An iterator over an iterator of code points, presenting the code points as code units. Uses the @p
	/// _Encoding specified to do so.
	///
	/// @tparam _Encoding The encoding to read the underlying range of code points as.
	/// @tparam _Range The range of input that will be fed into the _FromEncoding's decode operation.
	/// @tparam _ErrorHandler The error handler for any encode-step failures.
	/// @tparam _State The state type to use for the encode operations to intermediate code points.
	///
	/// @remarks This type produces proxies as their reference type, and are only readable, not writable iterators. The
	/// iterator presents code units one at a time, regardless of how many code units are output by one decode
	/// operation. This means if, for example, one (1) UTF-32 code point becomes four (4) UTF-8 code units, it will
	/// present each code unit one at a time. If you are looking to explicitly know what a single encode operation maps
	/// into as far as number of code points to code units (and vice-versa), you will have to use lower-level
	/// interfaces.
	//////
	template <typename _Encoding, typename _Range, typename _ErrorHandler = default_handler,
		typename _State = encoding_encode_state_t<_Encoding>>
	class encode_iterator
	: public __detail::__encoding_iterator<__detail::__transaction::__encode,
		  encode_iterator<_Encoding, _Range, _ErrorHandler, _State>, _Encoding, _Range, _ErrorHandler, _State> {
	private:
		using __base_t = __detail::__encoding_iterator<__detail::__transaction::__encode,
			encode_iterator<_Encoding, _Range, _ErrorHandler, _State>, _Encoding, _Range, _ErrorHandler, _State>;

	public:
		using __base_t::__base_t;
	};

	ZTD_TEXT_INLINE_ABI_NAMESPACE_CLOSE_I_
}} // namespace ztd::text

#endif // ZTD_TEXT_ENCODE_ITERATOR_HPP
