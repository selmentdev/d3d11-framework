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

#include "RenderTexture.h"
//------------------------------------------------------------------------------------------------//
#include "Headers.h"
//------------------------------------------------------------------------------------------------//
namespace Prototype
{
//------------------------------------------------------------------------------------------------//
RenderTexture::RenderTexture(ID3D11Device* device, UINT width, UINT height, DXGI_FORMAT format)
    : _device(device)
    , _texture(nullptr)
    , _rtv(nullptr)
    , _format(format)
{
    Resize(width, height);
}
//------------------------------------------------------------------------------------------------//
RenderTexture::~RenderTexture()
{
    SAFE_RELEASE(_srv);
    SAFE_RELEASE(_rtv);
    SAFE_RELEASE(_texture);
}
//------------------------------------------------------------------------------------------------//
void RenderTexture::Clear(ID3D11DeviceContext* context, const float color[4])
{
    context->ClearRenderTargetView(_rtv, color);
}
//------------------------------------------------------------------------------------------------//
void RenderTexture::Resize(UINT width, UINT height)
{
    SAFE_RELEASE(_rtv);
    SAFE_RELEASE(_srv);
    SAFE_RELEASE(_texture);

    D3D11_TEXTURE2D_DESC desc = {0};
    desc.Width                  = width;
    desc.Height                 = height;
    desc.MipLevels              = 1;
    desc.ArraySize              = 1;
    desc.Format                 = _format;
    desc.SampleDesc.Count       = 1;
    desc.SampleDesc.Quality     = 0;
    desc.Usage                  = D3D11_USAGE_DEFAULT;
    desc.BindFlags              = D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE;
    desc.CPUAccessFlags         = 0;
    desc.MiscFlags              = 0;

    SAFE_CALL(_device->CreateTexture2D(&desc, nullptr, &_texture));
    SAFE_CALL(Prototype::SetDebugObjectName(_texture, "RenderTexture.Texture2D"));
    SAFE_CALL(_device->CreateRenderTargetView(_texture, nullptr, &_rtv));
    SAFE_CALL(Prototype::SetDebugObjectName(_rtv, "RenderTexture.RTV"));

    D3D11_SHADER_RESOURCE_VIEW_DESC srvdesc;
    srvdesc.Format                      = _format;
    srvdesc.ViewDimension               = D3D11_SRV_DIMENSION_TEXTURE2D;
    srvdesc.Texture2D.MipLevels         = 1;
    srvdesc.Texture2D.MostDetailedMip   = 0;
    SAFE_CALL(_device->CreateShaderResourceView(_texture, &srvdesc, &_srv));
    SAFE_CALL(Prototype::SetDebugObjectName(_srv, "RenderTexture.SRV"));
}
//------------------------------------------------------------------------------------------------//
}
//------------------------------------------------------------------------------------------------//