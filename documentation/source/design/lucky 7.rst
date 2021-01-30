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

Encoding Design
===============

One of the core premises of this library is that any text in one encoding can be converted to another, without having to know anything about external encodings. This is how the library achieves infinite extensibility! We start by noting that almost any encoding conversion can be done so long as there is an intermediary that exists between the source and the destination. For encoded text, this is the line between code units (``code_unit`` for code) and code points (``code_point`` for code).

**Code units** are single elements of a linear sequence of encoded information. That could be a sequence of bytes, a sequence of 16-bit numbers, and more. A sequence of code units is typically specific to the encoding it has and is generally impossible to reason about in a general or generic sense.

**Code points** are single elements of a linear sequence of information that have been decoded. They are far more accessible because they are generally an agreed upon interchange point that most others can access and reason about.

We leverage that, for text, `**Unicode Code Points** <https://en.wikipedia.org/wiki/Unicode#Code_point_planes_and_blocks>`_ are an agreed upon interchange format, giving rise to this general framework for encoding and decoding text:


.. figure:: /img/encoding_path.png
   :alt: The typical encoding path, from one encoding to another.

   The generic pathway from one encoding to another for most (all?) text Encodings.


The way to tap into this concept is to create an object that models an encoding concept, which is commonly referred to as the "Lucky 7" concept. The concept leverages a technique that has been used at least since the early days of Bruno Haibile's and Daiko Ueno's ``iconv`` library, but formalizes it for interacting between 2 encodings.

We call this concept the **Lucky 7**.




Lucky 7
-------

**Lucky 7** is a conceptual idea a single encoding object is all you need to write to fulfill your end of the encoding bargain. It is called the Lucky 7 because only 7 things are required from you, as the author of the encoding object, to get started:

- 3 type definitions (``code_point``, ``code_unit``, ``state``)
- 2 static member variables (``max_code_points``, ``max_code_units``)
- 2 functions (``encode_one``, ``decode_one``)


.. code-block:: cpp
	:linenos:

	#include <cstddef>
	#include <span>

	struct empty_struct {};

	struct utf_ebcdic {
		// (1)
		using code_unit  = char;
		// (2)
		using code_point = char32_t;
		// (3)
		using state      = empty_struct;

		// (4)
		static constexpr inline std::size_t max_code_points = 1;
		// (5)
		static constexpr inline std::size_t max_code_units  = 6;
		
		// (6)
		ue_encode_result encode_one(
			std::span<char32_t> input,
			std::span<char> output,
			state& current,
			ue_encode_error_handler error_handler
		);

		// (7)
		ue_decode_result decode_one(
			std::span<char> input,
			std::span<char32_t> output,
			state& current,
			ue_decode_error_handler error_handler
		);
	};


There are some supporting structures here that we will explain one by one, but this is the anatomy of a simple encoding object that you and others can define to do this job. With this alone, you can support all of the following functionality:

- 
- 
- 
- 



Breaking it Down
----------------

The first three typedefs are what let internal and externel machinery know what kind of values you expect out of the ranges that go into the ``decode_one`` and ``encode_one`` function calls:

- ``code_unit`` - the input for decoding (``decode_one``) operations and the output for encode operations.
- ``code_point`` - the input for encode operations and the output for decoding (``decode_one``) operations.

``char`` is the encoding value type that the ranges work with for incoming and outgoing encoded data. ``char32_t`` is the code point value type for decoded information. With that settled, this makes it easy to look at the result types used:

.. code-block:: cpp
	:linenos:
	
	#include <ztd/text/encode_result.hpp>
	#include <ztd/text/decode_result.hpp>

	using ue_decode_result = ztd::text::decode_result<
		std::span<char>,
		std::span<char32_t>,
		empty_struct&
	>;

	using ue_encode_result = ztd::text::encode_result<
		std::span<char32_t>,
		std::span<char>,
		empty_struct&
	>;


These result structures are returned from encode and decode operations. They contain:

- An ``input`` member, which is the range that is passed into the ``decode_one`` and ``encode_one`` functions
- An ``output`` member
- A reference to the state that was passed in
- 

