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
.. 		https://www.apache.org/licenses/LICENSE-2.0
..
.. Unless required by applicable law or agreed to in writing, software
.. distributed under the License is distributed on an "AS IS" BASIS,
.. WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
.. See the License for the specific language governing permissions and
.. limitations under the License.
..
.. =============================================================================>

Validate Encodable
==================

Validation is the way to verify a given sequence of input can have a specific action performed on it. Particularly, we check here if the input of code points can be turned into code units of the given encoding. The way it does this, however, is two-fold:

- it first encodes the input code units, to see if it can do the transformation without loss of information; then,
- it decodes the output from the last step, to see if the final output is equivalent to the input.

The algorithm for this is as follows:

* ⏩ Is the ``input`` value empty? Return the current results with the the empty ``input``, ``valid`` set to ``true`` and ``state``\ s, everything is okay ✅! Otherwise,

   0. Set up an ``intermediate`` storage location of ``code_unit``\ s, using the ``max_code_units`` of the input encoding, for the next operations.
   1. Set up an ``intermediate_checked_output`` storage location of ``code_point``\ s, using the ``max_code_points`` of the input encoding, for the next operations.
   2. Do the ``encode_one`` step from ``input`` (using its ``begin()`` and ``end()``) into the ``intermediate`` ``code_unit`` storage location.

      * 🛑 If it failed, return with the current ``input`` (unmodified from before this iteration, if possible), ``valid`` set to false, and ``state``\ s.

   3. Do the ``decode_one`` step from the ``intermediate`` into the ``intermediate_checked_output``.

      * 🛑 If it failed, return with the current ``input`` (unmodified from before this iteration, if possible), ``valid`` set to false, and ``state``\ s.

   4. Compare the ``code_point``\ s of the ``input`` sequentially against the ``code_point``\ s within the ``intermediate_checked_output``.

      * 🛑 If it failed, return with the current ``input`` (unmodified from before this iteration, if possible), ``valid`` set to false, and ``state``\ s.

* ⏩ Update ``input``\ 's ``begin()`` value to point to after what was read by the ``decode_one`` step.
* ⤴️ Go back to the start.

This fundamental process works for all encoding objects, provided they implement the basic :doc:`Lucky 7 </design/lucky 7>`. The reason for checking if it can be turned back is to ensure that the input code units actually match up with the output code units. If an encoding performs a lossy transformation in one direction or the other, then validation will fail if it cannot reproduce the input exactly. And, you will know the exact place in the ``input`` that caused such a failure.

There are extension points used in the API that allow certain encodings to get around the limitation of having to do both the ``encode_one`` step and the ``decode_one`` step, giving individual encodings control over the verification of a single unit of input and of bulk validation as well.

Check out the API documentation for :doc:`ztd::text::validate_encodable_as </api/conversions/validate_encodable_as>` to learn more.
