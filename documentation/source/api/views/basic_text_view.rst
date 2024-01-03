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

🔨 basic_text_view (In Progress)
================================

.. warning::

	|unfinished_warning|

The ``basic_text_view`` class provides a one-by-one view of the stored range's code points and other functionality in a more complete form that goes beyond just code point iteration or code unit iteration like :doc:`ztd::text::decode_view </api/views/decode_view>` or :doc:`ztd::text::encode_view </api/views/encode_view>`.



Base Template
-------------

.. doxygenclass:: ztd::text::basic_text_view
	:members:



Aliases
-------

.. doxygentypedef:: ztd::text::text_view

.. doxygentypedef:: ztd::text::ntext_view

.. doxygentypedef:: ztd::text::wtext_view

.. doxygentypedef:: ztd::text::nltext_view

.. doxygentypedef:: ztd::text::wltext_view

.. doxygentypedef:: ztd::text::u8text_view

.. doxygentypedef:: ztd::text::u16text_view

.. doxygentypedef:: ztd::text::u32text_view
