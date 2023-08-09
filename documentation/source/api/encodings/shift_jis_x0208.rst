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

SHIFT-JISX0208
==============

The version of SHIFT-JIS that corresponds to standard X0208, one of the more prevalent versions.

Note that many versions of SHIFT-JIS do not correspond to one standard and often have different interpretations or characteristics. The communities which use them label them, indiscriminately, as SHIFT-JIS without any kind of specific indicator or even out-of-band modifier. The text community surrounding this is, with all due respect, one gigantic mess. Most industry professionals inside and outside of Japan dealing with such text tend to gravitate towards the SHIFT-JISX0208 release, and simply use replacement characters / invalid indicators for such input text.

As such, it is advisable to perhaps attempt to find some out-of-band data to see if a specific data is, indeed, meant to be SHIFT-JISX0208.



Aliases
-------

.. doxygenvariable:: ztd::text::shift_jis_x0208

.. doxygentypedef:: ztd::text::shift_jis



Base Templates
--------------

.. doxygentypedef:: ztd::text::basic_shift_jis

.. doxygenclass:: ztd::text::basic_shift_jis_x0208
	:members:
