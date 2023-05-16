.. =============================================================================
..
.. ztd.text
.. Copyright © 2022-2023 JeanHeyd "ThePhD" Meneide and Shepherd's Oasis, LLC
.. Contact: opensource@soasis.org
..
.. Commercial License Usage
.. Licensees holding valid commercial ztd.text licenses may use this file in
.. accordance with the commercial license agreement provided with the
.. Software or, alternatively, in accordance with the terms contained in
.. a written agreement between you and Shepherd's Oasis, LLC.
.. For licensing terms and conditions see your agreement. For
.. further information contact opensource@soasis.org.
..
.. Apache License Version 2 Usage
.. Alternatively, this file may be used under the terms of Apache License
.. Version 2.0 (the "License") for non-commercial use; you may not use this
.. file except in compliance with the License. You may obtain a copy of the
.. License at
..
.. https://www.apache.org/licenses/LICENSE-2.0
..
.. Unless required by applicable law or agreed to in writing, software
.. distributed under the License is distributed on an "AS IS" BASIS,
.. WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
.. See the License for the specific language governing permissions and
.. limitations under the License.
..
.. =============================================================================>

Available Encodings
===================

Below is a table of encodings. Here, we track which encodings can be represented using the :doc:`Lucky 7 </design/lucky 7>` technique, whether or not we have produced such an implementation, and (if applicable) a link to said implementation.

Most are table-driven encodings, but a select few are not. Tables and data for this are included as part of the `Encoding Tables repository <https://github.com/soasis/encoding_tables>`_

As a general point, we hope to support almost all of the encodings here in one form or another! If you'd like to request prioritization of a certain encoding, let us know.

.. _encodings-encoding-table:

