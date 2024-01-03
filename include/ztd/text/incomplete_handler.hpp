// =============================================================================
//
// ztd.text
// Copyright © JeanHeyd "ThePhD" Meneide and Shepherd's Oasis, LLC
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

#ifndef ZTD_TEXT_INCOMPLETE_HANDLER_HPP
#define ZTD_TEXT_INCOMPLETE_HANDLER_HPP

#include <ztd/text/version.hpp>

#include <ztd/text/decode_result.hpp>
#include <ztd/text/encode_result.hpp>
#include <ztd/text/code_unit.hpp>
#include <ztd/text/code_point.hpp>
#include <ztd/text/pass_handler.hpp>

#include <ztd/idk/type_traits.hpp>
#include <ztd/idk/ebco.hpp>

#include <array>
#include <cstddef>

#include <ztd/prologue.hpp>

namespace ztd { namespace text {
	ZTD_TEXT_INLINE_ABI_NAMESPACE_OPEN_I_

	//////
	/// @brief This handler detects if the error code is an incomplete seqence, and sets the error code to being
	/// okay before returning.
	///
	/// @tparam _Encoding The encoding type which dictates the `code_unit` and `code_point` buffers to store in
	/// the handler to catch unused input from the last parameter of error handler invocations by the encoding.
	/// @tparam _ErrorHandler An error handler to invoke if the encoding error code is @b NOT an incomplete
	/// sequence.
	///
	/// @remarks This type is often useful in conjunction with an accumulation state or buffer, which can be very
	/// handy for I/O (e.g., Networking) operations.
	template <typename _Encoding, typename _ErrorHandler = pass_handler_t>
	class basic_incomplete_handler : private ebco<_ErrorHandler> {
	private:
		using __error_handler_base_t = ebco<_ErrorHandler>;
		using _CodeUnit              = code_unit_t<_Encoding>;
		using _CodePoint             = code_point_t<_Encoding>;

	public:
		//////
		/// @brief The underlying error handler type.

		using error_handler = _ErrorHandler;

		//////
		/// @brief Constructs a ztd::text::basic_incomplete_handler with a default-constructed internal error handler.
		constexpr basic_incomplete_handler() noexcept(
			::std::is_nothrow_default_constructible_v<__error_handler_base_t>)
		: __error_handler_base_t(), _M_code_units(), _M_code_units_size(), _M_code_points(), _M_code_points_size() {
		}

		//////
		/// @brief Constructs a ztd::text::basic_incomplete_handler with the provided internal error handler object.
		///
		/// @param __error_handler The provided error handler object to copy in and use when the error is not an
		/// incomplete error.
		constexpr basic_incomplete_handler(const _ErrorHandler& __error_handler) noexcept(
			::std::is_nothrow_constructible_v<__error_handler_base_t, const _ErrorHandler&>)
		: __error_handler_base_t(__error_handler)
		, _M_code_units()
		, _M_code_units_size()
		, _M_code_points()
		, _M_code_points_size() {
		}

		//////
		/// @brief Constructs a ztd::text::basic_incomplete_handler with the provided internal error handler object.
		///
		/// @param __error_handler The provided error handler object to move in and use when the error is not an
		/// incomplete error.
		constexpr basic_incomplete_handler(_ErrorHandler&& __error_handler) noexcept(
			::std::is_nothrow_constructible_v<__error_handler_base_t, _ErrorHandler&&>)
		: __error_handler_base_t(::std::move(__error_handler))
		, _M_code_units()
		, _M_code_units_size()
		, _M_code_points()
		, _M_code_points_size() {
		}

		//////
		/// @brief Returns the base error handler that is called when a non-incomplete error occurs.

		constexpr _ErrorHandler& base() & noexcept {
			return this->__error_handler_base_t::get_value();
		}

		//////
		/// @brief Returns the base error handler that is called when a non-incomplete error occurs.

		constexpr const _ErrorHandler& base() const& noexcept {
			return this->__error_handler_base_t::get_value();
		}

		//////
		/// @brief Returns the base error handler that is called when a non-incomplete error occurs.

