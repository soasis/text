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

#ifndef ZTD_TEXT_TESTS_BASIC_UNICODE_STRINGS_HPP
#define ZTD_TEXT_TESTS_BASIC_UNICODE_STRINGS_HPP

#include <ztd/text/c_string_view.hpp>
#include <ztd/text/detail/span.hpp>

namespace ztd { namespace text { namespace tests {

	template <typename Container>
	inline std::span<const std::byte> as_bytes(Container& c) {
		return std::as_bytes(std::span<std::remove_pointer_t<decltype(c.data())>>(c.data(), c.size()));
	}

	template <typename Container>
	inline std::span<const std::byte> as_writeable_bytes(Container& c) {
		return std::as_writable_bytes(std::span<std::remove_pointer_t<decltype(c.data())>>(c.data(), c.size()));
	}

	inline constexpr const std::basic_string_view<char32_t> u32_unicode_sequence_truth
		= U"z♥䕦🍷私はガラスを食べられます。それは私を傷つけません。𐌼𐌰𐌲 𐌲𐌻𐌴𐍃 𐌹̈𐍄𐌰𐌽, 𐌽𐌹 𐌼𐌹𐍃 𐍅𐌿 𐌽𐌳𐌰𐌽 𐌱𐍂𐌹𐌲𐌲𐌹𐌸.யாமறிந்த "
		  U"மொழிகளிலே தமிழ்மொழி போல் இனிதாவது எங்கும் காணோம்,На берегу пустынных волн € أنا قادر على أكل الزجاج و هذا لا "
		  U"يؤلمني."
		  U"On the other hand, we denounce with righteous indignation and dislike men who are so beguiled and "
		  U"demoralized by the charms of pleasure of the moment, so blinded by desire, that they cannot foresee the "
		  U"pain and trouble that are bound to ensue; and equal blame belongs to those who fail in their duty "
		  U"through weakness of will, which is the same as saying through shrinking from toil and pain. These cases "
		  U"are perfectly simple and easy to distinguish. In a free hour, when our power of choice is untrammelled "
		  U"and when nothing prevents our being able to do what we like best, every pleasure is to be welcomed and "
		  U"every pain avoided. But in certain circumstances and owing to the claims of duty or the obligations of "
		  U"business it will frequently occur that pleasures have to be repudiated and annoyances accepted. The wise "
		  U"man therefore always holds in these matters to this principle of selection: he rejects pleasures to "
		  U"secure other greater pleasures, or else he endures pains to avoid worse pains."
		  U"오늘이 오늘이소서 매일이 오늘이소서 저물지도 새지도 말으시고 새려면 늘 언제나 오늘이소서.";
	inline constexpr const std::basic_string_view<char16_t> u16_unicode_sequence_truth
		= u"z♥䕦🍷私はガラスを食べられます。それは私を傷つけません。𐌼𐌰𐌲 𐌲𐌻𐌴𐍃 𐌹̈𐍄𐌰𐌽, 𐌽𐌹 𐌼𐌹𐍃 𐍅𐌿 𐌽𐌳𐌰𐌽 𐌱𐍂𐌹𐌲𐌲𐌹𐌸.யாமறிந்த "
		  u"மொழிகளிலே தமிழ்மொழி போல் இனிதாவது எங்கும் காணோம்,На берегу пустынных волн € أنا قادر على أكل الزجاج و هذا لا "
		  u"يؤلمني."
		  u"On the other hand, we denounce with righteous indignation and dislike men who are so beguiled and "
		  u"demoralized by the charms of pleasure of the moment, so blinded by desire, that they cannot foresee the "
		  u"pain and trouble that are bound to ensue; and equal blame belongs to those who fail in their duty "
		  u"through weakness of will, which is the same as saying through shrinking from toil and pain. These cases "
		  u"are perfectly simple and easy to distinguish. In a free hour, when our power of choice is untrammelled "
		  u"and when nothing prevents our being able to do what we like best, every pleasure is to be welcomed and "
		  u"every pain avoided. But in certain circumstances and owing to the claims of duty or the obligations of "
		  u"business it will frequently occur that pleasures have to be repudiated and annoyances accepted. The wise "
		  u"man therefore always holds in these matters to this principle of selection: he rejects pleasures to "
		  u"secure other greater pleasures, or else he endures pains to avoid worse pains."
		  u"오늘이 오늘이소서 매일이 오늘이소서 저물지도 새지도 말으시고 새려면 늘 언제나 오늘이소서.";
	inline constexpr const std::basic_string_view<char8_t> u8_unicode_sequence_truth
		= u8"z♥䕦🍷私はガラスを食べられます。それは私を傷つけません。𐌼𐌰𐌲 𐌲𐌻𐌴𐍃 𐌹̈𐍄𐌰𐌽, 𐌽𐌹 𐌼𐌹𐍃 𐍅𐌿 𐌽𐌳𐌰𐌽 𐌱𐍂𐌹𐌲𐌲𐌹𐌸.யாமறிந்த "
		  u8"மொழிகளிலே தமிழ்மொழி போல் இனிதாவது எங்கும் காணோம்,На берегу пустынных волн € أنا قادر على أكل الزجاج و هذا لا "
		  u8"يؤلمني."
		  u8"On the other hand, we denounce with righteous indignation and dislike men who are so beguiled and "
		  u8"demoralized by the charms of pleasure of the moment, so blinded by desire, that they cannot foresee the "
		  u8"pain and trouble that are bound to ensue; and equal blame belongs to those who fail in their duty "
		  u8"through weakness of will, which is the same as saying through shrinking from toil and pain. These cases "
		  u8"are perfectly simple and easy to distinguish. In a free hour, when our power of choice is untrammelled "
		  u8"and when nothing prevents our being able to do what we like best, every pleasure is to be welcomed and "
		  u8"every pain avoided. But in certain circumstances and owing to the claims of duty or the obligations of "
		  u8"business it will frequently occur that pleasures have to be repudiated and annoyances accepted. The "
		  u8"wise "
		  u8"man therefore always holds in these matters to this principle of selection: he rejects pleasures to "
		  u8"secure other greater pleasures, or else he endures pains to avoid worse pains."
		  u8"오늘이 오늘이소서 매일이 오늘이소서 저물지도 새지도 말으시고 새려면 늘 언제나 오늘이소서.";
	inline constexpr const std::basic_string_view<wchar_t> w_unicode_sequence_truth
		= L"z♥䕦🍷私はガラスを食べられます。それは私を傷つけません。𐌼𐌰𐌲 𐌲𐌻𐌴𐍃 𐌹̈𐍄𐌰𐌽, 𐌽𐌹 𐌼𐌹𐍃 𐍅𐌿 𐌽𐌳𐌰𐌽 𐌱𐍂𐌹𐌲𐌲𐌹𐌸.யாமறிந்த "
		  L"மொழிகளிலே தமிழ்மொழி போல் இனிதாவது எங்கும் காணோம்,На берегу пустынных волн € أنا قادر على أكل الزجاج و هذا لا "
		  L"يؤلمني."
		  L"On the other hand, we denounce with righteous indignation and dislike men who are so beguiled and "
		  L"demoralized by the charms of pleasure of the moment, so blinded by desire, that they cannot foresee the "
		  L"pain and trouble that are bound to ensue; and equal blame belongs to those who fail in their duty "
		  L"through weakness of will, which is the same as saying through shrinking from toil and pain. These cases "
		  L"are perfectly simple and easy to distinguish. In a free hour, when our power of choice is untrammelled "
		  L"and when nothing prevents our being able to do what we like best, every pleasure is to be welcomed and "
		  L"every pain avoided. But in certain circumstances and owing to the claims of duty or the obligations of "
		  L"business it will frequently occur that pleasures have to be repudiated and annoyances accepted. The wise "
		  L"man therefore always holds in these matters to this principle of selection: he rejects pleasures to "
		  L"secure other greater pleasures, or else he endures pains to avoid worse pains."
		  L"오늘이 오늘이소서 매일이 오늘이소서 저물지도 새지도 말으시고 새려면 늘 언제나 오늘이소서.";
	inline constexpr const std::basic_string_view<char> unicode_sequence_truth
		= "z♥䕦🍷私はガラスを食べられます。それは私を傷つけません。𐌼𐌰𐌲 𐌲𐌻𐌴𐍃 𐌹̈𐍄𐌰𐌽, 𐌽𐌹 𐌼𐌹𐍃 𐍅𐌿 𐌽𐌳𐌰𐌽 𐌱𐍂𐌹𐌲𐌲𐌹𐌸.யாமறிந்த "
		  "மொழிகளிலே தமிழ்மொழி போல் இனிதாவது எங்கும் காணோம்,На берегу пустынных волн € أنا قادر على أكل الزجاج و هذا لا "
		  "يؤلمني."
		  "On the other hand, we denounce with righteous indignation and dislike men who are so beguiled and "
		  "demoralized by the charms of pleasure of the moment, so blinded by desire, that they cannot foresee the "
		  "pain and trouble that are bound to ensue; and equal blame belongs to those who fail in their duty "
		  "through weakness of will, which is the same as saying through shrinking from toil and pain. These cases "
		  "are perfectly simple and easy to distinguish. In a free hour, when our power of choice is untrammelled "
		  "and when nothing prevents our being able to do what we like best, every pleasure is to be welcomed and "
		  "every pain avoided. But in certain circumstances and owing to the claims of duty or the obligations of "
		  "business it will frequently occur that pleasures have to be repudiated and annoyances accepted. The wise "
		  "man therefore always holds in these matters to this principle of selection: he rejects pleasures to "
		  "secure other greater pleasures, or else he endures pains to avoid worse pains."
		  "오늘이 오늘이소서 매일이 오늘이소서 저물지도 새지도 말으시고 새려면 늘 언제나 오늘이소서.";

