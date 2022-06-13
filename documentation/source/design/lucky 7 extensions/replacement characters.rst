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

Replacement Characters
======================

Replacement characters are a way to communicate to the end-user that something went wrong, without having to throw an exception that may stop the world or stop the encoding/decoding process altogether. The default error handler for text (:doc:`ztd::text::default_handler </api/error handlers/default_handler>`, unless :doc:`configured </config>` otherwise) provides room for you to provide your own encoding types, and it does so in two ways that is recognized by the library:


Always Has A Replacement
------------------------

If your type always has a replacement character, regardless of the situation, it can signal this by writing one of two functions:

- ``replacement_code_units()`` (for any failed ``encode`` step)
- ``replacement_code_points()`` (for any failed ``decode`` step)

These functions return a contiguous range of either ``code_unit``\ s or ``code_point``\ s, typically a ``std::span<const code_unit>`` or a ``std::span<const code_point>``.

.. literalinclude:: /../../examples/documentation/snippets/source/runtime_locale_encoding.cpp
	:language: cpp
	:linenos:
	:lines: 56-57,112-125,246

If the given replacement range is empty, then nothing is inserted at all (as this is a deliberate choice from the user. See the next section for how to have this function but graciously return "no replacements" for given runtime conditions).

This is employed, for example, in the :doc:`ztd::text::ascii </api/encodings/ascii>` encoding, which uses a `'?'` as its replacement ``code_unit`` and ``code_point`` value.



Maybe Has A Replacement
-----------------------

If your type might not have a range of replacement characters but you will not know that until run time, regardless of the situation, the encoding type can signal this by writing different functions:

- ``maybe_replacement_code_units()`` (for any failed ``encode`` step)
- ``maybe_replacement_code_points()`` (for any failed ``decode`` step)

These functions return a ``std::optional`` of a contiguous range of either ``code_unit``\ s or ``code_point``\ s, typically a ``std::optional<std::span<const code_unit>>`` or a ``std::optional<std::span<const code_point>>``. If the ``optional`` is not engaged (it does not have a value stored), then the replacement algorithm uses its default logic to insert a replacement character, if possible. Otherwise, if it does have a value, it uses that range. If it has a value but the range is empty, it uses that empty range (and inserts nothing).

This is useful for encodings which provide runtime-erased wrappers or that wrap platform APIs like Win32, whose `CPINFOEXW <https://docs.microsoft.com/en-us/windows/win32/api/winnls/ns-winnls-cpinfoexw>`_ structure contains both a ``WCHAR UnicodeDefaultChar;`` and a ``BYTE DefaultChar[MAX_DEFAULTCHAR];``. These can be provided as the range values after being stored on the encoding, or similar.



The Default
-----------

When none of the above can happen, the :doc:`ztd::text::replacement_handler_t </api/error handlers/replacement_handler>` will attempt to insert a Unicode Replacement Character (�, ``U'\uFFFD'``) or the `'?'` character into the stream, in various ways. See :doc:`ztd::text::replacement_handler_t </api/error handlers/replacement_handler>` for more details on that process!
