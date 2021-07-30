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

Wide Literal
============

The ``wide_literal`` encoding handles C and C++ wide string literals (``L"🐶"``) used at compile time and stored in the binary. The library uses a number of heuristics to determine with any degree of certainty what the encoding of string literals are, but in some cases it is not explicitly possible to achieve this goal.

If the library cannot figure out the wide literal encoding, the code will typically error with a ``static_assert``, loudly, that it cannot use the functions on the type when you attempt to do anything with them because it may mangle whatever input or output you are expecting.

If you know the encoding of wide literals for your project (you provide the command line switch, or similar), then you can define a :ref:`configuration macro named ZTD_CXX_COMPILE_TIME_WIDE_ENCODING_NAME <config-ZTD_CXX_COMPILE_TIME_WIDE_ENCODING_NAME>` to be a string literal of your type, such as ``"UTF-16"`` or ``"EUC-TW"``.

If the library does not recognize the encoding and cannot transcode it properly, it will also loudly warn you that it does not understand the encoding of the literal (in which case, file an issue about it and we will add it to the list of acceptable wide literal encodings).

If you like to live dangerously and do not care for the warnings, you can define :ref:`a configuration macro named ZTD_TEXT_YES_PLEASE_DESTROY_MY_WIDE_LITERALS_UTTERLY_I_MEAN_IT <config-ZTD_TEXT_YES_PLEASE_DESTROY_MY_WIDE_LITERALS_UTTERLY_I_MEAN_IT>` and it will just blindly go with whatever weird default it ended up deciding on.

(This is usually a catastrophically terrible idea, but let is not be said that we didn't give you the power to do great things, even if it cost you your foot.)

.. doxygenclass:: ztd::text::wide_literal
	:members:
