// Doxygen workaround
// Don't ask.
#include <ztd/text/is_ignorable_error_handler.hpp>
#include <ztd/text/unicode_code_point.hpp>
#include <ztd/text/decode_result.hpp>
#include <ztd/text/encode_result.hpp>
#include <ztd/text/no_encoding.hpp>
#include <ztd/text/encoding_error.hpp>
#include <ztd/text/iconv_names.hpp>
#include <ztd/text/detail/encoding_name.hpp>

#include <ztd/idk/span.hpp>
#include <ztd/idk/endian.hpp>
#include <ztd/idk/type_traits.hpp>
#include <ztd/idk/c_string_view.hpp>
#include <ztd/ranges/reconstruct.hpp>
#include <ztd/ranges/adl.hpp>
#include <ztd/ranges/algorithm.hpp>

#include <ztd/platform.hpp>

#include <array>
#include <string>
#include <string_view>
#include <climits>
