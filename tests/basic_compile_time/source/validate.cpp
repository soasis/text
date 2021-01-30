#include <ztd/text/tests/basic_unicode_strings.hpp>

#include <ztd/text/validate_code_units.hpp>

static_assert(ztd::text::validate_code_units(ztd::text::tests::u8_ansi_sequence_truth));
static_assert(ztd::text::validate_code_units(ztd::text::tests::u16_ansi_sequence_truth));
static_assert(ztd::text::validate_code_units(ztd::text::tests::u32_ansi_sequence_truth));

static_assert(ztd::text::validate_code_units(ztd::text::tests::u8_unicode_sequence_truth));
static_assert(ztd::text::validate_code_units(ztd::text::tests::u16_unicode_sequence_truth));
static_assert(ztd::text::validate_code_units(ztd::text::tests::u32_unicode_sequence_truth));