	inline constexpr const char32_t u32text_storage[] = { 0x00006b66, 0x00004fa6, 0x00005baa, 0x00007ae0, 0x00007b2c,
		0x00000039, 0x00006761, 0x00002014, 0x00002014, 0x00005e94, 0x00005c55, 0x00007fc5, 0x00004e16, 0x0000754c,
		0x00003002, 0x00004e0d, 0x00005206, 0x00004eba, 0x000079cd, 0x00003001, 0x000056fd, 0x00007c4d, 0x00005171,
		0x0000540c, 0x00006218, 0x00006597, 0x00003002, 0x0000007a, 0x00002665, 0x00004566, 0x0000fffd, 0x0000fffd,
		0x000079c1, 0x0000306f, 0x000030ac, 0x000030e9, 0x000030b9, 0x00003092, 0x000098df, 0x00003079, 0x00003089,
		0x0000308c, 0x0000307e, 0x00003059, 0x00003002, 0x0000305d, 0x0000308c, 0x0000306f, 0x000079c1, 0x00003092,
		0x000050b7, 0x00003064, 0x00003051, 0x0000307e, 0x0000305b, 0x00003093, 0x00003002, 0x00000baf, 0x00000bbe,
		0x00000bae, 0x00000bb1, 0x00000bbf, 0x00000ba8, 0x00000bcd, 0x00000ba4, 0x00000020, 0x00000bae, 0x00000bca,
		0x00000bb4, 0x00000bbf, 0x00000b95, 0x00000bb3, 0x00000bbf, 0x00000bb2, 0x00000bc7, 0x00000020, 0x00000ba4,
		0x00000bae, 0x00000bbf, 0x00000bb4, 0x00000bcd, 0x00000bae, 0x00000bca, 0x00000bb4, 0x00000bbf, 0x00000020,
		0x00000baa, 0x00000bcb, 0x00000bb2, 0x00000bcd, 0x00000020, 0x00000b87, 0x00000ba9, 0x00000bbf, 0x00000ba4,
		0x00000bbe, 0x00000bb5, 0x00000ba4, 0x00000bc1, 0x00000020, 0x00000b8e, 0x00000b99, 0x00000bcd, 0x00000b95,
		0x00000bc1, 0x00000bae, 0x00000bcd, 0x00000020, 0x00000b95, 0x00000bbe, 0x00000ba3, 0x00000bcb, 0x00000bae,
		0x00000bcd, 0x0000002c, 0x0000041d, 0x00000430, 0x00000020, 0x00000431, 0x00000435, 0x00000440, 0x00000435,
		0x00000433, 0x00000443, 0x00000020, 0x0000043f, 0x00000443, 0x00000441, 0x00000442, 0x0000044b, 0x0000043d,
		0x0000043d, 0x0000044b, 0x00000445, 0x00000020, 0x00000432, 0x0000043e, 0x0000043b, 0x0000043d, 0x00000020,
		0x000020ac, 0x00000020, 0x00000623, 0x00000646, 0x00000627, 0x00000020, 0x00000642, 0x00000627, 0x0000062f,
		0x00000631, 0x00000020, 0x00000639, 0x00000644, 0x00000649, 0x00000020, 0x00000623, 0x00000643, 0x00000644,
		0x00000020, 0x00000627, 0x00000644, 0x00000632, 0x0000062c, 0x00000627, 0x0000062c, 0x00000020, 0x00000648,
		0x00000020, 0x00000647, 0x00000630, 0x00000627, 0x00000020, 0x00000644, 0x00000627, 0x00000020, 0x0000064a,
		0x00000624, 0x00000644, 0x00000645, 0x00000646, 0x0000064a, 0x0000002e, 0x00000020, 0x0000004f, 0x0000006e,
		0x00000020, 0x00000074, 0x00000068, 0x00000065, 0x00000020, 0x0000006f, 0x00000074, 0x00000068, 0x00000065,
		0x00000072, 0x00000020, 0x00000068, 0x00000061, 0x0000006e, 0x00000064, 0x0000002c, 0x00000020, 0x00000077,
		0x00000065, 0x00000020, 0x00000064, 0x00000065, 0x0000006e, 0x0000006f, 0x00000075, 0x0000006e, 0x00000063,
		0x00000065, 0x00000020, 0x00000077, 0x00000069, 0x00000074, 0x00000068, 0x00000020, 0x00000072, 0x00000069,
		0x00000067, 0x00000068, 0x00000074, 0x00000065, 0x0000006f, 0x00000075, 0x00000073, 0x00000020, 0x00000069,
		0x0000006e, 0x00000064, 0x00000069, 0x00000067, 0x0000006e, 0x00000061, 0x00000074, 0x00000069, 0x0000006f,
		0x0000006e, 0x00000020, 0x00000061, 0x0000006e, 0x00000064, 0x00000020, 0x00000064, 0x00000069, 0x00000073,
		0x0000006c, 0x00000069, 0x0000006b, 0x00000065, 0x00000020, 0x0000006d, 0x00000065, 0x0000006e, 0x00000020,
		0x00000077, 0x00000068, 0x0000006f, 0x00000020, 0x00000061, 0x00000072, 0x00000065, 0x00000020, 0x00000073,
		0x0000006f, 0x00000020, 0x00000062, 0x00000065, 0x00000067, 0x00000075, 0x00000069, 0x0000006c, 0x00000065,
		0x00000064, 0x00000020, 0x00000061, 0x0000006e, 0x00000064, 0x00000020, 0x00000064, 0x00000065, 0x0000006d,
		0x0000006f, 0x00000072, 0x00000061, 0x0000006c, 0x00000069, 0x0000007a, 0x00000065, 0x00000064, 0x00000020,
		0x00000062, 0x00000079, 0x00000020, 0x00000074, 0x00000068, 0x00000065, 0x00000020, 0x00000063, 0x00000068,
		0x00000061, 0x00000072, 0x0000006d, 0x00000073, 0x00000020, 0x0000006f, 0x00000066, 0x00000020, 0x00000070,
		0x0000006c, 0x00000065, 0x00000061, 0x00000073, 0x00000075, 0x00000072, 0x00000065, 0x00000020, 0x0000006f,
		0x00000066, 0x00000020, 0x00000074, 0x00000068, 0x00000065, 0x00000020, 0x0000006d, 0x0000006f, 0x0000006d,
		0x00000065, 0x0000006e, 0x00000074, 0x0000002c, 0x00000020, 0x00000073, 0x0000006f, 0x00000020, 0x00000062,
		0x0000006c, 0x00000069, 0x0000006e, 0x00000064, 0x00000065, 0x00000064, 0x00000020, 0x00000062, 0x00000079,
		0x00000020, 0x00000064, 0x00000065, 0x00000073, 0x00000069, 0x00000072, 0x00000065, 0x0000002c, 0x00000020,
		0x00000074, 0x00000068, 0x00000061, 0x00000074, 0x00000020, 0x00000074, 0x00000068, 0x00000065, 0x00000079,
		0x00000020, 0x00000063, 0x00000061, 0x0000006e, 0x0000006e, 0x0000006f, 0x00000074, 0x00000020, 0x00000066,
		0x0000006f, 0x00000072, 0x00000065, 0x00000073, 0x00000065, 0x00000065, 0x00000020, 0x00000074, 0x00000068,
		0x00000065, 0x00000020, 0x00000070, 0x00000061, 0x00000069, 0x0000006e, 0x00000020, 0x00000061, 0x0000006e,
		0x00000064, 0x00000020, 0x00000074, 0x00000072, 0x0000006f, 0x00000075, 0x00000062, 0x0000006c, 0x00000065,
		0x00000020, 0x00000074, 0x00000068, 0x00000061, 0x00000074, 0x00000020, 0x00000061, 0x00000072, 0x00000065,
		0x00000020, 0x00000062, 0x0000006f, 0x00000075, 0x0000006e, 0x00000064, 0x00000020, 0x00000074, 0x0000006f,
		0x00000020, 0x00000065, 0x0000006e, 0x00000073, 0x00000075, 0x00000065, 0x0000003b, 0x00000020, 0x00000061,
		0x0000006e, 0x00000064, 0x00000020, 0x00000065, 0x00000071, 0x00000075, 0x00000061, 0x0000006c, 0x00000020,
		0x00000062, 0x0000006c, 0x00000061, 0x0000006d, 0x00000065, 0x00000020, 0x00000062, 0x00000065, 0x0000006c,
		0x0000006f, 0x0000006e, 0x00000067, 0x00000073, 0x00000020, 0x00000074, 0x0000006f, 0x00000020, 0x00000074,
		0x00000068, 0x0000006f, 0x00000073, 0x00000065, 0x00000020, 0x00000077, 0x00000068, 0x0000006f, 0x00000020,
		0x00000066, 0x00000061, 0x00000069, 0x0000006c, 0x00000020, 0x00000069, 0x0000006e, 0x00000020, 0x00000074,
		0x00000068, 0x00000065, 0x00000069, 0x00000072, 0x00000020, 0x00000064, 0x00000075, 0x00000074, 0x00000079,
		0x00000020, 0x00000074, 0x00000068, 0x00000072, 0x0000006f, 0x00000075, 0x00000067, 0x00000068, 0x00000020,
		0x00000077, 0x00000065, 0x00000061, 0x0000006b, 0x0000006e, 0x00000065, 0x00000073, 0x00000073, 0x00000020,
		0x0000006f, 0x00000066, 0x00000020, 0x00000077, 0x00000069, 0x0000006c, 0x0000006c, 0x0000002c, 0x00000020,
		0x00000077, 0x00000068, 0x00000069, 0x00000063, 0x00000068, 0x00000020, 0x00000069, 0x00000073, 0x00000020,
		0x00000074, 0x00000068, 0x00000065, 0x00000020, 0x00000073, 0x00000061, 0x0000006d, 0x00000065, 0x00000020,
		0x00000061, 0x00000073, 0x00000020, 0x00000073, 0x00000061, 0x00000079, 0x00000069, 0x0000006e, 0x00000067,
		0x00000020, 0x00000074, 0x00000068, 0x00000072, 0x0000006f, 0x00000075, 0x00000067, 0x00000068, 0x00000020,
		0x00000073, 0x00000068, 0x00000072, 0x00000069, 0x0000006e, 0x0000006b, 0x00000069, 0x0000006e, 0x00000067,
		0x00000020, 0x00000066, 0x00000072, 0x0000006f, 0x0000006d, 0x00000020, 0x00000074, 0x0000006f, 0x00000069,
		0x0000006c, 0x00000020, 0x00000061, 0x0000006e, 0x00000064, 0x00000020, 0x00000070, 0x00000061, 0x00000069,
		0x0000006e, 0x0000002e, 0x00000020, 0x00000054, 0x00000068, 0x00000065, 0x00000073, 0x00000065, 0x00000020,
		0x00000063, 0x00000061, 0x00000073, 0x00000065, 0x00000073, 0x00000020, 0x00000061, 0x00000072, 0x00000065,
		0x00000020, 0x00000070, 0x00000065, 0x00000072, 0x00000066, 0x00000065, 0x00000063, 0x00000074, 0x0000006c,
		0x00000079, 0x00000020, 0x00000073, 0x00000069, 0x0000006d, 0x00000070, 0x0000006c, 0x00000065, 0x00000020,
		0x00000061, 0x0000006e, 0x00000064, 0x00000020, 0x00000065, 0x00000061, 0x00000073, 0x00000079, 0x00000020,
		0x00000074, 0x0000006f, 0x00000020, 0x00000064, 0x00000069, 0x00000073, 0x00000074, 0x00000069, 0x0000006e,
		0x00000067, 0x00000075, 0x00000069, 0x00000073, 0x00000068, 0x0000002e, 0x00000020, 0x00000049, 0x0000006e,
		0x00000020, 0x00000061, 0x00000020, 0x00000066, 0x00000072, 0x00000065, 0x00000065, 0x00000020, 0x00000068,
		0x0000006f, 0x00000075, 0x00000072, 0x0000002c, 0x00000020, 0x00000077, 0x00000068, 0x00000065, 0x0000006e,
		0x00000020, 0x0000006f, 0x00000075, 0x00000072, 0x00000020, 0x00000070, 0x0000006f, 0x00000077, 0x00000065,
		0x00000072, 0x00000020, 0x0000006f, 0x00000066, 0x00000020, 0x00000063, 0x00000068, 0x0000006f, 0x00000069,
		0x00000063, 0x00000065, 0x00000020, 0x00000069, 0x00000073, 0x00000020, 0x00000075, 0x0000006e, 0x00000074,
		0x00000072, 0x00000061, 0x0000006d, 0x0000006d, 0x00000065, 0x0000006c, 0x0000006c, 0x00000065, 0x00000064,
		0x00000020, 0x00000061, 0x0000006e, 0x00000064, 0x00000020, 0x00000077, 0x00000068, 0x00000065, 0x0000006e,
		0x00000020, 0x0000006e, 0x0000006f, 0x00000074, 0x00000068, 0x00000069, 0x0000006e, 0x00000067, 0x00000020,
		0x00000070, 0x00000072, 0x00000065, 0x00000076, 0x00000065, 0x0000006e, 0x00000074, 0x00000073, 0x00000020,
		0x0000006f, 0x00000075, 0x00000072, 0x00000020, 0x00000062, 0x00000065, 0x00000069, 0x0000006e, 0x00000067,
		0x00000020, 0x00000061, 0x00000062, 0x0000006c, 0x00000065, 0x00000020, 0x00000074, 0x0000006f, 0x00000020,
		0x00000064, 0x0000006f, 0x00000020, 0x00000077, 0x00000068, 0x00000061, 0x00000074, 0x00000020, 0x00000077,
		0x00000065, 0x00000020, 0x0000006c, 0x00000069, 0x0000006b, 0x00000065, 0x00000020, 0x00000062, 0x00000065,
		0x00000073, 0x00000074, 0x0000002c, 0x00000020, 0x00000065, 0x00000076, 0x00000065, 0x00000072, 0x00000079,
		0x00000020, 0x00000070, 0x0000006c, 0x00000065, 0x00000061, 0x00000073, 0x00000075, 0x00000072, 0x00000065,
		0x00000020, 0x00000069, 0x00000073, 0x00000020, 0x00000074, 0x0000006f, 0x00000020, 0x00000062, 0x00000065,
		0x00000020, 0x00000077, 0x00000065, 0x0000006c, 0x00000063, 0x0000006f, 0x0000006d, 0x00000065, 0x00000064,
		0x00000020, 0x00000061, 0x0000006e, 0x00000064, 0x00000020, 0x00000065, 0x00000076, 0x00000065, 0x00000072,
		0x00000079, 0x00000020, 0x00000070, 0x00000061, 0x00000069, 0x0000006e, 0x00000020, 0x00000061, 0x00000076,
		0x0000006f, 0x00000069, 0x00000064, 0x00000065, 0x00000064, 0x0000002e, 0x00000020, 0x00000042, 0x00000075,
		0x00000074, 0x00000020, 0x00000069, 0x0000006e, 0x00000020, 0x00000063, 0x00000065, 0x00000072, 0x00000074,
		0x00000061, 0x00000069, 0x0000006e, 0x00000020, 0x00000063, 0x00000069, 0x00000072, 0x00000063, 0x00000075,
		0x0000006d, 0x00000073, 0x00000074, 0x00000061, 0x0000006e, 0x00000063, 0x00000065, 0x00000073, 0x00000020,
		0x00000061, 0x0000006e, 0x00000064, 0x00000020, 0x0000006f, 0x00000077, 0x00000069, 0x0000006e, 0x00000067,
		0x00000020, 0x00000074, 0x0000006f, 0x00000020, 0x00000074, 0x00000068, 0x00000065, 0x00000020, 0x00000063,
		0x0000006c, 0x00000061, 0x00000069, 0x0000006d, 0x00000073, 0x00000020, 0x0000006f, 0x00000066, 0x00000020,
		0x00000064, 0x00000075, 0x00000074, 0x00000079, 0x00000020, 0x0000006f, 0x00000072, 0x00000020, 0x00000074,
		0x00000068, 0x00000065, 0x00000020, 0x0000006f, 0x00000062, 0x0000006c, 0x00000069, 0x00000067, 0x00000061,
		0x00000074, 0x00000069, 0x0000006f, 0x0000006e, 0x00000073, 0x00000020, 0x0000006f, 0x00000066, 0x00000020,
		0x00000062, 0x00000075, 0x00000073, 0x00000069, 0x0000006e, 0x00000065, 0x00000073, 0x00000073, 0x00000020,
		0x00000069, 0x00000074, 0x00000020, 0x00000077, 0x00000069, 0x0000006c, 0x0000006c, 0x00000020, 0x00000066,
		0x00000072, 0x00000065, 0x00000071, 0x00000075, 0x00000065, 0x0000006e, 0x00000074, 0x0000006c, 0x00000079,
		0x00000020, 0x0000006f, 0x00000063, 0x00000063, 0x00000075, 0x00000072, 0x00000020, 0x00000074, 0x00000068,
		0x00000061, 0x00000074, 0x00000020, 0x00000070, 0x0000006c, 0x00000065, 0x00000061, 0x00000073, 0x00000075,
		0x00000072, 0x00000065, 0x00000073, 0x00000020, 0x00000068, 0x00000061, 0x00000076, 0x00000065, 0x00000020,
		0x00000074, 0x0000006f, 0x00000020, 0x00000062, 0x00000065, 0x00000020, 0x00000072, 0x00000065, 0x00000070,
		0x00000075, 0x00000064, 0x00000069, 0x00000061, 0x00000074, 0x00000065, 0x00000064, 0x00000020, 0x00000061,
		0x0000006e, 0x00000064, 0x00000020, 0x00000061, 0x0000006e, 0x0000006e, 0x0000006f, 0x00000079, 0x00000061,
		0x0000006e, 0x00000063, 0x00000065, 0x00000073, 0x00000020, 0x00000061, 0x00000063, 0x00000063, 0x00000065,
		0x00000070, 0x00000074, 0x00000065, 0x00000064, 0x0000002e, 0x00000020, 0x00000054, 0x00000068, 0x00000065,
		0x00000020, 0x00000077, 0x00000069, 0x00000073, 0x00000065, 0x00000020, 0x0000006d, 0x00000061, 0x0000006e,
		0x00000020, 0x00000074, 0x00000068, 0x00000065, 0x00000072, 0x00000065, 0x00000066, 0x0000006f, 0x00000072,
		0x00000065, 0x00000020, 0x00000061, 0x0000006c, 0x00000077, 0x00000061, 0x00000079, 0x00000073, 0x00000020,
		0x00000068, 0x0000006f, 0x0000006c, 0x00000064, 0x00000073, 0x00000020, 0x00000069, 0x0000006e, 0x00000020,
		0x00000074, 0x00000068, 0x00000065, 0x00000073, 0x00000065, 0x00000020, 0x0000006d, 0x00000061, 0x00000074,
		0x00000074, 0x00000065, 0x00000072, 0x00000073, 0x00000020, 0x00000074, 0x0000006f, 0x00000020, 0x00000074,
		0x00000068, 0x00000069, 0x00000073, 0x00000020, 0x00000070, 0x00000072, 0x00000069, 0x0000006e, 0x00000063,
		0x00000069, 0x00000070, 0x0000006c, 0x00000065, 0x00000020, 0x0000006f, 0x00000066, 0x00000020, 0x00000073,
		0x00000065, 0x0000006c, 0x00000065, 0x00000063, 0x00000074, 0x00000069, 0x0000006f, 0x0000006e, 0x0000003a,
		0x00000020, 0x00000068, 0x00000065, 0x00000020, 0x00000072, 0x00000065, 0x0000006a, 0x00000065, 0x00000063,
		0x00000074, 0x00000073, 0x00000020, 0x00000070, 0x0000006c, 0x00000065, 0x00000061, 0x00000073, 0x00000075,
		0x00000072, 0x00000065, 0x00000073, 0x00000020, 0x00000074, 0x0000006f, 0x00000020, 0x00000073, 0x00000065,
		0x00000063, 0x00000075, 0x00000072, 0x00000065, 0x00000020, 0x0000006f, 0x00000074, 0x00000068, 0x00000065,
		0x00000072, 0x00000020, 0x00000067, 0x00000072, 0x00000065, 0x00000061, 0x00000074, 0x00000065, 0x00000072,
		0x00000020, 0x00000070, 0x0000006c, 0x00000065, 0x00000061, 0x00000073, 0x00000075, 0x00000072, 0x00000065,
		0x00000073, 0x0000002c, 0x00000020, 0x0000006f, 0x00000072, 0x00000020, 0x00000065, 0x0000006c, 0x00000073,
		0x00000065, 0x00000020, 0x00000068, 0x00000065, 0x00000020, 0x00000065, 0x0000006e, 0x00000064, 0x00000075,
		0x00000072, 0x00000065, 0x00000073, 0x00000020, 0x00000070, 0x00000061, 0x00000069, 0x0000006e, 0x00000073,
		0x00000020, 0x00000074, 0x0000006f, 0x00000020, 0x00000061, 0x00000076, 0x0000006f, 0x00000069, 0x00000064,
		0x00000020, 0x00000077, 0x0000006f, 0x00000072, 0x00000073, 0x00000065, 0x00000020, 0x00000070, 0x00000061,
		0x00000069, 0x0000006e, 0x00000073, 0x0000002e, 0x00000020, 0x0000c624, 0x0000b298, 0x0000c774, 0x00000020,
		0x0000c624, 0x0000b298, 0x0000c774, 0x0000c18c, 0x0000c11c, 0x00000020, 0x0000b9e4, 0x0000c77c, 0x0000c774,
		0x00000020, 0x0000c624, 0x0000b298, 0x0000c774, 0x0000c18c, 0x0000c11c, 0x00000020, 0x0000c800, 0x0000bb3c,
		0x0000c9c0, 0x0000b3c4, 0x00000020, 0x0000c0c8, 0x0000c9c0, 0x0000b3c4, 0x00000020, 0x0000b9d0, 0x0000c73c,
		0x0000c2dc, 0x0000ace0, 0x00000020, 0x0000c0c8, 0x0000b824, 0x0000ba74, 0x00000020, 0x0000b298, 0x00000020,
		0x0000c5b8, 0x0000c81c, 0x0000b098, 0x00000020, 0x0000c624, 0x0000b298, 0x0000c774, 0x0000c18c, 0x0000c11c,
		0x0000002e, 0x00000020, 0x00005230, 0x00005e95, 0x0000662f, 0x00006211, 0x0000773c, 0x0000795e, 0x0000592a,
		0x0000597d, 0x0000ff0c, 0x00008fd8, 0x0000662f, 0x00006211, 0x0000540c, 0x00004e8b, 0x0000773c, 0x0000592a,
		0x000082b1, 0x00003002, 0x00006ca1, 0x00006709, 0x00004efb, 0x00004f55, 0x00004fdd, 0x00008bc1, 0x00007528,
		0x00004e86, 0x00000020, 0x00000070, 0x00000069, 0x00000070, 0x00000065, 0x0000006e, 0x00000076, 0x00000020,
		0x00007684, 0x00009879, 0x000076ee, 0x000080fd, 0x00005728, 0x0000968f, 0x00004fbf, 0x00004e00, 0x00004e2a,
		0x00000020, 0x00000043, 0x0000006f, 0x0000006e, 0x00000064, 0x00000061, 0x00000020, 0x00000065, 0x0000006e,
		0x00000076, 0x00000069, 0x00000072, 0x0000006f, 0x0000006e, 0x0000006d, 0x00000065, 0x0000006e, 0x00000074,
		0x00000020, 0x000091cc, 0x00008dd1, 0x00008d77, 0x00006765, 0x00005355, 0x00005143, 0x00006d4b, 0x00008bd5,
		0x0000ff01, 0x000030ed, 0x000030a4, 0x000030bf, 0x000030fc, 0x00008a18, 0x00008005, 0x00003001, 0x00005f85,
		0x00006a5f, 0x0000671f, 0x00009593, 0x0000306e, 0x000030d1, 0x000030fc, 0x000030c6, 0x000030a3, 0x000030fc,
		0x000053c2, 0x000052a0, 0x00003067, 0x000030b3, 0x000030ed, 0x000030ca, 0x00005909, 0x00007570, 0x0000682a,
		0x0000304c, 0x0000611f, 0x000067d3, 0x000062e1, 0x00005927, 0x00000020, 0x0000007c, 0x00000020, 0x000030b9,
		0x000030af, 0x000030fc, 0x000030d7, 0x0000901f, 0x00005831, 0x00000020, 0x00000023, 0x000030ed, 0x000030a4,
		0x000030bf, 0x000030fc, 0x00000020, 0x00000023, 0x000030b9, 0x000030af, 0x000030fc, 0x000030d7, 0x0000901f,
		0x00005831, 0x00000020, 0x00000023, 0x00009031, 0x0000520a, 0x00006587, 0x00006625, 0x00000020, 0x0000005c,
		0x00000022, 0x00000046, 0x00000061, 0x00000065, 0x0000006e, 0x0000005c, 0x00000022, 0x00000020, 0x0000006f,
		0x00000067, 0x00000020, 0x0000005c, 0x00000022, 0x00000068, 0x00000065, 0x00000072, 0x00000072, 0x00000065,
		0x00000067, 0x00000075, 0x00000064, 0x0000005c, 0x00000022, 0x00000020, 0x00000073, 0x00000061, 0x00000020,
		0x00000064, 0x00000065, 0x00000020, 0x00000069, 0x00000020, 0x00000067, 0x00000061, 0x0000006d, 0x0000006c,
		0x00000065, 0x00000020, 0x00000064, 0x00000061, 0x00000067, 0x00000065, 0x00000072, 0x0000002e, 0x00000049,
		0x00000074, 0x00000027, 0x00000073, 0x00000020, 0x0000006e, 0x00000065, 0x00000076, 0x00000065, 0x00000072,
		0x00000020, 0x00000074, 0x0000006f, 0x0000006f, 0x00000020, 0x00000065, 0x00000061, 0x00000072, 0x0000006c,
		0x00000079, 0x00000020, 0x00000074, 0x0000006f, 0x00000020, 0x0000006c, 0x00000065, 0x00000061, 0x00000072,
		0x0000006e, 0x00000020, 0x00000061, 0x00000020, 0x0000006c, 0x00000061, 0x0000006e, 0x00000067, 0x00000075,
		0x00000061, 0x00000067, 0x00000065, 0x0000002e, 0x0000002e, 0x0000002e, 0x00000020, 0x00000041, 0x00000075,
		0x00000072, 0x000000e9, 0x0000006c, 0x00000069, 0x00000065, 0x0000002e, 0x00000000 };
	inline constexpr const std::basic_string_view<char32_t> u32text = u32text_storage;

