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

is_decode_injective_v
=====================

Looks to see if the ``decode_one`` operation on a given encoding type is :term:`injective <injective>`.

This classification checks whether the given encoding type has a type definition called ``is_decode_injective`` on it, and if it does checks to see if its ``std::true_type``. If it's not present, or if it's ``std::false_type``, then the encoding is assumed to NOT be :term:`injective <injective>`.

.. doxygenclass:: ztd::text::is_decode_injective
	:members:

.. doxygenvariable:: ztd::text::is_decode_injective_v
