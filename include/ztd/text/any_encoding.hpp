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

#ifndef ZTD_TEXT_ANY_ENCODING_HPP
#define ZTD_TEXT_ANY_ENCODING_HPP

#include <ztd/text/version.hpp>

#include <ztd/text/encoding_scheme.hpp>
#include <ztd/text/encode_result.hpp>
#include <ztd/text/decode_result.hpp>
#include <ztd/text/validate_result.hpp>
#include <ztd/text/count_result.hpp>
#include <ztd/text/state.hpp>
#include <ztd/text/is_code_point_replaceable.hpp>
#include <ztd/text/is_code_unit_replaceable.hpp>
#include <ztd/text/code_point.hpp>
#include <ztd/text/state.hpp>
#include <ztd/text/default_encoding.hpp>
#include <ztd/text/encode.hpp>
#include <ztd/text/decode.hpp>
#include <ztd/text/validate_code_units.hpp>
#include <ztd/text/validate_code_points.hpp>
#include <ztd/text/count_code_units.hpp>
#include <ztd/text/count_code_points.hpp>

#include <ztd/text/detail/transcode_one.hpp>
#include <ztd/text/detail/range.hpp>
#include <ztd/text/detail/ebco.hpp>
#include <ztd/text/detail/span.hpp>

#include <cstdint>
#include <cstddef>
#include <utility>
#include <functional>
#include <memory>
#include <cassert>

namespace ztd { namespace text {

	ZTD_TEXT_INLINE_ABI_NAMESPACE_OPEN_I_

	namespace __detail {
		inline constexpr ::std::size_t __default_max_code_points_any_encoding = 8;
		inline constexpr ::std::size_t __default_max_code_units_any_encoding  = 32;
	} // namespace __detail

	//////
	/// @brief An encoding class which has the given encode output and input, as well as the decode input and output
	/// ranges, provided as fixed types alongside the maximum number of code units put in and pushed out.
	///
	/// @tparam _EncodeCodeUnits The output of @c encode_one and related operations.
	/// @tparam _EncodeCodePoints The input of @c encode_one and related operations.
	/// @tparam _DecodeCodeUnits The output of @c decode_one and related operations.
	/// @tparam _DecodeCodePoints The output of @c decode_one and related operations.
	/// @tparam _MaxCodeUnits The maximum number of code units that can be output through a given operation. Directly
	/// related to the maximum_code_units inline constexpr variable definition.
	/// @tparam _MaxCodePoints The maximum number of code points that can be output through a given operation. Directly
	/// related to the maximum_code_points inline constexpr variable definition.
	///
	/// @remarks This class is generally interacted with by using its derivate class, ztd::text::any_byte_encoding, and
	/// its convenience alias, ztd::text::any_encoding. This class's use is recommended only for power users who have
	/// encoding ranges that cannot be interacted with through @c std::span and therefore need other ways. We are
	/// looking into ways to produce a subrange<any_iterator> as a completely generic range to aid those individuals
	/// who do not want to deal in just @c std::span s.
	//////
	template <typename _EncodeCodeUnits, typename _EncodeCodePoints, typename _DecodeCodeUnits,
		typename _DecodeCodePoints, ::std::size_t _MaxCodeUnits = __detail::__default_max_code_units_any_encoding,
		::std::size_t _MaxCodePoints = __detail::__default_max_code_points_any_encoding>
	class any_encoding_with {
	public:
		class any_encode_state;
		class any_decode_state;

	public:
		//////
		/// @brief The state that can be used between calls to @c decode.
		///
		/// @remarks This is an opaque struct with no members. It follows the "encoding-dependent state" model, which
		/// means it has a constructor that takes an ztd::text::any_encoding_with so it can properly initialize its
		/// state.
		//////
		using decode_state = any_decode_state;
		//////
		/// @brief The state that can be used between calls to @c encode.
		///
		/// @remarks This is an opaque struct with no members. It follows the "encoding-dependent state" model, which
		/// means it has a constructor that takes an ztd::text::any_encoding_with so it can properly initialize its
		/// state.
		//////
		using encode_state = any_encode_state;
		//////
		/// @brief The individual units that result from an encode operation or are used as input to a decode
		/// operation.
		//////
		using code_unit = __detail::__range_value_type_t<_EncodeCodeUnits>;
		//////
		/// @brief The individual units that result from a decode operation or as used as input to an encode
		/// operation.
		//////
		using code_point = __detail::__range_value_type_t<_DecodeCodePoints>;
		//////
		/// @brief Whether or not the encode operation can process all forms of input into code point values.
		///
		/// @remarks This is always going to be false because this is a type-erased encoding; this value is determined
		/// by a runtime decision, which means that the most conservative and truthful answer is selected for this
		/// property.
		//////
		using is_encode_injective = ::std::false_type;
		//////
		/// @brief Whether or not the decode operation can process all forms of input into code point values.
		///
		/// @remarks This is always going to be false because this is a type-erased encoding; this value is determined
		/// by a runtime decision, which means that the most conservative and truthful answer is selected for this
		/// property.
		//////
		using is_decode_injective = ::std::false_type;
		//////
		/// @brief The maximum number of code points a single complete operation of decoding can produce. This is
		/// 1 for all Unicode Transformation Format (UTF) encodings.
		//////
		static inline constexpr ::std::size_t max_code_points = _MaxCodePoints;
		//////
		/// @brief The maximum code units a single complete operation of encoding can produce.
		///
		//////
		static inline constexpr ::std::size_t max_code_units = _MaxCodeUnits;

