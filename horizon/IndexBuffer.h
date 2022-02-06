#pragma once
#include "Vertex.h"
#include "utils.h"
#include "Device.h"
#include "CommandBuffer.h"
#include <memory>

using Index = u32;
class IndexBuffer {
public:
	IndexBuffer() = default;
	IndexBuffer(Device* device, CommandBuffer* commandBuffer, const std::vector<Index>&vertices);
	~IndexBuffer();
	VkBuffer get()const;
	u64 getIndicesCount()const;
private:
	VkBuffer mIndexBuffer;
	VkDeviceMemory mIndexBufferMemory;
	Device* mDevice = nullptr;
	u64 mIndicesCount;
};
