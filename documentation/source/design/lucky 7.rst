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

First Principles - "Lucky 7" and a Liberation-First Design
==========================================================

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
			ztd::text::span<char32_t> input,
			ztd::text::span<char> output,
			state& current,
			ue_encode_error_handler error_handler
		);

		// (7)
		ue_decode_result decode_one(
			ztd::text::span<char> input,
			ztd::text::span<char32_t> output,
			state& current,
			ue_decode_error_handler error_handler
		);
	};


There are some supporting structures here that we will explain one by one, but this is the anatomy of a simple encoding object that you and others can define to do this job. This anatomy explicitly enables some basic work:

- encoding a single indivisible unit of work from :term:`code points<code point>` to :term:`code units<code unit>`
- decoding a single indivisible unit of work from :term:`code units<code unit>` to :term:`code points<code point>`
- transcoding a single indivisible unit of work from the source encoding's :term:`code units<code unit>` to the destination encoding's :term:`code code units<code unit>`, if they share a common :term:`code point type <code point>`.

From these 3 operations above, everything else on this library can be built.



Breaking it Down
----------------

The first three typedefs are what let internal and externel machinery know what kind of values you expect out of the ranges that go into the ``decode_one`` and ``encode_one`` function calls:

- ``code_unit`` - the input for decoding (``decode_one``) operations and the output for encode operations.
- ``code_point`` - the input for encode operations and the output for decoding (``decode_one``) operations.

``char`` is the code unit type that the ranges work with for incoming and outgoing encoded data. ``char32_t`` is the code point type that the ranges use for incoming and outgoing decoded data. Given those, that gives us the ability to define the result types we will be working with.


Result Types
++++++++++++

Result types are specific structs in the library that mark encode and decode operations. They can be used by composing with the templated type :doc:`ztd::text::decode_result </api/decode_result>` and :doc:`ztd::text::encode_result </api/encode_result>`.

.. code-block:: cpp
	:linenos:
	
	#include <ztd/text/encode_result.hpp>
	#include <ztd/text/decode_result.hpp>

	using ue_decode_result = ztd::text::decode_result<
		ztd::text::span<char>,
		ztd::text::span<char32_t>,
		empty_struct
	>;

	using ue_encode_result = ztd::text::encode_result<
		ztd::text::span<char32_t>,
		ztd::text::span<char>,
		empty_struct
	>;


These result structures are returned from the lowest level ``encode`` and ``decode`` operations. They contain:

- An ``input`` member, which is the range that is passed into the ``decode_one`` and ``encode_one`` functions;
- An ``output`` member;
- A  ``state`` member, which is a reference to the ``state`` that was passed in to the ``decode_one`` or ``encode_one`` functions;
- An ``error_code`` member, which is an enumeration value from :doc:`ztd::text::encoding_error </api/encoding_error>`; and
- An ``handled_error`` member, which is a boolean value that says whether or not the given ``error_handler`` was invoked.

These variables can be used to query what exactly happened during the operation (``error_code`` and ``handled_error``), inspect any state passed into encodings (not used for an encoding such as ``utf_ebcdic``), and how much input and output has been read/what is left (by checking the ``input`` and ``output`` ranges whose ``.begin()`` value has been incremented compared to the input values). Understanding the result types now, we move to the error handler:


Error Handlers
++++++++++++++

The only other thing we need is the error handler, now. Generally, this is a template argument, but for the sake of illustration we use a concrete type here:

.. code-block:: cpp
	:linenos:

	#include <functional>

	using ue_decode_error_handler = std::function<
		ue_decode_result(
			const utf_ebcdic&,
			ue_decode_result,
			ztd::text::span<char>
		)
	>;

	using ue_encode_error_handler = std::function<
		ue_encode_result(
			const utf_ebcdic&,
			ue_encode_result,
			ztd::text::span<char32_t>
		)
	>;

The error handlers use a result-in, result-out design. The parameters given are:

0. The encoding which triggered the error. This allows you to access any information about the encoding object type or any values stored on the encoding object itself.
1. The result object. This object has the ``error_code`` member set to what went wrong (see :doc:`ztd::text::encoding_error </api/encoding_error>`), and any other changes made to the ``input`` or ``output`` during the operation.
2. A contiguous range (``ztd::text::span``) of ``code_unit``\ s or ``code_point``\ s that were already read by the algorithm. This is useful for when the ``input`` range uses input iterators, which sometimes cannot be "rolled back" after something is read (e.g., consider `std::istream_iterator <https://en.cppreference.com/w/cpp/iterator/istream_iterator>`_).

It returns the same type as the result object. Within this function, anyone can perform any modifications they like to the type, before returning it. This is an incredibly useful behavior that comes in handy for defining custom error handling behaviors, as shown in :doc:`the Error Handling Design section </design/error handling>`. For example, this allows us to do things like insert **REPLACEMENT_CHARACTER \\uFFFD** (�) into a encoding through the :doc:`ztd::text::replacement_handler </api/error handlers/replacement_handler>` or enable speedy encoding for pre-validated text using :doc:`ztd::text::assume_valid_handler </api/error handlers/assume_valid_handler>`. When writing your ``encode_one`` or ``decode_one`` function, it is your responsibility to invoke the error handler (or not, depending on the value of :doc:`ztd::text::is_ignorable_error_handler </api/is_ignorable_error_handler>`).


`Liberation Achieved <https://www.youtube.com/watch?v=cZGEscLKlEQ>`_
--------------------------------------------------------------------

If you achieve all these things, then we can guarantee that you can implement all of the desired functionality of an encoding library. This is the core design that underpins this whole library, and how it frees both Library Developers from needing to manically provide every possible encoding to end-users, and end-users from having to beg library developers to add support for a particular encoding.

.. image:: /img/freedom.png
	:alt: Image of the Presentation during the Black Flag Liberation section from Pure Virtual C++ 2020, a Conference by Microsoft and Sy Brand.
	:target: https://www.youtube.com/watch?v=w4qYf2pvPg4&t=2535

There is more depth one can add to an encoding object, but this is the base, required set of things to know and handle when it comes to working with ztd.text. You can build quite a complex set of features from this functionality, and we encourage you to keep reading through more of the design documentation to get an understanding for how this works!