		constexpr _ErrorHandler&& base() && noexcept {
			return this->__error_handler_base_t::get_value();
		}

		//////
		/// @brief Checks if the __result.error_code is ztd::text::encoding_error::incomplete_sequence, it saves the
		/// values from `__progress` and returns. Otherwise, invokes the provided error handler this object was
		/// constructed with.
		///
		/// @param[in] __encoding The Encoding that experienced the error.
		/// @param[in] __result The current state of the encode operation.
		/// @param[in] __input_progress Any code units or code points that were read but not yet used before the
		/// failure occurred. These will be stored in this handler.
		/// @param[in] __output_progress Any code points or code units that have not yet been written before the
		/// failure occurred. These will be stored in this handler.
		template <typename _Result, typename _InputProgress, typename _OutputProgress>
		constexpr auto operator()(const _Encoding& __encoding, _Result __result,
			const _InputProgress& __input_progress,
			const _OutputProgress& __output_progress) const& // cf
			noexcept(::std::is_nothrow_invocable_v<_ErrorHandler, const _Encoding&, _Result&&, const _InputProgress&,
			     const _OutputProgress&>) {
			if (__result.error_code == encoding_error::incomplete_sequence) {
				// it's incomplete and we are okay with that
				if constexpr (is_specialization_of_v<_Result, decode_result>) {
					this->_M_code_units_size = ::ztd::ranges::size(__input_progress);
					ranges::__rng_detail::__copy_n_unsafe(::ztd::ranges::cbegin(__input_progress),
						this->_M_code_units_size, this->_M_code_units.data());
					this->_M_code_points_size = ::ztd::ranges::size(__output_progress);
					ranges::__rng_detail::__copy_n_unsafe(::ztd::ranges::cbegin(__output_progress),
						this->_M_code_points_size, this->_M_code_points.data());
				}
				else {
					this->_M_code_units_size = ::ztd::ranges::size(__output_progress);
					ranges::__rng_detail::__copy_n_unsafe(::ztd::ranges::cbegin(__output_progress),
						this->_M_code_units_size, this->_M_code_units.data());
					this->_M_code_points_size = ::ztd::ranges::size(__input_progress);
					ranges::__rng_detail::__copy_n_unsafe(::ztd::ranges::cbegin(__input_progress),
						this->_M_code_points_size, this->_M_code_points.data());
				}
				return __result;
			}
			return this->get_value()(__encoding, ::std::move(__result), __input_progress, __output_progress);
		}

		//////
		/// @brief Checks if the __result.error_code is ztd::text::encoding_error::incomplete_sequence, it saves the
		/// values from `__progress` and returns. Otherwise, invokes the provided error handler this object was
		/// constructed with.
		///
		/// @param[in] __encoding The Encoding that experienced the error.
		/// @param[in] __result The current state of the encode operation.
		/// @param[in] __input_progress Any code units or code points that were read but not yet used before the
		/// failure occurred. These will be stored in this handler.
		/// @param[in] __output_progress Any code points or code units that have not yet been written before the
		/// failure occurred. These will be stored in this handler.
		template <typename _Result, typename _InputProgress, typename _OutputProgress>
		constexpr auto operator()(const _Encoding& __encoding, _Result __result,
			const _InputProgress& __input_progress,
			const _OutputProgress& __output_progress) & // cf
			noexcept(::std::is_nothrow_invocable_v<_ErrorHandler, const _Encoding&, _Result&&, const _InputProgress&,
			     const _OutputProgress&>) {
			if (__result.error_code == encoding_error::incomplete_sequence) {
				// it's incomplete and we are okay with that
				if constexpr (is_specialization_of_v<_Result, decode_result>) {
					this->_M_code_units_size = ::ztd::ranges::size(__input_progress);
					ranges::__rng_detail::__copy_n_unsafe(::ztd::ranges::cbegin(__input_progress),
						this->_M_code_units_size, this->_M_code_units.data());
					this->_M_code_points_size = ::ztd::ranges::size(__output_progress);
					ranges::__rng_detail::__copy_n_unsafe(::ztd::ranges::cbegin(__output_progress),
						this->_M_code_points_size, this->_M_code_points.data());
				}
				else {
					this->_M_code_units_size = ::ztd::ranges::size(__output_progress);
					ranges::__rng_detail::__copy_n_unsafe(::ztd::ranges::cbegin(__output_progress),
						this->_M_code_units_size, this->_M_code_units.data());
					this->_M_code_points_size = ::ztd::ranges::size(__input_progress);
					ranges::__rng_detail::__copy_n_unsafe(::ztd::ranges::cbegin(__input_progress),
						this->_M_code_points_size, this->_M_code_points.data());
				}
				return __result;
			}
			return this->get_value()(__encoding, ::std::move(__result), __input_progress, __output_progress);
		}

