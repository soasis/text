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

Bulk vs. Single Conversions
===========================

There is some wonder as to whether it is prudent for the Lucky 7 to be based on bulk conversions rather than the :term:`indivisible unit of work` it is based on right now. In general, if you have either a bulk or a single conversion, you can go from one to the other. However, while going from a single :term:`indivisible unit of work` to a bulk conversion by looping is a sound operation, trying to back-engineer a single indivisible unit of work from only a bulk conversion is not only an entirely unsound operation (it does not generalize to all cases), it is also has horrible performance unless intermediate buffers or allocations are deployed (and even then, **still** runs into soundness issues). Briefly:



Single Conversions
------------------

Single conversions are, effectively, doing “one unit of work” at a time. Whether converting Unicode one code point at a time or making iterators/views to go over a range of UTF-8 one bundle of non-error code units at a time, the implication here is that it uses minimal memory while guaranteeing that forward progress is made/work is done. It is absolutely not the most performant way of encoding, but it makes all other operations possible out of the composition of this single unit of work, so there's that. If a single conversion is the only thing you have, you can typically build up the bulk conversion from it.

In the opposite case, where only a bulk conversion API is available, single conversion API may still be implemented. Just take a bulk API, break the input off into a subrange of size 1 from the beginning of the input. Then, call the bulk API. If it succeeds, that is all that needs to be done. If not, take the subrange and make it a subrange of size 2 from the start of the input. Keep looping up until the bulk API successfully converts that sub-chunk of input, or until input exhaustion. There are, unfortunately, several problems with this methodology.

- It is horrifically inefficient. It is inadvisable to do this, feature set is literally the only goal of a given API.
- It is **unsound**, especially in the presence of optimizations. Consider a valid UTF-8 string consisting of 3 bytes: ``E2 81 87``. If :doc:`ztd::text::assume_valid_handler </api/error handlers/assume_valid_handler>` is used, or any similar "assume validitity" flag is set, then when the input is shortened to a length of 1, it will assume the 3-part UTF-8 character is actually a single, valid UTF-8 character. This will propagate through any system where that faux-"single" conversion API based off this bulk conversion will present an invalid single UTF-8 byte as a valid code point.

The above is just a few ways a bulk-only conversion that tries to simulate a single conversion API can go wrong. Pressuring a user to provide a single conversion first, and then a bulk conversion, will provide far better performance metrics and safety. For this library, we focus on providing a :doc:`Lucky 7 </design/lucky 7>` that focuses on the single case and then allows the user to extrapolate outside of that.

These performance metrics are documented in the :doc:`benchmarks of internal routines for ztd.text and ztd.cuneicode </benchmarks/internal - transcoding - UTF>`, where there is a special category of cuneicode using SIMD optimizations that then deploys the "single from bulk" conversion talked about below.



Bulk Conversions
----------------

Bulk conversions are a way of converting as much input as possible to fill up as much output as possible. The only stopping conditions for bulk conversions are exhausted input (success), not enough room in the output, an illegal input sequence, or an incomplete sequence (but only at the very end of the input when the input is exhausted). Bulk conversions open the door to using Single Instruction Multiple Data (SIMD) CPU instructions, GPU processing, parallel processing, and more, to convert large regions of text at a time.

More notably, given a stable single conversion function, running that conversion in a loop would produce the same effect, but may be slower due to various reasons (less able to optimize the loop, cannot be easily restructured to use SIMD, and more). Bulk conversions get around that by stating up-front they will process as much data as possible. A bulk conversion, derived from a properly-behaving conversion that works solely on a single indivisible unit of work, can not malfunction.
