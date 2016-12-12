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

#include "Framework/RenderWindow.h"
#include "Framework/Framebuffer.h"
#include "Framework/ConstantBuffer.h"
#include "Framework/IndexBuffer.h"
#include "Framework/VertexBuffer.h"
#include "Framework/VertexShader.h"
#include "Framework/PixelShader.h"
#include "Framework/RenderDepthStencil.h"
#include "Framework/RenderTexture.h"
#include "Framework/OcclusionQuery.h"
#include "Framework/Texture.h"
#include "Framework/Context.h"
#include "Framework/Mesh.h"
#include "Framework/Camera.h"

#include "Effects/GeometryBuffer.h"
#include "Effects/SSAO.h"

#include <strsafe.h>

#include <Initguid.h>
#include <dxgidebug.h>

namespace Utils
{
inline bool FormatArgs(char* result, size_t count, const char* format, va_list args)
{
    return SUCCEEDED(StringCchVPrintfA(result, count - 1, format, args)) != FALSE;
}

inline bool Format(char* result, size_t count, const char* format, ...)
{
    va_list args;
    va_start(args, format);
    return FormatArgs(result, count, format, args);
}

inline bool FormatArgs(wchar_t* result, size_t count, const wchar_t* format, va_list args)
{
    return SUCCEEDED(StringCchVPrintfW(result, count - 1, format, args));
}

inline bool Format(wchar_t* result, size_t count, const wchar_t* format, ...)
{
    va_list args;
    va_start(args, format);
    return FormatArgs(result, count, format, args);
}
}

