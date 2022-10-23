#include "post_process.h"

PostProcessData::PostProcessData(Backend::RHI *rhi) noexcept {
    // PP PASS
    post_process_cs = rhi->CreateShader(ShaderType::COMPUTE_SHADER, 0, asset_path / "shaders/post_process.comp.hsl");
    post_process_pass = rhi->CreateComputePipeline(ComputePipelineCreateInfo{});
    pp_color_image = rhi->CreateTexture(TextureCreateInfo{
        DescriptorType::DESCRIPTOR_TYPE_RW_TEXTURE, ResourceState::RESOURCE_STATE_UNORDERED_ACCESS,
        TextureType::TEXTURE_TYPE_2D, TextureFormat::TEXTURE_FORMAT_RGBA8_UNORM, _width, _height, 1, false});
    exposure_constants_buffer =
        rhi->CreateBuffer(BufferCreateInfo{DescriptorType::DESCRIPTOR_TYPE_CONSTANT_BUFFER,
                                           ResourceState::RESOURCE_STATE_SHADER_RESOURCE, sizeof(ExposureConstant)});
    post_process_pass->SetComputeShader(post_process_cs);
}
