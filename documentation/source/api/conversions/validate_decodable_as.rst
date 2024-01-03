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

validate_decodable_as
=====================

``ztd::text::validate_decodable_as`` is a function that takes an input sequence of ``code_unit``\ s and attempts to validate that they can be turned into the ``code_point``\ s of the provided encoding. Unlike the :doc:`ztd::text::count_as_decoded </api/conversions/validate_decodable_as>` function, this does not take an error handler. **Any** error, even if it would be corrected over, produces a stop in the algorithm and a :doc:`validate_result </api/validate_result>`/:doc:`stateless_validate_result </api/stateless_validate_result>` object gets returned with the ``.valid`` member set to false.

The overloads of this function increase the level of control with each passed argument. At the last overload with four arguments, the function attempts to work call some extension points or falls back to the base function call in this order:

- The ``text_validate_decodable_as(input, encoding, decode_state)`` extension point, if possible.
- The ``text_validate_decodable_as(input, encoding, decode_state, encode_state)`` extension point, if possible.
- An internal, implementation-defined customization point.
- The ``basic_validate_decodable_as`` base function.

The base function call, ``basic_validate_decodable_as``, simply performs the :doc:`core validating loop </design/converting/validate decodable>` using the :doc:`Lucky 7 </design/lucky 7>` design. The reason the last overload takes 2 state arguments is due to how the base implementation works from the core validating loop. If during the 3-argument overload it is detected that ``text_validate_decodable_as(input, encoding, decode_state)`` can be called, it will be called without attempt to create an ``encode_state`` value with :doc:`ztd::text::make_encode_state(...) </api/make_encode_state>`.

During the ``basic_validate_decodable_as`` loop, if it detects that there is a preferable ``text_validate_decodable_as_one``, it will call that method as ``text_validate_decodable_as_one(input, encoding, decode_state)`` inside of the loop rather than doing the core design.

The :doc:`ztd::text::validate_result </api/validate_result>` type only includes the ``decode_state`` in all cases.

.. note::

	👉 This means that if you implement none of the extension points whatsoever, implementing the basic ``decode_one`` and ``encode_one`` functions on your Encoding Object type will guarantee a proper, working implementation.

.. note::

	👉 If you need to call the "basic" form of this function that takes no secret implementation shortcuts or user-defined extension points, then call ``basic_validate_decodable_as`` directly. This can be useful to stop infinity loops when your extension points cannot handle certain inputs and thereby needs to "delegate" to the basic case.



~~~~~~~~~~~~



Functions
---------

.. doxygengroup:: ztd_text_validate_decodable_as
	:content-only:
