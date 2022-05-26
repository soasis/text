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

Marking an encoding as Unicode-Capable
======================================

Sometimes, you need to make your own encodings. Whether for legacy reasons or for interoperation reasons, you need the ability to write an encoding that can losslessly handle all :math:`2^21` code points. Whether it's writing a variant of `UTF-7 <https://en.wikipedia.org/wiki/UTF-7>`_, or dealing with a very specific legacy set like Unicode v6.0 with the Softbank Private Use Area, you are going to need to be able to say "hey, my encoding can handle all of the code points and therefore deserves to be treated like a Unicode encoding". There are 2 ways to do this, one for decisions that can be made at compile time, and one for decisions that can be made at runtime (e.g., over a ``variant_encoding<X, Y, Z>``).



compile time
------------

The cheapest way to tag an encoding as Unicode Capable and have the library recognize it as such when :doc:`ztd::text::is_unicode_encoding </api/is_unicode_encoding>` is used is to just define a member type definition:

.. code-block:: cpp

	class utf8_v6_softbank {
	public:
		// …
		using is_unicode_encoding = std::true_type;
		// …
	};

That is all you have to write. Both :doc:`ztd::text::is_unicode_encoding </api/is_unicode_encoding>` and :doc:`ztd::text::contains_unicode_encoding </api/contains_unicode_encoding>` will detect this and use it.



Run-time
--------

If your encoding cannot know at compile time whether or not it is a unicode encoding (e.g., for type-erased encodings, complex wrapping encodings, or encodings which rely on external operating system resources), you can define a method instead. When applicable, this will be picked up by the :doc:`ztd::text::contains_unicode_encoding </api/contains_unicode_encoding>` function. Here is an example of a runtime, locale-based encoding using platform-knowledge to pick up what the encoding might be, and determine if it can handle working in Unicode:

.. literalinclude:: /../../examples/documentation/snippets/source/runtime_locale_encoding.cpp
	:language: cpp
	:linenos:
	:lines: 46-47,75-99,233
	

That is it. :doc:`ztd::text::contains_unicode_encoding </api/contains_unicode_encoding>` will detect this and use your function call, so you should never be calling this or accessing the above compile time classification if necessary and always delegating to the ``ztd::text::contains_unicode_encoding`` function call.
