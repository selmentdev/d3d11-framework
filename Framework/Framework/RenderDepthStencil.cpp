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

#include "RenderDepthStencil.h"
//------------------------------------------------------------------------------------------------//
namespace Prototype
{
//------------------------------------------------------------------------------------------------//
RenderDepthStencil::RenderDepthStencil(ID3D11Device* device, UINT width, UINT height,
                                       bool useStencil)
    : _device(device)
    , _texture(nullptr)
    , _dsv(nullptr)
    , _format(DXGI_FORMAT_UNKNOWN)
{
    _format = useStencil ? DXGI_FORMAT_D24_UNORM_S8_UINT : DXGI_FORMAT_D32_FLOAT;

    Resize(width, height);
}
//------------------------------------------------------------------------------------------------//
RenderDepthStencil::~RenderDepthStencil()
{
    SAFE_RELEASE(_srv);
    SAFE_RELEASE(_dsv);
    SAFE_RELEASE(_texture);
}
//------------------------------------------------------------------------------------------------//
void RenderDepthStencil::Resize(UINT width, UINT height)
{
    SAFE_RELEASE(_srv);
    SAFE_RELEASE(_dsv);
    SAFE_RELEASE(_texture);

    D3D11_TEXTURE2D_DESC desc = {0};
    desc.Width                  = width;
    desc.Height                 = height;
    desc.MipLevels              = 1;
    desc.ArraySize              = 1;
    desc.Format                 = DXGI_FORMAT_R32_TYPELESS;
    desc.SampleDesc.Count       = 1;
    desc.SampleDesc.Quality     = 0;
    desc.Usage                  = D3D11_USAGE_DEFAULT;
    desc.BindFlags              = D3D11_BIND_DEPTH_STENCIL | D3D11_BIND_SHADER_RESOURCE;
    desc.CPUAccessFlags         = 0;
    desc.MiscFlags              = 0;

    SAFE_CALL(_device->CreateTexture2D(&desc, nullptr, &_texture));
    SAFE_CALL(Prototype::SetDebugObjectName(_texture, "RenderDepthStencil.Texture2D"));

    D3D11_DEPTH_STENCIL_VIEW_DESC dsvDesc;
    dsvDesc.Flags = 0;
    dsvDesc.Format = DXGI_FORMAT_D32_FLOAT;
    dsvDesc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
    dsvDesc.Texture2D.MipSlice  = 0;
    SAFE_CALL(_device->CreateDepthStencilView(_texture, &dsvDesc, &_dsv));
    SAFE_CALL(Prototype::SetDebugObjectName(_dsv, "RenderDepthStencil.DSV"));

    D3D11_SHADER_RESOURCE_VIEW_DESC srvdesc;
    srvdesc.Format                      = DXGI_FORMAT_R32_FLOAT;
    srvdesc.ViewDimension               = D3D11_SRV_DIMENSION_TEXTURE2D;
    srvdesc.Texture2D.MipLevels         = desc.MipLevels;
    srvdesc.Texture2D.MostDetailedMip   = 0;
    SAFE_CALL(_device->CreateShaderResourceView(_texture, &srvdesc, &_srv));
    SAFE_CALL(Prototype::SetDebugObjectName(_srv, "RenderDepthStencil.SRV"));
}
//------------------------------------------------------------------------------------------------//
void RenderDepthStencil::Clear(ID3D11DeviceContext* context)
{
    context->ClearDepthStencilView(_dsv, D3D11_CLEAR_DEPTH, 1.0f, 0x00);
}
//------------------------------------------------------------------------------------------------//
}
//------------------------------------------------------------------------------------------------//