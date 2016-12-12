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
#ifndef _Prototype_Headers_H__
#define _Prototype_Headers_H__

#define NOMINMAX
#include <Windows.h>
#include <d3d11.h>
#include <cassert>
#include <d3dcompiler.h>
#include <DirectXMath.h>

#pragma warning(disable : 4127)

#pragma comment(lib, "dxguid.lib")
#pragma comment(lib, "dxgi.lib")
#pragma comment(lib, "d3d11.lib")
#pragma comment(lib, "d3dcompiler.lib")

#define S(x) #x
#define S_(x) S(x)
#define S__LINE__ S_(__LINE__)

#define SAFE_RELEASE(Object) \
    do \
    { \
        if (Object != nullptr) \
        { \
            Object->Release(); \
            Object = nullptr; \
        } \
    } while (false)

#define SAFE_CALL(Expression) \
    do \
    { \
        ::HRESULT ____hr = (Expression); \
        if (FAILED(____hr)) \
        { \
            ::MessageBoxA(0, "Expression: `" #Expression "` failed ("__FILE__": "S__LINE__")", "Failure", MB_OK | MB_ICONERROR); \
            ::OutputDebugStringA("Expression: `" #Expression "` failed"); \
        } \
    } while (false)

namespace Prototype
{
template<UINT TNameLength>
HRESULT SetDebugObjectName(_In_ ID3D11DeviceChild* resource, const char (&name)[TNameLength])
{
#if defined(_DEBUG) || defined(PROFILE)
    return resource->SetPrivateData(WKPDID_D3DDebugObjectName, TNameLength - 1, name);
#else
    (void)resource;
    (void)name;
    return S_OK;
#endif
}
}

#endif /* _Prototype_Headers_H__ */