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

Encode
======

Encoding is the action of converting from one sequence of decoded information to a sequence of encoded information. The formula given for Encoding is effectively just the first half of the diagram shown in the :ref:`main Lucky 7 documentation <design-lucky 7-transcode-transcoding loop>`, reproduced here with emphasis added:

.. _design-converting-encode-encode decode loop:

.. figure:: /img/encode-decode-path.png
   :alt: Encode and Decode paths, split up and labeled with "Decode" on the top side, "Encode" on the bottom side, and the respective operation loops in each of the image's corners.

   The generic pathway between 2 encodings, but modified to show the exact difference between the encoding step and the decoding step.

In particular, we are interested in the operation that helps us go from the decoded input to the encoded output, which is the bottom half of the diagram. The input in this case is labeled "intermediate", because that is often what it is. But, there are many uses for working directly with the encoded data. A lot of the world does not speak directly in 21-bit Unicode Code Points, but instead speaks in UTF-8. Legacy systems are often found communicating with Code Pages (e.g., EBCDIC or SHIFT-JIS); until those systems go down or are replaced, it is imperative to send them well-formed data, whether over a network or across an inter-process communication bridge of any kind.

Thusly, we use the algorithm as below to do the work. Given an ``input`` of ``code_point``\ s with an ``encoding``, a target ``output``, and any necessary additional ``state``, we can generically convert that sequence of ``code_point``\ s into its encoded form:

* ⏩ Is the ``input`` value empty? Return the current results with the the empty ``input``, ``output``, and ``state``, everything is okay ✅! Otherwise,

   0. Do the ``encode_one`` step from ``input`` (using its ``begin()`` and ``end()``) into the ``output`` ``code_unit`` storage location.

      * 🛑 If it failed, return with the current ``input`` (unmodified from before this iteration, if possible), ``output``, and ``state``\ s.

* ⏩ Update ``input``\ 's ``begin()`` value to point to after what was read by the ``encode_one`` step.
* ⤴️ Go back to the start.

This involves a single encoding type, and so does not need any cooperation to go from the ``code_point`` sequence to the ``code_unit`` sequence.

Check out the API documentation for :doc:`ztd::text::encode </api/conversions/encode>` to learn more.
