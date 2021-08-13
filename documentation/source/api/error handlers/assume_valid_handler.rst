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

assume_valid_handler
====================

The ``assume_valid_handler`` is a Undefined-Behavior invoking error handler. If an error is encountered, the encoding can legally ignore and never, ever call the error handler at all. This can invoke |ub| on malformed input.

.. warning::

	⚠️ This should **only ever be used on the most trusted of input, ever, and that input should never come from a source that is a user or connected to ANY external input sources such as the Network, Shared Pipe, Inter-Procedural Call, or similar**. 


Implementers of encodings within templates can check for a potentially ignorable error handler like this one using :doc:`ztd::text::is_ignorable_error_handler_v </api/is_ignorable_error_handler>`.

.. doxygenvariable:: ztd::text::assume_valid_handler

.. doxygenclass:: ztd::text::assume_valid_handler_t
	:members:
