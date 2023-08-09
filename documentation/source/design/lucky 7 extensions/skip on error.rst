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

Skip Input on Error
===================

Some encodings know how to find the next valid sequence after they encounter an error. Many of these encodings are :doc:`self synchronizing codes </api/is_self_synchronizing_code>`, but yet still others may have enough information on their (encode/decode) state to skip additional invalid input. This is where the function :doc:`ztd::text::skip_input_error </api/skip_input_error>` comes in, and the Lucky 7 Extension hook that allows an encoding to define the function. There are 2 versions of this function that are optional and can be written by an encoding author:

- ``decode_result<…> skip_input_error(decode_result<…> result)``, which is meant to skip over bad input from a failed decode operation;
- and, ``encode_result<…> skip_input_error(encode_result<…> result)``, which is meant to skip over bad input from a failed encode operation.

They can appear in a normal encoding like so:

.. literalinclude:: /../../examples/documentation/snippets/include/my_encoding.hpp
	:linenos:
	:start-after: // ============================================================================ //
	:emphasize-lines: 32-

The :doc:`ztd::text::decode_result </api/decode_result>` and :doc:`ztd::text::encode_result </api/encode_result>` contain all of the necessary information to perform the skip (``input``, ``output``, ``error_code``, ``state``, and ``error_count`` fields) that could be needed. For example, on a hypothetical UTF-16 encoding, one could define a ``skip_input_error`` function that behaves like so:

.. literalinclude:: /../../examples/documentation/snippets/include/my_utf16.hpp
	:language: cpp
	:linenos:
	:start-after: // start my_utf16-snippet
	:emphasize-lines: 39-
	

Assuming, briefly, that this was all put into a file called ``my_utf16.hpp``, it can be used like this:

.. literalinclude:: /../../examples/documentation/snippets/source/design.skip_input_error.cpp
	:language: cpp
	:linenos:
	:start-after: // ============================================================================ //

This will come in handy when defining other Unicode variants that may need to skip multiple bits of bad input rather than juts passing over 1 code unit. :doc:`ztd::text::replacement_handler </api/error handlers/replacement_handler>` and :doc:`ztd::text::skip_handler </api/error handlers/skip_handler>` will both try to use these extension points to skip over bad input.
