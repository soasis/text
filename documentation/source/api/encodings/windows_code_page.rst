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

Windows Code Page
=================

``WideCharToMultiByte`` and ``MultiByteToWideChar`` on Windows platforms has a multitude of encodings with a particular set of quirks. Therefore, on Windows platforms, it may be advantageous to use this over the regular :cpp:class:`execution_encoding_t` type or :cpp:var:`execution_encoding` object. The constructor takes a raw ``int32_t`` whose sole purpose is to be one of the known `Code Page Identifiers`_ that Microsoft supplies on its platforms.

.. warning::

	This type is inefficient due to the poor design of the ``WideCharToMultiByte``/``MultiByteToWideChar`` functions. While this will allow users to achieve parity, if the user knows their encoding ahead of time and can provide either their own encoding object or :doc:`one of the encoding types provided by this library </api//encodings>`, it is highly preferred over this methodology.

.. warning::

	For certain distributions, specific language packs may need to be installed in order to use certain conversions tied to specific `Code Page Identifiers`_.

Alias
-----

.. doxygentypedef:: ztd::text::windows_code_page



Base Templates
--------------

.. doxygenclass:: ztd::text::basic_windows_code_page
	:members:


.. _Code Page Identifiers: https://learn.microsoft.com/en-us/windows/win32/intl/code-page-identifiers