		//////
		/// @brief Checks if the __result.error_code is ztd::text::encoding_error::incomplete_sequence, it saves the
		/// values from `__progress` and returns. Otherwise, invokes the provided error handler this object was
		/// constructed with.
		///
		/// @param[in] __encoding The Encoding that experienced the error.
		/// @param[in] __result The current state of the encode operation.
		/// @param[in] __input_progress Any code units or code points that were read but not yet used before the
		/// failure occurred. These will be stored in this handler.
		/// @param[in] __output_progress Any code points or code units that have not yet been written before the
		/// failure occurred. These will be stored in this handler.
		template <typename _Result, typename _InputProgress, typename _OutputProgress>
		constexpr auto operator()(const _Encoding& __encoding, _Result __result,
			const _InputProgress& __input_progress,
			const _OutputProgress& __output_progress) && // cf
			noexcept(::std::is_nothrow_invocable_v<_ErrorHandler, const _Encoding&, _Result&&, const _InputProgress&,
			     const _OutputProgress&>) {
			if (__result.error_code == encoding_error::incomplete_sequence) {
				// it's incomplete and we are okay with that
				if constexpr (is_specialization_of_v<_Result, decode_result>) {
					this->_M_code_units_size = ::ztd::ranges::size(__input_progress);
					ranges::__rng_detail::__copy_n_unsafe(::ztd::ranges::cbegin(__input_progress),
						this->_M_code_units_size, this->_M_code_units.data());
					this->_M_code_points_size = ::ztd::ranges::size(__output_progress);
					ranges::__rng_detail::__copy_n_unsafe(::ztd::ranges::cbegin(__output_progress),
						this->_M_code_points_size, this->_M_code_points.data());
				}
				else {
					this->_M_code_units_size = ::ztd::ranges::size(__output_progress);
					ranges::__rng_detail::__copy_n_unsafe(::ztd::ranges::cbegin(__output_progress),
						this->_M_code_units_size, this->_M_code_units.data());
					this->_M_code_points_size = ::ztd::ranges::size(__input_progress);
					ranges::__rng_detail::__copy_n_unsafe(::ztd::ranges::cbegin(__input_progress),
						this->_M_code_points_size, this->_M_code_points.data());
				}
				return __result;
			}
			return this->get_value()(__encoding, ::std::move(__result), __input_progress, __output_progress);
		}

		//////
		/// @brief Returns the code units from the last incomplete decode operations.

		::ztd::span<_CodeUnit> code_units() const noexcept {
			return ::ztd::span<_CodeUnit>(this->_M_code_units.data(), this->_M_code_units_size);
		}

		//////
		/// @brief Returns the code points from the last incomplete encode operations.

		::ztd::span<_CodePoint> code_points() const noexcept {
			return ::ztd::span<_CodePoint>(this->_M_code_units.data(), this->_M_code_units_size);
		}

	private:
		mutable ::std::array<_CodeUnit, max_code_units_v<_Encoding>> _M_code_units;
		mutable ::std::size_t _M_code_units_size;
		mutable ::std::array<_CodePoint, max_code_points_v<_Encoding>> _M_code_points;
		mutable ::std::size_t _M_code_points_size;
	};

	ZTD_TEXT_INLINE_ABI_NAMESPACE_CLOSE_I_
}} // namespace ztd::text


#include <ztd/epilogue.hpp>

#endif
