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

Benchmarks
==========

Below is a all of the benchmarks currently conducted and built-in to the code of this repository for the purposes of testing and improving the implementation. For further detail, read the article `here <https://thephd.dev/cuneicode-and-the-future-of-text-in-c>`_.

The specification for these benchmarks is as follows:

- The latest of each library was used as of 23 December, 2022.
- Windows 10 Pro machine, general user processes running in the background (but machine not being used).
- AMD Ryzen 5 3600 6-Core @ 3600 MHz (12 Logcal Processors), 32.0 GB Physical Memory
- Clang 15.0.3, latest available Clang at the time of generation with MSVC ABI.
- Entire software stack for every dependency build under default CMake flags (including ICU and libiconv from vcpkg).
- Anywhere from 150 to 10million samples per iteration, with mean (average) of 100 iterations forming transparent dots on graph.
- Each bar graph is mean of the 100 iterations, with provided standard deviation-based error bars.
- In general, unless explicitly noted, the fastest possible API under the constraints was used to produce the data.
	- "Unbounded" means that, where shown, the available space left for writing was not considered.
	- "Unchecked" means that, where shown, the input was not validated before being converted.
	- "Well-Formed", in the title, means that the input was well-formed.
	- (We do not do error benchmarks (yet).)

The categories of benchmarks are as follows:

.. toctree::
	:maxdepth: 1
	:glob:
	
	benchmarks/*
