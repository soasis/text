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

PETSCII (Shifted & Unshifted, Combined) / CBM ASCII
===================================================

PET Standard Code of Information Interchange (PETSCII) was used for Commodore Business Machines and then moved into other Commodore machines (and adjacent machines). It has a "shifted" version (when the shift key was held) and an "unshifted" version (when the shift key was not being held).

The state object for this encoding contains an enumeration that allows the user to select the shifted or unshifted versions at-will.



Aliases
-------

.. doxygenclass:: ztd::text::petscii_state

.. doxygenenum:: ztd::text::petscii_shift

.. doxygenvariable:: ztd::text::petscii



Base Templates
--------------

.. doxygenclass:: ztd::text::basic_petscii
	:members:
