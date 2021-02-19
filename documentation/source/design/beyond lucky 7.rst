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

Beyond Lucky 7 - Extended Encoding Object Design
================================================

While the given :doc:`Lucky 7 </design/lucky 7>` represents the simplest possible encoding object one can design, there are several more type definitions, member functions, and other things an individual can use to create more complex encoding objects. Below, we are going to review the most pertinent ones that allow for better extensibility of the core design and let you go `Even Further Beyond <https://www.youtube.com/watch?v=tTelnNmRUH0>`_.



Extension 0: Separate Encode/Decode States
------------------------------------------

It is no secret that encoding and decoding may carrying with them separate states. While converting from a legacy encoding to Unicode may require maintenance of a shift state or code unit modifier, the opposite direction may not need any at all. Therefore, as an optimization, an encoding object can define both an ``encode_state`` and a ``decode_state``, seperate from each other. As an example, here is a (simplified) version of how :doc:`ztd::text::execution </api/encodings/execution>`, the encoding for the Locale-based Runtime Execution Encoding, has two seperate states that need to be initialized in different manners:

.. literalinclude:: /../../examples/documentation/source/design.beyond_lucky_7.encode_state_and_decode_state.cpp
	:language: cpp
	:linenos:
	:lines: 30-62,87

This is the proper way to initialize a ``std::mbstate_t`` from the C standard library. Then, you can use it:

.. literalinclude:: /../../examples/documentation/source/design.beyond_lucky_7.encode_state_and_decode_state.cpp
	:language: cpp
	:linenos:
	:lines: 31-35,66-87

This allows you to maintain 2 different states from 
