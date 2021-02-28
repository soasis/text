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
..		http:..www.apache.org/licenses/LICENSE-2.0
..
.. Unless required by applicable law or agreed to in writing, software
.. distributed under the License is distributed on an "AS IS" BASIS,
.. WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
.. See the License for the specific language governing permissions and
.. limitations under the License.
..
.. =============================================================================>

encode
======

The ``encode`` grouping of functions (``encode``, ``encode_to``, and ``encode_into``) perform the task of doing bulk decoding from an ``input`` of ``code_point``\ s to the encoding's ``code_unit`` type.



Named Groups
------------

There are 3 named functions for this behavior, and each function comes with several function overloads. Each named function produces increasingly more information, letting you opt into just how much information and control you'd like over the algorithm and behavior. The first one simply returns a container with the transformation applied, discarding much of the operation's result information. This is useful for quick, one-off conversions where you do not care about any errors and would rather let it be handled by the error handler. The second ``_to`` suffixed functions return a container within a ``result`` type that contains additional information. The final ``_into`` suffixed functions take an output range to write into, letting you explicitly control just how much space there is to write into as well as returning a detailed ``result`` type.

The return type for these function calls is one of:

- the desired output container (highest level);
- :doc:`ztd::text::encode_result </api/encode_result>` or :doc:`ztd::text::stateless_encode_result </api/encode_result>` with the desired output container embedded as the `.output` parameter (mid level); or,
- :doc:`ztd::text::encode_result </api/encode_result>` or :doc:`ztd::text::stateless_encode_result </api/encode_result>` returning just the input and output ranges (lowest level).


``encode(...)``
+++++++++++++++

This is the highest level bulk function.

This set of function overloads takes the provided ``input``, ``encoding``, ``handler`` and ``state`` and produces an output container type. The default container will either be a ``std::basic_string`` of the ``code_unit`` type, or a ``std::vector`` if it is not a known "character" type.

The container type can be specified by passing it as an explicit template parameter to this function, such as ``ztd::text::encode<std::vector<std::byte>>(U"bark", ztd::text::utf16_be{});``. The output container is default constructed.

It will either call ``push_back``/``insert`` directly on the target container to fill it up, or serialize data to a temporary buffer (controlled by :ref:`ZTD_TEXT_INTERMEDIATE_BUFFER_SIZE <config-ZTD_TEXT_INTERMEDIATE_BUFFER_SIZE>`) before then copying it into the desired output container through any available means (bulk ``.insert``, repeated ``.push_back``, or repeated single ``.insert`` with the ``.cend()`` iterator in that order).

This is the "fire and forget" version of the ``encode`` function, returning only the container and not returning any of the result or state information used to construct it.


``encode_to(...)``
++++++++++++++++++

This is the mid level bulk function.

This set of function overloads takes the provided ``input``, ``encoding``, ``handler`` and ``state`` and produces an output container type that is embedded within a :doc:`ztd::text::encode_result </api/encode_result>`, or a :doc:`ztd::text::stateless_encode_result </api/stateless_encode_result>`, depending on whether or not you called the version which takes a :doc:`ztd::text::encode_state_t\<Encoding\> </api/encode_state>` as a parameter or if it had to create one on the stack internally and discard it after the operation was finished.

The container type can be specified by passing it as an explicit template parameter to this function, such as ``ztd::text::encode_to<std::string>(U"meow", ascii{});``. The output container is default constructed.

It will either call ``push_back``/``insert`` directly on the target container to fill it up, or serialize data to a temporary buffer (controlled by :ref:`ZTD_TEXT_INTERMEDIATE_BUFFER_SIZE <config-ZTD_TEXT_INTERMEDIATE_BUFFER_SIZE>`) before then copying it into the desired output container through any available means (bulk ``.insert``, repeated ``.push_back``, or repeated single ``.insert`` with the ``.cend()`` iterator in that order).

If nothing goes wrong or the error handler lets the algorithm continue, ``.input`` on the result should be empty.


``encode_into(...)``
++++++++++++++++++++

This is the lowest level bulk function.

This set of function overloads takes the provided ``input``, ``encoding``, ``output``, ``handler``, and ``state`` and writes data into the output range specified by ``output``. The result is a :doc:`ztd::text::encode_result </api/encode_result>`, or a :doc:`ztd::text::stateless_encode_result </api/stateless_encode_result>`, depending on whether or not you called the version which takes a :doc:`ztd::text::encode_state_t\<Encoding\> </api/encode_state>` as a parameter or if it had to create one on the stack internally and discard it after the operation was finished.

It is up to the end-user to provide a suitably-sized output range for ``output``, otherwise this operation may return with :doc:`ztd::text::encoding_error::insufficient_output </api/encoding_error>`. for the ``result``\ 's ``error_code`` member. The amount of space consumed can be determined by checking the ``std::distance`` between the ``.begin()`` of the original ``output`` parameter and the ``.begin()`` of the returned ``.output`` member. The result also has error information and an ``.input`` member for checking how much input was consumed.

If nothing goes wrong or the error handler lets the algorithm continue, ``.input`` on the result should be empty.



For Everything
--------------

All named functions have 4 overloads. Each of the "higher level" functions, at the end of their overload call chain, will call the lower-level ``encode_into`` to perform the work. The final ``encode_into`` call uses the following ordering of extension points into calling the base implementation:

- ``text_encode_into(input, encoding, output, handler, state)``
- An internal, implementation-defined customization point.
- ``basic_encode_into(input, encoding, output, handler, state)``

The final function call, ``basic_encode_into``, simply performs the :doc:`core encode loop </design/converting/encode>` using the :doc:`Lucky 7 </design/lucky 7>` design. This design also means minimal stack space is used, keeping the core algorithm suitable for resource-constrained devices.

.. note::

	👉 This means that if you implement none of the extension points whatsoever, implementing the basic ``encode_one`` function on your Encoding Object type will guarantee a proper, working implementation.

.. note::

	👉 If you need to call the "basic" form of this function that takes no secret implementation shortcuts or user-defined extension points, then call ``basic_encode_into`` directly. This can be useful to stop infinity loops when your extension points cannot handle certain inputs and thereby needs to "delegate" to the basic case.



~~~~~~~~~



Functions
---------

.. doxygengroup:: ztd_text_encode
	:content-only:
