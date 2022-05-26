#pragma once

#include <memory>
#include <vector>

#include <vulkan/vulkan.h>

#include <runtime/core/math/Math.h>
#include <runtime/function/rhi/vulkan/VulkanEnums.h>
#include "Device.h"

namespace Horizon {
	enum AttachmentUsageFlags {
		COLOR_ATTACHMENT = 1,
		DEPTH_STENCIL_ATTACHMENT = 2,
		PRESENT_SRC = 4
	};
	using AttachmentUsage = u32;

	struct AttachmentCreateInfo {
	public:
		TextureFormat format = TextureFormat::TEXTURE_FORMAT_RGBA8_UNORM;
		AttachmentUsage usage{};
		TextureType texture_type = TextureType::TEXTURE_TYPE_2D;
		u32 width = 0, height = 0, depth = 1;
	};

	class Attachment{
	public:
		Attachment(std::shared_ptr<Device> device, const AttachmentCreateInfo create_info);

		VkImage m_image;
		VkDeviceMemory m_image_memory;
		VkImageView m_image_view;
		VkFormat m_format;
	};

	class AttachmentDescriptor :public DescriptorBase {
		
	};
}