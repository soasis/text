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

Quick 'n' Dirty Tutorial
========================



Setup
-----

Use of this library is officially supported through the use of `CMake <https://cmake.org/>`_. Getting an updated CMake is difficult on non-Windows machines, especially if they come from your system's package manager distribution which tends to be several (dozen?) minor revisions out of date, or an entire major revision behind on CMake. To get a very close to up-to-date CMake, Python maintains an version that works across all systems. You can get it (and the ninja build system) by using the following command in your favorite command line application (assuming Python is already installed):

.. code-block:: sh
	:linenos:

	python -m pip install --user --update cmake ninja

If you depend on calling these executables using shorthand and not their full path, make sure that the Python "downloaded binaries" folder is contained with the ``PATH`` environment variable. Usually this is already done, but if you have trouble invoking ``cmake --version`` on your typical command line, please see the `Python pip install documentation for more details <https://pip.pypa.io/en/stable/cli/pip_install/>`_ for more information, in particular about the ``--user`` `option <https://pip.pypa.io/en/stable/cli/pip_install/#cmdoption-user>`_.

If you do not have Python or CMake/ninja, you must get a recent enough version `directly from CMake <https://cmake.org/download/>`_ and build/install it.



Using CMake
-----------

Here's a sample of the ``CMakeLists.txt`` to create a new project and pull in ztd.text in the simplest possible way:

.. literalinclude:: ../../examples/documentation/quick/setup/CMakeLists.txt
	:language: cmake
	:linenos:
	:start-after: # === snippet-project-declaration-start
	:end-before: # === snippet-project-declaration-end


This will automatically download and set up all the dependencies ztd.text needs (in this case, simply ztd.cmake, ztd.platform, `ztd.idk <https://ztdidk.readthedocs.io/en/latest/>`_, and `ztd.cuneicode <https://ztdcuneicode.readthedocs.io/en/latest/>`_ ). You can override how ztd.text gets these dependencies using the standard FetchContent described in the `CMake FetchContent Documentation <https://cmake.org/cmake/help/latest/module/FetchContent.html#command:fetchcontent_declare>`_. One that happens, simply use CMake's ``target_link_libraries(…)`` to add it to the code:

.. literalinclude:: ../../examples/documentation/quick/setup/CMakeLists.txt
	:language: cmake
	:linenos:
	:start-after: # === snippet-library-start
	:end-before: # === snippet-library-end

Once you have everything configured and set up the way you like, you can then use ztd.text in your code, as shown below:

.. literalinclude:: ../../examples/documentation/quick/setup/source/main.cpp
	:language: cpp
	:linenos:
	:start-after: // ============================================================================ //

Let's get started by digging into some examples!

.. note::
	
	If you would like to see more examples and additional changes besides what is covered below, please do feel free to make requests for them `here <https://github.com/soasis/text/issues>`_! This is not a very full-on tutorial and there is a lot of functionality that, still, needs explanation!



Transcoding
-----------

Transcoding is the action of taking data in one encoding and transforming it to another. ztd.text offers many ways to do this; here are a few different ways that have different expectations, needs, meanings, and tradeoffs.


Transcode between Unicode Encodings
+++++++++++++++++++++++++++++++++++

Going from a Unicode Encoding to another Unicode Encoding just requires going through the :doc:`ztd::text::transcode </api/conversions/transcode>` API. All you have to do after that is provide the appropriate :doc:`ztd::text::utf8 </api/encodings/utf8>`, :doc:`ztd::text::utf16 </api/encodings/utf16>`, or :doc:`ztd::text::utf32 </api/encodings/utf32>` encoding object:

.. literalinclude:: ../../examples/documentation/quick/basic/source/transcode-unicode.cpp
	:language: cpp
	:linenos:
	:start-after: // ============================================================================ //


Transcode from Execution Encoding to UTF-8
++++++++++++++++++++++++++++++++++++++++++

The :term:`execution encoding` is the encoding that comes with the system. It is typically the encoding that all locale data comes in, especially for e.g. command line parameters on Windows. To encode from such an enoding to the highly successful and popular UTF-8, you may use the same :doc:`ztd::text::transcode </api/conversions/transcode>` as above with the appropriate :doc:`ztd::text::(compat_)utf8 </api/encodings/utf8>`:

.. literalinclude:: ../../examples/documentation/quick/basic/source/transcode-execution-utf8.cpp
	:language: cpp
	:linenos:
	:start-after: // ============================================================================ //

The ``compat_`` prefix is to make sure we are using the typedef definition of the templated :cpp:type:`ztd::text::basic_utf8` that uses ``char`` units. This is helpful for working with legacy data streams. We use ``std::cout.write(…)`` explicitly to prevent as much direct interface from the terminal or locales as possible to write the data to the terminal, ensuring that on competent systems with reasonably up-to-date terminals will display out UTF-8 data untouched (and, hopefully, properly).


Transcoding with Output Container Controls
++++++++++++++++++++++++++++++++++++++++++

Occasionally, you need to:

- serialize to a container that isn't a ``std::basic_string``/``std::(u8/16/32)string``;
- OR, you need to serialize to a container but you need to know if anything went wrong.

This is where the functions that are suffixed ``_to`` come into play, and where the template argument provided to the non-suffixed ``ztd::text::transcode<…>(…)`` come into play.

.. literalinclude:: ../../examples/documentation/quick/basic/source/transcode-unicode-container.cpp
	:language: cpp
	:linenos:
	:start-after: // ============================================================================ //

The returned :doc:`ztd::text::transcode_result </api/transcode_result>` from the ``_to``-suffixed function gives more information about what went wrong, including the error count and the 


Transcoding with Errors
+++++++++++++++++++++++

Very often, text contains errors. Whether it's being interpreted as the wrong encoding or it contains file names or data mangled during a system crash, or it's just plain incorrect, bad data is a firm staple and constant reality for text processing. ztd.text offers many kinds of :doc:`error handlers </design/error handling>`. They have many different behaviors, from doing nothing and stopping the desired encoding operation, to skipping over bad text and not doing anything, to adding replacement characters, and more.

The :cpp:var:`ztd::text::default_handler`, unless configured differently, is to use replacement characters:

.. literalinclude:: ../../examples/documentation/quick/basic/source/transcode-unicode-errors.default.cpp
	:language: cpp
	:linenos:
	:start-after: // ============================================================================ //

The :cpp:var:`ztd::text::replacement_handler` explicitly inserts replacement characters where the failure occurs:

.. literalinclude:: ../../examples/documentation/quick/basic/source/transcode-unicode-errors.replacement.cpp
	:language: cpp
	:linenos:
	:start-after: // ============================================================================ //

To simply skip over bad input without outputting any replacement characters, use :cpp:var:`ztd::text::skip_handler`:

.. literalinclude:: ../../examples/documentation/quick/basic/source/transcode-unicode-errors.skip.cpp
	:language: cpp
	:linenos:
	:start-after: // ============================================================================ //

To stop in the middle of the operation and return immediately, employ the :cpp:var:`ztd::text::pass_handler`. This will leave text unprocessed, but offer a chance to inspect what is left and any corrective action that might need to be taken afterwards:

.. literalinclude:: ../../examples/documentation/quick/basic/source/transcode-unicode-errors.pass.cpp
	:language: cpp
	:linenos:
	:start-after: // ============================================================================ //

Error handlers like the :cpp:var:`ztd::text::skip_handler` and :cpp:var:`ztd::text::replacement_handler` (and potentially the :cpp:var:`ztd::text::default_handler`) are smart enough to not output multiple replacement characters for every single 8, 16, or 32-bit unit that contains an error, folding them down into one replacement character per distinct failure location:

.. literalinclude:: ../../examples/documentation/quick/basic/source/transcode-unicode-multi-errors.skip.cpp
	:language: cpp
	:linenos:
	:start-after: // ============================================================================ //

.. literalinclude:: ../../examples/documentation/quick/basic/source/transcode-unicode-multi-errors.default.cpp
	:language: cpp
	:linenos:
	:start-after: // ============================================================================ //

.. literalinclude:: ../../examples/documentation/quick/basic/source/transcode-unicode-multi-errors.replacement.cpp
	:language: cpp
	:linenos:
	:start-after: // ============================================================================ //

Comapred to the :cpp:var:`ztd::text::pass_handler`, which will stop at the first potential error:

.. literalinclude:: ../../examples/documentation/quick/basic/source/transcode-unicode-multi-errors.pass.cpp
	:language: cpp
	:linenos:
	:start-after: // ============================================================================ //

You can even :doc:`write your own custom error handlers </design/error handling/writing a handler>`.


Transcoding with Input, Output and Pivot Controls
+++++++++++++++++++++++++++++++++++++++++++++++++

Occasionally, you need to perform a transcoding operation that has no extension point and needs to go through an intermediate transition phase first (like UTF-8 ➡ Intermediate UTF-32 ➡ Shift-JIS). Normally, ztd.text will create an internal, stack-based buffer (controllable with :doc:`preprocessor definitions </config>`) to use as the intermediate. But sometimes you need to exercise control over even that, to keep memory usage predictable and stable in all situations. Enter the pivot parameter, which a user can use to give a custom buffer (any custom range) as the intermediate data holder:

