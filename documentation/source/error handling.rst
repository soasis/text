Error Handling
==============

Text is notorious for being a constant and consistent malformed source of input. From intermediate services mangling encodings and producing :ref:`Mojibake <definitions-mojibake>` to bungled normalization and bad programs not understanding even the slightest hint of code beyond ASCII, there is a lot of text data that is strictly bad for any program to consume.

When interfacing with range types such as :doc:`ztd::text::decode_view </api/views/encode_view>`, functions like :doc:`ztd::text::transcode </transcode>`, and individual ``.encode_one`` or ``.decode_one`` calls on encoding objects like :doc:`ztd::text::utf8 </api/encodings/utf8>`, you can:

- give an error handler type as a template parameter; or,
- pass it in as a normal argument to a function.

They can change the conversion and other operations happen works. Consider, for example, this piece of code which translates from Korean UTF-8 to ASCII:

.. code-block:: cpp
	:linenos:
	:emphasize-lines: 12,13

	#include <ztd/text/transcode.hpp>

	int main (int, char*[]) {
		std::string my_ascii_string
			= ztd::text::transcode(
				// input
				u8"안녕",
				// from this encoding
				ztd::text::utf8{},
				// to this encoding
				ztd::text::ascii{},
				// (1) decode-step error handler
				ztd::text::replacement_handler{},
				// (2) encode-step error handler
				ztd::text::replacement_handler{}
			);

		std::cout << my_ascii_string << std::endl; // (3)

		return 0;
	}

Clearly, the Korean characters present in the UTF-8 string just cannot fit in a strict, 7-bit ASCII encoding. What, then, becomes the printed output from ``std::cout`` at ``// (3)``? The answer is two ASCII question marks, ``??``. The :doc:`ztd::text::replacement_handler </api/error handlers/replacement_handler>` object passed in at ``// (1)`` and ``// (2)`` substitutes replacement characters (zero or more) into the output for any failed operation. There are multiple kinds of error handlers with varying behaviors:

- :doc:`replacement_handler </api/error handlers/default_handler>`, which inserts a substitution character specified by either the encoding object or some form using the default replacement character ``"U+FFFD"``;
- :doc:`default_handler </api/error handlers/default_handler>`, which is just a name for the ``replacement_handler`` or ``throw_handler`` or some other type based on compile-time configuration of the library;
- :doc:`throw_handler </api/error handlers/throw_handler>`, for throwing an exception on any failed operation;
- :doc:`incomplete_handler </api/error handlers/incomplete_handler>`, for throwing an exception on any failed encode/decode operation; and,
- :doc:`assume_valid_handler </api/error handlers/throw_handler>`, which triggers no checking for many error conditions and can leads to ☢️☢️Undefined Behavior☢️☢️ if used on malformed input.


.. warning::

	For the love of what little remains holy, PLEASE don't use ``ztd::text::assume_valid_handler`` unless you REALLY know you need it. It is a surefire way to open up vulnerabilities in your text processing algorithm. Not a single line of code using this type should pass code review if there is even the slightest thought that this will be used on any input that is not PERFECTLY under the DIRECT, PERSONAL control of the maintainers of the code.

These are all the error handlers that you have at your disposal, but they are just pre-provided types you can instantiate yourself. Nothing stops you from making your own error handling type!



Anatomy of a Handler
--------------------

An error handler is just a function (or an object with a function call operator) that takes 3 parameters and returns 1 result:

- takes the encoding that will call it when something goes wrong;
- takes the result object you expect to be working with (specifically, :doc:`ztd::text::encode_result </api/encode_result>` and :doc:`ztd::text::decode_result </api/decode_result>`), which contains the current state of affairs from the encoding operation;
- takes a contiguous range representing any input values that may have been read but will not be used; and,
- returns the same result type with any modifications (or not!) you'd like to make.

The first parameter is simple enough: if you invoke an ``encode_one`` or ``decode_one`` on a :doc:`ztd::text::utf8 </api/encodings/utf8>` object, then you can expect the first parameter of type ``ztd::text::utf8``. This can be handy if you need to access information about the encoding object or encoding type.

The second parameter is the result object. This contains all of the state and information that the decode operation/encode operation would return, if left unmodified by the error handler. If you don't want to do anything to it, simply pass it through by returning it with ``return result;``. Otherwise, you have access to the ``input`` range, the ``output`` range, any ``.state`` relevant to the operation, the ``.error_code``, and the ``.error_handled`` value. You can modify any one of theses, or even perform a recovery operation and change the ``.error_code`` to be ``ztd::text::encoding_error::ok``. Literally, anything can be done!

The third parameter is a contiguous range of inpts that were read. Typically, this is a ``std::span`` handed to you, or something that can construct a ``std::span`` or either code units or code points (whatever the input type has). This is useful for ``input_range``\ s and ``input_iterator``\ s where it is impossible to guarantee a value can be re-read, as is the case `with istream_iterator <https://en.cppreference.com/w/cpp/iterator/istream_iterator>`_ and other I/O-style iterators and ranges.


Writing A Handler
-----------------

When put together, it can generally look like this:

.. code-block:: cpp
	:linenos:

	#include <ztd/text/encode.hpp>
	#include <ztd/text/encoding.hpp>

	#include <iostream>

	template <typename Output>
	using ascii_encode_result = ztd::text::encode_result<
		// input range type
		std::u32string_view,
		// output range type; figured out from function call
		Output,
		// the state type for encode operations
		ztd::text::encoding_encode_state_t<ztd::text::ascii>
	>;

	struct my_printing_handler {
		template <typename Output>
		ascii_encode_result<Output> operator()(const ztd::text::ascii& encoding,
			ascii_encode_result<Output> result,
			std::span<const char32_t> unused_read_characters) const noexcept
		{
			// just printing some information
			std::cout << "An error occurred.\n"
				<< "\tError code value: "
				<< ztd::text::to_name(result.error_code)
				<< "\n"
				<< "\t# of unused characters: "
				<< unused_read_characters.size()
				<< "\n"
				<< "\tInput units left: " << result.input.size()
				<< "\n";
			// setting the error to "ok"
			// tells the algorithm to keep spinning,
			// even if nothing gets written to the output
			result.error_code = ztd::text::encoding_error::ok;
			return result;
		}
	};

	int main(int, char*[]) {
		std::string my_ascii_string = ztd::text::encode(
			// input
			U"안녕",
			// to this encoding
			ztd::text::ascii {},
			// handled with our type
			my_printing_handler {});

		std::cout << my_ascii_string << std::endl;

		return 0;
	}

The result in ``my_ascii_string`` should be an empty string: nothing should have succeeded and therefore the function will just return an empty string. The print out will look like this:

.. code-block:: shell

	An error occurred.
		Error code value: invalid_sequence
		# of unused characters: 1
		Input units left: 1
	An error occurred.
		Error code value: invalid_sequence
		# of unused characters: 1
		Input units left: 0
	


If you would like the higher-level called function to return more information to you, use the lower level :doc:`encode_to/encode_into </encode>`, :doc:`decode_to/decode_into </decode>`, :doc:`transcode_to/transcode_into </transcode>`.

If you need to do more, you can change from concrete types to templates, and work at increasingly higher levels of genericity in order to have the printing handler do more and more.