	inline constexpr const std::basic_string_view<char32_t> u32_ansi_sequence_truth = U"zá:iï";
	inline constexpr const std::basic_string_view<char16_t> u16_ansi_sequence_truth = u"zá:iï";
	inline constexpr const std::basic_string_view<char8_t> u8_ansi_sequence_truth   = u8"zá:iï";
	inline constexpr const std::basic_string_view<wchar_t> w_ansi_sequence_truth    = L"zá:iï";
	inline constexpr const std::basic_string_view<char> ansi_sequence_truth         = "zá:iï";

	inline constexpr const std::basic_string_view<char32_t> u32_unicode_single_truth = U"♥";
	inline constexpr const std::basic_string_view<char16_t> u16_unicode_single_truth = u"♥";
	inline constexpr const std::basic_string_view<char8_t> u8_unicode_single_truth   = u8"♥";
	inline constexpr const std::basic_string_view<wchar_t> w_unicode_single_truth    = L"♥";
	inline constexpr const std::basic_string_view<char> single_truth                 = "♥";

	inline constexpr const std::basic_string_view<char32_t> u32_ansi_single_truth = U"a";
	inline constexpr const std::basic_string_view<char16_t> u16_ansi_single_truth = u"a";
	inline constexpr const std::basic_string_view<char8_t> u8_ansi_single_truth   = u8"a";
	inline constexpr const std::basic_string_view<wchar_t> w_ansi_single_truth    = L"a";
	inline constexpr const std::basic_string_view<char> ansi_single_truth         = "a";

