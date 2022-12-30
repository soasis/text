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

replacement_handler
===================

The ``replacement_handler_t`` is the go-to error handling class. It is also the :doc:`ztd::text::default_handler </api/error handlers/default_handler>` unless configured otherwise.

Replacement works by using several different hooks on the provided encoding objects, or by falling back to some defaults if certain conditions are met. The user-controllable hooks are:

- ``encoding.replacement_code_units(...)``, a function (which can be ``static`` or ``constexpr``) that returns a range of code units to insert directly into an output stream on a failed ``encode`` operation. It can also be called as a secondary backup if an ``decode`` operation fails, whereupon it will use the values in the range to attempt ``decode``\ ing them into the output if possible. It can be empty, to indicate that nothing is to be inserted.
- ``encoding.replacement_code_points(...)``, a function (which can be ``static`` or ``constexpr``) that returns a range of code points to insert directly into an output stream on a failed ``decode`` operation. It can also be called as a secondary backup if an ``encode`` operation fails, whereupon it will use the values in the range to attempt ``encode``\ ing them into the output if possible. It can be empty, to indicate that nothing is to be inserted.
- ``encoding.maybe_replacement_code_units(...)``, a function (which can be ``static`` or ``constexpr``) that returns a maybe-range. If the expression ``if (maybe_returned_range)`` evaluates to ``true``, it will get the range returned by the function by performing a dereference of ``decltype(auto) returned_range = *maybe_returned_range;``. If the conditional expression does not evaluate to ``true``, it will assume that nothing can be returned from the function. This is useful for runtime-only encodings or encodings that wrap other encodings and may not have a replacement function. The dereferenced returned range is used exactly as its non-\ ``maybe`` counterpart.
- ``encoding.maybe_replacement_code_points(...)``, a function (which can be ``static`` or ``constexpr``) that returns a maybe-range. If the expression ``if (maybe_returned_range)`` evaluates to ``true``, it will get the range returned by the function by performing a dereference of ``decltype(auto) returned_range = *maybe_returned_range;``. If the conditional expression does not evaluate to ``true``, it will assume that nothing can be returned from the function. This is useful for runtime-only encodings or encodings that wrap other encodings and may not have a replacement function. The dereferenced returned range is used exactly as its non-\ ``maybe`` counterpart.

Each replacement handler can take the current ``encode_state``/\ ``decode_state`` parameter for its desired operation, if it so chooses. This will allow replacements to hook into the statefulness of any given encoding operation. It will call ``replacement_code_units(state)`` first, if it's well-formed. Otherwise, it will call ``replacement_code_units()``. It will do this with each of the 4 replacement functions mentioned above.

After a replacement is done (if any), the function will then skip over bad input. The skipping is done by calling :doc:`ztd::text::skip_input_error(…) </api/skip_input_error>` with the encoding and result object, before returning, and after replacements are done.

.. doxygenvariable:: ztd::text::replacement_handler

.. doxygenclass:: ztd::text::replacement_handler_t
	:members:
