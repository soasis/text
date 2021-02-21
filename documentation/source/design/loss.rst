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

Lost Information
================

One of the biggest problems facing text processing in programming languages today is the loss of information as its carried through any given system. In C and C++, this comes in the form of all strings - especially multibyte strings - being given the same type. For example:

.. code-block:: cpp
	:linenos:

	void read_name(const char* name) {
		// (1)
	}

As the maintainer of code inside of the function ``read_name``, what is the encoding of "name" at ``(1)``? What is its normalization form? What system did it originate from? The function written in C++ form offers very little benefit either:

.. code-block:: cpp
	:linenos:

	void read_name(std::string_view name) {
		// (1)
	}


Even here, we've only made marginal improvements. We know the string is stored in some heap by the default allocator, we have the size of the string, but that only tells us how many ``char`` units are stored, not how many conceptual, human-readable :term:`characters <character>` there are or any other pertinent information. Is this information encoded? Is it UTF-8? Maybe it's EBCDIC Code Page 833. Maybe it's UTF-7-IMAP. You don't know, and by the time you start inspecting or poking at the individual ``char`` :term:`code units <code unit>`, who knows what can happen? To make matters worse, even C++ and its Standard Library have poor support for encoding/decoding, let alone Unicode in general. These problems have been explained in quite a lot of detail up to ths point, but the pitfalls are many:

.. epigraph::
	
	... Where are potential problems?

	All over the place? Let's see...

	-- `R. Martinho Fernandes, last edited April 20th, 2018 <https://stackoverflow.com/a/17106065>`_


Some proponents say that if we just change everything to mean "UTF-8" (`const char*`, `std::string`, and more), then we can just assume UTF-8 throughout the entire application and only accept UTF-8 and that will end all our encoding problems. Typically, these people read `UTF-8 Everywhere <https://utf8everywhere.org/>`_ and then just go all-in on the philosophy, all the time.



"UTF-8 Everywhere!!"
--------------------

There are many in the programmign space that believe that just switching everything to UTF-8 everywhere will solve the problem. This is, unfortunately, greatly inadequate as a solution. For those who actually read the entire UTF-8 Everywhere manifesto in its fullness, they will come across this FAQ entry:

.. epigraph::

	**Q: Why not just let any programmer use their favorite encoding internally, as long as they knows how to use it?**

	A: We have nothing against correct usage of any encoding. However, it becomes a problem when the same type, such as std::string, means different things in different contexts. While it is ‘ANSI codepage’ for some, for others, it means ‘this code is broken and does not support non-English text’. In our programs, it means Unicode-aware UTF-8 string. This diversity is a source of many bugs and much misery. ...

	-- `FAQ Entry #6 <https://utf8everywhere.org/#faq.liberal>`_

The core problem with the "``std::string`` is always UTF-8" decision (even when they are as big as Gooogle, Apple, Facebook, or Microsoft and own everything from the data center to the browser you work with) is that they live on a planet with other people who do not share the same sweeping generalizations about their application environments. Nor have they invoked the ability to, magically, rewrite everyone's code or the data that's been put out by these programs in the last 50 or 60 years. This results in a gratuitous amount of replacement characters or :term:`Mojibake <mojibake>` when things do not encode or decode properly:

.. image:: /img/paris-post-office.jpg
	:alt: A package going between Russia and Paris, written in Mojibake because of interpreting text with the wrong encoding. It has been corrected in marker with the correct lettering, because they are so used to this occurence for international packages.

There is a distinct problem that human beings are so used to computers failing them with encoding that they know how to recognize the mistranslated text:

.. epigraph::

	We get so good at it that we can even recognize the bad text 😆. There's a wiki for it too...
	...
	It used to be a lot worse. UTF-8 definitely helps a whole lot.

	-- Elias Daler

So, what do we do from here?



Fighting Code Rot
-----------------

We need ways to fight bit rot and issues of function invariants -- like expected encoding on string objects -- from infesting code. While we can't rewrite every function declaration or wrap every functin declaration, one of the core mechanisms this library provides is a way of tracking and tagging this kind of invariant information, particularly at compile-time.

We know we can't solve interchange on a global level (e.g., demanding everyone use UTF-8) because, at some point, there is always going to be some small holdout of legacy data that has not yet been fixed or ported. The start of solving this is by having views and containers that keep encoding information with them after they are first constructed. This makes it possible to not "lose" that information as it flows through your program:

.. code-block:: cpp
	:linenos:

	using utf8_view = ztd::text::decode_view<ztd::text::utf8>;

	void read_name(utf8_view name) {
		// (1)
	}

Now, we have an :doc:`explicit decoding view </api/views/decode_view>` into a sequence of UTF-8 code units, that produces ``unicode_code_point``\ s that we can inspect and work with. This is much better, as it uses C++'s strong typing mechanisms to give us a useful view. This means that not only does the person outside of the ``read_name`` function understand that the function expects some UTF-8 encoded text, but the person inside the function knows that they are working with UTF-8 encoded text. This solves both ends of the user and maintainer divide.

Of course, sometimes this is not always possible. ABI stability mandates some functions can't have their signatures change. Other times, you can't modify the signature of functions youu don't own. This is still helpful in this case, as you can, at the nearest available point inside the function or outside of it, apply these transformations:


.. code-block:: cpp
	:linenos:

	void read_name(const char* untagged_name) {
		using utf8_view = ztd::text::decode_view<
			ztd::text::basic_utf8<char>, // use "char" as the code unit type
			std::string_view // explicitly use this view type
		>;
		// constructs a std::string_view and
		// stores it in the proper place
		utf8_view name(untagged_name);
		// use it...
	}

Because the range and container types are templated on not only encoding, but the underlying storage type, you can wrap up both parameter and return values. You can also access the underlying ``std::string_view`` using ``.base()``, so it remains easy to interop and work with pre-existing systems using newer, more explicit types. Other ranges become possible, including, say, the `__gnu_cxx::rope <https://gcc.gnu.org/onlinedocs/gcc-10.2.0/libstdc++/api/a08538.html>` class that is part of the GCC Extensions Library. It genuinely doesn't matter what you pick: we will wrap it up and present the proper interface to you. This also follows UTF-8 Everywhere's requirements for what it would want out of a C++ Library that does text Correctly™:

.. epigraph::

	If you design a library that accepts strings, the simple, standard and lightweight std::string would do just fine. On the contrary, it would be a mistake to reinvent a new string class and force everyone through your peculiar interface. Of course, if one needs more than just passing strings around, he should then use appropriate text processing tools. However, such tools are better to be independent of the storage class used, in the spirit of the container/algorithm separation in the STL.

	-- `UTF-8 Everywhere, FAQ Entry #19 <https://utf8everywhere.org/#faq.ood>`_

Rather than create a new ``std::string`` or ``std::string_view``, we simply wrap existing storage interfaces and provide specific views or operations on those things. This alleviates the burden of having to reinvent things that already work fine for byte-oriented interfaces, and helps programmers control (and prevent) bugs. They also get to communicate their intent in their APIs if they so desire ("This API takes a ``std::string_view``, but with the expectation that it's going to be decoded as ``utf8``). The wrapped type will always be available by calling ``.base()``, which means a developer can drop down to the level they think is appropriate when they want it (with the explicit acknowledgement they're going to be ruining things).
