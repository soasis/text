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

Preventing Lost information
===========================

One of the biggest problems facing text processing in programming languages today is the loss of information as its carried through any given system. In C and C++, this comes in the form of all strings - especially multibyte strings - being given the same type. For example:

.. code-block:: cpp
	:linenos:

	void read_name(const char* name) {
		// (1)
	}

As the maintainer of code inside of the function ``read_name``, what is the encoding of "name" at ``(1)``? What is its normalization form? What system did it originate from? The function written in C++ form offers very little benefit either:

.. code-block:: cpp
	:linenos:

	void read_name(std::string name) {
		// (1)
	}


Even here, we've only made marginal improvements. We know the string is stored in some heap by the default allocator, we have the size of the string, but that only tells us how many ``char`` units are stored, not how many conceptual, human-readable characters there are or any other pertinent information.
