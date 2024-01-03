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

punycode
========

Punycode is an encoding that turns Unicode-encoded text into a sequence of ASCII code units, making it readily inspectable by simple systems and human eyes to see differences between text even if confusable characters are used in the Unicode text.

It is most frequently used in the Internationalized Domain Name in Applications system, specifically for DNS purposes. Despite it's unfriendliness to stream-based, one-at-a-time interfaces, sufficiently heap-based state allows encoding and decoding in a useful manner.

It is also used as Rust's choice encoding for its symbol names when stored in binary form for use with any given platform's library/executable's linker and loader.



Aliases
-------

.. doxygenvariable:: ztd::text::punycode

.. doxygenvariable:: ztd::text::punycode_idna

.. doxygentypedef:: ztd::text::punycode_t

.. doxygentypedef:: ztd::text::punycode_idna_t

.. doxygentypedef:: ztd::text::basic_punycode

.. doxygentypedef:: ztd::text::basic_punycode_idna



Base Template
-------------

.. doxygenclass:: ztd::text::basic_any_punycode
	:members:
