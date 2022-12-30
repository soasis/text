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

make_decode_state
=================

This detects when the ``decode_state`` of a given ``encoding`` requires the ``encoding`` itself to make said state. If so, it will call the ``decode_state``\ 's constructor with the encoding passed in. Otherwise, it simply default-constructs a state. In either case, the constructed value is returned to the user.

The classification for this is done by :doc:`ztd::text::is_decode_state_independent </api/is_decode_state_independent>`.

.. doxygenfunction:: ztd::text::make_decode_state

.. doxygenfunction:: ztd::text::make_decode_state_with
