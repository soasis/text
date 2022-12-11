.. =============================================================================
..
.. ztd.text
.. Copyright © 2022 JeanHeyd "ThePhD" Meneide and Shepherd's Oasis, LLC
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

is_input_error_skippable
========================

For an encoding object ``encoding``, it checks that there exists a function that can be called with the format ``encoding.skip_input_error(result)``, where ``result`` is either a :doc:`ztd::text::decode_result\<…> </api/decode_result>` (for decoding operations which have failed) and :doc:`ztd::text::encode_result\<…> </api/encode_result>` (for encoding operations which have failed). This allows an encoding to control precisely how to move ahead with the input when a problem is encountered.

.. doxygenclass:: ztd::text::is_input_error_skippable
	:members:

.. doxygenvariable:: ztd::text::is_input_error_skippable_v
