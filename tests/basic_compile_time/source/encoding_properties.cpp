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

#include <ztd/text/encoding.hpp>
#include <ztd/text/is_unicode_encoding.hpp>
#include <ztd/text/is_full_range_representable.hpp>

static_assert(ztd::text::is_encode_injective_v<ztd::text::utf8>);
static_assert(ztd::text::is_decode_injective_v<ztd::text::utf8>);
using utf8_es = ztd::text::encoding_scheme<ztd::text::utf8>;
static_assert(ztd::text::is_decode_injective_v<utf8_es>);
static_assert(ztd::text::is_encode_injective_v<utf8_es>);
static_assert(!ztd::text::is_code_units_maybe_replaceable_v<utf8_es>);
static_assert(!ztd::text::is_code_units_replaceable_v<utf8_es>);
static_assert(!ztd::text::is_code_points_replaceable_v<utf8_es>);
static_assert(!ztd::text::is_code_points_maybe_replaceable_v<utf8_es>);

static_assert(ztd::text::is_encode_injective_v<ztd::text::utf16>);
static_assert(ztd::text::is_decode_injective_v<ztd::text::utf16>);
static_assert(!ztd::text::is_code_units_maybe_replaceable_v<ztd::text::utf16>);
static_assert(!ztd::text::is_code_units_replaceable_v<ztd::text::utf16>);
static_assert(!ztd::text::is_code_points_replaceable_v<ztd::text::utf16>);
static_assert(!ztd::text::is_code_points_maybe_replaceable_v<ztd::text::utf16>);
static_assert(ztd::text::is_encode_injective_v<ztd::text::utf16_le>);
static_assert(ztd::text::is_decode_injective_v<ztd::text::utf16_le>);
static_assert(!ztd::text::is_code_units_maybe_replaceable_v<ztd::text::utf16_le>);
static_assert(!ztd::text::is_code_units_replaceable_v<ztd::text::utf16_le>);
static_assert(!ztd::text::is_code_points_replaceable_v<ztd::text::utf16_le>);
static_assert(!ztd::text::is_code_points_maybe_replaceable_v<ztd::text::utf16_le>);
static_assert(ztd::text::is_encode_injective_v<ztd::text::utf16_be>);
static_assert(ztd::text::is_decode_injective_v<ztd::text::utf16_be>);
static_assert(!ztd::text::is_code_units_maybe_replaceable_v<ztd::text::utf16_be>);
static_assert(!ztd::text::is_code_units_replaceable_v<ztd::text::utf16_be>);
static_assert(!ztd::text::is_code_points_replaceable_v<ztd::text::utf16_be>);
static_assert(!ztd::text::is_code_points_maybe_replaceable_v<ztd::text::utf16_be>);
static_assert(ztd::text::is_encode_injective_v<ztd::text::utf16_ne>);
static_assert(ztd::text::is_decode_injective_v<ztd::text::utf16_ne>);
static_assert(!ztd::text::is_code_units_maybe_replaceable_v<ztd::text::utf16_ne>);
static_assert(!ztd::text::is_code_units_replaceable_v<ztd::text::utf16_ne>);
static_assert(!ztd::text::is_code_points_replaceable_v<ztd::text::utf16_ne>);
static_assert(!ztd::text::is_code_points_maybe_replaceable_v<ztd::text::utf16_ne>);

static_assert(ztd::text::is_encode_injective_v<ztd::text::utf32>);
static_assert(ztd::text::is_decode_injective_v<ztd::text::utf32>);
static_assert(!ztd::text::is_code_units_maybe_replaceable_v<ztd::text::utf32>);
static_assert(!ztd::text::is_code_units_replaceable_v<ztd::text::utf32>);
static_assert(!ztd::text::is_code_points_replaceable_v<ztd::text::utf32>);
static_assert(!ztd::text::is_code_points_maybe_replaceable_v<ztd::text::utf32>);
static_assert(ztd::text::is_encode_injective_v<ztd::text::utf32_le>);
static_assert(ztd::text::is_decode_injective_v<ztd::text::utf32_le>);
static_assert(!ztd::text::is_code_units_maybe_replaceable_v<ztd::text::utf32_le>);
static_assert(!ztd::text::is_code_units_replaceable_v<ztd::text::utf32_le>);
static_assert(!ztd::text::is_code_points_replaceable_v<ztd::text::utf32_le>);
static_assert(!ztd::text::is_code_points_maybe_replaceable_v<ztd::text::utf32_le>);
static_assert(ztd::text::is_encode_injective_v<ztd::text::utf32_be>);
static_assert(ztd::text::is_decode_injective_v<ztd::text::utf32_be>);
static_assert(!ztd::text::is_code_units_maybe_replaceable_v<ztd::text::utf32_be>);
static_assert(!ztd::text::is_code_units_replaceable_v<ztd::text::utf32_be>);
static_assert(!ztd::text::is_code_points_replaceable_v<ztd::text::utf32_be>);
static_assert(!ztd::text::is_code_points_maybe_replaceable_v<ztd::text::utf32_be>);
static_assert(ztd::text::is_encode_injective_v<ztd::text::utf32_ne>);
static_assert(ztd::text::is_decode_injective_v<ztd::text::utf32_ne>);
static_assert(!ztd::text::is_code_units_maybe_replaceable_v<ztd::text::utf32_ne>);
static_assert(!ztd::text::is_code_units_replaceable_v<ztd::text::utf32_ne>);
static_assert(!ztd::text::is_code_points_replaceable_v<ztd::text::utf32_ne>);
static_assert(!ztd::text::is_code_points_maybe_replaceable_v<ztd::text::utf32_ne>);

