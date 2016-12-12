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

#include "Headers.h"

namespace Prototype
{
struct RenderOperation_Effect
{
    RenderOperation_Effect()
    {
        _blend          = nullptr;
        _rasterizer     = nullptr;
        _depthStencil   = nullptr;

        for (size_t i = 0; i < 16; ++i)
        {
            _samplers[i] = nullptr;
        }

        _samplersCount  = 0;

        _blendFactor[0] = 0.0f;
        _blendFactor[1] = 0.0f;
        _blendFactor[2] = 0.0f;
        _blendFactor[3] = 0.0f;

        _mask           = 0xffffffff;
        _stencilRef     = 0x00000000;
    }

    ID3D11BlendState*           _blend;
    ID3D11RasterizerState*      _rasterizer;
    ID3D11DepthStencilState*    _depthStencil;
    ID3D11SamplerState*         _samplers[16];
    UINT                        _samplersCount;
    FLOAT                       _blendFactor[4];
    UINT                        _mask;
    UINT                        _stencilRef;
};

struct RenderOperation_Material
{
    RenderOperation_Material()
    {
        for (size_t i = 0; i < 16; ++i)
        {
            _textures[i] = nullptr;
        }

        _texturesCount      = 0;
        _cb_public_params   = nullptr;
    }

    ID3D11ShaderResourceView*   _textures[16];
    UINT                        _texturesCount;
    ID3D11Buffer*               _cb_public_params;
};

struct RenderOperation_Shader
{
    RenderOperation_Shader()
    {
        _vs         = nullptr;
        _ps         = nullptr;
        _cb_object  = nullptr;
        _cb_view    = nullptr;
    }

    ID3D11VertexShader*         _vs;
    ID3D11PixelShader*          _ps;
    ID3D11Buffer*               _cb_object;
    ID3D11Buffer*               _cb_view;
};

struct RenderOperation_Mesh
{
    RenderOperation_Mesh()
    {
        _il             = nullptr;
        _vb             = nullptr;
        _ib             = nullptr;
        _topology       = D3D11_PRIMITIVE_TOPOLOGY_UNDEFINED;
        _vb_offset      = 0;
        _vb_stride      = 0;
        _vb_count       = 0;
        _ib_offset      = 0;
        _ib_count       = 0;
        _ib_first       = 0;
        _vb_first       = 0;
        _ib_format      = DXGI_FORMAT_UNKNOWN;
    }

    ID3D11InputLayout*          _il;
    ID3D11Buffer*               _vb;
    ID3D11Buffer*               _ib;
    D3D11_PRIMITIVE_TOPOLOGY    _topology;
    UINT                        _vb_offset;
    UINT                        _vb_stride;
    UINT                        _vb_count;
    UINT                        _ib_offset;
    UINT                        _ib_count;
    UINT                        _ib_first;
    UINT                        _vb_first;
    DXGI_FORMAT                 _ib_format;
};

class Context
{
    static const int CBufferIndex_View          = 0;
    static const int CBufferIndex_Object        = 1;
    static const int CBufferIndex_PublicParams  = 2;

public:
    Context(ID3D11Device* device, ID3D11DeviceContext* context)
        : _device(device)
        , _context(context)
    {
    }

    void BindEffectStates(const RenderOperation_Effect* effect)
    {
        //_context->OMSetBlendState(effect->_blend, effect->_blendFactor, effect->_mask);
        //_context->OMSetDepthStencilState(effect->_depthStencil, effect->_stencilRef);
        _context->RSSetState(effect->_rasterizer);
    }

    void BindEffectSamplers_VS(const RenderOperation_Effect* effect)
    {
        _context->VSSetSamplers(0, effect->_samplersCount, &effect->_samplers[0]);
    }

    void BindEffectSamplers_PS(const RenderOperation_Effect* effect)
    {
        _context->PSSetSamplers(0, effect->_samplersCount, &effect->_samplers[0]);
    }

    void BindMaterial(const RenderOperation_Material* material)
    {
        _context->PSSetConstantBuffers(CBufferIndex_PublicParams, 1, &material->_cb_public_params);
        _context->VSSetConstantBuffers(CBufferIndex_PublicParams, 1, &material->_cb_public_params);
        _context->PSSetShaderResources(0, material->_texturesCount, material->_textures);
        _context->VSSetShaderResources(0, material->_texturesCount, material->_textures);
    }

    void UnbindMaterial(const RenderOperation_Material* material)
    {
        //_context->PSSetConstantBuffers(CBufferIndex_PublicParams, 1, &material->_cb_public_params);
        //_context->VSSetConstantBuffers(CBufferIndex_PublicParams, 1, &material->_cb_public_params);
        ID3D11ShaderResourceView* srv[16] = {nullptr};
        _context->PSSetShaderResources(0, material->_texturesCount, srv);
        _context->VSSetShaderResources(0, material->_texturesCount, srv);
    }

    void BindShaders(const RenderOperation_Shader* shader)
    {
        _context->PSSetShader(shader->_ps, nullptr, 0);
        _context->VSSetShader(shader->_vs, nullptr, 0);

        _context->PSSetConstantBuffers(CBufferIndex_View, 1, &shader->_cb_view);
        _context->PSSetConstantBuffers(CBufferIndex_Object, 1, &shader->_cb_object);
        _context->VSSetConstantBuffers(CBufferIndex_View, 1, &shader->_cb_view);
        _context->VSSetConstantBuffers(CBufferIndex_Object, 1, &shader->_cb_object);
    }

    void Render(const RenderOperation_Mesh* mesh)
    {
        _context->IASetInputLayout(mesh->_il);
        _context->IASetVertexBuffers(0, 1, &mesh->_vb, &mesh->_vb_stride, &mesh->_vb_offset);
        _context->IASetPrimitiveTopology(mesh->_topology);
        _context->IASetIndexBuffer(mesh->_ib, mesh->_ib_format, mesh->_ib_offset);
        _context->DrawIndexed(mesh->_ib_count, mesh->_ib_first, mesh->_vb_first);
    }

    void BindViewport(float width, float height)
    {
        D3D11_VIEWPORT viewport;
        viewport.Height = height;
        viewport.Width = width;
        viewport.MinDepth = 0.0f;
        viewport.MaxDepth = 1.0f;
        viewport.TopLeftX = 0.0f;
        viewport.TopLeftY = 0.0f;

        _context->RSSetViewports(1, &viewport);
    }

    ID3D11DeviceContext* GetContext() const
    {
        return _context;
    }

private:
    ID3D11Device*           _device;
    ID3D11DeviceContext*    _context;
};
}