.. list-table:: **Encoding Progress Table**
	:header-rows: 1

	* - Name
	  - Stateful
	  - :doc:`Implementable? </design/lucky 7>`
	  - Implemented
	* - UTF-8
	  - No
	  - Yes
	  - :doc:`Yes ✅ </api/encodings/utf8>`
	* - UTF-16
	  - No
	  - Yes
	  - :doc:`Yes ✅ </api/encodings/utf16>`
	* - UTF-32
	  - No
	  - Yes
	  - :doc:`Yes ✅ </api/encodings/utf32>`
	* - Modified UTF-8 (MUTF-8)
	  - No
	  - Yes
	  - :doc:`Yes ✅ </api/encodings/utf8>`
	* - | Wobbly Transformation
	    | Format-8 (WTF-8)
	  - No
	  - Yes
	  - :doc:`Yes ✅ </api/encodings/wtf8>`
	* - ASCII
	  - No
	  - Yes
	  - :doc:`Yes ✅ </api/encodings/ascii>`
	* - C Locale
	  - Yes (``std::mbstate_t``)
	  - Yes
	  - :doc:`Yes ✅ </api/encodings/execution>`
	* - C Locale, Wide
	  - Yes (``std::mbstate_t``)
	  - Yes
	  - :doc:`Yes ✅ </api/encodings/wide_execution>`
	* - String Literals
	  - Compiler-Dependent
	  - Yes
	  - :doc:`Yes ✅ </api/encodings/literal>`
	* - Wide String Literals
	  - Compiler-Dependent
	  - Yes
	  - :doc:`Yes ✅ </api/encodings/wide_literal>`
	* - "Anything" Wrapper
	  - Typed-Erased
	  - Yes
	  - :doc:`Yes ✅ </api/encodings/any_encoding>`
	* - Encoding Scheme
	  - Wrapping-Dependent
	  - Yes
	  - :doc:`Yes ✅ </api/encodings/encoding_scheme>`
	* - ``iconv`` Encoding
	  - Yes
	  - Yes
	  - :doc:`Yes ✅ </api/encodings/basic_iconv>`
	* - ``cuneicode`` Encoding
	  - Yes
	  - Yes
	  - :doc:`Yes ✅ </api/encodings/cuneicode_registry_encoding>`
	* - ATARIST
	  - No
	  - Yes
	  - :doc:`Yes ✅ </api/encodings/single_byte_encoding>`
	* - KOI8-U
	  - No
	  - Yes
	  - :doc:`Yes ✅ </api/encodings/single_byte_encoding>`
	* - KOI8-R
	  - No
	  - Yes
	  - :doc:`Yes ✅ </api/encodings/single_byte_encoding>`
	* - IBM 424 Hebrew (Bulletin)
	  - No
	  - Yes
	  - :doc:`Yes ✅ </api/encodings/single_byte_encoding>`
	* - IBM 856 Hebrew
	  - No
	  - Yes
	  - :doc:`Yes ✅ </api/encodings/single_byte_encoding>`
	* - IBM 866 Cyrillic (Russian)
	  - No
	  - Yes
	  - :doc:`Yes ✅ </api/encodings/single_byte_encoding>`
	* - IBM 1006 Urdu
	  - No
	  - Yes
	  - :doc:`Yes ✅ </api/encodings/single_byte_encoding>`
	* - Kazakh-STRK1048 / KZ-1048
	  - No
	  - Yes
	  - :doc:`Yes ✅ </api/encodings/single_byte_encoding>`
	* - Windows-874
	  - No
	  - Yes
	  - :doc:`Yes ✅ </api/encodings/single_byte_encoding>`
	* - Windows-1251
	  - No
	  - Yes
	  - :doc:`Yes ✅ </api/encodings/single_byte_encoding>`
	* - Windows-1252 / Latin-1
	  - No
	  - Yes
	  - :doc:`Yes ✅ </api/encodings/single_byte_encoding>`
	* - Windows-1253
	  - Yes
	  - Yes
	  - :doc:`Yes ✅ </api/encodings/single_byte_encoding>`
	* - Windows-1254
	  - No
	  - Yes
	  - :doc:`Yes ✅ </api/encodings/single_byte_encoding>`
	* - Windows-1255
	  - No
	  - Yes
	  - :doc:`Yes ✅ </api/encodings/single_byte_encoding>`
	* - Windows-1256
	  - No
	  - Yes
	  - :doc:`Yes ✅ </api/encodings/single_byte_encoding>`
	* - Windows-1257
	  - No
	  - Yes
	  - :doc:`Yes ✅ </api/encodings/single_byte_encoding>`
	* - Windows-1258
	  - No
	  - Yes
	  - :doc:`Yes ✅ </api/encodings/single_byte_encoding>`
	* - PETSCII (Shifted and Unshifted)
	  - No
	  - Yes
	  - :doc:`Yes ✅ </api/encodings/petscii>`
	* - UTF-EBCDIC
	  - No
	  - No (Licensed)
	  - No ❌
	* - UTF-7
	  - Yes
	  - No (Licensed)
	  - No ❌
	* - UTF-7-IMAP
	  - Yes
	  - No (Licensed)
	  - No ❌
	* - ISO-8859-1-1985
	  - ❓ Unresearched
	  - ❓ Unconfirmed
	  - No ❌
	* - ISO-8859-1 (1988)
	  - ❓ Unresearched
	  - ❓ Unconfirmed
	  - No ❌
	* - ISO-8859-2
	  - No
	  - Yes
	  - :doc:`Yes ✅ </api/encodings/single_byte_encoding>`
	* - ISO-8859-3
	  - No
	  - Yes
	  - :doc:`Yes ✅ </api/encodings/single_byte_encoding>`
	* - ISO-8859-4
	  - No
	  - Yes
	  - :doc:`Yes ✅ </api/encodings/single_byte_encoding>`
	* - ISO-8859-5
	  - No
	  - Yes
	  - :doc:`Yes ✅ </api/encodings/single_byte_encoding>`
	* - ISO-8859-6
	  - No
	  - Yes
	  - :doc:`Yes ✅ </api/encodings/single_byte_encoding>`
	* - ISO-8859-7
	  - No
	  - Yes
	  - :doc:`Yes ✅ </api/encodings/single_byte_encoding>`
	* - ISO-8859-8
	  - No
	  - Yes
	  - :doc:`Yes ✅ </api/encodings/single_byte_encoding>`
	* - ISO-8859-9
	  - ❓ Unresearched
	  - ❓ Unconfirmed
	  - No ❌
	* - ISO-8859-10
	  - No
	  - Yes
	  - :doc:`Yes ✅ </api/encodings/single_byte_encoding>`
	* - ISO-8859-13
	  - No
	  - Yes
	  - :doc:`Yes ✅ </api/encodings/single_byte_encoding>`
	* - ISO-8859-14
	  - No
	  - Yes
	  - :doc:`Yes ✅ </api/encodings/single_byte_encoding>`
	* - ISO-8859-15
	  - No
	  - Yes
	  - :doc:`Yes ✅ </api/encodings/single_byte_encoding>`
	* - ISO-8859-16
	  - No
	  - Yes
	  - :doc:`Yes ✅ </api/encodings/single_byte_encoding>`
	* - Tatar (Windows/ASCII)
	  - No
	  - Yes
	  - :doc:`Yes ✅ </api/encodings/single_byte_encoding>`
	* - Tatar (IBM/ANSI)
	  - No
	  - Yes
	  - :doc:`Yes ✅ </api/encodings/single_byte_encoding>`
	* - RISCOS-LATIN1
	  - ❓ Unresearched
	  - ❓ Unconfirmed
	  - No ❌
	* - TDS565
	  - ❓ Unresearched
	  - ❓ Unconfirmed
	  - No ❌
	* - CP437
	  - ❓ Unresearched
	  - ❓ Unconfirmed
	  - No ❌
	* - CP737
	  - ❓ Unresearched
	  - ❓ Unconfirmed
	  - No ❌
	* - CP775
	  - ❓ Unresearched
	  - ❓ Unconfirmed
	  - No ❌
	* - CP850
	  - ❓ Unresearched
	  - ❓ Unconfirmed
	  - No ❌
	* - CP852
	  - ❓ Unresearched
	  - ❓ Unconfirmed
	  - No ❌
	* - CP853
	  - ❓ Unresearched
	  - ❓ Unconfirmed
	  - No ❌
	* - CP855
	  - ❓ Unresearched
	  - ❓ Unconfirmed
	  - No ❌
	* - CP857
	  - ❓ Unresearched
	  - ❓ Unconfirmed
	  - No ❌
	* - CP858
	  - ❓ Unresearched
	  - ❓ Unconfirmed
	  - No ❌
	* - CP860
	  - ❓ Unresearched
	  - ❓ Unconfirmed
	  - No ❌
	* - CP861
	  - ❓ Unresearched
	  - ❓ Unconfirmed
	  - No ❌
	* - CP862
	  - ❓ Unresearched
	  - ❓ Unconfirmed
	  - No ❌
	* - CP863
	  - ❓ Unresearched
	  - ❓ Unconfirmed
	  - No ❌
	* - CP864
	  - ❓ Unresearched
	  - ❓ Unconfirmed
	  - No ❌
	* - CP865
	  - ❓ Unresearched
	  - ❓ Unconfirmed
	  - No ❌
	* - CP866
	  - ❓ Unresearched
	  - ❓ Unconfirmed
	  - No ❌
	* - CP869 (Nice)
	  - ❓ Unresearched
	  - ❓ Unconfirmed
	  - No ❌
	* - CP932
	  - ❓ Unresearched
	  - ❓ Unconfirmed
	  - No ❌
	* - CP936
	  - ❓ Unresearched
	  - ❓ Unconfirmed
	  - No ❌
	* - CP949
	  - ❓ Unresearched
	  - ❓ Unconfirmed
	  - No ❌
	* - CP1125
	  - ❓ Unresearched
	  - ❓ Unconfirmed
	  - No ❌
	* - CP1131
	  - ❓ Unresearched
	  - ❓ Unconfirmed
	  - No ❌
	* - CP1133
	  - ❓ Unresearched
	  - ❓ Unconfirmed
	  - No ❌
	* - CP1250
	  - ❓ Unresearched
	  - ❓ Unconfirmed
	  - No ❌
	* - MacRoman
	  - No
	  - Yes
	  - :doc:`Yes ✅ </api/encodings/single_byte_encoding>`
	* - MacCentralEurope
	  - ❓ Unresearched
	  - ❓ Unconfirmed
	  - No ❌
	* - MacIceland
	  - ❓ Unresearched
	  - ❓ Unconfirmed
	  - No ❌
	* - MacCroatian
	  - ❓ Unresearched
	  - ❓ Unconfirmed
	  - No ❌
	* - MacRomania
	  - ❓ Unresearched
	  - ❓ Unconfirmed
	  - No ❌
	* - MacCryllic
	  - No
	  - Yes
	  - :doc:`Yes ✅ </api/encodings/single_byte_encoding>`
	* - MacUkraine
	  - ❓ Unresearched
	  - ❓ Unconfirmed
	  - No ❌
	* - MacGreek
	  - ❓ Unresearched
	  - ❓ Unconfirmed
	  - No ❌
	* - MacTurkish
	  - ❓ Unresearched
	  - ❓ Unconfirmed
	  - No ❌
	* - Macintosh
	  - ❓ Unresearched
	  - ❓ Unconfirmed
	  - No ❌
	* - MacHebrew
	  - ❓ Unresearched
	  - ❓ Unconfirmed
	  - No ❌
	* - MacArabic
	  - ❓ Unresearched
	  - ❓ Unconfirmed
	  - No ❌
	* - MacThai
	  - ❓ Unresearched
	  - ❓ Unconfirmed
	  - No ❌
	* - SHIFT-JIS
	  - Yes, shift states
	  - Yes
	  - No ❌
	* - SHIFT-JISX0213
	  - Yes, shift states
	  - Yes
	  - :doc:`Yes ✅ </api/encodings/shift_jisx0208>`
	* - SHIFT-JISX0213
	  - Yes, shift states
	  - Yes
	  - No ❌
	* - JOHAB
	  - ❓ Unresearched
	  - ❓ Unconfirmed
	  - No ❌
	* - GB18030 (Unicode-capable)
	  - No
	  - Yes
	  - :doc:`Yes ✅ </api/encodings/gb18030>`
	* - EUC-KR
	  - No
	  - Yes
	  - :doc:`Yes ✅</api/encodings/euc_kr_uhc>`
	* - BIG5
	  - No
	  - Yes
	  - No ❌
	* - BIG5-2003
	  - No
	  - Yes
	  - No ❌
	* - Big5-HKSCS
	  - No
	  - Yes
	  - :doc:`Yes ✅</api/encodings/big5_hkscs>`
	* - TACE
	  - No
	  - Yes
	  - No ❌
	* - TSCII
	  - No
	  - Yes
	  - No ❌
	* - EUC-JP
	  - ❓ Unresearched
	  - ❓ Unconfirmed
	  - No ❌
	* - EUC-JISX0213
	  - ❓ Unresearched
	  - ❓ Unconfirmed
	  - No ❌
	* - EUC-CN
	  - ❓ Unresearched
	  - ❓ Unconfirmed
	  - No ❌
	* - EUC-TW
	  - ❓ Unresearched
	  - ❓ Unconfirmed
	  - No ❌
	* - Big5-HKSCS:1999
	  - No
	  - Yes
	  - No ❌
	* - Big5-HKSCS:2001
	  - No
	  - Yes
	  - No ❌
	* - Big5-HKSCS:2004
	  - No
	  - Yes
	  - No ❌
	* - GBK
	  - No
	  - Yes
	  - :doc:`Yes ✅ </api/encodings/gbk>`
	* - HZ
	  - ❓ Unresearched
	  - ❓ Unconfirmed
	  - No ❌
	* - ISO-2022-JP
	  - ❓ Unresearched
	  - ❓ Unconfirmed
	  - No ❌
	* - ISO-2022-JP-2
	  - ❓ Unresearched
	  - ❓ Unconfirmed
	  - No ❌
	* - ISO-2022-JP-1
	  - ❓ Unresearched
	  - ❓ Unconfirmed
	  - No ❌
	* - ISO-2022-JP-3
	  - ❓ Unresearched
	  - ❓ Unconfirmed
	  - No ❌
	* - ISO-2022-JP-MS
	  - ❓ Unresearched
	  - ❓ Unconfirmed
	  - No ❌
	* - ISO-2022-CN
	  - ❓ Unresearched
	  - ❓ Unconfirmed
	  - No ❌
	* - ISO-2022-CN-EXT
	  - ❓ Unresearched
	  - ❓ Unconfirmed
	  - No ❌
	* - ISO-2022-KR
	  - ❓ Unresearched
	  - ❓ Unconfirmed
	  - No ❌
	* - VISCII
	  - ❓ Unresearched
	  - ❓ Unconfirmed
	  - No ❌
	* - ARMSCII-8
	  - ❓ Unresearched
	  - ❓ Unconfirmed
	  - No ❌
	* - TCVN
	  - ❓ Unresearched
	  - ❓ Unconfirmed
	  - No ❌
	* - PT154
	  - ❓ Unresearched
	  - ❓ Unconfirmed
	  - No ❌
	* - TIS-620
	  - ❓ Unresearched
	  - ❓ Unconfirmed
	  - No ❌
	* - MuleLao-1
	  - ❓ Unresearched
	  - ❓ Unconfirmed
	  - No ❌
	* - HP-ROMAN8
	  - ❓ Unresearched
	  - ❓ Unconfirmed
	  - No ❌
	* - NEXTSTEP
	  - ❓ Unresearched
	  - ❓ Unconfirmed
	  - No ❌
	* - Georgian-Academy
	  - ❓ Unresearched
	  - ❓ Unconfirmed
	  - No ❌
	* - Georgian-PS
	  - ❓ Unresearched
	  - ❓ Unconfirmed
	  - No ❌

If you know of an encoding not listed here, let us know in the issue tracker!
