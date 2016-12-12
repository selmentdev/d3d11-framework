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

#pragma once
#ifndef _Prototype_RenderDepthStencil_H__
#define _Prototype_RenderDepthStencil_H__
//------------------------------------------------------------------------------------------------//
#include "Headers.h"
//------------------------------------------------------------------------------------------------//
namespace Prototype
{
//------------------------------------------------------------------------------------------------//
class RenderDepthStencil
{
public:
    RenderDepthStencil(ID3D11Device* device, UINT width, UINT height, bool useStencil);

    virtual ~RenderDepthStencil();

    void Resize(UINT width, UINT height);

    void Clear(ID3D11DeviceContext* context);

    ID3D11Texture2D* GetTexture2D() const
    {
        return _texture;
    }

    ID3D11DepthStencilView* GetDSV() const
    {
        return _dsv;
    }

    ID3D11ShaderResourceView* GetSRV() const
    {
        return _srv;
    }

private:
    ID3D11Device*               _device;
    ID3D11Texture2D*            _texture;
    ID3D11DepthStencilView*     _dsv;
    ID3D11ShaderResourceView*   _srv;
    DXGI_FORMAT                 _format;
};
//------------------------------------------------------------------------------------------------//
}
//------------------------------------------------------------------------------------------------//
#endif /* _Prototype_RenderDepthStencil_H__ */