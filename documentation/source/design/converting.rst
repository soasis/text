
Converting, Counting, and Validating Text
=========================================

Conversions are one of the more important aspects of dealing with textual data. To support this, ztd.text contains 7 different methods, each with various overloads and inner groupings of functions to aid in encoding, decoding, transcoding, validating, and counting code points and code units.

As shown in the :doc:`Lucky 7 Design </design/lucky 7>`, everything here is supported by just having either the required one or two encoding objects with the designated functions, variables and type definitions. The core of the explanation is in this algorithm:

* ⏩ Is the ``input`` value empty? If so, is the ``state`` finished and have nothing to output? If both are true, return the current results, everything is okay ✅.
* ⏩ Otherwise,

   0. Set up an ``intermediate`` buffer of ``code_point``\ s using the ``max_code_points`` of the input encoding count for the next operation.
   1. Do the ``decode_one`` step from ``input`` (using its ``begin()`` and ``end()``) into the ``intermediate`` ``code_point`` buffer.

      * 🛑 If it failed, return with the current ``input`` (unmodified from before this iteration, if possible), ``output``, and ``state``.

   2. Do the ``encode_one`` step from the ``intermediate`` into the ``output``.

      * 🛑 If it failed, return with the current ``input`` (unmodified from before this iteration, if possible), ``output``, and ``state``.

* ⏩ Update ``input``\ 's ``begin()`` value to point to after what was read by the ``decode_one`` step.
* ⤴️ Go back to the start.

That's it for the core loop. Failure is determined exclusively by whether or not the ``error_code`` returned from the decode or encode operation's result object is :doc:`ztd::text::encoding_error::ok </api/encoding_error>`. If it is OK, then the loop continues until the ``input`` is exhausted. Otherwise, it stops. This forms the basis of the library, and will essentially be our version of "Elements of Programming", but for working with Text:

.. image:: /img/masterful-eop.png
	:alt: A joke image from a presentation at C++ Russia 2020 Moscow Online, poking fun at having created the "Elements of Transcoding".
	:target: https://youtu.be/RnVWON7JmQ0?t=1380

The above algorithm can work for all the below operations:

- **transcoding**\ : the above loop presented as-is.
- **encoding**\ : take an ``input`` of ``code_point``\ s, and simply do not do the decoding step.
- **decoding**\ : take an ``input`` of ``code_unit``\ s, and simply do not do the encoding step.
- **validating code units**\ : do the transcoding loop into 2 intermediate buffers, and compare the result of the final ``intermediate`` output to the ``input``.
- **validating code points**\ : do the transcoding loop, but in the reverse direction for an ``input`` of ``code_point``\ s (encode first, then decode) into 2 intermediate buffers, and compare the result of the final ``intermediate`` output to the ``input``.
- **counting code units**\ : perform the "encoding" operation into an intermediate buffer and repeatedly count the number of buffered writes, discarding or ignoring the actual contents of the buffer each time.
- **counting code points**\ : perform the "decoding" operation into an intermediate buffer and repeatedly count the number of buffered writes, discarding or ignoring actual the contents of the buffer each time.

This covers the full universe of potential operations you may want to perform on encoded text, for the purposes of input and output. If you implement the :doc:`base Lucky 7 </design/lucky 7>` or implement the :doc:`extended Lucky 7 </design/beyond lucky 7>` for an encoding, you can gain access to the full ecosystem of encodings within your application.

.. toctree::
	:maxdepth: 1
	:hidden:

	converting/encode
	converting/decode
	converting/transcode
	converting/recode
	converting/validate encodable
	converting/validate decodable
	converting/validate transcodable
	converting/count as decoded
	converting/count as encoded
	converting/count as transcoded
