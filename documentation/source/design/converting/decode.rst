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

Decode
======

Decoding is the action of converting from one sequence of encoded information to a sequence of decoded information. The formula given for Decoding is effectively just the first half of the diagram shown in the :ref:`main Lucky 7 documentation <design-lucky 7-transcode-transcoding loop>`, reproduced here with emphasis added:

.. _design-converting-decode-encode decode loop:

.. figure:: /img/encode-decode-path.png
   :alt: Encode and Decode paths, split up and labeled with "Decode" on the top side, "Encode" on the bottom side, and the respective operation loops in each of the image's corners.

   The generic pathway between 2 encodings, but modified to show the exact difference between the encoding step and the decoding step.

In particular, we are interested in the operation that helps us go from the encoded input to the decoded output, which is the top half of the diagram. The output we are interested in is labeled as an "intermediate", because that is often what it is. But, there are many uses for working directly with the decoded data. Many Unicode algorithms are specified to work over :term:`unicode code points <unicode code point>` or :term:`unicode scalar values <unicode scalar value>`. In order to identify Word Breaks, classify Uppercase vs. Lowercase, perform Casefolding, Regex over certain properties properly, Normalize text for search + other operations, and many more things, one needs to be working with code points as the basic unit of operation.

Thusly, we use the algorithm as below to do the work. Given an ``input`` of ``code_unit``\ s with an ``encoding``, a target ``output``, and any necessary additional ``state``, we can generically bulk convert the input sequence to a form of ``code_point``\ s in the ``output``:

* ⏩ Is the ``input`` value empty? Return the current results with the the empty ``input``, ``output``, and ``state``, everything is okay ✅! Otherwise,

   0. Do the ``decode_one`` step from ``input`` (using its ``begin()`` and ``end()``) into the ``output`` ``code_point`` storage location.

      * 🛑 If it failed, return with the current ``input`` (unmodified from before this iteration, if possible), ``output``, and ``state``\ s.

* ⏩ Update ``input``\ 's ``begin()`` value to point to after what was read by the ``decode_one`` step.
* ⤴️ Go back to the start.

This involves a single encoding type, and so does not need any cooperation to go from the ``code_unit``\ s to the ``code_point``\ s. Notably, the encoding's ``code_point`` type will hopefully be some sort of :term:`unicode code point <unicode code point>` type (see: :doc:`ztd::text::is_code_point </api/is_unicode_code_point>` for a more code-based classification). Though, it does not have to be for :doc:`many different (and very valid) reasons </design/strong vs weak code points>`.

Check out the API documentation for :doc:`ztd::text::decode </api/conversions/decode>` to learn more.