static_assert(ztd::text::is_decode_injective_v<ztd::text::ascii>);
static_assert(!ztd::text::is_encode_injective_v<ztd::text::ascii>);
static_assert(!ztd::text::is_code_units_maybe_replaceable_v<ztd::text::ascii>);
static_assert(ztd::text::is_code_units_replaceable_v<ztd::text::ascii>);
static_assert(!ztd::text::is_code_points_replaceable_v<ztd::text::ascii>);
static_assert(!ztd::text::is_code_points_maybe_replaceable_v<ztd::text::ascii>);

using ascii_es = ztd::text::encoding_scheme<ztd::text::ascii>;
static_assert(ztd::text::is_decode_injective_v<ascii_es>);
static_assert(!ztd::text::is_encode_injective_v<ascii_es>);
static_assert(!ztd::text::is_code_units_maybe_replaceable_v<ascii_es>);
static_assert(ztd::text::is_code_units_replaceable_v<ascii_es>);
static_assert(!ztd::text::is_code_points_replaceable_v<ascii_es>);
static_assert(!ztd::text::is_code_points_maybe_replaceable_v<ascii_es>);

static_assert(ztd::text::is_unicode_encoding_v<ztd::text::literal>
          ? ztd::text::is_encode_injective_v<ztd::text::literal>
          : !ztd::text::is_encode_injective_v<ztd::text::literal>);
static_assert(ztd::text::is_decode_injective_v<ztd::text::literal>);

static_assert(ztd::text::is_unicode_encoding_v<ztd::text::wide_literal>
          ? ztd::text::is_encode_injective_v<ztd::text::wide_literal>
          : !ztd::text::is_encode_injective_v<ztd::text::wide_literal>);
static_assert(ztd::text::is_decode_injective_v<ztd::text::wide_literal>);

static_assert(!ztd::text::is_encode_injective_v<ztd::text::execution>);
static_assert(!ztd::text::is_decode_injective_v<ztd::text::execution>);

static_assert(!ztd::text::is_encode_injective_v<ztd::text::wide_execution>);
static_assert(!ztd::text::is_decode_injective_v<ztd::text::wide_execution>);

static_assert(!ztd::text::is_decode_injective_v<ztd::text::no_encoding>);
static_assert(!ztd::text::is_encode_injective_v<ztd::text::no_encoding>);
static_assert(!ztd::text::is_code_units_maybe_replaceable_v<ztd::text::no_encoding>);
static_assert(!ztd::text::is_code_units_replaceable_v<ztd::text::no_encoding>);
static_assert(!ztd::text::is_code_points_replaceable_v<ztd::text::no_encoding>);
static_assert(!ztd::text::is_code_points_maybe_replaceable_v<ztd::text::no_encoding>);

static_assert(ztd::text::is_decode_injective_v<ztd::text::no_codepoint_encoding>);
static_assert(ztd::text::is_encode_injective_v<ztd::text::no_codepoint_encoding>);
static_assert(!ztd::text::is_code_units_maybe_replaceable_v<ztd::text::no_codepoint_encoding>);
static_assert(!ztd::text::is_code_units_replaceable_v<ztd::text::no_codepoint_encoding>);
static_assert(!ztd::text::is_code_points_replaceable_v<ztd::text::no_codepoint_encoding>);
static_assert(!ztd::text::is_code_points_maybe_replaceable_v<ztd::text::no_codepoint_encoding>);
