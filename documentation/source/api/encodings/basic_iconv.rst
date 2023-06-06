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

basic_iconv
===========

This encoding is only available if the configuration macro/build option for ``ZTD_PLATFORM_LIBICONV`` is turned on.

This encoding is tied to the `iconv library <https://www.gnu.org/software/libiconv/>`_. It will attempt to use the header and the functions directly, and if not otherwise bootstrap iconv on first use of the encoding through ``GetProcAddress``/``dlsym`` and related. If it cannot find it will either assert, abort, or loudly annoy the user in some way. The code is retrieved dynamically where possible unless the user explicitly defines the build option for ``ZTD_TEXT_USE_STATIC_LIBICONV`` (for CMake), as iconv is under a LGPL/GPL licensed and cannot be traditionally built / statically linked with application code (though in the future we may provide a way for software to do that if the software being made with this library is also GPL-compatible software).

iconv has a fixed set of encodings it can be compiled with to support. States are pre-constructed in the encoding itself and copied as necessary when ``encode_state`` or ``decode_state``\ s are being created to call the iconv functions. The user can inspect the output error parameter from the ``basic_iconv`` constructor to know of failure, or not pass in the output error parameter and instead take one of a assert, thrown exception, or ``abort`` (preferred invocation in that order).



Base Template
-------------

.. doxygenclass:: ztd::text::basic_iconv
