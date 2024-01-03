.. =============================================================================
..
.. ztd.text
.. Copyright © JeanHeyd "ThePhD" Meneide and Shepherd's Oasis, LLC
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

Bibliography
============

These are all the resources that this documentation links to, in alphabetical order.


.. glossary::
	:sorted:

	cuneicode
		JeanHeyd Meneide & Shepherd's Oasis, LLC. cuneicode. September 2022. URL: `https://savannah.gnu.org/git/?group=libiconv <https://savannah.gnu.org/git/?group=libiconv>`_. `A software library <https://www.gnu.org/software/libiconv/>`_ for working with and converting text. Typically ships on most, if not all, POSIX and Linux systems.

	iconv
		Bruno Haible and Daiki Ueno. libiconv. August 2020. URL: `https://savannah.gnu.org/git/?group=libiconv <https://savannah.gnu.org/git/?group=libiconv>`_. `A software library <https://www.gnu.org/software/libiconv/>`_ for working with and converting text. Typically ships on most, if not all, POSIX and Linux systems.

	ICU
		Unicode Consortium. "International Components for Unicode". April 17th, 2019. URL: `https://github.com/hsivonen/encoding_rs <https://github.com/hsivonen/encoding_rs>`_ The premiere library for not only performing encoding conversions, but performing other Unicode-related algorithms on sequences of text.

	libogonek
		R. Martinho Fernandes. "libogonek: A C++11 Library for Unicode". September 29th, 2019. URL: `http://site.icu-project.org/ <http://site.icu-project.org/>`_ One of the first influential C++11 libraries to bring the concept of iterators and ranges to not only encoding, but normalization and others. It's great design was only limited by how incapable C++11 as a language was for what its author was trying to do.

	encoding_rs
		Henri Sivonen. "encoding_rs". February 2021. URL: `https://github.com/libogonek/ogonek <https://github.com/libogonek/ogonek>`_. A Rust library for performing encoding and decoding tasks. Takes a byte-based approach to handling encodings and decodings. The developer of this library worked on text for a very long time on Mozilla Firefox, and has great insight into the field of text `on their blog, https://hsivonen.fi <https://hsivonen.fi>`_.
	
	p0244
		Tom Honermann. "P0244 - Text_view: A C++ concepts and range based character encoding and code point enumeration library". URL: `https://wg21.link/p0244 <https://wg21.link/p0244>`_. A C++ proposal written by Tom Honermann, proposing some of the first ideas for an extensible text encoding interface and lightweight ranges built on top of that. Reference implementation: `https://github.com/tahonermann/text_view <https://github.com/tahonermann/text_view>`_.
	
	Non-Unicode in C++
		Henri Sivonen. "P0244 - Text_view: A C++ concepts and range based character encoding and code point enumeration library". URL: `https://hsivonen.fi/non-unicode-in-cpp/ <https://hsivonen.fi/non-unicode-in-cpp/>`_. A rebuttal to P0244's "strong code points" and "strong code units" opinion. This is talked about in depth in the design documentation for :doc:`strong vs. weak code point and code unit types </design/strong vs weak code points>`.

	p1041
		R. Martinho Fernandes. "P1041: Make char16_t/char32_t string literals be UTF-16/32". February 2019. URL: `https://wg21.link/p1041 <https://wg21.link/p1041>`_. This accepted paper enabled C++ to strongly associate all ``char16_t`` and ``char32_t`` string literals with UTF-16 and UTF-32. This is not the case for C.

	glibc-25744
		Tom Honermann and Carlos O’Donnell. ``mbrtowc`` with Big5-HKSCS returns 2 instead of 1 when consuming the second byte of certain double byte characters. `https://sourceware.org/bugzilla/show_bug.cgi?id=25744 <https://sourceware.org/bugzilla/show_bug.cgi?id=25744>`_. This bug report details the problem with the C standard library's ability to handle multiwide characters. This problem is also present in the "1:N" and "N:1" rules in the C++ standard library.

	n2282
		Philip K. Krause. "N2282 - Additional multibyte/wide string conversion functions". June 2018. URL: `http://www.open-std.org/jtc1/sc22/wg14/www/docs/n2282.htm <http://www.open-std.org/jtc1/sc22/wg14/www/docs/n2282.htm>`_. This paper attempted to add a few unicode functions to the list of things to do without changing anything.

	Fast UTF-8
		Bob Steagall. "Fast Conversion from UTF-8 with C++, DFAs, and SSE Intrinsics". September 26th, 2019. URL: `https://www.youtube.com/watch?v=5FQ87-Ecb-A <https://www.youtube.com/watch?v=5FQ87-Ecb-A>`_. This presentation demonstrates one of the ways an underlying fast decoder for UTF-8 can be written, rather than just letting the default work. This work can be hooked into the :doc:`conversion function extension points </design/converting>` location.

	Fast UTF-8 Validation
		Daniel Lemire. "Ridiculously fast unicode (UTF-8) validation". October 20th, 2020. URL: `https://lemire.me/blog/2020/10/20/ridiculously-fast-unicode-utf-8-validation/ <https://lemire.me/blog/2020/10/20/ridiculously-fast-unicode-utf-8-validation/>`_. This blog post is one of many that presents a faster, more optimized way to validate that UTF-8 is in its correctly form.