INT WINAPI wWinMain(HINSTANCE, HINSTANCE, LPWSTR lpszCmdLine, INT)
{
    IDXGIFactory1* dxgiFactory = nullptr;
    ID3D11Device* device = nullptr;
    ID3D11DeviceContext* context = nullptr;

    SAFE_CALL(CreateDXGIFactory1(__uuidof(IDXGIFactory1), reinterpret_cast<void**>(&dxgiFactory)));

    D3D_FEATURE_LEVEL featureLevel;

    static const D3D_FEATURE_LEVEL featureLevels[] =
    {
        D3D_FEATURE_LEVEL_11_0
    };

    UINT flags = 0;
#if defined(_DEBUG) || !defined(NDEBUG)
    flags |= D3D11_CREATE_DEVICE_DEBUG;
#endif

    SAFE_CALL(::D3D11CreateDevice(
                  nullptr, D3D_DRIVER_TYPE_HARDWARE, nullptr, flags, featureLevels, 1,
                  D3D11_SDK_VERSION, &device, &featureLevel, &context));

    const UINT viewport_Width = 1600;
    const UINT viewport_Height = 900;
    const UINT ssao_width = viewport_Width;     // divide by 4
    const UINT ssao_height = viewport_Height;   // ditto

    Prototype::RenderWindow window(dxgiFactory, device, viewport_Width, viewport_Height);

    window.SetVisible(true);

    Prototype::Context ctx(device, context);

    Prototype::Mesh mesh(device);
    mesh.LoadFromFile(lpszCmdLine);

    Effects::GeometryBuffer gbuffer(device, viewport_Width, viewport_Height);
    Effects::SSAO ssao(device);
    ssao.SetGeometryBuffer(&gbuffer);

    Prototype::Camera camera;
    camera.SetAspectRatio(static_cast<float>(viewport_Width) / static_cast<float>(viewport_Height));
    camera.UpdateProjection();
    camera.UpdateView();
    float angle = 0.0f;

    DWORD64 start, stop;
    DWORD64 total = 0;

    start = __rdtsc();
    ::Sleep(100);
    stop = __rdtsc();
    DWORD64 persecond = (stop - start) * 10;
    DWORD frames = 0;

    ssao.GetPublicParams().g_viewport_width = viewport_Width;
    ssao.GetPublicParams().g_viewport_height = viewport_Height;

    float last_frame = 0.016F;

    start = __rdtsc();

    while (window.IsClosed() == false)
    {
        #pragma region Interaction

        DirectX::XMVECTOR det;
        DirectX::XMStoreFloat4x4(&ssao.GetPublicParams().g_inverse_projection,
                                 DirectX::XMMatrixTranspose(DirectX::XMMatrixInverse(&det, camera.GetViewMatrix())));

        if (window.IsMouseRightButtonDown())
        {
            camera.Rotate(
                -0.02f * static_cast<float>(window.GetMouseDeltaX()),
                0.02f * static_cast<float>(window.GetMouseDeltaY())
            );
        }

        camera.Move(Prototype::CameraMovement_StrafeLeft,   (::GetAsyncKeyState('A') & 0x8000) != 0);
        camera.Move(Prototype::CameraMovement_StrafeRight,  (::GetAsyncKeyState('D') & 0x8000) != 0);
        camera.Move(Prototype::CameraMovement_Forward,      (::GetAsyncKeyState('W') & 0x8000) != 0);
        camera.Move(Prototype::CameraMovement_Backward,     (::GetAsyncKeyState('S') & 0x8000) != 0);
        camera.Move(Prototype::CameraMovement_Up,           (::GetAsyncKeyState('Q') & 0x8000) != 0);
        camera.Move(Prototype::CameraMovement_Down,         (::GetAsyncKeyState('E') & 0x8000) != 0);

        ssao.GetPublicParams().g_viewport_far = camera.GetFarZ();

        float speed = 1.0F;

        if (::GetAsyncKeyState(VK_LSHIFT) & 0x8000)
        {
            speed = 4.0f;
        }
        else if (::GetAsyncKeyState(VK_LCONTROL) & 0x8000)
        {
            speed = 2.0f;
        }

        if (::GetAsyncKeyState('1') & 0x8000)
        {
            ssao.GetPublicParams().g_render_mode = Effects::SSAO::RenderMode_Depth;
        }

        if (::GetAsyncKeyState('2') & 0x8000)
        {
            ssao.GetPublicParams().g_render_mode = Effects::SSAO::RenderMode_Normal;
        }

        if (::GetAsyncKeyState('3') & 0x8000)
        {
            ssao.GetPublicParams().g_render_mode = Effects::SSAO::RenderMode_Position;
        }

        if (::GetAsyncKeyState('4') & 0x8000)
        {
            ssao.GetPublicParams().g_render_mode = Effects::SSAO::RenderMode_SSAO;
        }

        if (::GetAsyncKeyState('5') & 0x8000)
        {
            ssao.GetPublicParams().g_render_mode = Effects::SSAO::RenderMode_X1;
        }

        if (::GetAsyncKeyState(VK_F1) & 0x8000)
        {
            ssao.GetPublicParams().g_base         += 0.01F;
        }

        if (::GetAsyncKeyState(VK_F2) & 0x8000)
        {
            ssao.GetPublicParams().g_base         -= 0.01F;
        }

        if (::GetAsyncKeyState(VK_F3) & 0x8000)
        {
            ssao.GetPublicParams().g_area         += 0.00001F;
        }

        if (::GetAsyncKeyState(VK_F4) & 0x8000)
        {
            ssao.GetPublicParams().g_area         -= 0.00001F;
        }

        if (::GetAsyncKeyState(VK_F5) & 0x8000)
        {
            ssao.GetPublicParams().g_intensity    += 0.01F;
        }

        if (::GetAsyncKeyState(VK_F6) & 0x8000)
        {
            ssao.GetPublicParams().g_intensity    -= 0.01F;
        }

        if (::GetAsyncKeyState(VK_F7) & 0x8000)
        {
            ssao.GetPublicParams().g_pixel_radius += 0.0001F;
        }

        if (::GetAsyncKeyState(VK_F8) & 0x8000)
        {
            ssao.GetPublicParams().g_pixel_radius -= 0.0001F;
        }

        if (::GetAsyncKeyState(VK_LEFT) & 0x8000)
        {
            camera.Rotate(-0.02F * speed, 0.0F);
        }

        if (::GetAsyncKeyState(VK_RIGHT) & 0x8000)
        {
            camera.Rotate(0.02F * speed, 0.0F);
        }

        if (::GetAsyncKeyState(VK_UP) & 0x8000)
        {
            camera.Rotate(0.0F, -0.02F * speed);
        }

        if (::GetAsyncKeyState(VK_DOWN) & 0x8000)
        {
            camera.Rotate(0.0F, 0.02F * speed);
        }

        camera.Update(last_frame);
        #pragma endregion

        window.ProcessMessage();

        ctx.BindViewport(static_cast<float>(viewport_Width), static_cast<float>(viewport_Height));

        gbuffer.SetProjectionMatrix(&ctx, camera.GetProjectionMatrix());
        gbuffer.SetWorldViewMatrix(&ctx, DirectX::XMMatrixRotationY(angle), camera.GetViewMatrix());

        gbuffer.Begin(&ctx);
        gbuffer.Render(&ctx, &mesh);
        gbuffer.End(&ctx);

        ctx.BindViewport(static_cast<float>(ssao_width), static_cast<float>(ssao_height));
        window.BeginRender(context);
        {
            ssao.Begin(&ctx);
            ssao.Render(&ctx);
            ssao.End(&ctx);
        }
        window.EndRender(context);

        ++frames;
        DWORD64 stop = __rdtsc();
        total += (stop - start);
        last_frame = static_cast<float>(stop - start) / static_cast<float>(persecond);
        start = stop;

        if (total > persecond)
        {
            total = 0;

            char buffer[256];
            Utils::Format(
                buffer, 256,
                "FPS %d (mesh triangles: %d) (B: %f, A: %f, I: %f, P: %f) RM: %d\n",
                frames, mesh.GetTriangleCount(),
                ssao.GetPublicParams().g_base,
                ssao.GetPublicParams().g_area,
                ssao.GetPublicParams().g_intensity,
                ssao.GetPublicParams().g_pixel_radius,
                ssao.GetPublicParams().g_render_mode);

            ::OutputDebugStringA(buffer);
            window.SetCaption(buffer);

            frames = 0;
        }

    }

#if 0
    IDXGIDebug* debug = nullptr;
    auto debug_handle = ::GetModuleHandleW(L"Dxgidebug.dll");
    typedef HRESULT (__stdcall * PDXGIGETDEBUGINTERFACE)(const IID&, void**);
    PDXGIGETDEBUGINTERFACE get_dbg_interface = reinterpret_cast<PDXGIGETDEBUGINTERFACE>
            (::GetProcAddress(debug_handle, "DXGIGetDebugInterface"));
    SAFE_CALL(get_dbg_interface(DXGI_DEBUG_D3D11, (void**)&debug));

    if (debug != nullptr)
    {
        debug->ReportLiveObjects(DXGI_DEBUG_D3D11, DXGI_DEBUG_RLO_ALL);
    }

    SAFE_RELEASE(debug);
#endif

    SAFE_RELEASE(context);
    SAFE_RELEASE(device);
    SAFE_RELEASE(dxgiFactory);
    return 0;
}