	inline constexpr const char32_t u32_unicode_invalid_input_arr[]                   = { 0xFFFFFFFF, 0 };
	inline constexpr const char16_t u16_unicode_invalid_input_arr[]                   = { 0xD800, 0 };
	inline constexpr const char8_t u8_unicode_invalid_input_arr[]                     = { 0xC0, 0 };
	inline constexpr const std::basic_string_view<char32_t> u32_unicode_invalid_input = u32_unicode_invalid_input_arr;
	inline constexpr const std::basic_string_view<char16_t> u16_unicode_invalid_input = u16_unicode_invalid_input_arr;
	inline constexpr const std::basic_string_view<char8_t> u8_unicode_invalid_input   = u8_unicode_invalid_input_arr;

	inline constexpr const std::basic_string_view<char32_t> u32_unicode_replacement_truth = U"\uFFFD";
	inline constexpr const std::basic_string_view<char16_t> u16_unicode_replacement_truth = u"\uFFFD";
	inline constexpr const std::basic_string_view<char8_t> u8_unicode_replacement_truth   = u8"\uFFFD";
	inline constexpr const std::basic_string_view<wchar_t> w_unicode_replacement_truth    = L"?";
	inline constexpr const std::basic_string_view<char> unicode_replacement_truth         = "?";

