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

⚠️ INTERNAL ⚠️ Transcoding - Unicode Encodings
===============================================

.. warning::
	The benchmarks in this section are solely for tracking the behavior and runtime of effectively niche or internal behaviors, such as single conversion from bulk functionality built into ztd.cuneicode's registry or how much it costs to run a basic loop.
	
	Please keep this in mind as you browse.



The purpose of these graphs is to see how expensive it is for various styles of encoding. It includes all of the other benchmarks, but adds a few more for ztd.cuneicode and ztd.text. These labels are the more important ones:

- "single from bulk loop"
- "basic"

These mirror the points documented here; specifically, using a limiting technique to recreate a one-at-a-time, indivisable unit of work from a single loop, or using a technique to process an entire set of input by using the one-by-one functions. The latter technique is the foundation of both ztd.text and ztd.cuneicode, so it is illuminating to see how (poorly) the former technique ("single from bulk") performs against the latter technique ("basic", the bulk-from-single functionality). This was also written about `here<>`_.

All Unicode Code Points
-----------------------

The following benchmarks test the conversion of the basic source character set (from C and C++), going from one UTF encoding to another UTF encoding. It helps test speed under larger conversions.

.. image:: /img/benchmark_results/internal/conversion_speed.large/utf8_to_utf16_well_formed.png

.. image:: /img/benchmark_results/internal/conversion_speed.large/utf16_to_utf8_well_formed.png

.. image:: /img/benchmark_results/internal/conversion_speed.large/utf32_to_utf8_well_formed.png

.. image:: /img/benchmark_results/internal/conversion_speed.large/utf8_to_utf32_well_formed.png

.. image:: /img/benchmark_results/internal/conversion_speed.large/utf16_to_utf32_well_formed.png


All Unicode Code Points, With Initialization
++++++++++++++++++++++++++++++++++++++++++++

The following benchmarks measure the same conversion, but with any initialization object included within the conversion (e.g., for modeling a library that cannot store a registry/conversion descriptor somewhere).

.. image:: /img/benchmark_results/internal/conversion_speed.large/utf8_to_utf16_well_formed_init.png

.. image:: /img/benchmark_results/internal/conversion_speed.large/utf16_to_utf8_well_formed_init.png

.. image:: /img/benchmark_results/internal/conversion_speed.large/utf32_to_utf8_well_formed_init.png

.. image:: /img/benchmark_results/internal/conversion_speed.large/utf8_to_utf32_well_formed_init.png

.. image:: /img/benchmark_results/internal/conversion_speed.large/utf16_to_utf32_well_formed_init.png



C Basic Source Character Set
----------------------------

The following benchmarks test a much smaller conversion on the basic source character set, checking not only ASCII but how well the conversion works for a much smaller inputs.

.. image:: /img/benchmark_results/internal/conversion_speed.small/utf8_to_utf16_well_formed.png

.. image:: /img/benchmark_results/internal/conversion_speed.small/utf16_to_utf8_well_formed.png

.. image:: /img/benchmark_results/internal/conversion_speed.small/utf32_to_utf8_well_formed.png

.. image:: /img/benchmark_results/internal/conversion_speed.small/utf8_to_utf32_well_formed.png

.. image:: /img/benchmark_results/internal/conversion_speed.small/utf16_to_utf32_well_formed.png


C Basic Source Character Set, With Initialization
+++++++++++++++++++++++++++++++++++++++++++++++++

The following benchmarks measure the same conversion, but with any initialization object included within the conversion (e.g., for modeling a library that cannot store a registry/conversion descriptor somewhere).

.. image:: /img/benchmark_results/internal/conversion_speed.small/utf8_to_utf16_well_formed_init.png

.. image:: /img/benchmark_results/internal/conversion_speed.small/utf16_to_utf8_well_formed_init.png

.. image:: /img/benchmark_results/internal/conversion_speed.small/utf32_to_utf8_well_formed_init.png

.. image:: /img/benchmark_results/internal/conversion_speed.small/utf8_to_utf32_well_formed_init.png

.. image:: /img/benchmark_results/internal/conversion_speed.small/utf16_to_utf32_well_formed_init.png
