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

max_recode_code_points
========================

The maximum number of code units needed for a given encoding object. This can be used to create a suitably-sized automatic storage duration buffer, e.g.

.. code-block:: cpp

	using code_point = ztd::text::code_point_t<ztd::text::utf8>;
	constexpr std::size_t max_output_size = ztd::text::max_recode_code_points_v<ztd::text::utf8, ztd::text::utf16>;
	
	// C++-style
	std::array<code_point, max_output_size> my_cxx_buffer;
	// or C-style
	code_point my_c_buffer[max_output_size];


.. doxygenvariable:: ztd::text::max_recode_code_points_v
