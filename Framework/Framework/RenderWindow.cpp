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

#include "RenderWindow.h"
//------------------------------------------------------------------------------------------------//
namespace Prototype
{
//------------------------------------------------------------------------------------------------//
LRESULT CALLBACK RenderWindow::WindowProcedure(::HWND handle, ::UINT message, ::WPARAM w,
        ::LPARAM l)
{
    RenderWindow* window = nullptr;

    if (message == WM_NCCREATE)
    {
        window = reinterpret_cast<RenderWindow*>(reinterpret_cast<LPCREATESTRUCTW>(l)->lpCreateParams);
        window->_handle = handle;
        SetWindowLongPtrW(handle, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(window));
    }
    else
    {
        window = reinterpret_cast<RenderWindow*>(GetWindowLongPtrW(handle, GWLP_USERDATA));
    }

    switch (message)
    {
    case WM_SIZE:
        {
            assert(window != nullptr);

            window->_clientWidth    = LOWORD(l);
            window->_clientHeight   = HIWORD(l);

            switch (w)
            {
            case SIZE_MINIMIZED:
                {
                    window->_active = false;
                    window->_minimized = true;
                    window->_maximized = false;
                    break;
                }

            case SIZE_MAXIMIZED:
                {
                    window->_active = true;
                    window->_minimized = false;
                    window->_maximized = true;
                    window->OnSize();
                    break;
                }

            case SIZE_RESTORED:
                {
                    if (window->_minimized)
                    {
                        window->_active = true;
                        window->_minimized = false;
                        window->OnSize();
                    }
                    else if (window->_maximized)
                    {
                        window->_active = true;
                        window->_maximized = false;
                        window->OnSize();
                    }
                    else if (window->_resizing)
                    {
                        ;
                    }
                    else
                    {
                        window->OnSize();
                    }

                    break;
                }
            }

            break;
        }

    case WM_ENTERSIZEMOVE:
        {
            window->_active = false;
            window->_resizing = true;
            return 0;
        }

    case WM_EXITSIZEMOVE:
        {
            window->_active = true;
            window->_resizing = false;
            window->OnSize();
            break;
        }

    case WM_LBUTTONDOWN:
        {
            window->_mouse_lbd = true;
            break;
        }

    case WM_LBUTTONUP:
        {
            window->_mouse_lbd = false;
            break;
        }

    case WM_MBUTTONDOWN:
        {
            window->_mouse_mbd = true;
            break;
        }

    case WM_MBUTTONUP:
        {
            window->_mouse_mbd = false;
            break;
        }

    case WM_RBUTTONDOWN:
        {
            window->_mouse_rbd = true;
            break;
        }

    case WM_RBUTTONUP:
        {
            window->_mouse_rbd = false;
            break;
        }

    case WM_MOUSEMOVE:
        {
            INT x = LOWORD(l);
            INT y = HIWORD(l);

            window->_mouse_delta_x = x - window->_mouse_position_x;
            window->_mouse_delta_y = y - window->_mouse_position_y;
            window->_mouse_position_x = x;
            window->_mouse_position_y = y;
            break;
        }

    case WM_MENUCHAR:
        {
            return MAKELRESULT(0, MNC_CLOSE);
        }

    case WM_GETMINMAXINFO:
        {
            ((MINMAXINFO*)l)->ptMinTrackSize.x = 320;
            ((MINMAXINFO*)l)->ptMinTrackSize.y = 200;
            return 0;
        }

    case WM_ACTIVATE:
        {
            assert(window != nullptr);

            bool active = (w != WA_INACTIVE);

            if (active != window->_active)
            {
                window->_active = active;
            }

            break;
        }

    case WM_CLOSE:
        {
            assert(window != nullptr);

            window->OnClose();
            window->_closed = true;
            ShowWindow(window->_handle, SW_HIDE);
            return 0;
        }
    }

    return DefWindowProcW(handle, message, w, l);
}
//------------------------------------------------------------------------------------------------//
RenderWindow::RenderWindow(IDXGIFactory1* factory, ID3D11Device* device, UINT width, UINT height)
    : _device(device)
    , _swapChain(nullptr)
    , _dxgiFactory(factory)
    , _mouse_delta_x(0)
    , _mouse_delta_y(0)
    , _mouse_position_x(0)
    , _mouse_position_y(0)
    , _mouse_lbd(false)
    , _mouse_mbd(false)
    , _mouse_rbd(false)
{
    Create(width, height);
    CreateResources();
}
//------------------------------------------------------------------------------------------------//
RenderWindow::~RenderWindow()
{
    DestroyResources();
    Destroy();

    SAFE_RELEASE(_swapChain);
}
//------------------------------------------------------------------------------------------------//
bool RenderWindow::TryRegisterWindowClass()
{
    ::WNDCLASSEXW wc;
    wc.cbSize = sizeof(::WNDCLASSEXW);

    ::HINSTANCE instance = ::GetModuleHandleW(nullptr);

    if (::GetClassInfoExW(instance, L"Ingene.RenderWindow", &wc) == FALSE)
    {
        wc.cbSize = sizeof(::WNDCLASSEXW);
        wc.style = CS_VREDRAW | CS_HREDRAW;
        wc.lpfnWndProc = &RenderWindow::WindowProcedure;
        wc.cbClsExtra = 0;
        wc.cbWndExtra = 0;
        wc.hInstance = instance;
        wc.hIcon = ::LoadIconW(0, IDI_APPLICATION);
        wc.hCursor = ::LoadCursorW(0, IDC_ARROW);
        wc.hbrBackground = static_cast<HBRUSH>(::GetStockObject(BLACK_BRUSH));
        wc.lpszMenuName = nullptr;
        wc.lpszClassName = L"Ingene.RenderWindow";
        wc.hIconSm = wc.hIcon;

        if (::RegisterClassExW(&wc) == 0)
        {
            // ::DWORD error = ::GetLastError();
            ///
            /// @todo
            ///     Fail / what? This error is terminal. Engine creates own render windows, and it's dll specific code
            ///     that can't be replaced in user code.

            return false;
        }
    }

    return true;
}
//------------------------------------------------------------------------------------------------//
bool RenderWindow::Create(UINT width, UINT height)
{
    assert(_handle == nullptr);

    if (TryRegisterWindowClass() == true)
    {
        // Get proper window style.
        //::DWORD dwStyle = WS_CAPTION | WS_MINIMIZEBOX | WS_SYSMENU | WS_OVERLAPPED | WS_MAXIMIZEBOX;
        ::DWORD dwStyle = WS_OVERLAPPEDWINDOW;
        ::DWORD dwExStyle = 0;

        _clientWidth = width;
        _clientHeight = height;

        ::RECT rect;
        rect.left = 0;
        rect.top = 0;
        rect.right = rect.left + _clientWidth;
        rect.bottom = rect.top + _clientHeight;

        ::AdjustWindowRectEx(&rect, dwStyle, FALSE, dwExStyle);

        HWND handle = CreateWindowExW(
                          dwExStyle,
                          L"Ingene.RenderWindow",
                          L"Ingene.RenderWindow",
                          dwStyle,
                          CW_USEDEFAULT,
                          CW_USEDEFAULT,
                          rect.right - rect.left,
                          rect.bottom - rect.top,
                          nullptr,
                          nullptr,
                          GetModuleHandleW(nullptr),
                          reinterpret_cast<LPVOID>(this));

        if (handle != nullptr)
        {
            _handle = handle;
            _closed = false;

            return true;
        }
    }

    return false;
}
//------------------------------------------------------------------------------------------------//
bool RenderWindow::Destroy()
{
    assert(_handle != nullptr);

    return ::DestroyWindow(_handle) != FALSE;
}
//------------------------------------------------------------------------------------------------//
void RenderWindow::SetVisible(bool value)
{
    if (value != FALSE)
    {
        ::ShowWindow(_handle, SW_SHOW);
        ::UpdateWindow(_handle);
    }
    else
    {
        ::ShowWindow(_handle, SW_HIDE);
    }
}
//------------------------------------------------------------------------------------------------//
bool RenderWindow::ProcessMessage()
{
    MSG msg;

    while (PeekMessage(&msg, _handle, 0, 0, PM_REMOVE) != FALSE)
    {
        ::DispatchMessage(&msg);
    }

    return true;
}
//------------------------------------------------------------------------------------------------//
bool RenderWindow::CreateResources()
{
    DXGI_SWAP_CHAIN_DESC scd = {0};
    scd.BufferCount             = 1;
    scd.BufferDesc.Format       = DXGI_FORMAT_R8G8B8A8_UNORM;
    scd.BufferDesc.Width        = _clientWidth;
    scd.BufferDesc.Height       = _clientHeight;
    scd.BufferUsage             = D3D11_BIND_RENDER_TARGET;
    scd.Flags                   = 0;
    scd.SampleDesc.Count        = 1;
    scd.SampleDesc.Quality      = 0;
    scd.SwapEffect              = DXGI_SWAP_EFFECT_DISCARD;
    scd.Windowed                = TRUE;
    scd.OutputWindow            = _handle;

    SAFE_CALL(_dxgiFactory->CreateSwapChain(_device, &scd, &_swapChain));
    SAFE_CALL(_dxgiFactory->MakeWindowAssociation(_handle, DXGI_MWA_NO_WINDOW_CHANGES));

    OnSize();
    return true;
}
//------------------------------------------------------------------------------------------------//
bool RenderWindow::DestroyResources()
{
    _swapChain->SetFullscreenState(FALSE, nullptr);

    SAFE_RELEASE(_rtv);
    SAFE_RELEASE(_dsv);
    SAFE_RELEASE(_dsb);

    SAFE_RELEASE(_swapChain);
    return true;
}
//------------------------------------------------------------------------------------------------//
void RenderWindow::OnSize()
{
    SAFE_RELEASE(_rtv);
    SAFE_RELEASE(_dsv);
    SAFE_RELEASE(_dsb);

    SAFE_CALL(_swapChain->ResizeBuffers(1, static_cast<UINT>(_clientWidth),
                                        static_cast<UINT>(_clientHeight), DXGI_FORMAT_R8G8B8A8_UNORM,
                                        DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH));

    ID3D11Texture2D* backBuffer = nullptr;
    SAFE_CALL(_swapChain->GetBuffer(0, __uuidof(ID3D11Texture2D),
                                    reinterpret_cast<void**>(&backBuffer)));
    SAFE_CALL(_device->CreateRenderTargetView(backBuffer, nullptr, &_rtv));
    SAFE_CALL(Prototype::SetDebugObjectName(_rtv, "RenderWindow.RTV"));
    SAFE_RELEASE(backBuffer);

    D3D11_TEXTURE2D_DESC dsbDesc;
    dsbDesc.Width               = _clientWidth;
    dsbDesc.Height              = _clientHeight;
    dsbDesc.MipLevels           = 1;
    dsbDesc.ArraySize           = 1;
    dsbDesc.Format              = DXGI_FORMAT_D24_UNORM_S8_UINT;
    dsbDesc.SampleDesc.Count    = 1;
    dsbDesc.SampleDesc.Quality  = 0;
    dsbDesc.Usage               = D3D11_USAGE_DEFAULT;
    dsbDesc.BindFlags           = D3D11_BIND_DEPTH_STENCIL;
    dsbDesc.CPUAccessFlags      = 0;
    dsbDesc.MiscFlags           = 0;

    SAFE_CALL(_device->CreateTexture2D(&dsbDesc, nullptr, &_dsb));
    SAFE_CALL(Prototype::SetDebugObjectName(_dsb, "RenderWindow.DSB"));
    SAFE_CALL(_device->CreateDepthStencilView(_dsb, nullptr, &_dsv));
    SAFE_CALL(Prototype::SetDebugObjectName(_dsv, "RenderWindow.DSV"));
}
//------------------------------------------------------------------------------------------------//
void RenderWindow::OnClose()
{
    _swapChain->SetFullscreenState(FALSE, nullptr);
}
//------------------------------------------------------------------------------------------------//
void RenderWindow::BeginRender(ID3D11DeviceContext* context)
{
    static const float color[4] = {0.0f, 0.0f, 0.0f, 0.0f};
    context->OMSetRenderTargets(1, &_rtv, _dsv);
    context->ClearDepthStencilView(_dsv, D3D11_CLEAR_DEPTH, 1.0f, 0x00);
    context->ClearRenderTargetView(_rtv, color);
}
//------------------------------------------------------------------------------------------------//
void RenderWindow::EndRender(ID3D11DeviceContext* context)
{
    (void)context;
    SAFE_CALL(_swapChain->Present(0, 0));
}
//------------------------------------------------------------------------------------------------//
}
//------------------------------------------------------------------------------------------------//