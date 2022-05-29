#pragma once

#include "vk_mem_alloc.h"

#include <runtime/function/rhi/Texture.h>

namespace Horizon
{
	namespace RHI
	{

		class VulkanTexture : public Texture2
		{
		public:
			VulkanTexture(VmaAllocator allocator, const TextureCreateInfo &buffer_create_info) noexcept;
			~VulkanTexture() noexcept;

		private:
			virtual void Destroy() noexcept override;

		private:
			VkImage m_image;
			VmaAllocation m_allocation;
			VmaAllocator m_allocator;
		};

	}
}