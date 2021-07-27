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
.. 		http://..www.apache.org/licenses/LICENSE-2.0
..
.. Unless required by applicable law or agreed to in writing, software
.. distributed under the License is distributed on an "AS IS" BASIS,
.. WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
.. See the License for the specific language governing permissions and
.. limitations under the License.
..
.. =============================================================================>

Lossy Operation Protection
==========================

Occasionally, you will end up in a situation where you want to convert some text from its pristine and ideal Unicode form to some other form. Maybe for interopation purposes, maybe because some function call can't properly handle embedded ``NUL``\ s in the text so you need to :doc:`use an overlong sequence to encode the 0 value in your text </api/encodings/mutf8>`. No matter what the case is, you need to leave the world of :term:`Unicode Code Points <unicode code point>`, :term:`Unicode Scalar Values <unicode scalar value>`, and all the guarantees they provide you. Let's take an example, going from UTF-8 to 7-bit-clean ASCII:

.. literalinclude:: /../../examples/documentation/compile_fails/source/error_handler.replacement.ascii.cpp
	:language: cpp
	:linenos:
	:emphasize-lines: 6-7
	:start-after: // ============================================================================>

This will produce a compile time error (with this error number for MSVC as an example):

.. epigraph::

	error C2338: The encode (output) portion of this transcode is a lossy, non-injective operation. This means you may lose data that you did not intend to lose; specify an 'out_handler' error handler parameter to ``transcode[_to](in, in_encoding, out_encoding, in_handler, out_handler, ...)`` or ``transcode_into(in, in_encoding, out, out_encoding, in_handler, out_handler, ...)`` explicitly in order to bypass this.

The reason this happens is because we can detect, at compile time, that the conversion from Unicode Code Points to ASCII is a lossy transformation. When this happens, we realize the conversion will be a lossy one: therefore, it makes sense that the user cannot perform the encoding or decoding operation without being explicit about how they are going to handle errors because there is such a gigantically enormous possibility that they will mangle incoming text.

Since this library is trying to prevent :term:`Mojibake <mojibake>` and other encoding problems, you are required to tag any potentially-lossy encoding with an error handler, to be explicit and acknowledge that you may or may not be ruining someone's day:

.. literalinclude:: /../../examples/documentation/snippets/source/error_handler.replacement.ascii.cpp
	:language: cpp
	:linenos:
	:emphasize-lines: 15-16
	:start-after: // ============================================================================>

Any encoding which does not meet the requirements of either :doc:`ztd::text::is_encode_injective_v </api/is_encode_injective>` or :doc:`ztd::text::is_decode_injective_v </api/is_decode_injective>` (or both, for transcoding which uses both an encode and a decode operation) will throw an error if you specify no error handlers in the text. This is done through the :doc:`Injectivity Lucky 7 Extensions </design/lucky 7 extensions/injective>` that go beyond the :doc:`traditional Lucky 7 </design/lucky 7>` with 2 ``std::true_type``/``std::false_type`` definitions.
