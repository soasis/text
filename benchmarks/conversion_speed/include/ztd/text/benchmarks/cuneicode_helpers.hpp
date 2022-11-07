// =============================================================================
//
// ztd.text
// Copyright © 2022 JeanHeyd "ThePhD" Meneide and Shepherd's Oasis, LLC
// Contact: opensource@soasis.org
//
// Commercial License Usage
// Licensees holding valid commercial ztd.text licenses may use this file in
// accordance with the commercial license agreement provided with the
// Software or, alternatively, in accordance with the terms contained in
// a written agreement between you and Shepherd's Oasis, LLC.
// For licensing terms and conditions see your agreement. For
// further information contact opensource@soasis.org.
//
// Apache License Version 2 Usage
// Alternatively, this file may be used under the terms of Apache License
// Version 2.0 (the "License") for non-commercial use; you may not use this
// file except in compliance with the License. You may obtain a copy of the
// License at
//
//		http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
//
// ============================================================================ //

#pragma once

#ifndef ZTD_TEXT_BENCHMARKS_CONVERSION_SPEED_CUNEICODE_HELPERS_HPP
#define ZTD_TEXT_BENCHMARKS_CONVERSION_SPEED_CUNEICODE_HELPERS_HPP

#include <ztd/cuneicode.h>

#include <memory>
#include <vector>
#include <memory_resource>

struct conversion_deleter {
	void operator()(cnc_conversion* conv) const noexcept {
		cnc_conv_delete(conv);
	}
};

struct registry_deleter {
	void operator()(cnc_conversion_registry* registry) const noexcept {
		cnc_registry_delete(registry);
	}
};

struct conversion_closer {
	void operator()(cnc_conversion* conv) const noexcept {
		cnc_conv_close(conv);
	}
};

struct registry_closer {
	void operator()(cnc_conversion_registry* registry) const noexcept {
		cnc_close_registry(registry);
	}
};

inline void* mbr_allocate(size_t requested_size, size_t alignment, size_t* p_actual_size, void* user_data) {
	std::pmr::monotonic_buffer_resource& mbr = *static_cast<std::pmr::monotonic_buffer_resource*>(user_data);
	void* ptr                                = mbr.allocate(requested_size, alignment);
	*p_actual_size                           = requested_size;
	return ptr;
}

inline void mbr_deallocate(void* ptr, size_t ptr_size, size_t alignment, void* user_data) {
	std::pmr::monotonic_buffer_resource& mbr = *static_cast<std::pmr::monotonic_buffer_resource*>(user_data);
	mbr.deallocate(ptr, ptr_size, alignment);
}

inline cnc_conversion_heap create_monotonic_buffer_heap(std::pmr::monotonic_buffer_resource& resource) {
	cnc_conversion_heap mbr_heap = { &resource, mbr_allocate, nullptr, nullptr, nullptr, mbr_deallocate };
	return mbr_heap;
}

#endif