.. literalinclude:: ../../examples/documentation/quick/basic/source/transcode-utf16-shift_jis-pivot.cpp
	:language: cpp
	:linenos:
	:start-after: // ============================================================================ //

Here, we use an **exceptionally** small buffer to keep memory usage down. Note that the buffer should be at least as large as :cpp:var:`ztd::text::max_code_points_v\<FromEncoding> <ztd::text::max_code_points_v>` (``FromEncoding`` in this case being the :cpp:var:`ztd::text::compat_utf8` encoding) so that no :cpp:enumerator:`"insufficient output size" errors <ztd::text::encoding_error::insufficient_output_space>` occur during translation. (For :doc:`ztd::text::encode </api/conversions/encode>` operations, the buffer should be at least as large as :cpp:var:`ztd::text::max_code_units_v\<FromEncoding> <ztd::text::max_code_units_v>`) If the pivot buffer is too small this can produce unpredictable failures and unexpected behavior from unanticipated errors, so make sure to always provide a suitably-sized pivot buffer! Or, alternatively, just let the implementation use its defaults, which are (generally) tuned to work out well enough for most conversion routines and platforms.



Encoding & Decoding
-------------------

Encoding and decoding look identical to :ref:`Transcoding <quick:transcoding>`, just using the functions :doc:`ztd::text::decode </api/conversions/decode>` and :doc:`ztd::text::encode </api/conversions/encode>` functions. ``ztd::text::decode`` will always produce a sequence of the encoding's code point type (:doc:`ztd::text::code_point_t\<some_encoding_type> </api/code_point>`). ``ztd::text::encode`` will always produce a sequence of the encoding's code unit type (:doc:`ztd::text::code_unit_t\<some_encoding_type> </api/code_unit>`), and the lower-level functions ending in ``_to`` and ``_into`` will produce a :doc:`ztd::text::encode_result </api/encode_result>` (for encoding) or :doc:`ztd::text::decode_result </api/decode_result>` (for decoding):

.. literalinclude:: ../../examples/documentation/quick/basic/source/encode-decode.cpp
	:language: cpp
	:linenos:
	:start-after: // ============================================================================ //


Encode and decode operations are part of each encoding, represented by its encoding type. Every encoding object natively understands how to go from a sequence of its encoded data to its decoded data, and vice-versa, with the ``encode_one`` and ``decode_one`` functions. One should not call these functions directly, however, and instead used the above-provided functinos. Because decode and encode operations do not feature intermediate steps, there is no :doc:`ztd::text::pivot\<…> </api/pivot>` for these functions.



Counting
--------

Counting is done using the :doc:`ztd::text::count_as_decoded </api/conversions/count_as_decoded>`, :doc:`ztd::text::count_as_encoded </api/conversions/count_as_encoded>`, and :doc:`ztd::text::count_as_transcoded </api/conversions/count_as_transcoded>`. As the names imply, it yields the number of code points or code units that will result from an attempted encode, decode, or transcode operation in a sequence of text. It will return a :doc:`ztd::text::count_result </api/count_result>` detailing that information:

.. literalinclude:: ../../examples/documentation/quick/basic/source/count-punycode.cpp
	:language: cpp
	:linenos:
	:start-after: // ============================================================================ //

Getting counts is essential to being able to size allocated buffers for exactly what is necessary, or make use of small buffer optimizations by checking sizes before potentially spilling over into larger allocations.



Validation
----------

Validation is checking whether or not the input sequence can be encoded, decoded, or transcoded to by (and to) the given encoding. It works through the :doc:`ztd::text::validate_decodable </api/conversions/validate_decodable_as>`, :doc:`ztd::text::validate_encodable </api/conversions/validate_encodable_as>`, and :doc:`ztd::text::validate_transcodable </api/conversions/validate_transcodable_as>` functions.

.. literalinclude:: ../../examples/documentation/quick/basic/source/validate-utf32.cpp
	:language: cpp
	:linenos:
	:start-after: // ============================================================================ //



There's More!
-------------

There is more you can do with this library, from :doc:`authoring your own encoding objects/types </design/lucky 7>` to :doc:`taking control of the perforance of conversions </design/lucky 7 extensions/speed>`. More will be added to this Getting Started as time goes on, but if you have any inkling of something that should work, give it a try! If it fails in a way you don't think is helpful, please let us known through any of our available communication channels so we can assist you!
