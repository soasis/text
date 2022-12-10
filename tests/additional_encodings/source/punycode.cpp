// =============================================================================
//
// ztd.text
// Copyright © 2022 JeanHeyd "ThePhD" Meneide and Shepherd's Oasis, LLC
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
// ============================================================================ //

#include <ztd/text.hpp>

#include <catch2/catch_all.hpp>

TEST_CASE("text/punycode", "test a quick roundtrip using punycode and UTF-8 through UTF-32") {
	const char original[] = u8"😩 OᴘᴇɴSSL ᴅɪᴅ ɴᴏᴛ ᴛᴇsᴛ ᴇᴠᴇɴ ʙᴀsɪᴄ ᴘᴜɴʏᴄᴏᴅᴇ RFC sᴛʀɪɴɢs ꜰᴏʀ ᴛʜᴇɪʀ ɪᴍᴘʟᴇᴍᴇɴᴛᴀᴛɪᴏɴ⁉";
	const char expected_punycode[]
	     = " OSSL   s  s  RFC ss   "
	       "-cqj0qgheba6zgdehhb85bfc31d5m2evf4423k0a7nd6abq3flcampfa17ac5froq64c0a2a7nbcyjnb1b7yp96t0e31nkf95i";
	std::string punycode_encoded = ztd::text::transcode(original, ztd::text::compat_utf8, ztd::text::punycode);
	std::string roundtrip_original
	     = ztd::text::transcode(punycode_encoded, ztd::text::punycode, ztd::text::compat_utf8);
	REQUIRE(original == roundtrip_original);
	REQUIRE(punycode_encoded == expected_punycode);
}
