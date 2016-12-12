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
#include "Headers.h"
#include "IndexBuffer.h"
#include "VertexBuffer.h"
#include "Context.h"
//------------------------------------------------------------------------------------------------//
namespace Prototype
{
//------------------------------------------------------------------------------------------------//
class Mesh
{
public:
    struct Vertex
    {
        DirectX::XMFLOAT4   Position;
        DirectX::XMFLOAT3   Normal;
    };

public:
    Mesh(ID3D11Device* device);
    virtual ~Mesh();

    void LoadFromFile(const wchar_t* filename); // TODO: napisac

    IndexBuffer* GetIndexBuffer() const
    {
        return _ib;
    }

    VertexBuffer* GetVertexBuffer() const
    {
        return _vb;
    }

    const RenderOperation_Mesh* GetRenderOperation() const
    {
        return &_rop_mesh;
    }

    UINT GetTriangleCount() const
    {
        return _triangle_count;
    }

private:
    IndexBuffer*            _ib;
    VertexBuffer*           _vb;
    RenderOperation_Mesh    _rop_mesh;
    ID3D11Device*           _device;

    UINT                    _triangle_count;
};
//------------------------------------------------------------------------------------------------//
}
//------------------------------------------------------------------------------------------------//