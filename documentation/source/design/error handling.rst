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

Error Handling
==============

Text is notorious for being a constant and consistent malformed source of input. From intermediate services mangling encodings and producing :term:`Mojibake <mojibake>` to bungled normalization and bad programs not understanding even the slightest hint of code beyond ASCII, there is a lot of text data that is strictly bad for any program to consume.

When interfacing with range types such as :doc:`ztd::text::decode_view </api/views/encode_view>`, functions like :doc:`ztd::text::transcode </api/conversions/transcode>`, and individual ``.encode_one`` or ``.decode_one`` calls on encoding objects like :doc:`ztd::text::utf8 </api/encodings/utf8>`, you can:

- give an error handler type as a template parameter and as part of the constructor; or,
- pass it in as a normal argument to the function to be used.

They can change the conversion and other operations happen works. Consider, for example, this piece of code which translates from Korean UTF-8 to ASCII:

.. literalinclude:: /../../examples/documentation/compile_fails/source/error_handler.replacement.ascii.cpp
	:language: cpp
	:linenos:
	:start-after: // ============================================================================>

Clearly, the Korean characters present in the UTF-8 string just cannot fit in a strict, 7-bit ASCII encoding. What, then, becomes the printed output from ``std::cout`` at ``// (2)``? The answer is two ASCII question marks, ``??``. The :doc:`ztd::text::replacement_handler_t </api/error handlers/replacement_handler>` object passed in at ``// (1)`` substitutes replacement characters (zero or more) into the output for any failed operation. There are multiple kinds of error handlers with varying behaviors:

- :doc:`replacement_handler_t </api/error handlers/default_handler>`, which inserts a substitution character specified by either the encoding object or some form using the default replacement character ``"U+FFFD"``;
- :doc:`pass_handler </api/error handlers/pass_handler>`, which simply returns the error result as it and, if there is an error, halts higher-level operations from proceeding forward;
- :doc:`default_handler </api/error handlers/default_handler>`, which is just a name for the ``replacement_handler_t`` or ``throw_handler`` or some other type based on compile time configuration of the library;
- :doc:`throw_handler </api/error handlers/throw_handler>`, for throwing an exception on any failed operation;
- :doc:`incomplete_handler </api/error handlers/incomplete_handler>`, for throwing an exception on any failed encode/decode operation; and,
- :doc:`assume_valid_handler </api/error handlers/throw_handler>`, which triggers no checking for many error conditions and can leads to |ub| if used on malformed input.


.. warning::

	⚠️ For the love of what little remains holy, PLEASE don't use ``ztd::text::assume_valid_handler`` unless you REALLY know you need it. It is a surefire way to open up vulnerabilities in your text processing algorithm. Not a single line of code using this type should pass code review if there is even the slightest thought that this will be used on any input that is not PERFECTLY under the DIRECT, PERSONAL control of the authors, auditors, and maintainers of the code.

These are all the error handlers that you have at your disposal, but they are just pre-provided types you can instantiate yourself. Nothing stops you from making your own error handling type! In order to do that, however, you need to understand what an error handler is composed of, and what it's got inside of itself.

.. toctree::
	:maxdepth: 1

	error handling/error handler anatomy
	error handling/writing a handler
	error handling/lossy protection
