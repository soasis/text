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

Injective: Promoting Safety in Encodings
========================================

As detailed in the :doc:`/design/error handling/lossy protection` section, ``is_encode_injective`` and ``is_decode_injective`` help the library understand when a conversion you are doing cannot be guaranteed at compile time to be lossless. :term:`Injectivity <injective>` is a high-brow mathematical term:

.. epigraph::

	In mathematics, an injective function (also known as injection, or one-to-one function) is a function that maps distinct elements of its domain to distinct elements of its codomain.

	-- `Wikipedia, February 2nd, 2021 <https://en.wikipedia.org/wiki/Injective_function>`_


This is very fancy speak for the fact that for every complete, well-formed input value, there is a well-formed, distinct output value. It does not have to cover all of the potential output values: so long as there is a one-to-one mapping that is unambigious for all the input values, it is injective. For practical purposes, it means that all of the code unit sequences that are valid can produce a unique code point sequence ("the decode operation is injective"). And, in the reverse case, it means that all the code point sequences that are valid can produce a unique code unit sequence ("the encode operation is injective").

These two properties appear on the type itself, and is a way to opt-in to saying that a conversion is not lossy (e.g., it preserves information perfectly if the input is well-formed). You can define them by placing them on your Encoding Object Type's definition:

.. code-block:: cpp
	:linenos:
	:emphasize-lines: 2-3

	struct any_unicode_byte_encoding {
		using is_decode_injective = std::true_type;
		using is_encode_injective = std::true_type;
		using code_unit = std::byte;
		using code_point = ztd::text::unicode_scalar_value;
		// …
	};

This signals that the ``encode_one`` and ``decode_one`` functions — if they are given well-formed input — will never be lossy between their ``code_point`` type and their ``code_unit`` types when performing the desired operation. If only one half of that equation is lossy, then you can mark only one, or the other. For example, :doc:`ztd::text::ascii </api/encodings/ascii>` is lossy only in for the ``encode_one`` operation, so it has ``is_decode_injective = std::true_type;`` for ``decode`` operations, but ``is_encode_injective = std::false_type;`` for ``encode`` operations:

.. literalinclude:: /../../include/ztd/text/ascii.hpp
	:language: cpp
	:linenos:
	:dedent: 1
	:lines: 74-76,95-107,273
	:emphasize-lines: 4-16

If the type definition is not present and is not ``std::true_type``, then the implementation assumes that this is false for a given encoding. See :doc:`ztd::text::is_decode_injective </api/is_decode_injective>` and :doc:`ztd::text::is_encode_injective </api/is_encode_injective>` for more information.
