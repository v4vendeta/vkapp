#pragma once

#include <vk_mem_alloc.h>
#include <runtime/rhi/Texture.h>
#include <runtime/rhi/vulkan/VulkanBuffer.h>
#include <runtime/rhi/vulkan/VulkanUtils.h>

namespace Horizon::Backend {
class VulkanTexture : public Texture {
  public:
    VulkanTexture(const VulkanRendererContext &context, const TextureCreateInfo &buffer_create_info) noexcept;
    virtual ~VulkanTexture() noexcept;
    VulkanTexture(const VulkanTexture &rhs) noexcept = delete;
    VulkanTexture &operator=(const VulkanTexture &rhs) noexcept = delete;
    VulkanTexture(VulkanTexture &&rhs) noexcept = delete;
    VulkanTexture &operator=(VulkanTexture &&rhs) noexcept = delete;
    VkDescriptorImageInfo *GetDescriptorImageInfo(DescriptorType descriptor_type) noexcept;

  public:
    const VulkanRendererContext &m_context;
    VkImage m_image{};
    VkImageView m_image_view{};
    VmaAllocation m_memory{};
    VkDescriptorImageInfo descriptor_image_info{};
    VulkanBuffer *m_stage_buffer{};
};

} // namespace Horizon::Backend