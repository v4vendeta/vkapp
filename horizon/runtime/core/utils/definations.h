#pragma once

#define NOMINMAX

#include <cstdint>

#include <vulkan/vulkan.h>
#include <d3d12.h>

namespace Horizon {

	using u8 = uint8_t;
	using u16 = uint16_t;
	using u32 = uint32_t;
	using u64 = uint64_t;
	using i8 = int8_t;
	using i16 = int16_t;
	using i32 = int32_t;
	using i64 = int64_t;

	using f32 = float;
	using f64 = double;

#define USE_ASYNC_COMPUTE_TRANSFER 1
#define RENDERDOC_ENABLED 1
}