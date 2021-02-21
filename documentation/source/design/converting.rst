
Converting, Counting, and Validating Text
=========================================

Conversions are one of the more important aspects of dealing with textual data. To support this, ztd.text contains 7 different methods, each with various overloads and inner groupings of functions to aid in encoding, decoding, transcoding, validating, and counting code points and code units.

As shown in the :doc:`Lucky 7 Design </design/lucky 7>`, everything here is supported by just having either the required one or two encoding objects with the designated functions, variables and type definitions. The core of the explanation is in this loop:

* ⏩ Is ``is_empty(input)`` true? Return the current result, everything is okay ✅. Otherwise,

	0. Set up an ``intermediate`` buffer of ``code_point``\ s using the ``max_code_point`` count for the next operation.
	1. Do the ``decode_one`` step from ``input`` into the ``intermediate`` ``code_point`` buffer.

		* If it failed, return with the current (unmodified, if possible) ``input``, ``output``, and ``state``. ⏹️

	2. Do the ``encode_one`` step from the ``intermediate`` into the ``output``.

		* If it failed, return with the current (unmodified, if possible) ``input``, ``output``, and ``state``. ⏹️

* Update ``input``\ 's ``begin`` value to point to after what was read by the ``decode_one`` step.
* Go back to the start. ⤴️

That's it for the core loop. Failure is determined exclusively by whether or not the ``error_code`` returned from the decode or encode operation's result object is :doc:`ztd::text::encoding_error::ok </api/encoding_error>`. If it is OK, then the loop continues until the ``input`` is exhausted. Otherwise, it stops.

.. .. image:: /img/core-loop.png
..	:alt: An image from a presentation at C++ Russia 2020 Moscow Online, showing some of the core loop.
..	:target: https://www.youtube.com/watch?v=RnVWON7JmQ0


This loop produces what is known as Transcoding. That is, it converts — through a common, intermediate Code Point type -- from one code unit type in one encoding to the other code unit types in the other encoding. Typically, the intermediate is a :doc:`ztd::text::unicode_code_point </api/unicode_code_point>`, :doc:`ztd::text::unicode_scalar_value </api/unicode_scalar_value>`, or ``char32_t``.

This loop can be modified to perform:

- **transcoding**\ : the above loop presented as-is.
- **encoding**\ : take an ``input`` of ``code_point``\ s, and simply do not do the decoding step.
- **decoding**\ : take an ``input`` of ``code_unit``\ s, and simply do not do the encoding step.
- **validating code units**\ : do the transcoding loop into 2 intermediate buffers, and compare the result of the final ``intermediate`` output to the ``input``.
- **validating code points**\ : do the transcoding loop, but in the reverse direction for an ``input`` of ``code_point``\ s (encode first, then decode) into 2 intermediate buffers, and compare the result of the final ``intermediate`` output to the ``input``.
- **counting code units**\ : perform the "encoding" operation into an intermediate buffer and repeatedly count the number of buffered writes, discarding or ignoring the actual contents of the buffer each time.
- **counting code points**\ : perform the "decoding" operation into an intermediate buffer and repeatedly count the number of buffered writes, discarding or ignoring actual the contents of the buffer each time.

This covers the full universe of potential operations you may want to perform on encoded text, for the purposes of input and output. It guarantees that, if you implement the :doc:`base Lucky 7 </design/lucky 7>` or implement the :doc:`extended Lucky 7 </design/beyond lucky 7>`, you can gain access to the full ecosystem of encodings within your application.

.. You can also increase the speed and do much more interesting things by taking advantage of certain properties from the operations. ztd.text lets you do this, and its explained in each of the following sections in more depth:

.. toctree::
	:maxdepth: 1
	:hidden:

	converting/encode
	converting/decode
	converting/transcode
	converting/validate code points
	converting/validate code units
	converting/count code points
	converting/count code units
