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

``ztd.text``
============

The premiere library for handling text in different encoding forms and reducing transcoding bugs in your C++ software.



Who Is This Library For?
------------------------

If:

- you want to convert from one Unicode encoding to another Unicode encoding;
- you want a no-overhead way to track and keep data in a specific encoding (Unicode-based or not);
- you want a no-memory-overhead way to archive;
- you want to prevent data in the wrong encoding from infiltrating your application and causing `Mojibake <https://en.wikipedia.org/wiki/Mojibake>`_;
- you want to work with higher-level primitives (code points, graphames) when iterating text that do not break your text apart;
- you want safe defaults for working with text;

then ``ztd.text`` is for you!

.. toctree::
	:maxdepth: 1
	:caption: Contents:

	start
	quick
	in the wild
	definitions
	design
	encodings
	config
	api
	license




Indices and tables
==================

* :ref:`genindex`
* :ref:`modindex`
* :ref:`search`
