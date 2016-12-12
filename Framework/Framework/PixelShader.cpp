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

#include "PixelShader.h"
//------------------------------------------------------------------------------------------------//
namespace Prototype
{
//------------------------------------------------------------------------------------------------//
PixelShader::PixelShader(ID3D11Device* device, const wchar_t* sourceFileName)
    : _device(device)
{
    ID3DBlob* code = nullptr;
    ID3DBlob* errors = nullptr;

    UINT flags = 0;
#if defined(_DEBUG) || !defined(NDEBUG)
    flags |= D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION;
#else
    flags |= D3DCOMPILE_OPTIMIZATION_LEVEL3;
#endif

    HRESULT hr = D3DCompileFromFile(sourceFileName, nullptr, nullptr, "ps_main", "ps_5_0",
                                    flags, 0, &code, &errors);

    if (errors != nullptr)
    {
        const char* data = (const char*)errors->GetBufferPointer();
        ::OutputDebugStringA(data);
    }

    SAFE_CALL(hr);

    SAFE_CALL(device->CreatePixelShader(code->GetBufferPointer(), code->GetBufferSize(), nullptr,
                                        &_shader));
    SAFE_CALL(Prototype::SetDebugObjectName(_shader, "Pixel Shader"));
    SAFE_RELEASE(code);
    SAFE_RELEASE(errors);
}
//------------------------------------------------------------------------------------------------//
PixelShader::~PixelShader()
{
    SAFE_RELEASE(_shader);
}
//------------------------------------------------------------------------------------------------//
}
//------------------------------------------------------------------------------------------------//