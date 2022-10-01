// Doxygen workaround
// Don't ask.
#include <ztd/text/encoding_scheme.hpp>
#include <ztd/text/encode_result.hpp>
#include <ztd/text/decode_result.hpp>
#include <ztd/text/validate_result.hpp>
#include <ztd/text/count_result.hpp>
#include <ztd/text/state.hpp>
#include <ztd/text/is_code_points_replaceable.hpp>
#include <ztd/text/is_code_units_replaceable.hpp>
#include <ztd/text/is_unicode_encoding.hpp>
#include <ztd/text/code_point.hpp>
#include <ztd/text/state.hpp>
#include <ztd/text/default_encoding.hpp>
#include <ztd/text/encode.hpp>
#include <ztd/text/decode.hpp>
#include <ztd/text/validate_decodable_as.hpp>
#include <ztd/text/validate_encodable_as.hpp>
#include <ztd/text/count_as_encoded.hpp>
#include <ztd/text/count_as_decoded.hpp>
#include <ztd/text/detail/transcode_routines.hpp>

#include <ztd/idk/ebco.hpp>
#include <ztd/idk/span.hpp>
#include <ztd/idk/type_traits.hpp>
#include <ztd/idk/tag.hpp>
#include <ztd/idk/text_encoding_id.hpp>
#include <ztd/ranges/range.hpp>
#include <ztd/ranges/adl.hpp>
