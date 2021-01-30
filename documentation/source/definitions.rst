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
.. =============================================================================

Glossary of Terms & Definitions
===============================

Occassionally, we may need to use precise language to describe what we want. This contains a list of definitions that can be linked to from the documentation to help describe key concepts that are useful for the explication of the concepts and ideas found in this documentation.


.. _definitions-code_unit:

code unit
	A single unit of encoded information. This is typically, 8-, 16-, or 32-bit entites arranged in some sequential fashion that, when read or treated in a certain manner, end up composing higher-level units which make up readable text. Much of the world's most useful encodings that encode text use multiple code units in sequence to give a specific meaning to something, which makes most encodings variable length encodings.

.. _definitions-code_point:

code point
	A single unit of decoded information. Most typically associated with :ref:`unicode code points <definitions-unicode_code_point>`, but they can be other things such as :ref:`unicode scalar values <definitions-unicode_scalar_value>` or even a 13-bit value.

	Note that a single code point does not imply a :ref:`"character" <definitions-character>`, as that is a complex entity in human language and writing that cannot be mapped easily to a single unit of decoded information.

.. _definitions-character:

character
	This word carries with it 2 meanings, thanks to C-style languages and their predecessors. Sometimes, ``char``\ s, ``wchar_t``\ s, ``char8_t``\ s, and similar are called "narrow character"s, "wide character"s, "UTF-8 characters" and similar. This is the result of a poor legacy in software and hardware nomenclature. These are not character types, but rather types that _may_ represent the abstract notion of a character but frequently, and often, do not. After all, you wouldn't be here reading this if it did and non-English wasn't busted in your application, now would you?

	The other definition is just an abstract unit of information in human languages and writing. The closest approximation that Unicode has for the human language/writing character is a :ref:`Grapheme Cluster <definitions-grapheme_cluster>`.

.. _definitions-unicode_code_point:

unicode code point
	A single unit of decoded information for Unicode. It represents the smallest, non-encoded, and indivisible piece of information that can be used to talk about higher level algorithms, properties, and more from the Unicode Standard.

	A unicode code point has been reserved to take at most 21 bits of space to identify itself.

	A single unicode code point is NOT equivalent to a :ref:`character <definitions-character>`, and multiple of them can be put together or taken apart and still have their sequence form a :ref:`"character" <definitions-character>`. For a more holsitic, human-like interpretation of code points or other data, see :ref:`grapheme clusters <definitions-grapheme_cluster>`.

.. _definitions-unicode_scalar_value:

unicode scalar value
	A single unit of decoded information for Unicode. It's definition is identical to that of :ref:`unicode code points <definitions-unicode_code_point>`, with the additional constraint that every unicode svalar value may not be a "Surrogate Value". Surrogate values are non-characters used exclusively for the purpose of encoding and decoding specific sequences of code units, and therefore carry no useful meaning in general interchange. They may appear in text streams in certain encodings: see :doc:`Wobbly Transformation Format-8 (WTF-8) </api/encodings/wtf8>` for an example.

.. _definitions-grapheme_cluster:

grapheme cluster
	The closest the Unicode Standard gets to recognizing a :ref:`human-readable and writable character <definitions-character>`, grapheme cluster's are arbitrarily sized bundles of :ref:`unicode code points <definitions-unicode_code_point>` that compose of a single concept that might match what a :ref:`"character" <definitions-character>` is in any given human language.

.. _definitions-encoding:

encoding
	A set of functionality that includes an encode process or a decode process (or both). The encode process takes in a stream of code points and puts out a stream of code units. The decode process takes in a stream of code units and puts out a stream of code points. In a concrete sense, there are a number of additional operations an encoding needs: see the :doc:`Lucky 7 design concept</design/lucky 7>`.

.. _definitions-encode:

encode
	Converting from a stream of input, typically code points, to a stream of output, typically code units. The output may be less suitable for general interchange or consumption, or is in a specific interchange format for the interoperation. Freqently, this library expects and works with the goal that any decoding process is producing :ref:`unicode code points <definitions-unicode_code_point>` or :ref:`unicode scalar values <definitions-unicode_scalar_value>` from some set of :ref:`code units <definitions-code_unit>`.

.. _definitions-decode:

decode
	Converting from a stream of input, typically code units, to a stream of output, typically code points. The output is generally in a form that is more widely consummable or easier to process than when it started. Freqently, this library expects and works with the goal that any decoding process is producing :ref:`unicode code points <definitions-unicode_code_point>` or :ref:`unicode scalar values <definitions-unicode_scalar_value>` from some set of :ref:`code units <definitions-code_unit>`.

.. _definitions-transcode:

transcode
	Converting from one form of encoded information to another form of encoded information. In the context of this library, it means going from an input in one :ref:`encoding <definitions-encoding>`'s code units to an output of another encoding's code units. Typically, this is done by invoking the :ref:`decode <definitions-decode>` of the original encoding to reach a common interchange format (such as :ref:`unicode code points <definitions-unicode_code_point>`) before taking that intermediate output and piping it through the :ref:`encode <definitions-encode>` step of the other encoding. Different transcode operations may not need to go through a common interchange, and may transcode "directly", as a way to improve space utilization, time spent, or both.

.. _definitions-mojibake:

mojibake
	When text is converted from one encoding to another encoding, and it results in mangled and garbled text.
