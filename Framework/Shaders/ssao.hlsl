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

Texture2D texture_Depth         : register(t0);
Texture2D texture_Normal        : register(t1);
Texture2D texture_RandomNormal  : register(t2);

SamplerState sampler_Clamped    : register(s0);
SamplerState sampler_Wrapped    : register(s1);

#define RENDERMODE_DEPTH        0
#define RENDERMODE_NORMAL       1
#define RENDERMODE_POSITION     2
#define RENDERMODE_SSAO         3

#define ENABLE_RENDER_MODE      1

cbuffer Params : register(b2)
{
    float4x4    g_inverse_view;
    float       g_pixel_radius;
    float       g_viewport_far;
    float       g_base;
    float       g_area;
    float       g_intensity;
    int         g_render_mode;
    float       g_viewport_width;
    float       g_viewport_height;
};

struct VS_INPUT
{
    float4      Position        : POSITION;
    float2      TexCoord        : TEXCOORD;
};

struct VS_OUTPUT
{
    float4      Position        : SV_POSITION;
    float2      TexCoord        : TEXCOORD0;
};

VS_OUTPUT vs_main(const VS_INPUT input)
{
    VS_OUTPUT output = (VS_OUTPUT)0;

    output.Position     = input.Position;
    output.TexCoord     = input.TexCoord;
    return output;
}

float3 decode_normal(float3 value)
{
    return normalize((2.0F * value) - 1.0F);
}

float3 encode_normal(float3 value)
{
    return 0.5F * (value + 1.0F);
}

float linearize_depth(float depth, float near, float far)
{
    return (2.0F * near) / (far + near - depth * (far - near));
}

float sample_depth(float2 coord)
{
    return texture_Depth.SampleLevel(sampler_Clamped, coord, 0).x;
}

float3 sample_normal(float2 coord)
{
    return normalize(decode_normal(texture_Normal.SampleLevel(sampler_Clamped, coord, 0).xyz));
}

float3 sample_normal_world(float2 coord)
{
    float3 normal = sample_normal(coord);
    return mul(g_inverse_view, normal);
}

float3 sample_random_normal(float2 coord)
{
    return normalize(decode_normal(texture_RandomNormal.SampleLevel(sampler_Wrapped, coord * 14.173, 0).xyz));
}

static float3 g_sample_sphere[16] =
{
    float3( 0.5381F,  0.1856F, -0.4319F), float3( 0.1379F,  0.2486F,  0.4430F),
    float3( 0.3371F,  0.5679F, -0.0057F), float3(-0.6999F, -0.0451F, -0.0019F),
    float3( 0.0689F, -0.1598F, -0.8547F), float3(-0.4776F,  0.2847F, -0.0271F),
    float3(-0.0146F,  0.1402F,  0.0762F), float3( 0.0100F, -0.1924F, -0.0344F),
    float3(-0.3169F,  0.1063F,  0.0158F), float3(-0.3577F, -0.5301F, -0.4358F),
    float3( 0.0103F, -0.5869F,  0.0046F), float3(-0.0897F, -0.4940F,  0.3287F),
    float3( 0.7119F, -0.0154F, -0.0918F), float3(-0.0533F,  0.0596F, -0.5411F),
    float3( 0.0352F, -0.0631F,  0.5460F), float3( 0.0560F,  0.0069F, -0.1843F),
};


float compute_ssao_depth(float2 coord)
{
    const float falloff = 0.000001F;
    float3 random = normalize(sample_random_normal(coord).rgb);
    float depth = sample_depth(coord).r;
    float3 position = float3(coord.x, coord.y, depth);
    float3 normal = sample_normal(coord);

    float radius_depth = g_pixel_radius / depth;
    float occlusion = 0.0F;

    [loop]
    for (int i = 0; i < 16; ++i)
    {
        float3 ray = radius_depth * reflect(g_sample_sphere[i], random);
        float3 hemi_ray = position + sign(dot(ray, normal)) * ray;

        float occ_depth = sample_depth(saturate(hemi_ray.xy)).r;
        float difference = depth - occ_depth;

        occlusion += step(falloff, difference) * (1.0F - smoothstep(falloff, g_area, difference));
    }

    float ao = 1.0F - g_intensity * occlusion * (1.0F / 16.0F);
    return saturate(ao + g_base);
}

float4 SSAO(const in float2 coord)
{
    float value = compute_ssao_depth(coord);

    return float4(value, value, value, 1.0F);
}

float4 ps_main(const VS_OUTPUT input) : SV_TARGET
{
#if ENABLE_RENDER_MODE

    if (g_render_mode == RENDERMODE_DEPTH)
    {
        float depth = sample_depth(input.TexCoord);
        return float4(linearize_depth(depth, 0.001, g_viewport_far).rrr, 1.0F);
    }
    else if (g_render_mode == RENDERMODE_NORMAL)
    {
        return float4(encode_normal(sample_normal(input.TexCoord)), 0.0F);
    }
    else if (g_render_mode == RENDERMODE_POSITION)
    {
        return sample_random_normal(input.TexCoord).xyzz;
    }
    else if (g_render_mode == RENDERMODE_SSAO)
    {
        return float4(compute_ssao_depth(input.TexCoord).xxx, 1.0F);
    }

    return SSAO(input.TexCoord) * float4(encode_normal(sample_normal_world(input.TexCoord).rgb), 0.0F);
#else
    return SSAO(input.TexCoord);
#endif
}