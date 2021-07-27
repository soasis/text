.. =============================================================================
..
.. ztd.text
.. Copyright © 2021 JeanHeyd "ThePhD" Meneide and Shepherd's Oasis, LLC
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
.. 		https://www.apache.org/licenses/LICENSE-2.0
..
.. Unless required by applicable law or agreed to in writing, software
.. distributed under the License is distributed on an "AS IS" BASIS,
.. WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
.. See the License for the specific language governing permissions and
.. limitations under the License.
..
.. =============================================================================>

contains_unicode_encoding
=========================

This function determines whether or not the type is or contains a unicode encoding. This means any encoding wherein the entirety of Unicode, all 21 bits, can be represented without loss of information. For a full list of encodings which are considered Unicode Encodings by this library, see :doc:`the Known Unicode Encodings list</known unicode encodings>`.

This function checks for 2 things.

- It checks to see if the call ``encoding.contains_unicode_encoding()`` is well-formed and returns a boolean value. If this is the case, it calls ``encoding.contains_unicode_encoding()`` and returns that value.
- It looks to see if the provided ``encoding`` has a member type called ``::is_unicode_encoding``. If this is the case, then it returns ``is_unicode_encoding_v<Type>``.

If none of these work, then it returns ``false``.

.. doxygenfunction:: ztd::text::contains_unicode_encoding
