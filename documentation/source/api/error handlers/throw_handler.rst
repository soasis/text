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

throw_handler
=============

The ``throw_handler`` simply throws a :doc:`ztd::text::encoding_error </api/encoding_error>` as an exception. This should only EVER be used for pre-verified, trusted input sources, debugging purposes, or similar; malformed text is a common enough occurrence that throwing errors by default or using this handler by default is a **bad idea** in almost every way.

Throwing on encoding, decoding, and other errors can easily result in Denial of Service target points if this is used in conjunction with user or untrusted input sources.

.. doxygenclass:: ztd::text::throw_handler
	:members:
