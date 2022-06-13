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

Execution
=========

This is the locale-based, runtime encoding. It uses a number of compile-time and runtime heuristics to eventually be resolved to an implementation-defined encoding. It is not required to work in constant expressions either: for this, use :doc:`ztd::text::literal </api/encodings/literal>`, which represents the compile-time string (e.g. ``"my string"``) encoding.

Currently, the hierachy of behaviors is like so:

- If the platform is MacOS, then it assumes this is :doc:`UTF-8 </api/encodings/utf8>`;
- Otherwise, if libiconv is available, then it attempts to use :doc:`iconv </api/encodings/basic_iconv>` configured to the ``"char"``-identified encoding;
- Otherwise, if the headers ``<cuchar>`` or ``<uchar.h>`` are available, then it attempts to use a gnarly, lossy, and dangerous encoding that potentially traffics through the C Standard Library and Locale APIs;
- Otherwise, it produces a compile-time error.

.. warning::

	The C Standard Library has many design defects in its production of code points, which may make it unsuitable even if your C Standard Library recognizes certain locales (e.g., Big5-HKSCS). The runtime will always attempt to load ``iconv`` if the definition is turned on, since it may do a better job than the C Standard Library's interfaces until C23.

Even if, on a given platform, it can be assumed to be a static encoding (e.g., Apple/MacOS where it always returns the "C" Locale but processes text as UTF-8), ``ztd::text::execution`` will always present itself as a runtime and unknowable encoding. This is to prevent portability issues from relying on, e.g., ``ztd::text::is_decode_injective_v<ztd::text::execution>`` being true during development and working with that assumption, only to have it break when ported to a platform where that assumption no longer holds.

.. doxygenvariable:: ztd::text::execution

.. doxygentypedef:: ztd::text::execution_t



Internal Types
--------------

.. warning::

	|reserved_name|


.. ``<cuchar>``-based
.. ++++++++++++++++++
.. 
.. .. doxygenclass:: ztd::text::__txt_impl::__execution_cuchar
.. 	:members:

MacOS-based
+++++++++++

.. doxygenclass:: ztd::text::__txt_impl::__execution_mac_os
	:members:
