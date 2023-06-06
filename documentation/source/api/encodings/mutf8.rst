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

Modified UTF-8
==============

Modified Unicode Transformation Format 8 (MUTF-8) is a UTF-8 format employed by some Android components and other ecosystems. It's special property is that it encodes the NULL character in C-style strings (``'\0'``) as an overlong sequence. This is normally illegal in UTF-8, but allowed here to allow easier interoperation with these systems.



Aliases
-------

.. doxygenvariable:: ztd::text::mutf8

.. doxygentypedef:: ztd::text::mutf8_t


Base Template
-------------

.. doxygenclass:: ztd::text::basic_mutf8
	:members:
