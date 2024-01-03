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

Recode
======

Transcoding is the action of converting from one sequence of encoded information to another sequence of (usually differently) encoded information. The formula given for Transcoding is actually exactly the same as the one shown in the :ref:`main Lucky 7 documentation <design-lucky 7-transcoding loop>`, reproduced here:

The core tenant here is that as long as there is a common intermediary between the 2 encodings, you can decode from the given input into that shared common intermediary (e.g., :term:`unicode code points <unicode code point>` or :term:`unicode scalar values <unicode scalar value>`), then encode from the common intermediary to the second encoding's output. This is a pretty basic way of translating data and it's not even a particularly new idea (:term:`iconv <iconv>` has been doing this for well over a decade now, :term:`libogonek <libogonek>` got this core idea rolling in a C++ interface, and in general this is quite literally how all data interchange has been done since forever). The equalizer here is that, unlike other industries that struggle to define an interchange format, Unicode Code Points has become the clear and overwhelming interoperation choice for people handling text all over the world.

Thusly, we use the algorithm as below to do the work. Given an ``input`` of ``code_unit``\ s with a ``from_encoding``, a ``to_encoding`` with a target ``output``, and any necessary additional ``state``\ s, we can generically convert that one encoding to the other so long as those encodings follow the Lucky 7 design:

* ⏩ Is the ``input`` value empty? If so, is the ``state`` finished and have nothing to output? If both are true, return the current results with the the empty ``input``, ``output``, and ``state``, everything is okay ✅!
* ⏩ Otherwise,

   0. Set up an ``intermediate`` storage location of ``code_point``\ s, using the ``max_code_points`` of the input encoding as the maximum size of the storage location, for the next operation.
   1. Do the ``decode_one`` step from ``input`` (using its ``begin()`` and ``end()``) into the ``intermediate`` ``code_point`` storage location.

      * 🛑 If it failed, return with the current ``input`` (unmodified from before this iteration, if possible), ``output``, and ``state``\ s.

   2. Do the ``encode_one`` step from the ``intermediate`` into the ``output``.

      * 🛑 If it failed, return with the current ``input`` (unmodified from before this iteration, if possible), ``output``, and ``state``\ s.

* ⏩ Update ``input``\ 's ``begin()`` value to point to after what was read by the ``decode_one`` step.
* ⤴️ Go back to the start.

This fundamental process works for any 2 encoding pairs, and does not require the first encoding ``from_encoding`` to know any details about the second encoding ``to_encoding``! This means a user is only responsible for upholding their end of the bargain with their encoding object, and can thusly interoperate with every other encoding that speaks in the same intermediate, decoded values (i.e. :term:`unicode code points <unicode code point>`).

Check out the API documentation for :doc:`ztd::text::recode </api/conversions/recode>` to learn more.
