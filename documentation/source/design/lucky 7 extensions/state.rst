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

State, Completion, Runtime Data, and More
=========================================

Some states need extra functionality or additional information to function properly. This can manifest as:

- needing extra data on a per-conversion basis that you can maintain yourself;
- needing 2 different types for encode/decode operations;
- OR, needing runtime-dependent, conversion-dependent information for a specific converison.



Extra Data and Completion
-------------------------

State objects are always passed into the function by non-``const`` l-value reference (e.g. ``void f(state_type& state);``). This means that, once a state is created, it can be used to influence how a specific algorithm works. While most encodings strive to have little to no meaningful state, others can have very meaningful state that should not be discarded between function calls or that may contribute meaningfully to the encoding or decoding process.

To aid with this, a state type can have a callable function of the form `is_complete()`:

.. code-block:: cpp
	:linenos:

	class encode_state {
	public:
		encode_state_handle_t handle;

		bool is_complete() const noexcept {
			return state_handle_has_no_more_output(handle);
		}
	};

The ``state_handle_t`` and ``state_handle_has_no_more_output`` are fictitious, but they represent how the given ``encode_state`` type would signal that it has no more work to be done. This is useful for algorithms which may need to be signaled that a stream has no more data and should thus produce an error if the final bits of data do not form a complete sequence, or if there are encoding algorithms (such as :doc:`punycode </api/encodings/punycode>`) that need to collect all input before doing output operations. When the state has this function present, a user can use :doc:`ztd::text::is_state_complete(some_state) </api/is_state_complete>` as part of a condition to check if a given conversion sequence and its state have fully serialized all possible data.




Separate Encode/Decode States
-----------------------------

It is no secret that encoding and decoding may carrying with them separate states. While converting from a legacy encoding to Unicode may require maintenance of a shift state or code unit modifier, the opposite direction may not need any at all. Therefore, as an optimization, an encoding object can define both an ``encode_state`` and a ``decode_state``, seperate from each other. As an example, here is a (simplified) version of how :doc:`ztd::text::execution </api/encodings/execution>`, the encoding for the Locale-based Runtime Execution Encoding, has two seperate states that need to be initialized in different manners:

.. literalinclude:: /../../examples/documentation/snippets/source/runtime_locale_encoding.cpp
	:language: cpp
	:linenos:
	:lines: 56-57,65-84,246

This is the proper way to initialize a ``std::mbstate_t`` from the C standard library. Then, you can use it! Here's a complete implementation using the new ``encode_state`` and ``decode_state`` types:

.. literalinclude:: /../../examples/documentation/snippets/source/runtime_locale_encoding.cpp
	:language: cpp
	:linenos:
	:lines: 56,127-246

This allows you to maintain 2 different states, initialized in 2 different ways, one for each of the ``encode_one`` and ``decode_one`` function paths.



Encoding-Dependent States
-------------------------

Some states need additional information in order to be constructed and used properly. This can be the case when the encoding has stored some type-erased information, as :doc:`ztd::text::any_encoding </api/encodings/any_encoding>` does, or as if you wrote a ``variant_encoding<utf8le, utf16be, ...>``. For example, given a ``type_erased_encoding`` like so:

.. code-block:: cpp
	:linenos:

	class type_erased_encoding {
	private:
		struct erased_state {
			virtual ~erased_state () {}
		};

		struct erased_encoding {
			virtual std::unique_ptr<erased_state> create_decode_state() = 0;
			virtual std::unique_ptr<erased_state> create_encode_state() = 0;

			virtual ~erased_encoding () {}
		};

		template <typename Encoding>
		struct typed_encoding : erased_encoding {
			Encoding encoding;

			struct decode_state : erased_state {
				using state_type = ztd::text::decode_state_t<Encoding>;
				state_type state;

				decode_state(const Encoding& some_encoding)
				: state(ztd::text::make_decode_state(some_encoding)) {
					// get a decode state from the given encoding
				}
			};

			struct encode_state : erased_state {
				using state_type = ztd::text::encode_state_t<Encoding>;
				state_type state;

				decode_state(const Encoding& some_encoding)
				: state(ztd::text::make_encode_state(some_encoding)) {
					// get a decode state from the given encoding
				}
			};

			typed_encoding(Encoding&& some_encoding)
			: encoding(std::move(some_encoding)) {
				// move encoding in
			}

			typed_encoding(const Encoding& some_encoding)
			: encoding(some_encoding) {
				// copy encoding in
			}

			virtual std::unique_ptr<erased_state> create_decode_state() override {
				return std::make_unique<decode_state>(encoding);
			}

			virtual std::unique_ptr<erased_state> create_encode_state() override {
				return std::make_unique<encode_state>(encoding);
			}
		};

		std::unique_ptr<erased_encoding> stored;

	public:
		template <typename AnyEncoding>
		type_erased(AnyEncoding&& some_encoding)
		: stored_ptr(std::make_unique<typed_encoding<std::remove_cvref_t<AnyEncoding>>>(
			std::forward<AnyEncoding>(some_encoding))
		) {
			// store any encoding in the member unique pointer
		}

		// ... rest of the implementation
	};

We can see that creating a state with a default constructor no longer works, because the state itself requires more information than can be known by just the constructor itself. It needs access to the wrapped encoding. The solution to this problem is an opt-in when creating your state types by giving your `state` type a constructor that takes the encoding type:

.. code-block:: cpp
	:linenos:
	:emphasize-lines: 7-11,18-22

	class type_erased_encoding {
		// from above, etc. …
	public:
		// public-facing wrappers
		struct type_erased_decode_state {
		public:
			// special constructor!!
			type_erased_state (const type_erased_encoding& encoding)
			: stored(encoding.stored->create_decode_state()) {

			}
		private:
			std::unique_ptr<erased_state> stored;
		};

		struct type_erased_encode_state {
		public:
			// special constructor!!
			type_erased_state (const type_erased_encoding& encoding)
			: stored(encoding.stored->create_encode_state()) {
				// hold onto type-erased state
			}
		private:
			std::unique_ptr<erased_state> stored;
		};

		using decode_state = type_erased_state;
		using encode_state = type_erased_state;

		// ... rest of the Lucky 7 members
	};

These special constructors will create the necessary state using information from the ``type_erased_encoding`` to do it properly. This will allow us to have states that properly reflect what was erased when we perform a given higher-level :doc:`conversion operation or algorithm </design/converting>`.

This encoding-aware state-construction behavior is detected by the :doc:`ztd::text::is_state_independent </api/is_state_independent>`, :doc:`ztd::text::is_decode_state_independent </api/is_decode_state_independent>`, and :doc:`ztd::text::is_encode_state_independent </api/is_encode_state_independent>` classifications.

These classifications are used in the :doc:`ztd::text::make_decode_state </api/make_decode_state>` and :doc:`ztd::text::make_encode_state </api/make_encode_state>` function calls to correctly construct a state object, which is what the API uses to make states for its higher-level function calls. If you are working in a generic context, you should use these functions too when working in this minute details. However, if you're not working with templates, consider simply using the already-provided :doc:`ztd::text::any_encoding </api/encodings/any_encoding>` to do exactly what this example shows, with some extra attention to detail and internal optimizations done on your behalf.
