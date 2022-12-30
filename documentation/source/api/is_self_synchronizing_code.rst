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

is_self_synchronizing_code
==========================

An encoding -- after experiencing an error or when being dropped into the middle -- can seek out the start of the next sequence of well-formed input unambiguously, compared with other encodings that reuse bit patterns from the first matching code unit / byte in subsequent code units / bytes in that pattern. This becomes a property of the encoding, and is therefore called *self synchronizing*. Self synchronizing codes are frequently considered superior when there is a lack of context.

The primary :doc:`Unicode encodings </known unicode encodings>` are self synchronizing, but other versions both proprietary and not may fail to be self-synchronizing such as UTF-7.

.. doxygenclass:: ztd::text::is_self_synchronizing_code
	:members:

.. doxygenvariable:: ztd::text::is_self_synchronizing_code_v
