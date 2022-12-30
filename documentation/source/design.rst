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

Design Goals and Philosophy
===========================

The goal of this library are to

- enable people to write new code that can properly handle encoded information, specifically text;
- offer them effective means to convert that information in various ways;
- impose no run-time overhead compared to writing the code by hand; and
- statically track encodings, where possible, to make lossless or bad conversions a compile time error rather than a runtime problem;

These four goals inform the design of the library to its deepest levels and helps us go through the following important tenents:


.. toctree::
	:maxdepth: 1
	:caption: Contents:

	design/lucky 7
	design/loss
	design/error handling
	design/converting
	design/strong vs weak code points
	design/beyond lucky 7
