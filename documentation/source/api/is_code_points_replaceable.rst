.. =============================================================================
..
.. ztd.text
.. Copyright © 2022-2023 JeanHeyd "ThePhD" Meneide and Shepherd's Oasis, LLC
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

is_code_points_(maybe\ _)replaceable
====================================

These two traits detect whether or not the given Encoding type have calls on them which return either a replacement range (``is_code_points_repleacable``) or a ``std::optional`` of a replacement range (``is_code_points_maybe_replaceable``).

The former is useful when it is guaranteed that your encoding will have a replacement range on it and does not need the extra cost of an indirection from not knowing. The latter is useful when something like a wrapped encoding may or may not have a replacement sequence.

.. doxygenclass:: ztd::text::is_code_points_replaceable
	:members:

.. doxygenvariable:: ztd::text::is_code_points_replaceable_v

.. doxygenclass:: ztd::text::is_code_points_maybe_replaceable
	:members:

.. doxygenvariable:: ztd::text::is_code_points_maybe_replaceable_v