	private:
		using __encode_result                   = encode_result<_EncodeCodePoints, _EncodeCodeUnits, encode_state>;
		using __decode_result                   = decode_result<_DecodeCodeUnits, _DecodeCodePoints, decode_state>;
		using __count_code_units_result         = count_result<_EncodeCodePoints, encode_state>;
		using __count_code_points_result        = count_result<_DecodeCodeUnits, decode_state>;
		using __validate_code_units_result      = validate_result<_DecodeCodeUnits, decode_state>;
		using __validate_code_points_result     = validate_result<_EncodeCodePoints, encode_state>;
		using __decode_error_handler            = ::std::function<__decode_result(
               const any_encoding_with&, __decode_result, const ::std::span<const code_unit>&)>;
		using __encode_error_handler            = ::std::function<__encode_result(
               const any_encoding_with&, __encode_result, const ::std::span<const code_point>&)>;
		using __count_code_points_error_handler = ::std::function<__decode_result(
			const any_encoding_with&, __count_code_points_result, const ::std::span<const code_unit>&)>;
		using __count_code_units_error_handler  = ::std::function<__encode_result(
               const any_encoding_with&, __count_code_units_result, const ::std::span<const code_point>&)>;

		struct __erased_state {
			virtual ~__erased_state() {
			}
		};

		struct __erased {
			virtual ::std::span<code_point> __replacement_code_points() const noexcept = 0;
			virtual ::std::span<code_unit> __replacement_code_units() const noexcept   = 0;

			virtual __decode_result __decode_one(_DecodeCodeUnits __input, _DecodeCodePoints __output,
				__decode_error_handler __error_handler, decode_state& __state) const = 0;
			virtual __encode_result __encode_one(_EncodeCodePoints __input, _EncodeCodeUnits __output,
				__encode_error_handler __error_handler, encode_state& __state) const = 0;
			virtual __validate_code_units_result __validate_code_units_one(
				_DecodeCodeUnits __input, decode_state& __state) const = 0;
			virtual __validate_code_points_result __validate_code_points_one(
				_EncodeCodePoints __input, encode_state& __state) const                         = 0;
			virtual __count_code_points_result __count_code_points_one(_DecodeCodeUnits __input,
				__count_code_points_error_handler __error_handler, decode_state& __state) const = 0;
			virtual __count_code_units_result __count_code_units_one(_EncodeCodePoints __input,
				__count_code_units_error_handler __error_handler, encode_state& __state) const  = 0;

			virtual __decode_result __decode(_DecodeCodeUnits __input, _DecodeCodePoints __output,
				__decode_error_handler __error_handler, decode_state& __state) const = 0;
			virtual __encode_result __encode(_EncodeCodePoints __input, _EncodeCodeUnits __output,
				__encode_error_handler __error_handler, encode_state& __state) const = 0;
			virtual __validate_code_units_result __validate_code_units(
				_DecodeCodeUnits __input, decode_state& __state) const = 0;
			virtual __validate_code_points_result __validate_code_points(
				_EncodeCodePoints __input, encode_state& __state) const                         = 0;
			virtual __count_code_points_result __count_code_points(_DecodeCodeUnits __input,
				__count_code_points_error_handler __error_handler, decode_state& __state) const = 0;
			virtual __count_code_units_result __count_code_units(_EncodeCodePoints __input,
				__count_code_units_error_handler __error_handler, encode_state& __state) const  = 0;

			virtual std::unique_ptr<__erased_state> __create_encode_state() const = 0;
			virtual std::unique_ptr<__erased_state> __create_decode_state() const = 0;

			virtual ~__erased() {
			}
		};

	public:
		//////
		/// @brief The state for any encoding's decode state.
		///
		//////
		class any_decode_state {
		public:
			//////
			/// @brief Creates a state properly initialized from the stored encoding.
			///
			//////
			any_decode_state(const any_encoding_with& __encoding)
			: _M_state(__encoding._M_storage->__create_decode_state()) {
			}

			__erased_state* _M_get_erased_state() const noexcept {
				return _M_state.get();
			}

		private:
			template <typename>
			friend struct __typed;

			std::unique_ptr<__erased_state> _M_state;
		};

		//////
		/// @brief The state for any encoding's encode state.
		///
		//////
		class any_encode_state {
		public:
			//////
			/// @brief Creates a state properly initialized from the stored encoding.
			///
			//////
			any_encode_state(const any_encoding_with& __encoding)
			: _M_state(__encoding._M_storage->__create_encode_state()) {
			}

			__erased_state* _M_get_erased_state() const noexcept {
				return _M_state.get();
			}

		private:
			/* broken in Clang/GCC */
			template <typename>
			friend struct __typed;

			std::unique_ptr<__erased_state> _M_state;
		};

	private:
		template <typename _State>
		struct __typed_state : public __erased_state, public __detail::__ebco<_State, 0> {
		private:
			using __base_t = __detail::__ebco<_State, 0>;

		public:
			using __base_t::__base_t;
		};

