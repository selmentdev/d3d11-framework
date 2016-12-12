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
#ifndef _Prototype_ConstantBuffer_H__
#define _Prototype_ConstantBuffer_H__
//------------------------------------------------------------------------------------------------//
#include "Headers.h"
//------------------------------------------------------------------------------------------------//
#define ROUNDUP_ALIGN_16(Value) ((Value + 0xF) & (~0xF))
//------------------------------------------------------------------------------------------------//
namespace Prototype
{
//------------------------------------------------------------------------------------------------//
template <typename T>
class ConstantBuffer
{
public:
    ConstantBuffer(ID3D11Device* device);
    ConstantBuffer(ID3D11Device* device, const T* data);

    virtual ~ConstantBuffer();

    ID3D11Buffer* GetBuffer() const
    {
        return _buffer;
    }

    T* BeginUpdate(ID3D11DeviceContext* context);
    void EndUpdate(ID3D11DeviceContext* context);

private:
    ID3D11Device*       _device;
    ID3D11Buffer*       _buffer;
};
//------------------------------------------------------------------------------------------------//
template <typename T>
inline ConstantBuffer<T>::ConstantBuffer(ID3D11Device* device)
    : _device(device)
    , _buffer(nullptr)
{
    D3D11_BUFFER_DESC desc;
    desc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
    desc.ByteWidth = ROUNDUP_ALIGN_16(sizeof(T));
    desc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
    desc.MiscFlags = 0;
    desc.StructureByteStride = 0;
    desc.Usage = D3D11_USAGE_DYNAMIC;

    D3D11_SUBRESOURCE_DATA subresource;
    subresource.pSysMem = nullptr;
    subresource.SysMemPitch = 0;
    subresource.SysMemSlicePitch = 0;

    SAFE_CALL(_device->CreateBuffer(&desc, nullptr, &_buffer));
    SAFE_CALL(Prototype::SetDebugObjectName(_buffer, "Constant Buffer"));
}
//------------------------------------------------------------------------------------------------//
template <typename T>
inline ConstantBuffer<T>::ConstantBuffer(ID3D11Device* device, const T* data)
    : _device(device)
    , _buffer(nullptr)
{
    D3D11_BUFFER_DESC desc;
    desc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
    desc.ByteWidth = ROUNDUP_ALIGN_16(sizeof(T));
    desc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
    desc.MiscFlags = 0;
    desc.StructureByteStride = 0;
    desc.Usage = D3D11_USAGE_DYNAMIC;

    D3D11_SUBRESOURCE_DATA subresource;
    subresource.pSysMem = data;
    subresource.SysMemPitch = 0;
    subresource.SysMemSlicePitch = 0;

    SAFE_CALL(_device->CreateBuffer(&desc, &subresource, &_buffer));
}
//------------------------------------------------------------------------------------------------//
template <typename T>
inline ConstantBuffer<T>::~ConstantBuffer()
{
    SAFE_RELEASE(_buffer);
}
//------------------------------------------------------------------------------------------------//
template <typename T>
inline T* ConstantBuffer<T>::BeginUpdate(ID3D11DeviceContext* context)
{
    D3D11_MAPPED_SUBRESOURCE subresource = {0};
    SAFE_CALL(context->Map(_buffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &subresource));

    return reinterpret_cast<T*>(subresource.pData);
}
//------------------------------------------------------------------------------------------------//
template <typename T>
void ConstantBuffer<T>::EndUpdate(ID3D11DeviceContext* context)
{
    context->Unmap(_buffer, 0);
}
//------------------------------------------------------------------------------------------------//
}
//------------------------------------------------------------------------------------------------//
#endif /* _Prototype_ConstantBuffer_H__ */