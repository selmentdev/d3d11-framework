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
#include "../Framework/Texture.h"
#include "../Framework/Camera.h"
//------------------------------------------------------------------------------------------------//
#include "GeometryBuffer.h"
//------------------------------------------------------------------------------------------------//
namespace Effects
{
class SSAO
{
public:
    struct Vertex
    {
        DirectX::XMFLOAT4   Position;
        DirectX::XMFLOAT2   TexCoord;
    };

    struct __declspec(align(1)) PublicParams
    {
        DirectX::XMFLOAT4X4 g_inverse_projection;
        float       g_pixel_radius;
        float       g_viewport_far;
        float       g_base;
        float       g_area;
        float       g_intensity;
        int         g_render_mode;
        float       g_viewport_width;
        float       g_viewport_height;
    };

    enum RenderMode
    {
        RenderMode_Depth        = 0,
        RenderMode_Normal       = 1,
        RenderMode_Position     = 2,
        RenderMode_SSAO         = 3,
        RenderMode_X1           = 4,
    };
public:
    SSAO(ID3D11Device* device);
    virtual ~SSAO();

    void SetGeometryBuffer(GeometryBuffer* gb);

    void Begin(Prototype::Context* context);
    void End(Prototype::Context* context);
    void Render(Prototype::Context* context);

    PublicParams& GetPublicParams()
    {
        return _public_params;
    }
    const PublicParams& GetPublicParams() const
    {
        return _public_params;
    }

private:
    ID3D11Device*                           _device;
    ID3D11InputLayout*                      _il;
    ID3D11SamplerState*                     _sampler_Position;
    ID3D11SamplerState*                     _sampler_RandomNormal;

    Prototype::PixelShader                  _ps;
    Prototype::VertexShader                 _vs;
    Prototype::VertexBuffer*                _vb;
    Prototype::IndexBuffer*                 _ib;

    Prototype::Texture2D                    _random_normal;

    Prototype::ConstantBuffer<PublicParams> _cb_params;

    Prototype::RenderOperation_Shader       _rop_shader;
    Prototype::RenderOperation_Material     _rop_material;
    Prototype::RenderOperation_Mesh         _rop_mesh;
    Prototype::RenderOperation_Effect       _rop_effect;

    PublicParams                            _public_params;
};
//------------------------------------------------------------------------------------------------//
}
//------------------------------------------------------------------------------------------------//