		template <typename _Encoding>
		struct __typed : public __erased, private __detail::__ebco<_Encoding, 0> {
		private:
			static_assert(max_code_points_v<_Encoding> <= max_code_points,
				"encoding must have less than or equal to the number of max potential output code points");
			static_assert(max_code_units_v<_Encoding> <= max_code_units,
				"encoding must have less than or equal to the number of max potential output code units");

			using __real_decode_state = encoding_decode_state_t<_Encoding>;
			using __real_encode_state = encoding_encode_state_t<_Encoding>;
			using __base_t            = __detail::__ebco<_Encoding, 0>;

		public:
			using __base_t::__base_t;

			virtual ::std::span<code_point> __replacement_code_points() const noexcept override {
				if constexpr (__detail::__is_code_points_replaceable_v<_Encoding>) {
					return this->__base_t::get_value().replacement_code_points();
				}
				else {
					return {};
				}
			}

			virtual ::std::span<code_unit> __replacement_code_units() const noexcept override {
				if constexpr (__detail::__is_code_units_replaceable_v<_Encoding>) {
					return this->__base_t::get_value().replacement_code_units();
				}
				else {
					return {};
				}
			}

			// TODO: use proper wrapping handlers,
			// not the pass-through handlers here (so that the functions get called correctly)

			// modifiers: single operations
			virtual __decode_result __decode_one(_DecodeCodeUnits __input, _DecodeCodePoints __output,
				__decode_error_handler __error_handler, decode_state& __state) const override {
				__real_decode_state& __actual_state = this->_M_get_state(__state);
				__detail::__pass_through_handler __pass_handler;
				auto __raw_result = this->__base_t::get_value().decode_one(
					::std::move(__input), ::std::move(__output), __pass_handler, __actual_state);
				return __decode_result(::std::move(__raw_result.input), ::std::move(__raw_result.output), __state,
					__raw_result.error_code, __raw_result.handled_error);
			}

			virtual __encode_result __encode_one(_EncodeCodePoints __input, _EncodeCodeUnits __output,
				__encode_error_handler __error_handler, encode_state& __state) const override {
				__real_encode_state& __actual_state = this->_M_get_state(__state);
				__detail::__pass_through_handler __pass_handler;
				auto __raw_result = this->__base_t::get_value().encode_one(
					::std::move(__input), ::std::move(__output), __pass_handler, __actual_state);
				return __encode_result(::std::move(__raw_result.input), ::std::move(__raw_result.output), __state,
					__raw_result.error_code, __raw_result.handled_error);
			}

			virtual __validate_code_units_result __validate_code_units_one(
				_DecodeCodeUnits __input, decode_state& __state) const override {
				__real_decode_state& __actual_state = this->_M_get_state(__state);
				auto& __encoding                    = this->__base_t::get_value();
				if constexpr (__detail::__is_detected_v<__detail::__detect_object_validate_code_units_one,
					              _Encoding, _DecodeCodePoints, __real_decode_state>) {
					auto __raw_result = __encoding.validate_code_units_one(::std::move(__input), __actual_state);
					return __validate_code_units_result(
						::std::move(__raw_result.input), __raw_result.valid, __state);
				}
				else {
					__real_encode_state __encode_state = make_encode_state(__encoding);
					auto __raw_result                  = __detail::__basic_validate_code_units_one(
                              ::std::move(__input), __encoding, __actual_state, __encode_state);
					return __validate_code_units_result(
						::std::move(__raw_result.input), __raw_result.valid, __state);
				}
			}

			virtual __validate_code_points_result __validate_code_points_one(
				_EncodeCodePoints __input, encode_state& __state) const override {
				__real_encode_state& __actual_state = this->_M_get_state(__state);
				auto& __encoding                    = this->__base_t::get_value();
				if constexpr (__detail::__is_detected_v<__detail::__detect_object_validate_code_points_one,
					              _Encoding, _EncodeCodePoints, __real_encode_state>) {
					auto __raw_result = __encoding.validate_code_points_one(::std::move(__input), __actual_state);
					return __validate_code_points_result(
						::std::move(__raw_result.input), __raw_result.valid, __state);
				}
				else {
					__real_decode_state __decode_state = make_decode_state(__encoding);
					auto __raw_result                  = __detail::__basic_validate_code_points_one(
                              ::std::move(__input), __encoding, __actual_state, __decode_state);
					return __validate_code_points_result(
						::std::move(__raw_result.input), __raw_result.valid, __state);
				}
			}

			virtual __count_code_points_result __count_code_points_one(_DecodeCodeUnits __input,
				__count_code_points_error_handler __error_handler, decode_state& __state) const override {
				__real_decode_state& __actual_state = this->_M_get_state(__state);
				__detail::__pass_through_handler __pass_handler;
				auto& __encoding = this->__base_t::get_value();
				if constexpr (__detail::__is_detected_v<__detail::__detect_object_count_code_points_one, _Encoding,
					              __detail::__pass_through_handler, __real_decode_state>) {
					auto __raw_result
						= __encoding.count_code_points_one(::std::move(__input), __pass_handler, __actual_state);
					return __count_code_points_result(::std::move(__raw_result.input), __raw_result.count, __state,
						__raw_result.error_code, __raw_result.handled_error);
				}
				else {
					auto __raw_result = __detail::__basic_count_code_points_one(
						::std::move(__input), __encoding, __pass_handler, __actual_state);
					return __count_code_points_result(
						::std::move(__raw_result.input), __raw_result.count, __state);
				}
			}

