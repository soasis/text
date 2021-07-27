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

count_encodable
=================

``ztd::text::count_encodable`` is a function that takes an input sequence of ``code_point``\ s and attempts to count them, according to the error handler that is given. Because the error handler is included as part of the function call (and is provided by default is one is not passed in), the count operation will also continue to count if the error handler sets the ``error_code`` member of the result to ``ztd::text::encoding_error::ok`` but still performs some action. This is, for example, the case with :doc:`ztd::text::replacement_handler </api/error handlers/replacement_handler>` - output replacement code units or code points will be counted as part of the final count and returned with ``result.error_code == ztd::text::encoding_error::ok``. You can differentiate error-less text from non-error text by checking ``result.errors_were_handled()``, which will be true if the error handler is called regardless of whether or not the error handler "smooths" the problem over by inserting replacement characters, doing nothing, or otherwise.

The overloads of this function increase the level of control you have with each passed argument. At the last overload with four arguments, the function attempts to work call some extension points or falls back to the base function call in this order:

- The ``text_count_encodable(input, encoding, handler, state)`` extension point, if possible.
- An internal, implementation-defined customization point.
- The ``basic_count_encodable`` base function.

The base function call, ``basic_count_encodable``, simply performs the :doc:`core counting loop </design/converting/count encodable>` using the :doc:`Lucky 7 </design/lucky 7>` design.

During the ``basic_count_encodable`` loop, if it detects that there is a preferable ``text_count_encodable_one``, it will call that method as ``text_count_encodable_one(input, encoding, handler, state)`` inside of the loop rather than doing the core design.

.. note::

	👉 This means that if you implement none of the extension points whatsoever, implementing the basic ``encode_one`` function on your Encoding Object type will guarantee a proper, working implementation.

.. note::

	👉 If you need to call the "basic" form of this function that takes no secret implementation shortcuts or user-defined extension points, then call ``basic_count_encodable`` directly. This can be useful to stop infinity loops when your extension points cannot handle certain inputs and thereby needs to "delegate" to the basic case.



~~~~~~~~~~~~



Functions
---------

.. doxygengroup:: ztd_text_count_encodable
	:content-only:
