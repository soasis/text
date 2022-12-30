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

unicode_code_point
==================

If :ref:`ZTD_TEXT_UNICODE_CODE_POINT_DISTINCT_TYPE <config-ZTD_TEXT_UNICODE_CODE_POINT_DISTINCT_TYPE>` is turned on, this type definition points to an internal class which implements the invariant of being a :term:`unicode code point <unicode code point>`. Otherwise, it is simply ``char32_t``.

A unicode code point is stored as an at least 32-bit value, but may occupy more space depending on the architecture. It requires 21 bits of space to fit the required unicode code point definition. If the distinct type is used, then this type will trap (``abort``/``assert``) if the value is greater than the allowed 21 bits.

.. doxygentypedef:: ztd::text::unicode_code_point



Internal Type
-------------

.. warning::

	|reserved_name|

.. doxygenclass:: ztd::text::__txt_impl::__unicode_code_point
	:members:
