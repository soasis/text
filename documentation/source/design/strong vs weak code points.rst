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

Strong vs. Weak Code Units/Points and Legacy Encodings
======================================================

Every encoding object must have ``code_point`` and ``code_unit`` type definitions on it. Typically, this is set to :doc:`ztd::text::unicode_code_point </api/unicode_code_point>`. But, if you go through a Prior Work for this library, you will notice Tom Honermann's `reference implementation for text_view <https://github.com/tahonermann/text_view>`_ has a concept of even more strictly controlled ``code_unit`` and ``character_type`` than this library. From the associated paper:

.. epigraph::

	This library defines a character class template parameterized by character set type used to represent character values. The purpose of this class template is to make explicit the association of a code point value and a character set.

	...

	It has also been suggested that ``char32_t`` might suffice as the only character type; that decoding of any encoded string include implicit transcoding to Unicode code points. The author believes that this suggestion is not feasible...


	-- `Tom Honermann, P0244 text_view <https://wg21.link/p0244>`_



The Case for Strength
---------------------

This general philosophy in Honermann's text_view means that you do not just use ``unsigned char`` or ``unicode_code_point`` for code unit and code point types, but instead traffic more directly in, for example, ``ebcdic_char`` and ``ebcdic_code_point`` types. They are essentially strong type definitions and strong wrappers simpler, "lower level" types like ``char32_t`` and ``char``. It has the following tradeoffs:

- ✔️ Can directly connect a range and its ``value_type`` to a specific encoding (e.g., ``default_code_point_encoding_t<ascii_code_point>`` means ``ascii``, definitively).
- ✔️ Actively prevents passing one type of range/view to a function expecting another (e.g., ``std::basic_string<ascii_char>`` cannot accidentally be given to a function expecting ``std::string``, where the expectation might be for an :term:`execution encoded <execution encoding>` string.)
- ✔️ Easy to strip out all encoding/codec information and the range types themselves can still recover it (e.g. ``ascii_code_point* u32_c_str_ptr`` can be strongly associated with the ``ascii`` encoding, whereas ``unicode_code_point* u32_c_str_ptr`` loses all that information.)
- ❌ Requires ``reinterpret_cast`` or ``std::memcpy``/``std::copy`` to work with most existing code that do not have such strongly typed pointers.
- ❌ Can generate a lot of template type spam for what are essentially just ``char``.
- ❌ Not very good in ``constexpr``, where ``reinterpret_cast`` isn't allowed and there are pre-existing ``constexpr`` functions that are not templated.

The question boils down to: should we have strong code point and code unit types **by default** in the library?

Henri Sivonen — author of `encoding_rs <https://github.com/hsivonen/encoding_rs>`_ and expert in the text domain — strongly disagrees.



The Counterpoint
----------------

In a long piece on P0422, the C and C++ landscape, and Standardization efforts, Henri writes:

.. epigraph::

	I think the C++ standard should adopt the approach of “Unicode-only internally” for new text processing facilities and should not support non-Unicode execution encodings in newly-introduced features. This allows new features to have less abstraction obfuscation for Unicode usage, avoids digging legacy applications deeper into non-Unicode commitment, and avoids the specification and implementation effort of adapting new features to make sense for non-Unicode execution encodings.

	-- `Henri Sivonen, It’s Time to Stop Adding New Features for Non-Unicode Execution Encodings in C++ <https://hsivonen.fi/non-unicode-in-cpp/>`_

This is a different set of choices and a different set of priorities from the outset. Sivonen's work specifically is that with Browsers and large code bases like Firefox; they are responsible for making very good traction and progress on encoding issues in a world that is filled primarily with Unicode, but still has millions of documents that are not in Unicode and, for the foreseeable future, won't end up as Unicode.

This is a strong argument for simply channeling ``char16_t``, ``char32_t``, and -- since C++20 -- ``char8_t`` as the only types one would need. Firefox at most deals in UTF-16 (due to the JavaScript engine for legacy reasons) and UTF-8, internally. At the boundaries, it deals with many more text encodings, because it `has to from the world wide web <https://encoding.spec.whatwg.org/>`_. Occasionally, UTF-32 will appear in someone's codebase for interoperation purposes or algorithms that need to operate on something better than code units. 

Unicode is also... well, a [UNI]versal [CODE]. Its purposes are interoperation, interchange, and common ground between all the encodings, and it has been the clear winner for this for quite some time now. Sivonen makes a compelling point for just considering Unicode — and only Unicode — for all future text endeavors.

Do we really need to focus on having support for legacy encodings? Or at least, do we really need support for legacy encodings at the level that Tom Honermann's text_view is trying to achieve?

