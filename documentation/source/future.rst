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

Progress & Future Work
======================

This is where the status and progress of the library will be kept up to date. You can also check the `Issue Tracker <https://github.com/soasis/text/issues>`_ for specific issues and things being worked on! We also maintain a :doc:`very large list of encodings </encodings>`, so you can check if a specific encoding you are looking for is supported (and if you will need to implement an :doc:`Encoding Object </design/lucky 7>` for it).



Copyable State
--------------

Right now, all state parameters are assumed to be move-only. This is detrimental to creating cheap views like ``.code_points()`` on ``basic_text_view``, and harms other types as well. Work should be done either to make copyable state, or allow passing state in more effectively (we currently do the passing technique at the moment).

- ☐ Do all states need to be copyable? Can it be done selectively? (At the moment: ``basic_text_view`` and ``text_view`` very well may need it, and as more Shift-State encodings become a part of the library, even more need…)



Transcoding Iterators/Transcode View ✅
---------------------------------------

Right now these types would not work especially well for input and output ranges. They should be modified just like the internal ``ztd::text::__txt_detail::__encoding_iterator`` class types, so that they work with ``input_iterator`` and ``output_iterator`` types.

- ☑ Improve constructor delegation and make sure to explicitly implement default construction vs. letting it happen with ``=default`` (which does not work for some of the base types present).
- ☑ Modify implementation to cache data and position when an input or output iterator is detected.
- ☑ Return ``const value_type&`` for ``reference`` to enable C++20 ranges to work properly.
- ☑ Mark as ``enable_borrowed_range`` when C++20 is detected.



Normalization
-------------

``ztd::text::nfkd/nfk/nfc/nfkc/fcc`` are all skeletons right now that need to be filled out for the purposes of giving this library normalization views.

- ☐ nfkc
- ☐ nfc
- ☐ nfkd
- ☐ nfd
- ☐ Hook up to ``basic_text_view`` and ``basic_text`` when finished



``basic_text_view``
-------------------

``ztd::text::basic_text_view<Encoding, NormalizationForm, Range, ...>`` is to be the premiere view for looking at text and preserving both the normalization and encoding form during insertion and erasure. It is not fully implemented yet, even though basic skeletons exist for it in the code base.

- ☐ Grapheme Cluster Iterators
- ☑ Code Point iterators
- ☐ Grapheme Cluster Iterators
- ☐ Comparison operators (If the normalization form is the same and :doc:`is_bitwise_transcoding_compatible </api/is_transcoding_compatible>`, then ``memcmp``. If just normalization form and encoding is same, ``memcmp``. Otherwise, code point by code point comparison.)



``basic_text``
--------------

``ztd::text::basic_text<Encoding, NormalizationForm, Storage, ...>`` is to be the premiere container for storing text and preserving both the normalization and encoding form during insertion and erasure. It is not fully implemented yet, even though basic skeletons exist for it in the code base.

- ☑ Code Point iterators/ranges
- ☐ Grapheme Cluster Iterators
- ☐ Comparison operators (If the normalization form is the same and :doc:`is_bitwise_transcoding_compatible </api/is_transcoding_compatible>`, then ``memcmp``. If just normalization form and encoding is same, ``memcmp``. Otherwise, code point by code point comparison.)
- ☐ Insertion (Fast normalization-preserving splicing/inserting algorithm)
- ☐ Deletion
- ☐ Converting Constructors between compatible types (errors the same way :doc:`lossy conversion protection </design/error handling/lossy protection>` describes if they are not compatible, forcing a user to pass in an error handler.)



``iconv``
---------

There should be an encoding that loads ``iconv`` dynamically from the system, if it is present, before using it to do conversions.



``cuneicode``
-------------

There should be a cuneicode-based encoding, for the update C implementation of all of these things.