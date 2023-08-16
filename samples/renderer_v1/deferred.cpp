#include "deferred.h"

DeferredData::DeferredData(RHI *rhi) noexcept : mRhi(rhi) {

    // geometry pass
    {
        gbuffer0 = rhi->CreateRenderTarget(RenderTargetCreateInfo{RenderTargetFormat::TEXTURE_FORMAT_RGBA8_SNORM,
                                                                  RenderTargetType::COLOR, width, height});
        gbuffer1 = rhi->CreateRenderTarget(RenderTargetCreateInfo{RenderTargetFormat::TEXTURE_FORMAT_RGBA8_UNORM,
                                                                  RenderTargetType::COLOR, width, height});
        gbuffer2 = rhi->CreateRenderTarget(RenderTargetCreateInfo{RenderTargetFormat::TEXTURE_FORMAT_RGBA8_UNORM,
                                                                  RenderTargetType::COLOR, width, height});
        gbuffer3 = rhi->CreateRenderTarget(RenderTargetCreateInfo{RenderTargetFormat::TEXTURE_FORMAT_RGBA8_UNORM,
                                                                  RenderTargetType::COLOR, width, height});
        gbuffer4 = rhi->CreateRenderTarget(RenderTargetCreateInfo{RenderTargetFormat::TEXTURE_FORMAT_RG32_SFLOAT,
                                                                  RenderTargetType::COLOR, width, height});
        depth = rhi->CreateRenderTarget(RenderTargetCreateInfo{RenderTargetFormat::TEXTURE_FORMAT_D32_SFLOAT,
                                                               RenderTargetType::DEPTH_STENCIL, width, height});

        graphics_pass_ci.vertex_input_state.attribute_count = 5;

        auto &pos = graphics_pass_ci.vertex_input_state.attributes[0];
        pos.attrib_format = VertexAttribFormat::F32; // position
        pos.portion = 3;
        pos.binding = 0;
        pos.location = 0;
        pos.offset = 0;
        pos.input_rate = VertexInputRate::VERTEX_ATTRIB_RATE_VERTEX;

        auto &normal = graphics_pass_ci.vertex_input_state.attributes[1];
        normal.attrib_format = VertexAttribFormat::F32; // normal, TOOD: SN16 is a better format
        normal.portion = 3;
        normal.binding = 0;
        normal.location = 1;
        normal.input_rate = VertexInputRate::VERTEX_ATTRIB_RATE_VERTEX;
        normal.offset = offsetof(Vertex, normal);

        auto &uv0 = graphics_pass_ci.vertex_input_state.attributes[2];
        uv0.attrib_format = VertexAttribFormat::F32; // uv0 TOOD: UN16 is a better format
        uv0.portion = 2;
        uv0.binding = 0;
        uv0.location = 2;
        uv0.input_rate = VertexInputRate::VERTEX_ATTRIB_RATE_VERTEX;
        uv0.offset = offsetof(Vertex, uv0);

        auto &uv1 = graphics_pass_ci.vertex_input_state.attributes[3];
        uv1.attrib_format = VertexAttribFormat::F32; // uv1 TOOD: UN16 is a better format
        uv1.portion = 2;
        uv1.binding = 0;
        uv1.location = 3;
        uv1.input_rate = VertexInputRate::VERTEX_ATTRIB_RATE_VERTEX;
        uv1.offset = offsetof(Vertex, uv1);

        auto &tangent = graphics_pass_ci.vertex_input_state.attributes[4];
        tangent.attrib_format = VertexAttribFormat::F32;
        tangent.portion = 3;
        tangent.binding = 0;
        tangent.location = 4;
        tangent.input_rate = VertexInputRate::VERTEX_ATTRIB_RATE_VERTEX;
        tangent.offset = offsetof(Vertex, tangent);

        graphics_pass_ci.view_port_state.width = width;
        graphics_pass_ci.view_port_state.height = height;

        graphics_pass_ci.depth_stencil_state.depth_func = DepthFunc::LESS;
        graphics_pass_ci.depth_stencil_state.depthNear = 0.0f;
        graphics_pass_ci.depth_stencil_state.depthNear = 1.0f;
        graphics_pass_ci.depth_stencil_state.depth_test = true;
        graphics_pass_ci.depth_stencil_state.depth_write = true;
        graphics_pass_ci.depth_stencil_state.stencil_enabled = false;

        graphics_pass_ci.input_assembly_state.topology = PrimitiveTopology::TRIANGLE_LIST;

        graphics_pass_ci.multi_sample_state.sample_count = 1;

        graphics_pass_ci.rasterization_state.cull_mode = CullMode::BACK;
        graphics_pass_ci.rasterization_state.discard = false;
        graphics_pass_ci.rasterization_state.fill_mode = FillMode::TRIANGLE;
        graphics_pass_ci.rasterization_state.front_face = FrontFace::CCW;

        graphics_pass_ci.render_target_formats.color_attachment_count = 5;

        graphics_pass_ci.render_target_formats.color_attachment_formats = std::vector<TextureFormat>{
            gbuffer0->GetTexture()->m_format, gbuffer1->GetTexture()->m_format, gbuffer2->GetTexture()->m_format,
            gbuffer3->GetTexture()->m_format, gbuffer4->GetTexture()->m_format};
        graphics_pass_ci.render_target_formats.has_depth = true;
        graphics_pass_ci.render_target_formats.depth_stencil_format = depth->GetTexture()->m_format;

        geometry_pass = rhi->CreateGraphicsPipeline(graphics_pass_ci);
    }

    {
        geometry_vs = rhi->CreateShader(ShaderType::VERTEX_SHADER,  shader_dir / "gbuffer_bindless.vert.hsl");

        geometry_ps = rhi->CreateShader(ShaderType::PIXEL_SHADER,  shader_dir / "gbuffer_bindless.frag.hsl");

        shading_cs = rhi->CreateShader(ShaderType::COMPUTE_SHADER,  shader_dir / "deferred_shading.comp.hsl");
    }

    {
        deferred_shading_constants_buffer = rhi->CreateBuffer(
            BufferCreateInfo{DescriptorType::DESCRIPTOR_TYPE_CONSTANT_BUFFER,
                             ResourceState::RESOURCE_STATE_SHADER_RESOURCE, sizeof(DeferredShadingConstants)});

        diffuse_irradiance_sh3_buffer = rhi->CreateBuffer(
            BufferCreateInfo{DescriptorType::DESCRIPTOR_TYPE_CONSTANT_BUFFER,
                             ResourceState::RESOURCE_STATE_SHADER_RESOURCE, sizeof(DiffuseIrradianceSH3)});
    }

    // SHADING PASS
    shading_pass = rhi->CreateComputePipeline(ComputePipelineCreateInfo{});
    shading_color_image = rhi->CreateTexture(TextureCreateInfo{
        DescriptorType::DESCRIPTOR_TYPE_RW_TEXTURE, ResourceState::RESOURCE_STATE_UNORDERED_ACCESS,
        TextureType::TEXTURE_TYPE_2D, TextureFormat::TEXTURE_FORMAT_RGBA16_SFLOAT, width, height, 1, false});
    {

        // ibl

        diffuse_irradiance_sh3_constants.sh = {
            Math::float4{0.473198890686035f, 0.519405245780945f, 0.554664373397827f, 0.0f},
            Math::float4{0.416269570589066f, 0.466901600360870f, 0.595043838024139f, 0.0f},
            Math::float4{0.070390045642853f, 0.072113677859306f, 0.075183071196079f, 0.0f},
            Math::float4{0.200731590390205f, -0.189936503767967f, -0.178353592753410f, 0.0f},
            Math::float4{0.165346711874008f, -0.156177446246147f, -0.144699439406395f, 0.0f},
            Math::float4{0.037444319576025f, 0.041276078671217f, 0.046160303056240f, 0.0f},
            Math::float4{0.007342631462961f, -0.009751657955348f, -0.015737744048238f, 0.0f},
            Math::float4{0.023010414093733f, -0.011694960296154f, 0.001283747726120f, 0.0f},
            Math::float4{0.000401695695473f, -0.013503036461771f, -0.041937090456486f, 0.0f}};

        prefilered_irradiance_env_map_data =
            TextureLoader::Load(asset_path / "envrionment/football/footballSpecularHDR.dds");

        {
            TextureCreateInfo texture_create_info{};
            texture_create_info.width = prefilered_irradiance_env_map_data.width;
            texture_create_info.height = prefilered_irradiance_env_map_data.height;
            texture_create_info.array_layer = prefilered_irradiance_env_map_data.layer_count;
            texture_create_info.enanble_mipmap = true;
            texture_create_info.texture_type = TextureType::TEXTURE_TYPE_CUBE;
            texture_create_info.descriptor_types = DescriptorType::DESCRIPTOR_TYPE_TEXTURE_CUBE;
            texture_create_info.initial_state = ResourceState::RESOURCE_STATE_SHADER_RESOURCE;
            texture_create_info.texture_format = prefilered_irradiance_env_map_data.format;
            prefiltered_irradiance_env_map = rhi->CreateTexture(texture_create_info);
        }
        brdf_lut_data_desc = TextureLoader::Load(asset_path / "envrionment/football/footballBrdf.dds");
        {
            TextureCreateInfo texture_create_info{};
            texture_create_info.width = brdf_lut_data_desc.width;
            texture_create_info.height = brdf_lut_data_desc.height;
            texture_create_info.array_layer = brdf_lut_data_desc.layer_count;
            texture_create_info.enanble_mipmap = false;
            texture_create_info.texture_type = TextureType::TEXTURE_TYPE_2D;
            texture_create_info.descriptor_types = DescriptorType::DESCRIPTOR_TYPE_TEXTURE_CUBE;
            texture_create_info.initial_state = ResourceState::RESOURCE_STATE_SHADER_RESOURCE;
            texture_create_info.texture_format = brdf_lut_data_desc.format;
            brdf_lut = rhi->CreateTexture(texture_create_info);
        }
    }

    SamplerDesc sampler_desc{};
    sampler_desc.min_filter = FilterType::FILTER_LINEAR;
    sampler_desc.mag_filter = FilterType::FILTER_LINEAR;
    sampler_desc.mip_map_mode = MipMapMode::MIPMAP_MODE_LINEAR;
    sampler_desc.address_u = AddressMode::ADDRESS_MODE_CLAMP_TO_EDGE;
    sampler_desc.address_v = AddressMode::ADDRESS_MODE_CLAMP_TO_EDGE;
    sampler_desc.address_w = AddressMode::ADDRESS_MODE_CLAMP_TO_EDGE;

    ibl_sampler = rhi->CreateSampler(sampler_desc);

    deferred_shading_constants.width = width;
    deferred_shading_constants.height = height;
    deferred_shading_constants.ibl_intensity = 10000.0;

    geometry_pass->SetGraphicsShader(geometry_vs, geometry_ps);
    shading_pass->SetComputeShader(shading_cs);
}
DeferredData::~DeferredData() noexcept {
    mRhi->DestroyShader(geometry_vs);
    mRhi->DestroyShader(geometry_ps);

    mRhi->DestroyShader(shading_cs);

    mRhi->DestroyPipeline(geometry_pass);
    mRhi->DestroyPipeline(shading_pass);

    mRhi->DestroyBuffer(deferred_shading_constants_buffer);

    mRhi->DestroyRenderTarget(depth);
    mRhi->DestroyRenderTarget(gbuffer0);
    mRhi->DestroyRenderTarget(gbuffer1);
    mRhi->DestroyRenderTarget(gbuffer2);
    mRhi->DestroyRenderTarget(gbuffer3);
    mRhi->DestroyRenderTarget(gbuffer4);

    mRhi->DestroyTexture(shading_color_image);

    mRhi->DestroyBuffer(diffuse_irradiance_sh3_buffer);
    mRhi->DestroyTexture(brdf_lut);
    mRhi->DestroyTexture(prefiltered_irradiance_env_map);
    mRhi->DestroySampler(ibl_sampler);
}