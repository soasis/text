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

is_(bitwise\ _)transcoding_compatible
=====================================

This classification checks if two encodings are compatible, or bitwise compatible. The heuristic for normal compatibility is simple:

- it checks if the two encodings are identical;
- it checks if the two encodings are near-identical derivations of one another (e.g., :doc:`UTF-8 </api/encodings/utf8>` being converted to :doc:`MUTF-8 </api/encodings/mutf8>` (but not in the other direction)); or,
- it checks if the code point types between the two encodings are the same, or if they are :doc:`both some form of unicode code point </api/is_unicode_code_point>`.

This type specifically uses the first type as the ``From`` encoding (e.g., the one to decode the input code unit sequence) and the second type as the ``To`` encoding (e.g., the one to encode the intermediate decoded code point sequence).

.. note::

	|specializations_okay_true_false_type|

.. warning::

	⚠️ Specializing this type for types which are not either transcoding compatible or bitwise compatible can result in |ub| within the library.

.. doxygenclass:: ztd::text::is_transcoding_compatible
	:members:

.. doxygenvariable:: ztd::text::is_transcoding_compatible_v

.. doxygenclass:: ztd::text::is_bitwise_transcoding_compatible
	:members:

.. doxygenvariable:: ztd::text::is_bitwise_transcoding_compatible_v
