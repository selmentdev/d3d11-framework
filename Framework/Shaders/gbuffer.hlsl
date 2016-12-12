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

cbuffer CbView : register(b0)
{
    float4x4    Matrix_Projection;
};

cbuffer CbObject : register(b1)
{
    float4x4    Matrix_World;
    float4x4    Matrix_View;
};

struct VS_INPUT
{
    float4      Position        : POSITION;
    float3      Normal          : NORMAL;
};

struct VS_OUTPUT
{
    float4      Position        : SV_POSITION;
    float3      Normal          : NORMAL;
};

float3 decode_normal(float3 value)
{
    return (2.0F * value) - 1.0F;
}

float3 encode_normal(float3 value)
{
    return 0.5F * (value + 1.0F);
}

VS_OUTPUT vs_main(const VS_INPUT input)
{
    VS_OUTPUT output = (VS_OUTPUT)0;

    float4x4 W = Matrix_World;
    float4x4 WV = mul(Matrix_World, Matrix_View);
    float4x4 WVP = mul(WV, Matrix_Projection);

    output.Position = mul(input.Position, WVP);
#if 1
    float3x3 N = (float3x3)WV;
    output.Normal = mul(input.Normal, transpose(N));
#else
    // Remove translation by setting w = 0.0F
    output.Normal = mul(float4(input.Normal, 0.0F), WV);
#endif
    return output;
}

struct GBUFFER_OUTPUT
{
    float4      Normal          : SV_TARGET0;
};

GBUFFER_OUTPUT ps_main(const VS_OUTPUT input)
{
    GBUFFER_OUTPUT output = (GBUFFER_OUTPUT)0;

    output.Normal       = encode_normal(normalize(input.Normal)).xyzx;

    return output;
}