ztd.text's answer is simple:

.. image:: /img/pnld-lass.jpg
	:alt: The "¿Porque no los dos?" reaction image.



Allow Both, Prefer One
----------------------

ztd.text prefers Henri Sivonen's approach to the library in general. The ``code_unit`` type is generally a weakly-typed choice of one of the 6 viable code unit types in C++ (``char``, ``wchar_t``, ``unsigned char``, ``char8_t``, ``char16_t``, and ``char32_t``). The ``code_point`` type is typically just ``unicode_code_point`` (an alias for ``char32_t`` :ref:`by default <config-ZTD_TEXT_UNICODE_CODE_POINT_DISTINCT_TYPE>`) or ``unicode_scalar_value`` (a strong type :ref:`by default <config-ZTD_TEXT_UNICODE_SCALAR_VALUE_DISTINCT_TYPE>`, because it carries extra pertinent information about itself that can aid the library). Unfortunately, this means that :doc:`ztd::text::default_code_point_encoding_t </api/default_code_point_encoding>` is not a very rich type mapping (it generally just spits out UTF-8).

This does not mean all future algorithms bear the burden of supporting an infinity of text encodings. But, the work for encoding and decoding text is isolated and constrained specifically to the :doc:`encoding objects </encodings>`, :doc:`view types </api/views/decode_view>`, and :doc:`functions </api/conversions/transcode>` that power this library. Down-stream algorithms — like those found in Zach Laine's Boost.Text — work only with range/iterator types whose ``value_type`` are either ``unicode_code_point``\ s or ``unicode_scalar_value``\ s.

By having a core, standard ecosystem that works primarily with ``unicode_code_point`` and ``unicode_scalar_value``, we heavily incentivize the use of these two types as the only interchange types. Furthermore, because all of the encodings provided by this library use ``unicode_code_point`` as their ``code_point`` type, we set a strong example for the rest of the ecosystem who may work with and look at these files. This is even the case for the default byte-based encoding :doc:`ztd::text::any_encoding </api/encodings/any_encoding>`, which strongly incentivizes compatibility with the ecosystem by making it clear that there is a preferred default mode of communication (which is, :doc:`ztd::text::unicode_code_point </api/unicode_code_point>`). In effect, we produce The Unicode™Ⓡ Vortex™:

.. image:: /img/unicode-vortex.png
	:alt: 2-liter soda bottles, 2 of them, connected by their uncapped openings. There is a vortex of water in them transferring water from the top to the bottom. The vortex is labeled "Your Glorious Unicode Application", the top is labeled "Input Badlands", and the bottom is labeled "Interoperation Hellscape".

This might be the perfect world for most people, but even so there's room inside that funneled vortex for more.


Leaving Room
++++++++++++

There is room in Sivonen's world, even with perfectly-consistent and fully-Unicode internals, for Honermann's dream of never losing encoding information at even the lowest levels. After all, if someone takes the time to wrap up external interfaces (Shared Pipes, Network Connections, Terminal Interfaces, ``char`` Devices, and more), they should have the ability to tag these interfaces with either encoding objects or strong, ``reinterpret_cast``\ -able pointer values.

That's why encodings can still define their own ``code_unit`` and ``code_point`` types; even if this library — or the Standard Library — traffics in strictly ``unicode_code_point``\ s, it doesn't mean the user should be forced to do that if they are willing to put in the effort for a more type-safe world.

Being able to know, at compile-time, without any objects or markup, that a particular pointer + size pairing is meant for a specific encoding is a powerful way to maintain invariants and track the flow of data without runtime cost through a program. It can also make it easy to find places where external, non-Unicode data is making it "too far" into the system, and try to push a conversion closer to the edges of the program.

While ztd.text will traffic and work with ``char32_t`` and consider it a ``unicode_code_point`` value :doc:`under most circumstances </api/is_unicode_code_point>`, users are free to define and extend this classification for their own types and generally create as strict (or loose) as taxonomy as they desire.


In Sum
++++++

The library still overwhelmingly traffics in Unicode, and we believe that by making it the default and producing an overwhelming body of code that treats it as such as can push people towards that default. Using ``char32_t``\ s, ``unicode_code_point``\ s, and ``unicode_scalar_value``\ s as Sivonen advocates should have a very low "activation energy". Reaching for the strict world envisioned with Honermann's ``text_view`` and its associated implementation is still possible, but requires more energy. We do not force the user to put in that energy.

As long as both are possible, users can find satisfaction for both of their use cases. Even if Honermann's design is more work, it is still quite useful and can lead to a very robust and statically-verifiable design in even huge, complex software systems.
