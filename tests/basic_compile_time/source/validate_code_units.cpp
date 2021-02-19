#include <ztd/text/tests/basic_unicode_strings.hpp>

#include <ztd/text/validate_code_units.hpp>
#include <ztd/text/encoding.hpp>
#include <ztd/text/is_unicode_encoding.hpp>

inline namespace ztd_text_tests_basic_compile_time_validate_code_units {

	template <typename T>
	static void delayed() {
#if ZTD_TEXT_IS_ON(ZTD_TEXT_STD_LIBRARY_IS_CONSTANT_EVALUATED_I_)
		// Basic Source Character Set, using defaults
		// Can only be done if we have constant evaluation inside to swap to the literal encoding
		static_assert(ztd::text::validate_code_units(ztd::text::tests::basic_source_character_set));
		static_assert(ztd::text::validate_code_units(ztd::text::tests::w_basic_source_character_set));
#endif
		static_assert(ztd::text::validate_code_units(ztd::text::tests::u8_basic_source_character_set));
		static_assert(ztd::text::validate_code_units(ztd::text::tests::u16_basic_source_character_set));
		static_assert(ztd::text::validate_code_units(ztd::text::tests::u32_basic_source_character_set));

		// Basic Source Character Set, explicit encoding
		static_assert(
		     ztd::text::validate_code_units(ztd::text::tests::basic_source_character_set, ztd::text::literal()));
		static_assert(ztd::text::validate_code_units(
		     ztd::text::tests::w_basic_source_character_set, ztd::text::wide_literal()));
		static_assert(
		     ztd::text::validate_code_units(ztd::text::tests::u8_basic_source_character_set, ztd::text::utf8()));
		static_assert(
		     ztd::text::validate_code_units(ztd::text::tests::u16_basic_source_character_set, ztd::text::utf16()));
		static_assert(
		     ztd::text::validate_code_units(ztd::text::tests::u32_basic_source_character_set, ztd::text::utf32()));

		// Larger unicode sequences, using defaults
#if ZTD_TEXT_IS_ON(ZTD_TEXT_STD_LIBRARY_IS_CONSTANT_EVALUATED_I_)
		// Basic Source Character Set, using defaults
		// Can only be done if we have constant evaluation inside to swap to the literal encoding
		if constexpr (ztd::text::__detail::__always_true_v<
		                   T> && ztd::text::is_unicode_encoding_v<ztd::text::literal>) {
			static_assert(ztd::text::validate_code_units(ztd::text::tests::unicode_sequence_truth_native_endian));
		}
		if constexpr (ztd::text::__detail::__always_true_v<
		                   T> && ztd::text::is_unicode_encoding_v<ztd::text::wide_literal>) {
			static_assert(ztd::text::validate_code_units(ztd::text::tests::w_unicode_sequence_truth_native_endian));
		}
#endif
		static_assert(ztd::text::validate_code_units(ztd::text::tests::u8_unicode_sequence_truth_native_endian));
		static_assert(ztd::text::validate_code_units(ztd::text::tests::u16_unicode_sequence_truth_native_endian));
		static_assert(ztd::text::validate_code_units(ztd::text::tests::u32_unicode_sequence_truth_native_endian));

		// Larger unicode sequences, explicit encoding
		if constexpr (ztd::text::__detail::__always_true_v<
		                   T> && ztd::text::is_unicode_encoding_v<ztd::text::literal>) {
			static_assert(ztd::text::validate_code_units(
			     ztd::text::tests::unicode_sequence_truth_native_endian, ztd::text::literal()));
		}
		if constexpr (ztd::text::__detail::__always_true_v<
		                   T> && ztd::text::is_unicode_encoding_v<ztd::text::wide_literal>) {
			static_assert(ztd::text::validate_code_units(
			     ztd::text::tests::w_unicode_sequence_truth_native_endian, ztd::text::wide_literal()));
		}
		static_assert(ztd::text::validate_code_units(
		     ztd::text::tests::u8_unicode_sequence_truth_native_endian, ztd::text::utf8()));
		static_assert(ztd::text::validate_code_units(
		     ztd::text::tests::u16_unicode_sequence_truth_native_endian, ztd::text::utf16()));
		static_assert(ztd::text::validate_code_units(
		     ztd::text::tests::u32_unicode_sequence_truth_native_endian, ztd::text::utf32()));
	}

	static void instantiate() {
		delayed<void>();
	}

} // namespace ztd_text_tests_basic_compile_time_validate_code_units