			virtual __count_code_units_result __count_code_units_one(_EncodeCodePoints __input,
				__count_code_units_error_handler __error_handler, encode_state& __state) const override {
				__real_encode_state& __actual_state = this->_M_get_state(__state);
				__detail::__pass_through_handler __pass_handler;
				auto& __encoding = this->__base_t::get_value();
				if constexpr (__detail::__is_detected_v<__detail::__detect_object_count_code_units_one, _Encoding,
					              _EncodeCodePoints, __detail::__pass_through_handler, __real_encode_state>) {
					auto __raw_result
						= __encoding.count_code_units_one(::std::move(__input), __pass_handler, __actual_state);
					return __count_code_units_result(::std::move(__raw_result.input), __raw_result.count, __state,
						__raw_result.error_code, __raw_result.handled_error);
				}
				else {
					auto __raw_result = __detail::__basic_count_code_units_one(
						::std::move(__input), __encoding, __pass_handler, __actual_state);
					return __count_code_units_result(::std::move(__raw_result.input), __raw_result.count, __state,
						__raw_result.error_code, __raw_result.handled_error);
				}
			}

			// modifiers: bulk
			virtual __decode_result __decode(_DecodeCodeUnits __input, _DecodeCodePoints __output,
				__decode_error_handler __error_handler, decode_state& __state) const override {
				__real_decode_state& __actual_state = this->_M_get_state(__state);
				__detail::__pass_through_handler __pass_handler;
				auto __raw_result = ::ztd::text::decode_into(::std::move(__input), this->__base_t::get_value(),
					::std::move(__output), __pass_handler, __actual_state);
				return __decode_result(::std::move(__raw_result.input), ::std::move(__raw_result.output), __state,
					__raw_result.error_code, __raw_result.handled_error);
			}

			virtual __encode_result __encode(_EncodeCodePoints __input, _EncodeCodeUnits __output,
				__encode_error_handler __error_handler, encode_state& __state) const override {
				__real_encode_state& __actual_state = this->_M_get_state(__state);
				__detail::__pass_through_handler __pass_handler;
				auto __raw_result = ::ztd::text::encode_into(::std::move(__input), this->__base_t::get_value(),
					::std::move(__output), __pass_handler, __actual_state);
				return __encode_result(::std::move(__raw_result.input), ::std::move(__raw_result.output), __state,
					__raw_result.error_code, __raw_result.handled_error);
			}

			virtual __validate_code_units_result __validate_code_units(
				_DecodeCodeUnits __input, decode_state& __state) const override {
				__real_decode_state& __actual_state = this->_M_get_state(__state);
				auto __raw_result                   = ::ztd::text::validate_code_units(
                         ::std::move(__input), this->__base_t::get_value(), __actual_state);
				return __validate_code_units_result(::std::move(__raw_result.input), __raw_result.valid, __state);
			}

			virtual __validate_code_points_result __validate_code_points(
				_EncodeCodePoints __input, encode_state& __state) const override {
				__real_encode_state& __actual_state = this->_M_get_state(__state);
				auto __raw_result                   = ::ztd::text::validate_code_points(
                         ::std::move(__input), this->__base_t::get_value(), __actual_state);
				return __validate_code_points_result(::std::move(__raw_result.input), __raw_result.valid, __state);
			}

			virtual __count_code_points_result __count_code_points(_DecodeCodeUnits __input,
				__count_code_points_error_handler __error_handler, decode_state& __state) const override {
				__real_decode_state& __actual_state = this->_M_get_state(__state);
				__detail::__pass_through_handler __pass_handler;
				auto __raw_result = ::ztd::text::count_code_points(
					::std::move(__input), this->__base_t::get_value(), __pass_handler, __actual_state);
				return __count_code_points_result(::std::move(__raw_result.input), __raw_result.count, __state,
					__raw_result.error_code, __raw_result.handled_error);
			}

			virtual __count_code_units_result __count_code_units(_EncodeCodePoints __input,
				__count_code_units_error_handler __error_handler, encode_state& __state) const override {
				__real_encode_state& __actual_state = this->_M_get_state(__state);
				auto& __encoding                    = this->__base_t::get_value();
				__detail::__pass_through_handler __pass_handler;
				auto __raw_result = ::ztd::text::count_code_units(
					::std::move(__input), __encoding, __pass_handler, __actual_state);
				return __count_code_units_result(::std::move(__raw_result.input), __raw_result.count, __state,
					__raw_result.error_code, __raw_result.handled_error);
			}

			virtual std::unique_ptr<__erased_state> __create_encode_state() const {
				auto& __encoding = this->__base_t::get_value();
				return std::make_unique<__typed_state<__real_encode_state>>(make_encode_state(__encoding));
			}

			virtual std::unique_ptr<__erased_state> __create_decode_state() const {
				auto& __encoding = this->__base_t::get_value();
				return std::make_unique<__typed_state<__real_decode_state>>(make_decode_state(__encoding));
			}

