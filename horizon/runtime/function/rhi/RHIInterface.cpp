#include <runtime/function/rhi/RHIInterface.h>

namespace Horizon::RHI {
	RHIInterface::RHIInterface() noexcept
	{
		//m_thread_pool = std::make_unique<ThreadPool>(std::thread::hardware_concurrency() - 1);
		m_shader_compiler = std::make_shared<ShaderCompiler>();
	}
}