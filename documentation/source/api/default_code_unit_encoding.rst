.. =============================================================================
..
.. ztd.text
.. Copyright © 2022 JeanHeyd "ThePhD" Meneide and Shepherd's Oasis, LLC
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

default_code_unit_encoding
==========================

Picks the default encoding for the given code unit type (for both run time, and compile time with the appropriately selected version of this property). The default association table is below. Errors if there is no default association.

.. note::

	|specializations_okay_different_types|


.. list-table::
	:header-rows: 1

	* - **Type**
	  - **Encoding**
	* - ``signed char``
	  - :doc:`ztd::text::basic_ascii\<signed char\> </api/encodings/ascii>`
	* - ``char``
	  - :doc:`ztd::text::execution </api/encodings/execution>`
	* - ``char`` (compile time)
	  - :doc:`ztd::text::literal </api/encodings/literal>`
	* - ``wchar_t``
	  - :doc:`ztd::text::wide_execution </api/encodings/wide_execution>`
	* - ``wchar_t`` (compile time)
	  - :doc:`ztd::text::wide_literal </api/encodings/wide_literal>`
	* - ``char8_t``
	  - :doc:`ztd::text::basic_utf8\<char8_t\> </api/encodings/utf8>`
	* - ``ztd::uchar8_t``
	  - :doc:`ztd::text::basic_utf8\<uchar8_t\> </api/encodings/utf8>`
	* - ``std::byte``
	  - :doc:`ztd::text::basic_utf8\<std::byte\> </api/encodings/utf8>`
	* - ``char16_t``
	  - :doc:`ztd::text::utf16 </api/encodings/utf16>`
	* - ``char32_t``
	  - :doc:`ztd::text::utf32 </api/encodings/utf32>`


.. doxygenclass:: ztd::text::default_code_unit_encoding
	:members:

.. doxygentypedef:: ztd::text::default_code_unit_encoding_t

.. doxygenclass:: ztd::text::default_consteval_code_unit_encoding
	:members:

.. doxygentypedef:: ztd::text::default_consteval_code_unit_encoding_t