		private:
			__real_encode_state& _M_get_state(encode_state& __state) const {
				__erased_state* __erased_ptr = __state._M_get_erased_state();
				__typed_state<__real_encode_state>* __typed_ptr
					= static_cast<__typed_state<__real_encode_state>*>(__erased_ptr);
				return __typed_ptr->get_value();
			}

			__real_decode_state& _M_get_state(decode_state& __state) const {
				__erased_state* __erased_ptr = __state._M_get_erased_state();
				__typed_state<__real_decode_state>* __typed_ptr
					= static_cast<__typed_state<__real_decode_state>*>(__erased_ptr);
				return __typed_ptr->get_value();
			}
		};

		::std::unique_ptr<__erased> _M_storage;

	public:
		//////
		/// @brief Cannot default-construct a ztd::text::any_encoding_with object.
		///
		//////
		any_encoding_with() = delete;

		//////
		/// @brief Constructs a ztd::text::any_encoding_with with the encoding object and any additional arguments.
		///
		/// @tparam _Encoding The Encoding specified by the first argument.
		///
		/// @param[in] __encoding The encoding object that informs the ztd::text::any_encoding_with what encoding
		/// object to store.
		/// @param[in] __args Any additional arguments used to construct the encoding in the erased storage.
		//////
		template <typename _Encoding, typename... _Args,
			::std::enable_if_t<
			     !::std::is_same_v<_Encoding,
			          any_encoding_with> && !__detail::__is_specialization_of_v<__detail::__remove_cvref_t<_Encoding>, ::std::in_place_type_t>>* = nullptr>
		any_encoding_with(_Encoding&& __encoding, _Args&&... __args)
		: any_encoding_with(::std::in_place_type<__detail::__remove_cvref_t<_Encoding>>,
			::std::forward<_Encoding>(__encoding), std::forward<_Args>(__args)...) {
		}

		//////
		/// @brief Constructs a ztd::text::any_encoding_with with the encoding type specified in the @p __tag
		/// argument.
		///
		/// @tparam _Encoding The Encoding specified by the @p __tag argument.
		///
		/// @param[in] __tag The type marker that informs the ztd::text::any_encoding_with what encoding object to
		/// store.
		/// @param[in] __args Any additional arguments used to construct the encoding in the erased storage.
		//////
		template <typename _Encoding, typename... _Args>
		any_encoding_with(::std::in_place_type_t<_Encoding> __tag, _Args&&... __args)
		: _M_storage(::std::make_unique<__typed<_Encoding>>(::std::forward<_Args>(__args)...)) {
			(void)__tag;
		}

		//////
		/// @brief Cannot copy-construct a ztd::text::any_encoding_with object.
		///
		//////
		any_encoding_with(const any_encoding_with&) = delete;

		//////
		/// @brief Cannot copy-assign a ztd::text::any_encoding_with object.
		///
		//////
		any_encoding_with& operator=(const any_encoding_with&) = delete;

		//////
		/// @brief Move-constructs a ztd::text::any_encoding_with from the provided r-value reference.
		///
		/// @remarks This leaves the passed-in r-value reference without an encoding object. Calling any function on a
		/// moved-fron ztd::text::any_encoding_with, except for destruction, is a violation and invokes Undefined
		/// Behavior (generally, a crash).
		//////
		any_encoding_with(any_encoding_with&&) = default;

		//////
		/// @brief Move-assigns a ztd::text::any_encoding_with from the provided r-value reference.
		///
		/// @remarks This leaves the passed-in r-value reference without an encoding object. Calling any function on a
		/// moved-fron ztd::text::any_encoding_with, except for destruction, is a violation and invokes Undefined
		/// Behavior (generally, a crash).
		//////
		any_encoding_with& operator=(any_encoding_with&&) = default;

		//////
		/// @brief Retrieves the replacement code points for when conversions fail and ztd::text::replacement_handler
		/// (or equivalent) needs to make a substitution.
		///
		/// @return A @c std::span of code units. The return value is empty if the stored encoding does not have a
		/// valid @c replacement_code_points static variable.
		//////
		::std::span<code_point> replacement_code_points() const noexcept {
			return this->_M_storage->__replacement_code_points();
		}

		//////
		/// @brief Retrieves the replacement code units for when conversions fail and ztd::text::replacement_handler
		/// (or equivalent) needs to make a substitution.
		///
		/// @return A @c std::span of code units. The return value is empty if the stored encoding does not have a
		/// valid @c replacement_code_units variable.
		//////
		::std::span<code_unit> replacement_code_units() const noexcept {
			return this->_M_storage->__replacement_code_units();
		}

		//////
		/// @brief Decodes a single complete unit of information as code points and produces a result with the
		/// input and output ranges moved past what was successfully read and written; or, produces an error and
		/// returns the input and output ranges untouched.
		///
		/// @param[in] __input The input view to read code uunits from.
		/// @param[in] __output The output view to write code points into.
		/// @param[in] __error_handler The error handler to invoke if encoding fails.
		/// @param[in, out] __state The necessary state information. For this encoding, the state is empty and means
		/// very little.
		///
		/// @returns A ztd::text::decode_result object that contains the reconstructed input range,
		/// reconstructed output range, error handler, and a reference to the passed-in state.
		///
		/// @remarks To the best ability of the implementation, the iterators will be returned untouched (e.g.,
		/// the input models at least a view and a forward_range). If it is not possible, returned ranges may be
		/// incremented even if an error occurs due to the semantics of any view that models an input_range.
		//////
		__decode_result decode_one(_DecodeCodeUnits __input, _DecodeCodePoints __output,
			__decode_error_handler __error_handler, decode_state& __state) const {
			return this->_M_storage->__decode_one(
				::std::move(__input), ::std::move(__output), ::std::move(__error_handler), __state);
		}

		//////
		/// @brief Encodes a single complete unit of information as code units and produces a result with the
		/// input and output ranges moved past what was successfully read and written; or, produces an error and
		/// returns the input and output ranges untouched.
		///
		/// @param[in] __input The input view to read code points from.
		/// @param[in] __output The output view to write code units into.
		/// @param[in] __error_handler The error handler to invoke if encoding fails.
		/// @param[in, out] __state The necessary state information. For this encoding, the state is empty and means
		/// very little.
		///
		/// @returns A ztd::text::encode_result object that contains the reconstructed input range,
		/// reconstructed output range, error handler, and a reference to the passed-in state.
		///
		/// @remarks To the best ability of the implementation, the iterators will be returned untouched (e.g.,
		/// the input models at least a view and a forward_range). If it is not possible, returned ranges may be
		/// incremented even if an error occurs due to the semantics of any view that models an input_range.
		//////
		__encode_result encode_one(_EncodeCodePoints __input, _EncodeCodeUnits __output,
			__encode_error_handler __error_handler, encode_state& __state) const {
			return this->_M_storage->__encode_one(
				::std::move(__input), ::std::move(__output), ::std::move(__error_handler), __state);
		}

	private:
		__validate_code_units_result __validate_code_units_one(
			_DecodeCodeUnits __input, decode_state& __state) const {
			return this->_M_storage->__validate_code_units_one(::std::move(__input), __state);
		}

		__validate_code_points_result __validate_code_points_one(
			_EncodeCodePoints __input, encode_state& __state) const {
			return this->_M_storage->__validate_code_points_one(::std::move(__input), __state);
		}

		__count_code_points_result __count_code_points_one(_DecodeCodeUnits __input,
			__count_code_points_error_handler __error_handler, decode_state& __state) const {
			return this->_M_storage->__count_code_points_one(
				::std::move(__input), ::std::move(__error_handler), __state);
		}

		__count_code_units_result __count_code_units_one(
			_EncodeCodePoints __input, __encode_error_handler __error_handler, encode_state& __state) const {
			return this->_M_storage->__count_code_units_one(
				::std::move(__input), ::std::move(__error_handler), __state);
		}

		__decode_result __decode(_DecodeCodeUnits __input, _DecodeCodePoints __output,
			__decode_error_handler __error_handler, decode_state& __state) const {
			return this->_M_storage->__decode(
				::std::move(__input), ::std::move(__output), ::std::move(__error_handler), __state);
		}

		__encode_result __encode(_EncodeCodePoints __input, _EncodeCodeUnits __output,
			__encode_error_handler __error_handler, encode_state& __state) const {
			return this->_M_storage->__encode(
				::std::move(__input), ::std::move(__output), ::std::move(__error_handler), __state);
		}

		__validate_code_units_result __validate_code_units(
			_DecodeCodeUnits __input, __decode_error_handler __error_handler, decode_state& __state) const {
			return this->_M_storage->__validate_code_units(
				::std::move(__input), ::std::move(__error_handler), __state);
		}

		__validate_code_points_result __validate_code_points(
			_EncodeCodePoints __input, __encode_error_handler __error_handler, encode_state& __state) const {
			return this->_M_storage->__validate_code_points(
				::std::move(__input), ::std::move(__error_handler), __state);
		}

		__count_code_points_result __count_code_points(
			_DecodeCodeUnits __input, __decode_error_handler __error_handler, decode_state& __state) const {
			return this->_M_storage->__count_code_points(
				::std::move(__input), ::std::move(__error_handler), __state);
		}

		__count_code_units_result __count_code_units(
			_EncodeCodePoints __input, __encode_error_handler __error_handler, encode_state& __state) const {
			return this->_M_storage->__count_code_units(::std::move(__input), ::std::move(__error_handler), __state);
		}


		//////
		/// @internal
		///
		/// @brief Extension point hooks for the implementation-side only.
		//////
		template <typename _Output, typename _ErrorHandler, typename _State>
		constexpr friend auto __text_decode(_DecodeCodeUnits __input,
			__detail::__type_identity_t<const any_encoding_with&> __encoding, _Output&& __output,
			_ErrorHandler&& __error_handler, _State& __state) {
			return __encoding.__decode(::std::move(__input), ::std::forward<_Output>(__output),
				::std::forward<_ErrorHandler>(__error_handler), __state);
		}

		//////
		/// @internal
		///
		/// @brief Extension point hooks for the implementation-side only.
		//////
		template <typename _Output, typename _ErrorHandler, typename _State>
		constexpr friend auto __text_encode(_EncodeCodePoints __input,
			__detail::__type_identity_t<const any_encoding_with&> __encoding, _Output&& __output,
			_ErrorHandler&& __error_handler, _State& __state) {
			return __encoding.__encode(::std::move(__input), ::std::forward<_Output>(__output),
				::std::forward<_ErrorHandler>(__error_handler), __state);
		}

