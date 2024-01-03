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

default_handler
===============

The default handler for all operations. A class type that simply wraps :doc:`ztd::text::replacement_handler_t </api/error handlers/replacement_handler>` unless configured otherwise. You can change it to throw by default (NOT recommended) by using :ref:`ZTD_TEXT_DEFAULT_HANDLER_THROWS <config-ZTD_TEXT_DEFAULT_HANDLER_THROWS>`.

Using this type, implicitly or explicitly, signals to ztd.text that you would like it to gently admonish you if any part of a conversion could be potentially lossy (valid data is put in, but it cannot be handled by the desired encode/decode/transcode operation).

.. doxygenvariable:: ztd::text::default_handler

.. doxygenclass:: ztd::text::default_handler_t
	:members:
