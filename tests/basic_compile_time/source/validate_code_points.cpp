#include <ztd/text/tests/basic_unicode_strings.hpp>

#include <ztd/text/validate_code_points.hpp>
#include <ztd/text/encoding.hpp>
#include <ztd/text/is_unicode_encoding.hpp>

inline namespace ztd_text_tests_basic_compile_time_validate_code_points {

	template <typename T>
	static void delayed() {
		// Basic Source Character Set, using defaults
		static_assert(ztd::text::validate_code_points(ztd::text::tests::u32_basic_source_character_set));

		// Basic Source Character Set, explicit encoding
		static_assert(
		     ztd::text::validate_code_points(ztd::text::tests::u32_basic_source_character_set, ztd::text::literal()));
		static_assert(ztd::text::validate_code_points(
		     ztd::text::tests::u32_basic_source_character_set, ztd::text::wide_literal()));
		static_assert(
		     ztd::text::validate_code_points(ztd::text::tests::u32_basic_source_character_set, ztd::text::utf8()));
		static_assert(
		     ztd::text::validate_code_points(ztd::text::tests::u32_basic_source_character_set, ztd::text::utf16()));
		static_assert(
		     ztd::text::validate_code_points(ztd::text::tests::u32_basic_source_character_set, ztd::text::utf32()));

		// Larger unicode sequences, using defaults
		static_assert(ztd::text::validate_code_points(ztd::text::tests::u32_unicode_sequence_truth_native_endian));

		// Larger unicode sequences, explicit encoding
		if constexpr (ztd::text::__detail::__always_true_v<
		                   T> && ztd::text::is_unicode_encoding_v<ztd::text::literal>) {
			static_assert(ztd::text::validate_code_points(
			     ztd::text::tests::u32_unicode_sequence_truth_native_endian, ztd::text::literal()));
		}
		if constexpr (ztd::text::__detail::__always_true_v<
		                   T> && ztd::text::is_unicode_encoding_v<ztd::text::wide_literal>) {
			static_assert(ztd::text::validate_code_points(
			     ztd::text::tests::u32_unicode_sequence_truth_native_endian, ztd::text::wide_literal()));
		}
		static_assert(ztd::text::validate_code_points(
		     ztd::text::tests::u32_unicode_sequence_truth_native_endian, ztd::text::utf8()));
		static_assert(ztd::text::validate_code_points(
		     ztd::text::tests::u32_unicode_sequence_truth_native_endian, ztd::text::utf16()));
		static_assert(ztd::text::validate_code_points(
		     ztd::text::tests::u32_unicode_sequence_truth_native_endian, ztd::text::utf32()));
	}

	void instantiate() {
		delayed<void>();
	}
} // namespace ztd_text_tests_basic_compile_time_validate_code_points
