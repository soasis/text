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

skip_input_error
================

Utilizes :doc:`ztd::text::is_input_error_skippable </api/is_input_error_skippable>` to check if a hypothetical encoding object ``encoding`` and a hypothetical result object ``result`` can be utilized to call `encoding.skip_input_error(result)`. Otherwise, it only skips forward by

It is particularly used with :doc:`ztd::text::replacement_handler </api/error handlers/replacement_handler>` and :doc:`ztd::text::skip_handler </api/error handlers/skip_handler>`. You can use this function to perform encoding-aware and encoding-specific skips for a given encode and decode operation. This is particularly useful for Unicode encodings, as well as other variable-width encodings in the wild, which may be good to accommodate for in your own error handlers.

.. doxygenfunction:: ztd::text::skip_input_error
