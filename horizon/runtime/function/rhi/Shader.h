#pragma once

#include <string>

#include <runtime/function/rhi/RHIUtils.h>

namespace Horizon::RHI {
class Shader {
  public:
    // Shader(ShaderType type, IDxcBlob* dxil_byte_code) noexcept;
    Shader(ShaderType type) noexcept;
    virtual ~Shader() noexcept = default;
    Shader(const Shader &rhs) noexcept = delete;
    Shader &operator=(const Shader &rhs) noexcept = delete;
    Shader(Shader &&rhs) noexcept = delete;
    Shader &operator=(Shader &&rhs) noexcept = delete;
    // virtual void* GetBufferPointer() const noexcept = 0;
    // virtual u64 GetBufferSize() const noexcept = 0;
    ShaderType GetType() const noexcept;
    const std::string &GetEntryPoint() const noexcept;

  private:
    const ShaderType m_type{};
    std::string m_entry_point{};
};

} // namespace Horizon::RHI