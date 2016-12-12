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

#include "VertexBuffer.h"
//------------------------------------------------------------------------------------------------//
namespace Prototype
{
//------------------------------------------------------------------------------------------------//
VertexBuffer::VertexBuffer(ID3D11Device* device, const void* data, size_t length)
    : _device(device)
    , _buffer(nullptr)
{
    D3D11_BUFFER_DESC desc;
    desc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
    desc.ByteWidth = static_cast<UINT>(length);
    desc.CPUAccessFlags = 0;
    desc.MiscFlags = 0;
    desc.StructureByteStride = 0;
    desc.Usage = D3D11_USAGE_IMMUTABLE;

    D3D11_SUBRESOURCE_DATA subresource;
    subresource.pSysMem = data;
    subresource.SysMemPitch = 0;
    subresource.SysMemSlicePitch = 0;

    SAFE_CALL(_device->CreateBuffer(&desc, &subresource, &_buffer));
    SAFE_CALL(Prototype::SetDebugObjectName(_buffer, "VertexBuffer"));
}
//------------------------------------------------------------------------------------------------//
VertexBuffer::~VertexBuffer()
{
    SAFE_RELEASE(_buffer);
}
//------------------------------------------------------------------------------------------------//
}
//------------------------------------------------------------------------------------------------//