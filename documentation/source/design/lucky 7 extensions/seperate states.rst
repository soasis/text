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

Separate Encode/Decode States
=============================

It is no secret that encoding and decoding may carrying with them separate states. While converting from a legacy encoding to Unicode may require maintenance of a shift state or code unit modifier, the opposite direction may not need any at all. Therefore, as an optimization, an encoding object can define both an ``encode_state`` and a ``decode_state``, seperate from each other. As an example, here is a (simplified) version of how :doc:`ztd::text::execution </api/encodings/execution>`, the encoding for the Locale-based Runtime Execution Encoding, has two seperate states that need to be initialized in different manners:

.. literalinclude:: /../../examples/documentation/source/runtime_locale_encoding.cpp
	:language: cpp
	:linenos:
	:lines: 46-47,55-73,233

This is the proper way to initialize a ``std::mbstate_t`` from the C standard library. Then, you can use it! Here's a complete implementation using the new ``encode_state`` and ``decode_state`` types:

.. literalinclude:: /../../examples/documentation/source/runtime_locale_encoding.cpp
	:language: cpp
	:linenos:
	:lines: 46,116-233

This allows you to maintain 2 different states, initialized in 2 different ways, one for each of the ``encode_one`` and ``decode_one`` paths.
