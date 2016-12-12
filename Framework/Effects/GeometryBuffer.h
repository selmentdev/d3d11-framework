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
//------------------------------------------------------------------------------------------------//
#include <DirectXMath.h>
//------------------------------------------------------------------------------------------------//
#include "../Framework/ConstantBuffer.h"
#include "../Framework/VertexShader.h"
#include "../Framework/PixelShader.h"
#include "../Framework/Context.h"
#include "../Framework/Mesh.h"
#include "../Framework/RenderDepthStencil.h"
#include "../Framework/RenderTexture.h"
#include "../Framework/Framebuffer.h"
//------------------------------------------------------------------------------------------------//
namespace Effects
{
class GeometryBuffer
{
public:
    struct ObjectData
    {
        DirectX::XMMATRIX       World;
        DirectX::XMMATRIX       View;
    };

    struct ViewData
    {
        DirectX::XMMATRIX       Projection;
    };

public:
    GeometryBuffer(ID3D11Device* device, UINT width, UINT height);
    ~GeometryBuffer();

    void Resize(Prototype::Context* context, UINT width, UINT height);

    void Begin(Prototype::Context* context);
    void End(Prototype::Context* context);
    void Render(Prototype::Context* context, Prototype::Mesh* mesh);

    void SetProjectionMatrix(Prototype::Context* context, DirectX::XMMATRIX projection);

    void SetWorldViewMatrix(Prototype::Context* context, DirectX::XMMATRIX world,
                            DirectX::XMMATRIX view);

    const Prototype::RenderDepthStencil& GetDepth() const
    {
        return _depth;
    }

    const Prototype::RenderTexture& GetNormal() const
    {
        return _normal;
    }

    //const Prototype::RenderTexture& GetPosition() const { return _position; }

    //const Prototype::RenderTexture& GetDiffuse() const { return _diffuse; }

private:
    ID3D11Device*                           _device;
    ID3D11InputLayout*                      _il;
    ID3D11RasterizerState*                  _rs;

    Prototype::RenderDepthStencil           _depth;
    Prototype::RenderTexture                _normal;
    //Prototype::RenderTexture                _position;
    //Prototype::RenderTexture                _diffuse;
    Prototype::Framebuffer                  _fb;

    Prototype::PixelShader                  _ps;
    Prototype::VertexShader                 _vs;
    Prototype::ConstantBuffer<ObjectData>   _cb_object;
    Prototype::ConstantBuffer<ViewData>     _cb_view;
    Prototype::RenderOperation_Shader       _rop_shader;
    Prototype::RenderOperation_Effect       _rop_effect;
};
}
//------------------------------------------------------------------------------------------------//