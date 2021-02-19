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

decode_view
===========

The ``decode_view`` class provides a one-by-one view of the stored range's code points as the desired encoding's code units. Dereferencing the iterators returns a single ``code_point`` value corresponding to the desired encoding's transformation of the internal code units.

The range-based classes are excellent ways to walk over units of information in a low-memory environment, as they only store the minimum amount of data necessary to perform their operations on the fly. This reduces the speed but is fine for one-at-a-time encoding operations. To decode eagerly and in bulk, see :doc:`the decode functions </api/conversions/decode>`.

.. doxygenclass:: ztd::text::decode_view
	:members:
