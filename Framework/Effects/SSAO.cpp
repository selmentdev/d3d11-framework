//
// Copyright(c) 2013 Karol Grzybowski
//
// Permission is hereby granted, free of charge, to any person obtaining a copy of this software
// and associated documentation files(the "Software"), to deal in the Software without restriction,
// including without limitation the rights to use, copy, modify, merge, publish, distribute,
// sublicense, and / or sell copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions :
//
// The above copyright notice and this permission notice shall be included in all copies or
// substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT
// NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
// NONINFRINGEMENT.IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM,
// DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
//

#include "SSAO.h"
//------------------------------------------------------------------------------------------------//
namespace Effects
{
//------------------------------------------------------------------------------------------------//
SSAO::SSAO(ID3D11Device* device)
    : _device(device)
    , _ps(device, L"Shaders/SSAO.hlsl")
    , _vs(device, L"Shaders/SSAO.hlsl")
    , _cb_params(device)
    , _random_normal(device, L"Textures/Random.dds")
{
    _public_params.g_viewport_far   = 1000.0F;
    _public_params.g_pixel_radius   = 0.0148F;
    _public_params.g_render_mode    = RenderMode_SSAO;
    _public_params.g_base           = 0.2F;
    _public_params.g_intensity      = 1.0F;
    _public_params.g_area           = 0.0004F; //0.0075F;


    D3D11_INPUT_ELEMENT_DESC input_layout[] =
    {
        {"POSITION",        0, DXGI_FORMAT_R32G32B32A32_FLOAT,  0,  0,  D3D11_INPUT_PER_VERTEX_DATA, 0},
        {"TEXCOORD",        0, DXGI_FORMAT_R32G32_FLOAT,        0,  16, D3D11_INPUT_PER_VERTEX_DATA, 0},
    };
    SAFE_CALL(_device->CreateInputLayout(
                  input_layout, sizeof(input_layout) / sizeof(input_layout[0]),
                  _vs.GetShaderCode()->GetBufferPointer(), _vs.GetShaderCode()->GetBufferSize(), &_il));

    _rop_shader._ps     = _ps.GetShader();
    _rop_shader._vs     = _vs.GetShader();

    _rop_material._cb_public_params     = _cb_params.GetBuffer();

    static const Vertex vertices[] =
    {
        {DirectX::XMFLOAT4(-1.0f, -1.0f, 0.0f, 1.0f), DirectX::XMFLOAT2(1.0f, 1.0f)},
        {DirectX::XMFLOAT4( 1.0f, -1.0f, 0.0f, 1.0f), DirectX::XMFLOAT2(0.0f, 1.0f)},
        {DirectX::XMFLOAT4( 1.0f,  1.0f, 0.0f, 1.0f), DirectX::XMFLOAT2(0.0f, 0.0f)},
        {DirectX::XMFLOAT4(-1.0f,  1.0f, 0.0f, 1.0f), DirectX::XMFLOAT2(1.0f, 0.0f)}
    };

    static const INT16 indices[] =
    {
        0, 3, 2,
        0, 2, 1
    };

    _vb = new Prototype::VertexBuffer(_device, vertices, sizeof(vertices));
    _ib = new Prototype::IndexBuffer(_device, indices, sizeof(indices));

    D3D11_SAMPLER_DESC samplerDesc;
    samplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_CLAMP;
    samplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_CLAMP;
    samplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_CLAMP;
    samplerDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
    samplerDesc.MinLOD = -FLT_MAX;
    samplerDesc.MaxLOD = FLT_MAX;
    samplerDesc.MipLODBias = 0.0f;
    samplerDesc.MaxAnisotropy = 16;
    samplerDesc.ComparisonFunc = D3D11_COMPARISON_NEVER;
    samplerDesc.BorderColor[0] = samplerDesc.BorderColor[1] = samplerDesc.BorderColor[2] =
                                     samplerDesc.BorderColor[3] = 1.0f;
    SAFE_CALL(_device->CreateSamplerState(&samplerDesc, &_sampler_Position));

    samplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
    samplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
    samplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
    SAFE_CALL(_device->CreateSamplerState(&samplerDesc, &_sampler_RandomNormal));

    _rop_effect._samplers[0]            = _sampler_Position;
    _rop_effect._samplers[1]            = _sampler_RandomNormal;
    _rop_effect._samplersCount          = 2;

    _rop_material._cb_public_params     = _cb_params.GetBuffer();

    _rop_mesh._il           = _il;

    _rop_mesh._ib           = _ib->GetBuffer();
    _rop_mesh._ib_count     = 6;
    _rop_mesh._ib_format    = DXGI_FORMAT_R16_UINT;
    _rop_mesh._topology     = D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST;
    _rop_mesh._vb           = _vb->GetBuffer();
    _rop_mesh._vb_stride    = sizeof(Vertex);
}
//------------------------------------------------------------------------------------------------//
SSAO::~SSAO()
{
    SAFE_RELEASE(_il);
    SAFE_RELEASE(_sampler_Position);
    SAFE_RELEASE(_sampler_RandomNormal);

    delete _vb;
    delete _ib;
}
//------------------------------------------------------------------------------------------------//
void SSAO::SetGeometryBuffer(GeometryBuffer* gb)
{
    _rop_material._textures[0] = gb->GetDepth().GetSRV();
    _rop_material._textures[1] = gb->GetNormal().GetSRV();
    _rop_material._textures[2] = _random_normal.GetSRV();
    _rop_material._texturesCount = 3;
}
//------------------------------------------------------------------------------------------------//
void SSAO::Begin(Prototype::Context* context)
{
    auto& params = *_cb_params.BeginUpdate(context->GetContext());
    params = _public_params;
    _cb_params.EndUpdate(context->GetContext());

    context->BindMaterial(&_rop_material);
    context->BindShaders(&_rop_shader);
    context->BindEffectStates(&_rop_effect);
    context->BindEffectSamplers_PS(&_rop_effect);
}
//------------------------------------------------------------------------------------------------//
void SSAO::End(Prototype::Context* context)
{
    context->UnbindMaterial(&_rop_material);
}
//------------------------------------------------------------------------------------------------//
void SSAO::Render(Prototype::Context* context)
{
    context->Render(&_rop_mesh);
}
//------------------------------------------------------------------------------------------------//
}
//------------------------------------------------------------------------------------------------//