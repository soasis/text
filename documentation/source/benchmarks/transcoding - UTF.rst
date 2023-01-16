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

Transcoding - Unicode Encodings
===============================

Conversion speed is one of the biggest points of a transcoding API. The benchmarks here are meant to test how fast a conversion could potentially be, taking into account any affordances the API can give for speed (such as not checking the output write, not validating for input, or doing an indivisible unit of conversion in a loop/a range conversion/a). All the input text right now is completely valid and well-formed input; some of the benchmarks will take advantage of this to assume the input is valid. Such benchmarks are presented as separate graphs, and only for the APIs that allow it.



All Unicode Code Points
-----------------------

The following benchmarks test the conversion of the basic source character set (from C and C++), going from one UTF encoding to another UTF encoding. It helps test speed under larger conversions.

.. image:: /img/benchmark_results/conversion_speed.large/utf8_to_utf16_well_formed.png

.. image:: /img/benchmark_results/conversion_speed.large/utf16_to_utf8_well_formed.png

.. image:: /img/benchmark_results/conversion_speed.large/utf32_to_utf8_well_formed.png

.. image:: /img/benchmark_results/conversion_speed.large/utf8_to_utf32_well_formed.png

.. image:: /img/benchmark_results/conversion_speed.large/utf16_to_utf32_well_formed.png


All Unicode Code Points, With Initialization
++++++++++++++++++++++++++++++++++++++++++++

The following benchmarks measure the same conversion, but with any initialization object included within the conversion (e.g., for modeling a library that cannot store a registry/conversion descriptor somewhere).

.. image:: /img/benchmark_results/conversion_speed.large/utf8_to_utf16_well_formed_init.png

.. image:: /img/benchmark_results/conversion_speed.large/utf16_to_utf8_well_formed_init.png

.. image:: /img/benchmark_results/conversion_speed.large/utf32_to_utf8_well_formed_init.png

.. image:: /img/benchmark_results/conversion_speed.large/utf8_to_utf32_well_formed_init.png

.. image:: /img/benchmark_results/conversion_speed.large/utf16_to_utf32_well_formed_init.png



C Basic Source Character Set
----------------------------

The following benchmarks test a much smaller conversion on the basic source character set, checking not only ASCII but how well the conversion works for a much smaller inputs.

.. image:: /img/benchmark_results/conversion_speed.small/utf8_to_utf16_well_formed.png

.. image:: /img/benchmark_results/conversion_speed.small/utf16_to_utf8_well_formed.png

.. image:: /img/benchmark_results/conversion_speed.small/utf32_to_utf8_well_formed.png

.. image:: /img/benchmark_results/conversion_speed.small/utf8_to_utf32_well_formed.png

.. image:: /img/benchmark_results/conversion_speed.small/utf16_to_utf32_well_formed.png


C Basic Source Character Set, With Initialization
+++++++++++++++++++++++++++++++++++++++++++++++++

The following benchmarks measure the same conversion, but with any initialization object included within the conversion (e.g., for modeling a library that cannot store a registry/conversion descriptor somewhere).

.. image:: /img/benchmark_results/conversion_speed.small/utf8_to_utf16_well_formed_init.png

.. image:: /img/benchmark_results/conversion_speed.small/utf16_to_utf8_well_formed_init.png

.. image:: /img/benchmark_results/conversion_speed.small/utf32_to_utf8_well_formed_init.png

.. image:: /img/benchmark_results/conversion_speed.small/utf8_to_utf32_well_formed_init.png

.. image:: /img/benchmark_results/conversion_speed.small/utf16_to_utf32_well_formed_init.png
