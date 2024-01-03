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

🔨 cuneicode_registry_encoding (In Progress)
============================================

This encoding is tied to the `cuneicode library <https://ztdcuneicode.rtfd.io>`_. The cuneicode library is a C library for validation, counting, and transcoding between a fixed set of encodings, with an additional plug for arbitrary encodings that can be added at run-time. This is in opposition to :doc:`iconv </api/encodings/basic_iconv>`, where additional encodings can only be added by-hand through recompiling the code or hooking specific system configuration points.

cuneicode has a variable number of encodings it can be compiled with to support. States are pre-constructed in the encoding itself and copied as necessary when ``encode_state`` or ``decode_state``\ s are being created to call the desired conversion functions. The user can inspect the output error parameter from the ``cuneicode_registry_encoding`` constructor to know of failure, or not pass in the output error parameter and instead take one of a assert, thrown exception, or ``abort`` (preferred invocation in that order).
