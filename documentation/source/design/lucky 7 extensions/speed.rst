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

Need for Speed: Extension Points
================================

The core encoding/decoding loops and the :doc:`Lucky 7 design </design/lucky 7>`, while flexible, can come with performance degradation due to its one-by-one nature. There are many well-researched speedups to validating, counting, and converting UTF and other kinds of text. In order to accommodate these, ztd.text has a number of places to overload the core behavior by way of named Argument Dependent Lookup (ADL or Koenig Lookup, named after Andrew Koenig) functions that serve as extension points. They are listed, with their expected argument forms / counts, here.



Extension points: Arguments
---------------------------

For all extension points, arguments are given based on what was input to one of the original higher-level functions. They have these forms and general requimrents:

- ``tag`` - The first argument to every extension point that takes a single encoding. The ``tag`` type is ``ztd::tag<...>`` with any ``const``, ``volatile``, or references (``&`` and ``&&``) removed from the ``decltype`` of the ``encoding``.
- ``duo_tag`` - The first argument to every extension point that takes 2 encodings. The ``tag`` type is ``ztd::tag<...>`` with any ``const``, ``volatile``, or references (``&`` and ``&&``) removed from the ``decltype`` of the two ``encoding``\ s.
- ``encoding`` - The encoding used to perform the operation. Can be prefixed with ``from_`` or ``to_`` in the argument list to show it is one of two encodings used to perform e.g. a transcode operation.
- ``input`` - The input range. Can be of any type. Most encodings should at the very least handle basic iterator-iterator pairs correctly. These are allowed to have ``const``-correct iterators that produce ``const``-correct references, so never assume you can write to the input, and appropriately ``const``-qualify any ``std::span``\ s you use.
- ``output`` - The output range. Can be of any output range type, such as a ``unbounded_view<>`` with a ``back_inserter`` or a ``std::span`` for direct memory writes. The types only requirement is that you can write to it by getting an iterator from ``begin(...)``, and calling ``*it = value;``.
- ``handler`` - The error handler used to perform the operation. Can be prefixed with ``from_`` or ``to_`` in the argument list to show it is one of two error handlers used to perform e.g. a transcode operation.
- ``state`` - The state objects used to perform the operation. States are always passed by non-\ ``const``, l-value reference. Can be prefixed with ``from_`` or ``to_`` in the argument list to show it is one of two states associated with an encoding with the same prefix.
- ``pivot`` - The :doc:`pivot range </api/pivot>` that can be used for any intermediary storage; should be used in place of allocating data within a routine so an end-user can avoid allocation if he/she desires for any intermediate productions.



Extension Points: Forms & Return Types
--------------------------------------

Overriding any one of these extension points allows you to hook that behavior. It is very much required that you either use concrete types to provide these ADL extension points, or heavily constrain them using `SFINAE (preferred for C++17 and below) <https://en.cppreference.com/w/cpp/language/sfinae>`_ or `Concepts (only C++20 and above) <https://en.cppreference.com/w/cpp/concepts>`_.


``text_decode``
+++++++++++++++

Form: ``text_decode(tag, input, encoding, output, handler, state)``.

An extension point to speed up decoding operations for a given encoding, its input and outpuut ranges, and the associated error handler and state. This can be helpful for encodings which :doc:`may need to hide certain parts of their state </design/lucky 7 extensions/state>`.

Must return a :doc:`ztd::text::decode_result </api/decode_result>`.


``text_encode``
+++++++++++++++

Form: ``text_encode(input, encoding, output, handler, state)``.

An extension point to speed up encoding operations for a given encoding, its input and outpuut ranges, and the associated error handler and state. This can be helpful for encodings which :doc:`may need to hide certain parts of their state </design/lucky 7 extensions/state>`.

Must return a :doc:`ztd::text::encode_result </api/encode_result>`.


``text_transcode``
++++++++++++++++++

Form: ``text_transcode(input, from_encoding, output, to_encoding, from_handler, to_handler,`` ``from_state, to_state, pivot)``

An extension point to speed up transcoding in bulk, for a given encoding pair, its input and output ranges, and its error handlers and states. Useful for known encoding pairs that have faster conversion paths between them.

Must return a :doc:`ztd::text::transcode_result </api/transcode_result>`.


``text_transcode_one``
++++++++++++++++++++++

Form: ``text_transcode_one(input, from_encoding, output, to_encoding, from_handler, to_handler,`` ``from_state, to_state)``

An extension point to provide faster one-by-one encoding transformations for a given encoding pair, its input and output ranges, and its error handlers and states. This is **not** a bulk extension point conversion. It is used in the :doc:`ztd::text::transcode_view </api/views/transcode_view>` type to increase the speed of iteration, where possible.

Must return a :doc:`ztd::text::transcode_result </api/transcode_result>`.


``text_validate_encodable_as_one``
++++++++++++++++++++++++++++++++++

Form: ``text_validate_encodable_as_one(input, encoding, state)``

An extension point to provide faster one-by-one validation. Provides a shortcut to not needing to perform both a ``decode_one`` and an ``encode_one`` step during the basic validation loop.

Must return a :doc:`ztd::text::validate_result </api/validate_result>`.


``text_validate_decodable_as_one``
++++++++++++++++++++++++++++++++++

Form: ``text_validate_decodable_as_one(input, encoding, state)``

An extension point to provide faster one-by-one validation. Provides a shortcut to not needing to perform both a ``encode_one`` and an ``decode_one`` step during the basic validation loop.

Must return a :doc:`ztd::text::validate_result </api/validate_result>`.


``text_validate_transcodable_as_one``
+++++++++++++++++++++++++++++++++++++

Form: ``text_validate_transcodable_as_one(input, from_encoding, to_encoding, decode_state, encode_state, pivot)``

An extension point to provide faster one-by-one validation. Provides a shortcut to not needing to perform both a ``encode_one`` and an ``decode_one`` step during the basic validation loop.

Must return a :doc:`ztd::text::validate_transcode_result </api/validate_transcode_result>`.


``text_validate_encodable_as``
++++++++++++++++++++++++++++++

Form: ``text_validate_encodable_as(input, encoding, state)``

An extension point to provide faster bulk code point validation. There are many tricks to speed up validationg of text using bit twiddling of the input sequence and more.

Must return a :doc:`ztd::text::validate_result </api/validate_result>`.


``text_validate_decodable_as``
++++++++++++++++++++++++++++++

Form: ``text_validate_decodable_as(input, encoding, state)``

An extension point to provide faster bulk code unit validation. There are many tricks to speed up validationg of text using bit twiddling of the input sequence and more.

Must return a :doc:`ztd::text::validate_result </api/validate_result>`.


``text_count_as_encoded_one``
+++++++++++++++++++++++++++++

Form: ``text_count_as_encoded_one(input, encoding, handler, state, pivot)``

An extension point to provide faster one-by-one counting. Computation cycles can be saved by only needing to check a subset of things. For example, specific code point ranges can be used to get a count for UTF-16 faster than by encoding into an empty buffer.

Must return a :doc:`ztd::text::count_result </api/count_result>`.


``text_count_as_decoded_one``
+++++++++++++++++++++++++++++

Form: ``text_count_as_decoded_one(input, encoding, handler, state)``

An extension point to provide faster one-by-one counting. Computation cycles can be saved by only needing to check a subset of things. For example, the leading byte in UTF-8 can provide an immediate count for how many trailing bytes, leading to a faster counting algorithm.

Must return a :doc:`ztd::text::count_result </api/count_result>`.


``text_count_as_encoded``
++++++++++++++++++++++++++

Form: ``text_count_as_encoded(input, encoding, handler, state)``

An extension point for faster bulk code point validation.

Must return a :doc:`ztd::text::count_result </api/count_result>`.


``text_count_as_decoded``
+++++++++++++++++++++++++

Form: ``text_count_as_decoded(input, encoding, handler, state)``

An extension point for faster bulk code point validation.

Must return a :doc:`ztd::text::count_result </api/count_result>`.



That's All of Them
------------------

Each of these extension points are important to one person, or another. For example, Daniel Lemire spends a lot of time optimizing :term:`UTF-8 routines for fast validation <Fast UTF-8 Validation>` or :term:`Fast Deterministic Finite Automata (DFA) decoding of UTF-8 and more <Fast UTF-8>`. There are many more sped up counting, validating, encoding, and decoding routines: therefore it is critical that any library writer or application developer can produce those for their encodings and, on occassion, override the base behavior and implementation-defined internal speed up written by ztd.text itself.
