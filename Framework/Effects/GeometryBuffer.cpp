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

#include "GeometryBuffer.h"
//------------------------------------------------------------------------------------------------//
namespace Effects
{
//------------------------------------------------------------------------------------------------//
GeometryBuffer::GeometryBuffer(ID3D11Device* device, UINT width, UINT height)
    : _device(device)
    , _vs(device, L"Shaders/GBuffer.hlsl")
    , _ps(device, L"Shaders/GBuffer.hlsl")
    , _cb_object(device)
    , _cb_view(device)
    , _depth(device, width, height, false)
    , _normal(device, width, height, DXGI_FORMAT_R8G8B8A8_UNORM)
{
    _rop_shader._cb_object      = _cb_object.GetBuffer();
    _rop_shader._cb_view        = _cb_view.GetBuffer();
    _rop_shader._ps             = _ps.GetShader();
    _rop_shader._vs             = _vs.GetShader();

    D3D11_INPUT_ELEMENT_DESC input_layout[] =
    {
        {"POSITION",        0,  DXGI_FORMAT_R32G32B32A32_FLOAT,     0,  0,           D3D11_INPUT_PER_VERTEX_DATA, 0},
        {"NORMAL",          0,  DXGI_FORMAT_R32G32B32_FLOAT,        0,  16,          D3D11_INPUT_PER_VERTEX_DATA, 0},
    };

    SAFE_CALL(_device->CreateInputLayout(input_layout, sizeof(input_layout) / sizeof(input_layout[0]),
                                         _vs.GetShaderCode()->GetBufferPointer(), _vs.GetShaderCode()->GetBufferSize(), &_il));

    D3D11_RASTERIZER_DESC rsDesc;
    rsDesc.FillMode = D3D11_FILL_SOLID;
    rsDesc.CullMode = D3D11_CULL_BACK;
    rsDesc.FrontCounterClockwise = FALSE;
    rsDesc.DepthBias = 0;
    rsDesc.DepthBiasClamp = 0.0f;
    rsDesc.SlopeScaledDepthBias = 0.0f;
    rsDesc.DepthClipEnable = TRUE;
    rsDesc.ScissorEnable = FALSE;
    rsDesc.MultisampleEnable = FALSE;
    rsDesc.AntialiasedLineEnable = FALSE;
    SAFE_CALL(_device->CreateRasterizerState(&rsDesc, &_rs));

    _rop_effect._rasterizer     = _rs;

    _fb.SetCount(1);
    _fb.SetDepthStencil(_depth.GetDSV());
    _fb.SetRenderTarget(0, _normal.GetRTV());
    //_fb.SetRenderTarget(1, _position.GetRTV());
    //_fb.SetRenderTarget(2, _diffuse.GetRTV());
}
//------------------------------------------------------------------------------------------------//
GeometryBuffer::~GeometryBuffer()
{
    SAFE_RELEASE(_rs);
    SAFE_RELEASE(_il);
}
//------------------------------------------------------------------------------------------------//
void GeometryBuffer::Resize(Prototype::Context* context, UINT width, UINT height)
{
    (void)context;
    _depth.Resize(width, height);
    _normal.Resize(width, height);
    //_position.Resize(width, height);
    //_diffuse.Resize(width, height);
}
//------------------------------------------------------------------------------------------------//
void GeometryBuffer::Begin(Prototype::Context* context)
{
    static const float color[4] = {0.0F, 0.0F, 0.0F, 0.0F};
    _depth.Clear(context->GetContext());
    _normal.Clear(context->GetContext(), color);
    //_position.Clear(context->GetContext(), color);
    //_diffuse.Clear(context->GetContext(), color);

    _fb.Bind(context->GetContext());
    context->BindShaders(&_rop_shader);
    context->BindEffectStates(&_rop_effect);
}
//------------------------------------------------------------------------------------------------//
void GeometryBuffer::End(Prototype::Context* context)
{
    ID3D11RenderTargetView* rtv = nullptr;
    context->GetContext()->OMSetRenderTargets(1, &rtv, nullptr);
}
//------------------------------------------------------------------------------------------------//
void GeometryBuffer::Render(Prototype::Context* context, Prototype::Mesh* mesh)
{
    Prototype::RenderOperation_Mesh rop = (*mesh->GetRenderOperation());
    rop._il = _il;

    context->Render(&rop);
}
//------------------------------------------------------------------------------------------------//
void GeometryBuffer::SetProjectionMatrix(Prototype::Context* context, DirectX::XMMATRIX projection)
{
    ViewData* data      = _cb_view.BeginUpdate(context->GetContext());
    data->Projection    = DirectX::XMMatrixTranspose(projection);
    _cb_view.EndUpdate(context->GetContext());
}
//------------------------------------------------------------------------------------------------//
void GeometryBuffer::SetWorldViewMatrix(Prototype::Context* context, DirectX::XMMATRIX world,
                                        DirectX::XMMATRIX view)
{
    ObjectData* data    = _cb_object.BeginUpdate(context->GetContext());
    data->World         = DirectX::XMMatrixTranspose(world);
    data->View          = DirectX::XMMatrixTranspose(view);
    _cb_object.EndUpdate(context->GetContext());
}
//------------------------------------------------------------------------------------------------//
}
//------------------------------------------------------------------------------------------------//