#pragma once

#include <memory>

#include <BS_thread_pool.hpp>

#include <runtime/core/thread_pool/ThreadPool.h>
#include <runtime/core/window/Window.h>
#include <runtime/function/rhi/RHIUtils.h>
#include <runtime/interface/EngineConfig.h>
#include <runtime/system/input/InputSystem.h>
#include <runtime/system/render/RenderSystem.h>

namespace Horizon {
class EngineRuntime final {
  public:
    EngineRuntime(const EngineConfig &config) noexcept;

    EngineRuntime(const EngineRuntime &rhs) noexcept = delete;

    EngineRuntime &operator=(const EngineRuntime &rhs) noexcept = delete;

    EngineRuntime(EngineRuntime &&rhs) noexcept = delete;

    EngineRuntime &operator=(EngineRuntime &&rhs) noexcept = delete;

    void BeginNewFrame() const noexcept;

  public:
    std::unique_ptr<Window> m_window{};
    std::unique_ptr<RenderSystem> m_render_system{};
    std::unique_ptr<InputSystem> m_input_system{};
    // https://github.com/bshoshany/thread-pool
    std::unique_ptr<BS::thread_pool> tp;
};

} // namespace Horizon