		//////
		/// @internal
		///
		/// @brief Extension point hooks for the implementation-side only.
		//////
		template <typename _EncodeState>
		constexpr friend auto __text_validate_code_points_one(_DecodeCodeUnits __input,
			__detail::__type_identity_t<const any_encoding_with&> __encoding, _EncodeState& __state) {
			return __encoding.__validate_code_points_one(::std::move(__input), __state);
		}

		//////
		/// @internal
		///
		/// @brief Extension point hooks for the implementation-side only.
		//////
		template <typename _DecodeState>
		constexpr friend auto __text_validate_code_units_one(_EncodeCodePoints __input,
			__detail::__type_identity_t<const any_encoding_with&> __encoding, _DecodeState& __state) {
			return __encoding.__validate_code_units_one(::std::move(__input), __state);
		}

		//////
		/// @internal
		///
		/// @brief Extension point hooks for the implementation-side only.
		//////
		template <typename _EncodeState>
		constexpr friend auto __text_validate_code_points(_DecodeCodeUnits __input,
			__detail::__type_identity_t<const any_encoding_with&> __encoding, _EncodeState& __state) {
			return __encoding.__validate_code_points(::std::move(__input), __state);
		}

		//////
		/// @internal
		///
		/// @brief Extension point hooks for the implementation-side only.
		//////
		template <typename _Input, typename _DecodeState>
		constexpr friend auto __text_validate_code_units(_EncodeCodePoints __input,
			__detail::__type_identity_t<const any_encoding_with&> __encoding, _DecodeState& __state) {
			return __encoding.__validate_code_units(::std::move(__input), __state);
		}

		template <typename _ErrorHandler, typename _State>
		constexpr friend auto __text_count_code_points_one(_DecodeCodeUnits __input,
			__detail::__type_identity_t<const any_encoding_with&> __encoding, _ErrorHandler&& __error_handler,
			_State& __state) {
			return __encoding.__count_code_points_one(
				::std::move(__input), ::std::forward<_ErrorHandler>(__error_handler), __state);
		}

		//////
		/// @internal
		///
		/// @brief Extension point hooks for the implementation-side only.
		//////
		template <typename _ErrorHandler, typename _State>
		constexpr friend auto __text_count_code_units_one(_EncodeCodePoints __input,
			__detail::__type_identity_t<const any_encoding_with&> __encoding, _ErrorHandler&& __error_handler,
			_State& __state) {
			return __encoding.__count_code_units_one(
				::std::move(__input), ::std::forward<_ErrorHandler>(__error_handler), __state);
		}

		//////
		/// @internal
		///
		/// @brief Extension point hooks for the implementation-side only.
		//////
		template <typename _ErrorHandler, typename _State>
		constexpr friend auto __text_count_code_points(_DecodeCodeUnits __input,
			__detail::__type_identity_t<const any_encoding_with&> __encoding, _ErrorHandler&& __error_handler,
			_State& __state) {
			return __encoding.__count_code_points(::std::move(__input), ::std::move(__error_handler), __state);
		}

		//////
		/// @internal
		///
		/// @brief Extension point hooks for the implementation-side only.
		//////
		template <typename _ErrorHandler, typename _State>
		constexpr friend auto __text_count_code_units(_EncodeCodePoints __input,
			__detail::__type_identity_t<const any_encoding_with&> __encoding, _ErrorHandler&& __error_handler,
			_State& __state) {
			return __encoding.__count_code_units(
				::std::move(__input), ::std::forward<_ErrorHandler>(__error_handler), __state);
		}
	};

	//////
	/// @brief A type-erased encoding that uses the specified code unit, code point, and input/output ranges for the
	/// various operations.
	//////
	template <typename _EncodeCodeUnit, typename _EncodeCodePoint = const unicode_code_point,
		typename _DecodeCodeUnit     = ::std::add_const_t<_EncodeCodeUnit>,
		typename _DecodeCodePoint    = ::std::remove_const_t<_EncodeCodePoint>,
		::std::size_t _MaxCodeUnits  = __detail::__default_max_code_units_any_encoding,
		::std::size_t _MaxCodePoints = __detail::__default_max_code_points_any_encoding>
	using any_encoding_of = any_encoding_with<::std::span<_EncodeCodeUnit>, ::std::span<_EncodeCodePoint>,
		::std::span<_DecodeCodeUnit>, ::std::span<_DecodeCodePoint>, _MaxCodeUnits, _MaxCodePoints>;

	//////
	/// @brief An encoding type that wraps up other encodings to specifically traffic in the given @p _Byte type
	/// provided, which is typically set to @c "std::byte".
	///
	/// @tparam _Byte The byte type to use. Typically, this is either @c "unsigned char" or @c "std::byte".
	///
	/// @remarks This type traffics solely in @c "std::span"s, which for most people is fine. Others may want to
	/// interface with different iterator types (e.g., from a custom Rope implementation or other). For those, one must
	/// first create ranges that can operate with those iterators, then use them themselves. (It's not an ideal process
	/// at the moment, and we are looking to make this experience better.) It is recommended to use the provided
	/// ztd::text::any_encoding type definition instead of accessing this directly, unless you have a reason for using
	/// a different byte type (e.g., interfacing with legacy APIs).
	//////
	template <typename _Byte>
	class any_byte_encoding : public any_encoding_of<_Byte> {
	private:
		using __base_t = any_encoding_of<::std::byte>;

