.. =============================================================================
..
.. ztd.text
.. Copyright © JeanHeyd "ThePhD" Meneide and Shepherd's Oasis, LLC
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

Single-byte Encodings / Lookup Encodings
========================================

These encodings are single-byte encodings that contain no inherent complexity and are driven by high-bit-1, 8-bit lookup tables or direct 8-bit lookup tables. They are all listed below for brevity.

Known Encodings
---------------

.. doxygenvariable:: ztd::text::atari_st

.. doxygenvariable:: ztd::text::atascii

.. doxygenvariable:: ztd::text::ibm_424_hebrew_bulletin

.. doxygenvariable:: ztd::text::ibm_856_hebrew

.. doxygenvariable:: ztd::text::ibm_866_cyrillic

.. doxygenvariable:: ztd::text::ibm_1006_urdu

.. doxygenvariable:: ztd::text::iso_8859_1_1985

.. doxygenvariable:: ztd::text::iso_8859_1_1998

.. doxygenvariable:: ztd::text::iso_8859_1

.. doxygenvariable:: ztd::text::iso_8859_2

.. doxygenvariable:: ztd::text::iso_8859_3

.. doxygenvariable:: ztd::text::iso_8859_4

.. doxygenvariable:: ztd::text::iso_8859_5

.. doxygenvariable:: ztd::text::iso_8859_6

.. doxygenvariable:: ztd::text::iso_8859_7

.. doxygenvariable:: ztd::text::iso_8859_8

.. doxygenvariable:: ztd::text::iso_8859_10

.. doxygenvariable:: ztd::text::iso_8859_13

.. doxygenvariable:: ztd::text::iso_8859_14

.. doxygenvariable:: ztd::text::iso_8859_15

.. doxygenvariable:: ztd::text::iso_8859_16

.. doxygenvariable:: ztd::text::kamenicky

.. doxygenvariable:: ztd::text::kazakh_strk1048

.. doxygenvariable:: ztd::text::koi8_r

.. doxygenvariable:: ztd::text::koi8_u

.. doxygenvariable:: ztd::text::petscii_unshifted

.. doxygenvariable:: ztd::text::petscii_shifted

.. doxygenvariable:: ztd::text::tatar_ansi

.. doxygenvariable:: ztd::text::tatar_ascii

.. doxygenvariable:: ztd::text::windows_437_dos_latin_us

.. doxygenvariable:: ztd::text::windows_865_dos_nordic

.. doxygenvariable:: ztd::text::windows_874

.. doxygenvariable:: ztd::text::windows_1251

.. doxygenvariable:: ztd::text::windows_1252

.. doxygenvariable:: ztd::text::windows_1253

.. doxygenvariable:: ztd::text::windows_1254

.. doxygenvariable:: ztd::text::windows_1255

.. doxygenvariable:: ztd::text::windows_1256

.. doxygenvariable:: ztd::text::windows_1257

.. doxygenvariable:: ztd::text::windows_1258



Base Templates
--------------

.. doxygenclass:: ztd::text::basic_atascii
	:members:

.. doxygenclass:: ztd::text::basic_atari_st
	:members:

.. doxygenclass:: ztd::text::basic_ibm_424_hebrew_bulletin
	:members:

.. doxygenclass:: ztd::text::basic_ibm_856_hebrew
	:members:

.. doxygenclass:: ztd::text::basic_ibm_866_cyrillic
	:members:

.. doxygenclass:: ztd::text::basic_ibm_1006_urdu
	:members:

.. doxygenclass:: ztd::text::basic_iso_8859_1_1985
	:members:

.. doxygenclass:: ztd::text::basic_iso_8859_1_1998
	:members:

.. doxygenclass:: ztd::text::basic_iso_8859_1
	:members:

.. doxygenclass:: ztd::text::basic_iso_8859_2
	:members:

.. doxygenclass:: ztd::text::basic_iso_8859_3
	:members:

.. doxygenclass:: ztd::text::basic_iso_8859_4
	:members:

.. doxygenclass:: ztd::text::basic_iso_8859_5
	:members:

.. doxygenclass:: ztd::text::basic_iso_8859_6
	:members:

.. doxygenclass:: ztd::text::basic_iso_8859_7
	:members:

.. doxygenclass:: ztd::text::basic_iso_8859_8
	:members:

.. doxygenclass:: ztd::text::basic_iso_8859_10
	:members:

.. doxygenclass:: ztd::text::basic_iso_8859_13
	:members:

.. doxygenclass:: ztd::text::basic_iso_8859_14
	:members:

.. doxygenclass:: ztd::text::basic_iso_8859_15
	:members:

.. doxygenclass:: ztd::text::basic_iso_8859_16
	:members:

.. doxygenclass:: ztd::text::basic_kamenicky
	:members:

.. doxygenclass:: ztd::text::basic_kazakh_strk1048
	:members:

.. doxygenclass:: ztd::text::basic_koi8_r
	:members:

.. doxygenclass:: ztd::text::basic_koi8_u
	:members:

.. doxygenclass:: ztd::text::basic_petscii_unshifted
	:members:

.. doxygenclass:: ztd::text::basic_petscii_shifted
	:members:

.. doxygenclass:: ztd::text::basic_tatar_ansi
	:members:

.. doxygenclass:: ztd::text::basic_tatar_ascii
	:members:

.. doxygenclass:: ztd::text::basic_windows_437_dos_latin_us
	:members:

.. doxygenclass:: ztd::text::basic_windows_865_dos_nordic
	:members:

.. doxygenclass:: ztd::text::basic_windows_874
	:members:

.. doxygenclass:: ztd::text::basic_windows_1251
	:members:

.. doxygenclass:: ztd::text::basic_windows_1252
	:members:

.. doxygenclass:: ztd::text::basic_windows_1253
	:members:

.. doxygenclass:: ztd::text::basic_windows_1254
	:members:

.. doxygenclass:: ztd::text::basic_windows_1255
	:members:

.. doxygenclass:: ztd::text::basic_windows_1256
	:members:

.. doxygenclass:: ztd::text::basic_windows_1257
	:members:

.. doxygenclass:: ztd::text::basic_windows_1258
	:members:
