Thank you for considering a contribution to this library!

By contributing to this library, you agree that your contributions will be released to the copyright of JeanHeyd "ThePhD" Meneide and Shepherd's Oasis, LLC under the Apache 2.0 License. See the [LICENSE](LICENSE.md) file for details.

If you're okay with the License and such, you will be asked to agree to this, and/or sign a Contributor License Agreement to contribute to the core library if need be!

Code should be formatted using the provide [`.clang-format`](.clang-format). If it does a terrible job, then hand-format it with the proper indentation (using tabs) and alignment (using spaces) in-between `// clang-format off` and `// clang-format on` comment lines.

If you are contributing new code, it is a good idea to include a test case or two in the relevant test area for run-time, compile-time, or more. If those tests need data files, they can be generated through CMake when necessary or, if small enough, stored in the repository itself to be used and then copied to a proper directory for the test or example code. See the [example CMakeLists.txt for the "basic" examples](examples/basic/CMakeLists.txt) for a minor demonstration for built-in data and source and how to organize and copy it.

If you are contributing fixes, make sure to test those fixes as best as you can by adding test cases or example code that exercises those fixes! If it is performance related, write a benchmark for it so it can become part of the data we track for this.
