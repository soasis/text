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

API Reference
=============

This is simply a listing of all the available pages containing various APIs, or links to pages that link to API documentation.



Containers
----------

.. toctree::
	:maxdepth: 1
	:glob:
	
	api/containers/*



Views
-----

.. toctree::
	:maxdepth: 1
	:glob:
	
	api/views/*



Encodings
---------

See also :doc:`the top-level encodings page <encodings>` for more details about the overall status and known vs. implemented encodings.

.. toctree:: 
	:maxdepth: 1
	:glob:
	
	api/encodings/*



Error Handlers
--------------

.. toctree:: 
	:maxdepth: 1
	:glob:
	
	api/error handlers/*



Conversion and Counting Functions
---------------------------------

.. toctree::
	:maxdepth: 1
	:glob:
	
	api/conversions/*
	


Properties and Classifications
------------------------------

.. toctree::
	:maxdepth: 1

	api/code_point
	api/code_unit
	api/decode_state
	api/encode_state
	api/max_code_points
	api/max_code_units
	api/is_state_independent
	api/is_decode_state_independent
	api/is_encode_state_independent
	api/is_decode_injective
	api/is_encode_injective
	api/is_code_units_replaceable
	api/is_code_points_replaceable
	api/is_ignorable_error_handler
	api/is_unicode_encoding
	api/contains_unicode_encoding
	api/is_unicode_code_point
	api/is_unicode_scalar_value
	api/is_transcoding_compatible
	api/default_code_point_encoding
	api/default_code_unit_encoding



Result Types, Status Codes and Quality Aides
--------------------------------------------

.. toctree::
	:maxdepth: 1

	api/char8_t
	api/endian
	api/encoding_error
	api/tag
	api/make_decode_state
	api/make_encode_state
	api/unicode_code_point
	api/unicode_scalar_value
	api/stateless_decode_result
	api/decode_result
	api/stateless_encode_result
	api/encode_result
	api/stateless_transcode_result
	api/transcode_result
	api/stateless_count_result
	api/count_result
	api/stateless_validate_result
	api/validate_result
