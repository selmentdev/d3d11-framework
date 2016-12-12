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

float gather_occlusion(float3 p, float3 n, float3 occ_p)
{
    float g_scale = g_base;
    float g_bias = g_area;

    float3 v = occ_p - p;
    float d = length(v);
    v /= d;
    d *= g_scale;

    return max(0.0F, dot(n, v) - g_bias) * (1.0F / (1.0F + d)) * g_intensity;
}

static float2 poissonDisk[16] =
{
    float2(-0.94201624F, -0.39906216F),
    float2( 0.94558609F, -0.76890725F),
    float2(-0.09418410F, -0.92938870F),
    float2( 0.34495938F,  0.29387760F),
    float2(-0.91588581F,  0.45771432F),
    float2(-0.81544232F, -0.87912464F),
    float2(-0.38277543F,  0.27676845F),
    float2( 0.97484398F,  0.75648379F),
    float2( 0.44323325F, -0.97511554F),
    float2( 0.53742981F, -0.47373420F),
    float2(-0.26496911F, -0.41893023F),
    float2( 0.79197514F,  0.19090188F),
    float2(-0.24188840F,  0.99706507F),
    float2(-0.81409955F,  0.91437590F),
    float2( 0.19984126F,  0.78641367F),
    float2( 0.14383161F, -0.14100790F)
};

float compute_ssao(float2 src_coord)
{
    float src_depth = sample_depth(src_coord);

    float3 src_position = sample_position(src_coord);
    float3 src_normal = sample_normal(src_coord);

    float occ_total = 0.0F;
    float samples = 16.0F;

    [loop]
    for (int i = 0; i < samples; ++i)
    {
        float2 offset = poissonDisk[i];

        float2 occ_coord = src_coord + offset * g_pixel_radius;
        float occ_depth = sample_depth(occ_coord);

        [branch]
        if (occ_depth < src_depth)
        {
            float3 occ_point = sample_position(occ_coord);
            occ_total += gather_occlusion(src_position, src_normal, occ_point);
        }
    }

    return 1.0F - (occ_total / samples);
}
