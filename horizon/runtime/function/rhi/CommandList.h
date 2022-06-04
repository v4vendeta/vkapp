#pragma once

#include <runtime/function/rhi/Buffer.h>
#include "vulkan/ResourceBarrier.h"
namespace Horizon
{
    namespace RHI
    {

        class CommandList
        {
        public:
            CommandList() noexcept;
            ~CommandList() noexcept;
            virtual void BeginRecording() noexcept = 0;
            virtual void EndRecording() noexcept = 0;

            virtual void Draw() noexcept = 0;
            virtual void DrawIndirect() noexcept = 0;
            virtual void Dispatch() noexcept = 0;
            virtual void DispatchIndirect() noexcept = 0;
            virtual void UpdateBuffer(Buffer* buffer, void* data, u64 size) noexcept = 0;
            virtual void UpdateTexture() noexcept = 0;

            virtual void CopyBuffer(Buffer* dst_buffer, Buffer* src_buffer) noexcept = 0;
            virtual void CopyTexture() noexcept = 0;

            virtual void InsertBarrier(const BarrierDesc& desc) noexcept = 0;
            virtual void Submit() noexcept = 0;
        protected:
            bool is_recording = false;
        private:
            // CommandListType m_type;
        };
    }
}
