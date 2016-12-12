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

#include "Mesh.h"
#include <string>
#include <iostream>
#include <vector>
#include <fstream>
#include <sstream>
#include <algorithm>
#include <iterator>
#include <cassert>
#include <DirectXMath.h>
//------------------------------------------------------------------------------------------------//
namespace Prototype
{
//------------------------------------------------------------------------------------------------//
Mesh::Mesh(ID3D11Device* device)
    : _ib(nullptr)
    , _vb(nullptr)
    , _device(device)
    , _triangle_count(0)
{
}
//------------------------------------------------------------------------------------------------//
Mesh::~Mesh()
{
    if (_ib != nullptr)
    {
        delete _ib;
    }

    if (_vb != nullptr)
    {
        delete _vb;
    }
}
//------------------------------------------------------------------------------------------------//
void Mesh::LoadFromFile(const wchar_t* filename)
{
    // Zaladuj verteksy i indeksy do odpowiednich buforow (ib - indexy, vb - vertexy)
    // Podaj po prostu dane w konstruktorach buforow.
    // Pozniej uzupelnij _rop_mesh
    std::string line;
    std::ifstream myfile(filename);

    std::vector<DirectX::XMFLOAT4> positions;
    std::vector<Mesh::Vertex> vertices;
    std::vector<INT32> indices;

    if (myfile.is_open())
    {
        while ( myfile.good() )
        {
            std::getline(myfile, line);

            if (line.length() > 3)
            {
                switch (line[0])
                {
                case 'v':
                    {
                        DirectX::XMFLOAT4 position;
                        sscanf_s(line.c_str(), "%*s %f %f %f", &position.x, &position.y, &position.z);

                        position.w = 1.0f;

                        positions.push_back(position);

                        break;
                    }

                case 'f':
                    {
                        ++_triangle_count;
                        int x1 = -1;
                        int x2 = -1;
                        int x3 = -1;

                        sscanf_s(line.c_str(), "f %d %d %d", &x1, &x2, &x3);

                        DirectX::XMVECTOR p0 = DirectX::XMLoadFloat4(&positions[x1 - 1]);
                        DirectX::XMVECTOR p1 = DirectX::XMLoadFloat4(&positions[x2 - 1]);
                        DirectX::XMVECTOR p2 = DirectX::XMLoadFloat4(&positions[x3 - 1]);

                        DirectX::XMVECTOR plane = DirectX::XMPlaneFromPoints(p0, p1, p2);
                        plane = DirectX::XMPlaneNormalize(plane);
                        plane = DirectX::XMVectorSetW(plane, 1.0f);
                        INT32 indexBase = static_cast<INT32>(vertices.size());

                        DirectX::XMFLOAT3 fnormal;
                        plane = DirectX::XMVector3Normalize(plane);
                        DirectX::XMStoreFloat3(&fnormal, plane);
                        fnormal.y = -fnormal.y;

                        //fnormal.x = -fnormal.x;
                        //fnormal.y = -fnormal.y;
                        fnormal.z = -fnormal.z;
                        plane = DirectX::XMLoadFloat3(&fnormal);


                        Mesh::Vertex v0;
                        Mesh::Vertex v1;
                        Mesh::Vertex v2;

                        DirectX::XMStoreFloat4(&v0.Position, p0);
                        DirectX::XMStoreFloat4(&v1.Position, p1);
                        DirectX::XMStoreFloat4(&v2.Position, p2);

                        DirectX::XMStoreFloat3(&v0.Normal, plane);
                        DirectX::XMStoreFloat3(&v1.Normal, plane);
                        DirectX::XMStoreFloat3(&v2.Normal, plane);

                        indices.push_back(indexBase++);
                        indices.push_back(indexBase++);
                        indices.push_back(indexBase++);

                        vertices.push_back(v0);
                        vertices.push_back(v1);
                        vertices.push_back(v2);

                        assert(x1 != -1);
                        assert(x2 != -1);
                        assert(x3 != -1);
                        break;
                    }
                }
            }
        }

        myfile.close();

        _ib = new IndexBuffer(_device, &indices[0], indices.size() * sizeof(INT32));
        _vb = new VertexBuffer(_device, &vertices[0], vertices.size() * sizeof(Mesh::Vertex));
    }

    _rop_mesh._ib               = _ib->GetBuffer();
    _rop_mesh._ib_count         = static_cast<UINT>(indices.size());
    _rop_mesh._ib_format        = DXGI_FORMAT_R32_UINT;
    _rop_mesh._topology         = D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST;
    _rop_mesh._vb               = _vb->GetBuffer();
    _rop_mesh._vb_stride        = sizeof(Mesh::Vertex);
}
//------------------------------------------------------------------------------------------------//
}
//------------------------------------------------------------------------------------------------//