	inline constexpr const std::basic_string_view<char32_t> u32_basic_source_character_set
		= U"\f\v\t \nabcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789_{}[]#()<>%:;.?*+-/^&|~!=,\\\"'";
	inline constexpr const std::basic_string_view<char16_t> u16_basic_source_character_set
		= u"\f\v\t \nabcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789_{}[]#()<>%:;.?*+-/^&|~!=,\\\"'";
	inline constexpr const std::basic_string_view<char8_t> u8_basic_source_character_set
		= u8"\f\v\t \nabcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789_{}[]#()<>%:;.?*+-/^&|~!=,\\\"'";
	inline constexpr const std::basic_string_view<wchar_t> w_basic_source_character_set
		= L"\f\v\t \nabcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789_{}[]#()<>%:;.?*+-/^&|~!=,\\\"'";
	inline constexpr const std::basic_string_view<char> basic_source_character_set
		= "\f\v\t \nabcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789_{}[]#()<>%:;.?*+-/^&|~!=,\\\"'";

	inline static const std::span<const std::byte> u32_unicode_sequence_bytes_truth
		= ztd::text::tests::as_bytes(ztd::text::tests::u32_unicode_sequence_truth);
	inline static const std::span<const std::byte> u16_unicode_sequence_bytes_truth
		= ztd::text::tests::as_bytes(ztd::text::tests::u16_unicode_sequence_truth);
	inline static const std::span<const std::byte> u8_unicode_sequence_bytes_truth
		= ztd::text::tests::as_bytes(ztd::text::tests::u8_unicode_sequence_truth);
	inline static const std::span<const std::byte> w_unicode_sequence_bytes_truth
		= ztd::text::tests::as_bytes(ztd::text::tests::w_unicode_sequence_truth);
	inline static const std::span<const std::byte> unicode_sequence_bytes_truth
		= ztd::text::tests::as_bytes(ztd::text::tests::unicode_sequence_truth);

	inline static const std::span<const std::byte> u32_ansi_sequence_bytes_truth
		= ztd::text::tests::as_bytes(ztd::text::tests::u32_ansi_sequence_truth);
	inline static const std::span<const std::byte> u16_ansi_sequence_bytes_truth
		= ztd::text::tests::as_bytes(ztd::text::tests::u16_ansi_sequence_truth);
	inline static const std::span<const std::byte> u8_ansi_sequence_bytes_truth
		= ztd::text::tests::as_bytes(ztd::text::tests::u8_ansi_sequence_truth);
	inline static const std::span<const std::byte> w_ansi_sequence_bytes_truth
		= ztd::text::tests::as_bytes(ztd::text::tests::w_ansi_sequence_truth);
	inline static const std::span<const std::byte> ansi_sequence_bytes_truth
		= ztd::text::tests::as_bytes(ztd::text::tests::ansi_sequence_truth);

}}} // namespace ztd::text::tests

#endif // ZTD_TEXT_TESTS_BASIC_UNICODE_STRINGS_HPP
