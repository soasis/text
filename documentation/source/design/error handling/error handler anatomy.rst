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

Error Handler Anatomy
=====================

An error handler is just a function (or an object with a function call operator) that takes 3 parameters and returns 1 result:

- takes the encoding that will call it when something goes wrong;
- takes the result object you expect to be working with (specifically, :doc:`ztd::text::encode_result </api/encode_result>` and :doc:`ztd::text::decode_result </api/decode_result>`), which contains the current state of affairs from the encoding operation;
- takes a contiguous range representing any input values that may have been read but will not be used; and,
- returns the same result type with any modifications (or not!) you'd like to make.

They are classes with a function call operator and utilizes a few templates. Here's the skeleton for one:

.. literalinclude:: /../../examples/documentation/source/error_handler.anatomy.cpp
	:language: cpp
	:linenos:
	:start-after: // ============================================================================>

This skeleton, by itself, works. It doesn't do anything: it just returns the ``result`` object as-is. This will result in the algorithm stopping exactly where the error occurs, and returning back to the user. This is because the ``result`` has an ``error_code`` member variable, and that member variable, when it reaches the higher level algorithms, stops all encoding, decoding, transcoding, counting, validation, and etc. work and exists with the proper information.



First Parameter
---------------

The first parameter is simple enough: it is the encoding that is calling this error handler. If you invoke an ``encode_one`` or ``decode_one`` (or a higher-level conversion algorithm) on a :doc:`ztd::text::utf8 </api/encodings/utf8>` object, then you can expect the first parameter of type ``ztd::text::utf8``.

.. note::

	If the function call ``.encode_one`` or ``.decode_one`` is a static function that has no instance, then the encoding object will create a temporary instance to pass to the function. This happens with most encodings that do not contain any pertinent information on the encoding object itself, like all the Unicode encodings and the ASCII/locale/string literal encodings.

This can be handy if you need to access information about the encoding object or encoding type. You can get information about the encoding by using:

- :doc:`ztd::text::encode_state_t </api/decode_state>`
- :doc:`ztd::text::decode_state_t </api/encode_state>`
- :doc:`ztd::text::code_unit_t\<Encoding\> </api/code_unit>`
- :doc:`ztd::text::code_point_t\<Encoding\> </api/code_point>`
- :doc:`ztd::text::code_unit_v\<Encoding\> </api/code_unit>`
- :doc:`ztd::text::code_point_v\<Encoding\> </api/code_point>`



Second Parameter
----------------

The second parameter is the result object. It is of the type :doc:`ztd::text::decode_result </api/decode_result>` or :doc:`ztd::text::encode_result </api/encode_result>`. The two types have identical information inside of them, but have different names so that a function call operator can tell the difference between the two, if it's necessary.

This contains all of the state and information that the decode operation/encode operation would return, if left unmodified by the error handler. If you don't want to do anything to it, simply pass it through by returning it with ``return result;``. Otherwise, you have access to the ``input`` range, the ``output`` range, any ``.state`` relevant to the operation, the ``.error_code``, and the ``.error_handled`` value. You can modify any one of theses, or even perform a recovery operation and change the ``.error_code`` to be ``ztd::text::encoding_error::ok``. Literally, anything can be done!

For example, someone can see if there is space left in the ``result.output`` parameter, and if so attempt to serialize a replacement character in place there (this is what :doc:`ztd::text::replacement_handler </api/error handlers/replacement_handler>` does).



Third Parameter
---------------

The third parameter is a contiguous range of inpts that were read. Typically, this is a ``ztd::text::span`` handed to you, or something that can construct a ``ztd::text::span`` or either code units or code points (whatever the input type has). This is useful for ``input_range``\ s and ``input_iterator``\ s where it is impossible to guarantee a value can be re-read, as is the case `with istream_iterator <https://en.cppreference.com/w/cpp/iterator/istream_iterator>`_ and other I/O-style iterators and ranges.

This is useful for grabbing any unused-but-read input data, and storing it for later. For example, reading from a network buffer where the network still has more data means that getting a :doc:`ztd::text::encoding_error::incomplete_sequence </api/encoding_error>` is not **really** an error. It just means to save what has not been read yet, change the buffer pointer to leave those characters in the right place, and try again. This can also be done a little bit easier by utilizing the :doc:`ztd::text::incomplete_handler </api/error handlers/incomplete_handler>`.



Secret Type Definition
----------------------

There is a type definition you can add to your error handler to signal that it is okay to ignore it's calls. It goes on the struct and looks like:

.. code-block:: cpp

	using assume_valid = std::false_type; // or std::true_type

This is allows any encoding which uses :doc:`ztd::text::is_ignorable_error_handler </api/is_ignorable_error_handler>` property on your error handler to know if it's okay to ignore the error handler when bad things happen. Having this functionality means you can create a "debug handler" for text you previously know is valid, but might want to check during a debug or tracing build or something as it encodes and decodes through the system:

.. code-block:: cpp
	:linenos:

	struct my_debug_handler {

		// Assume it's valid if the config value
		// is explicitly turned off
		using assume_valid = std::integral_constant<
			bool, (MY_ENCODING_TRACE_IS_TURNED_OFF != 0)
		>;

		// rest of the implementation...
	};
