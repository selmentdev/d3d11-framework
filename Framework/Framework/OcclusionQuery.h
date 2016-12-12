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
#ifndef _Prototype_OcclusionQuery_H__
#define _Prototype_OcclusionQuery_H__
//------------------------------------------------------------------------------------------------//
#include "Headers.h"
//------------------------------------------------------------------------------------------------//
namespace Prototype
{
//------------------------------------------------------------------------------------------------//
class OcclusionQuery
{
public:
    OcclusionQuery(ID3D11Device* device);
    virtual ~OcclusionQuery();

    void Begin(ID3D11DeviceContext* context);
    void End(ID3D11DeviceContext* context);

    UINT64 GetResult(ID3D11DeviceContext* context) const;

private:
    ID3D11Device*       _device;
    ID3D11Query*        _query;
};
//------------------------------------------------------------------------------------------------//
}
//------------------------------------------------------------------------------------------------//
#endif /* _Prototype_OcclusionQuery_H__ */