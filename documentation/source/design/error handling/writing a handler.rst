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
..		http:..www.apache.org/licenses/LICENSE-2.0
..
.. Unless required by applicable law or agreed to in writing, software
.. distributed under the License is distributed on an "AS IS" BASIS,
.. WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
.. See the License for the specific language governing permissions and
.. limitations under the License.
..
.. =============================================================================>

Writing A Handler
=================

When put together, it can generally look like this:

.. literalinclude:: /../../examples/documentation/source/error_handler.custom.ascii.cpp
	:language: cpp
	:linenos:
	:start-after: // ============================================================================>

The result in ``my_ascii_string`` should be an empty string: nothing should have succeeded and therefore the function will just return an empty string. The print out will look like this:

.. code-block:: none

	An error occurred.
		Error code value: invalid_sequence
		# of unused characters: 1
		Input units left: 1
	An error occurred.
		Error code value: invalid_sequence
		# of unused characters: 1
		Input units left: 0

If you would like the higher-level called function to return more information to you, use the lower level :doc:`encode_to/encode_into </api/conversions/encode>`, :doc:`decode_to/decode_into </api/conversions/decode>`, :doc:`transcode_to/transcode_into </api/conversions/transcode>`.

If you need to do more, you can change from concrete types to templates, and work at increasingly higher levels of genericity in order to have the printing handler do more and more.