	public:
		//////
		/// @brief Cannot default-construct a ztd::text::any_byte_encoding object.
		///
		//////
		any_byte_encoding() = delete;

		//////
		/// @brief Constructs a ztd::text::any_byte_encoding with the encoding object and any additional arguments.
		///
		/// @param[in] __encoding The encoding object that informs the ztd::text::any_byte_encoding what encoding
		/// object to store.
		/// @param[in] __args Any additional arguments used to construct the encoding in the erased storage.
		///
		/// @remarks If the provided encoding does not have a byte code_unit type, it is wrapped in an
		/// ztd::text::encoding_scheme first.
		//////
		template <typename _Encoding, typename... _Args,
			::std::enable_if_t<
			     !::std::is_same_v<_Encoding,
			          any_byte_encoding> && !::std::is_same_v<_Encoding, __base_t> && !__detail::__is_specialization_of_v<__detail::__remove_cvref_t<_Encoding>, ::std::in_place_type_t>>* = nullptr>
		any_byte_encoding(_Encoding&& __encoding, _Args&&... __args)
		: any_byte_encoding(::std::in_place_type<__detail::__remove_cvref_t<_Encoding>>,
			::std::forward<_Encoding>(__encoding), std::forward<_Args>(__args)...) {
		}

		//////
		/// @brief Constructs a ztd::text::any_byte_encoding with the encoding object and any additional arguments.
		///
		/// @tparam _Encoding The Encoding specified by the tag argument.
		///
		/// @param[in] __args Any additional arguments used to construct the encoding in the erased storage.
		///
		/// @remarks If the provided encoding does not have a byte code_unit type, it is wrapped in an
		/// ztd::text::encoding_scheme first.
		//////
		template <typename _Encoding, typename... _Args,
			::std::enable_if_t<
			     !::std::is_same_v<_Byte, encoding_code_unit_t<__detail::__remove_cvref_t<_Encoding>>>>* = nullptr>
		any_byte_encoding(::std::in_place_type_t<_Encoding>, _Args&&... __args)
		: __base_t(
			::std::in_place_type_t<encoding_scheme<__detail::__remove_cvref_t<_Encoding>, endian::native, _Byte>> {},
			::std::forward<_Args>(__args)...) {
		}

		//////
		/// @brief Constructs a ztd::text::any_byte_encoding with the encoding object and any additional arguments.
		///
		/// @tparam _Encoding The Encoding specified by the tag argument.
		///
		/// @param[in] __tag A tag containing the encoding type.
		/// @param[in] __args Any additional arguments used to construct the encoding in the erased storage.
		///
		/// @remarks If the provided encoding does not have a byte code_unit type, it is wrapped in an
		/// ztd::text::encoding_scheme first.
		//////
		template <typename _Encoding, typename... _Args,
			::std::enable_if_t<
			     ::std::is_same_v<_Byte, encoding_code_unit_t<__detail::__remove_cvref_t<_Encoding>>>>* = nullptr>
		any_byte_encoding(::std::in_place_type_t<_Encoding> __tag, _Args&&... __args)
		: __base_t(::std::move(__tag), ::std::forward<_Args>(__args)...) {
		}

		//////
		/// @brief Cannot copy-construct a ztd::text::any_byte_encoding object.
		///
		//////
		any_byte_encoding(const any_byte_encoding&) = delete;

		//////
		/// @brief Cannot copy-assign a ztd::text::any_byte_encoding object.
		///
		//////
		any_byte_encoding& operator=(const any_byte_encoding&) = delete;

		//////
		/// @brief Move-constructs a ztd::text::any_byte_encoding from the provided r-value reference.
		///
		/// @remarks This leaves the passed-in r-value reference without an encoding object. Calling any function on a
		/// moved-fron ztd::text::any_byte_encoding, except for destruction, is a violation and invokes Undefined
		/// Behavior (generally, a crash).
		//////
		any_byte_encoding(any_byte_encoding&&) = default;

		//////
		/// @brief Move-assigns a ztd::text::any_byte_encoding from the provided r-value reference.
		///
		/// @remarks This leaves the passed-in r-value reference without an encoding object. Calling any function on a
		/// moved-fron ztd::text::any_byte_encoding, except for destruction, is a violation and invokes Undefined
		/// Behavior (generally, a crash).
		//////
		any_byte_encoding& operator=(any_byte_encoding&&) = default;
	};

	//////
	/// @brief The canonical erased encoding type which uses a @c std::byte as its code unit type and an @c
	/// unicode_code_point as its code point type, with spans for input and output operations.
	///
	/// @remarks If the input encoding does not match @c std::byte, it will be first wrapped in a
	/// ztd::text::encoding_scheme first.
	//////
	using any_encoding = any_byte_encoding<::std::byte>;

	ZTD_TEXT_INLINE_ABI_NAMESPACE_CLOSE_I_
}} // namespace ztd::text

#endif // ZTD_TEXT_ANY_ENCODING_HPP
