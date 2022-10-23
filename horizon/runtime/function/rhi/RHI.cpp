#include <runtime/function/rhi/RHI.h>
#include <runtime/function/rhi/ResourceCache.h>

namespace Horizon::Backend {

thread_local std::unique_ptr<CommandContext> thread_command_context;

RHI::RHI() noexcept {}

RHI::~RHI() noexcept {}

} // namespace Horizon::Backend