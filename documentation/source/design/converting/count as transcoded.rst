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

Count as Transcoded
====================

This operation counts how much text will result from a transcode operation. Essentially, we run the :doc:`encoding algorithm </design/converting/transcode>` loop, but instead of giving the end user the re-encoded values, we instead simply provide the count for running that bulk operation.

Thusly, we use the algorithm as below to do the work. Given an ``input`` of ``code_unit``\ s with an ``encoding``, an initial ``count`` set at 0, and any necessary additional ``state``, we can generically predict how many code units will result from a decoding operation by running the following loop:

* ⏩ Is the ``input`` value empty? Return the current results with the the empty ``input``, curent ``count``, and ``state``, everything is okay ✅! Otherwise,

   0. Set up an ``intermediate`` storage location of ``code_point``\ s (of the input encoding), using the ``max_code_points`` of the input encoding; and, set up an ``intermediate_output`` storage location of ``code_unit``\ s (of the output encoding), for the next operations.
   1. Do the ``decode_one`` step from ``input`` (using its ``begin()`` and ``end()``) into the ``intermediate`` ``code_point`` storage location, saving the returned ``intermediate_output`` from the ``decode_one`` call.

      * 🛑 If it failed, return with the current ``input`` (unmodified from before this iteration, if possible), current ``count``, and ``state``\ s.

   2. Do the ``encode_one`` step from ``intermdiate`` (using its ``begin()`` and ``end()``) into the ``intermediate_output`` ``code_unit`` storage location, saving the returned ``intermediate_output`` from the ``encode_one`` call.

      * 🛑 If it failed, return with the current ``input`` (unmodified from before this iteration, if possible), current ``count``, and ``state``\ s.

   3. Compute the difference between the ``begin(intermediate_output)`` from the original step, and the ``begin(result.output)`` returned by ``encode_one``; add that difference to the current ``count``.

* ⏩ Update ``input``\ 's ``begin()`` value to point to after what was read by the ``encode_one`` step.
* ⤴️ Go back to the start.

This involves a single encoding type, and so does not need any cooperation to count the ``code_point``\ s. Note that this algorithm doesn't show what the error handler does; if the error handler "erases" the failure by setting the result type's ``.error_code == ztd::text::encoding_error::ok``, then the algorithm will keep going. This is useful to, for example, detect the maximum size of an operation even if it errors and would result in replacement characters being inserted (e.g., from :doc:`ztd::text::replacement_handler_t </api/error handlers/replacement_handler>`).

Check out the API documentation for :doc:`ztd::text::count_as_transcoded </api/conversions/count_as_transcoded>` to learn more.
