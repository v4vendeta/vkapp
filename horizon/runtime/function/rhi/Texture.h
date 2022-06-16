#pragma once

#include <runtime/core/utils/definations.h>
#include <runtime/function/rhi/RHIUtils.h>

namespace Horizon
{
	namespace RHI
	{
		class Texture
		{
		public:
			Texture(const TextureCreateInfo &texture_create_info) noexcept;
			virtual ~Texture() noexcept = default;

		private:
			virtual void Destroy() noexcept = 0;

		protected:
			TextureType m_type;
			TextureFormat m_format;
			u32 m_usage;
			u32 m_width, m_height, m_depth;
		};
	}
}