//#pragma once
//
//#include "stdafx.h"
//
//#include <array>
//
//#include <runtime/function/rhi/CommandContext.h>
//#include <runtime/function/rhi/dx12/DX12Buffer.h>
//#include <runtime/function/rhi/dx12/DX12CommandList.h>
//
//namespace Horizon::Backend {
//
//class DX12CommandContext : public CommandContext {
//  public:
//    DX12CommandContext(ID3D12Device *device) noexcept;
//    DX12CommandContext(const DX12CommandContext &command_list) noexcept =
//        default;
//    DX12CommandContext(DX12CommandContext &&command_list) noexcept = default;
//    ~DX12CommandContext() noexcept override;
//    CommandList *GetCommandList(CommandQueueType type) noexcept override;
//    void Reset() noexcept override;
//
//  private:
//    ID3D12Device *m_device;
//    // each thread has pools to allocate graphics/compute/transfer commandlist
//    std::array<ID3D12CommandAllocator *, 3> m_command_pools{};
//
//    std::array<std::vector<DX12CommandList *>, 3> m_command_lists{};
//    std::array<u32, 3> m_command_lists_count{};
//};
//} // namespace Horizon::Backend
