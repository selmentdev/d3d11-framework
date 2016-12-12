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

#include "OcclusionQuery.h"
//------------------------------------------------------------------------------------------------//
namespace Prototype
{
//------------------------------------------------------------------------------------------------//
OcclusionQuery::OcclusionQuery(ID3D11Device* device)
    : _device(device)
    , _query(nullptr)
{
    D3D11_QUERY_DESC desc;
    desc.Query = D3D11_QUERY_OCCLUSION;
    desc.MiscFlags = 0;
    SAFE_CALL(_device->CreateQuery(&desc, &_query));
    SAFE_CALL(Prototype::SetDebugObjectName(_query, "OcclusionQuery"));
}
//------------------------------------------------------------------------------------------------//
OcclusionQuery::~OcclusionQuery()
{
    SAFE_RELEASE(_query);
}
//------------------------------------------------------------------------------------------------//
void OcclusionQuery::Begin(ID3D11DeviceContext* context)
{
    context->Begin(_query);
}
//------------------------------------------------------------------------------------------------//
void OcclusionQuery::End(ID3D11DeviceContext* context)
{
    context->End(_query);
}
//------------------------------------------------------------------------------------------------//
UINT64 OcclusionQuery::GetResult(ID3D11DeviceContext* context) const
{
    UINT64 result = 0;

    while (context->GetData(_query, &result, 8, 0) == S_FALSE)
    {
        ;
    }

    return result;
}
//------------------------------------------------------------------------------------------------//
}
//------------------------------------------------------------------------------------------------//