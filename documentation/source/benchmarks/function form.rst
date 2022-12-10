.. =============================================================================
..
.. ztd.text
.. Copyright © 2022 JeanHeyd "ThePhD" Meneide and Shepherd's Oasis, LLC
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

Function Form
=============

The following benchmarks test the conversion of the entire set of valid Unicode Scalar Values (all Unicode code points, except the surrogate values used for UTF-16). The transcoding done is UTF-32 to UTF-8, with as-identical an internal conversion routine as possible. The only differences in the internal workings of the conversion are in reaction to the inputs and outputs given.

The goal of these benchmarks is to measure how using a basic, single (indivisible unit of work) conversion run in a loop can affect speed based on how the function is structured. This is mostly relevant in the case of libraries like `musl-libc <https://musl.libc.org/>`_, where the maintainers expressly declared they would NOT optimize the bulk-conversion functions to do anything and would rather write the base conversion loop:

.. epigraph::

	…

	On musl (where I'm familiar with performance properties), byte-at-a-time conversion is roughly half the speed of bulk, which looks big but is diminishingly so if you're actually doing something with the result (just converting to wchar_t for its own sake is not very useful).

	-- Rich Felker, December 30th, 2019, `musl libc mailing list <https://www.openwall.com/lists/musl/2019/12/30/8>`_.

Here, we can see how writing the bulk conversion functions in terms of the single functions are affected by a given API design.



All Unicode Code Points
-----------------------

.. image:: /img/benchmark_results/function_form.large/transform_two_spans.png


C Basic Source Character Set
----------------------------

.. image:: /img/benchmark_results/function_form.large/transform_two_spans.png

The
