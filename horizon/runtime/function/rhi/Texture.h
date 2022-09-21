#pragma once

#include <runtime/core/utils/Definations.h>
#include <runtime/function/rhi/RHIUtils.h>

namespace Horizon::RHI {

class Texture {
  public:
    Texture(const TextureCreateInfo &texture_create_info) noexcept;
    virtual ~Texture() noexcept = default;
    Texture(const Texture &rhs) noexcept = delete;
    Texture &operator=(const Texture &rhs) noexcept = delete;
    Texture(Texture &&rhs) noexcept = delete;
    Texture &operator=(Texture &&rhs) noexcept = delete;
    // virtual void *GetBufferPointer() noexcept = 0;
  public:
    const DescriptorType m_descriptor_type;
    ResourceState m_state{};
    const TextureType m_type{};
    const TextureFormat m_format{};

    const u32 m_width{}, m_height{}, m_depth{};
    const u32 m_channels{4};

  protected:
};
} // namespace Horizon::RHI
