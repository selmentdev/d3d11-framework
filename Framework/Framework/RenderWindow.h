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
#ifndef _Prototype_RenderWindow_H__
#define _Prototype_RenderWindow_H__
//------------------------------------------------------------------------------------------------//
#include "Headers.h"
//------------------------------------------------------------------------------------------------//
namespace Prototype
{
//------------------------------------------------------------------------------------------------//
class RenderWindow
{
public:
    RenderWindow(IDXGIFactory1* factory, ID3D11Device* device, UINT width, UINT height);
    virtual ~RenderWindow();

    void SetVisible(bool value);
    bool ProcessMessage();

    bool IsClosed() const
    {
        return _closed;
    }

    bool IsActive() const
    {
        return _active;
    }

    bool IsMouseLeftButtonDown() const
    {
        return _mouse_lbd;
    }

    bool IsMouseMiddleButtonDown() const
    {
        return _mouse_mbd;
    }

    bool IsMouseRightButtonDown() const
    {
        return _mouse_rbd;
    }

    INT GetMousePositionX() const
    {
        return _mouse_position_x;
    }

    INT GetMousePositionY() const
    {
        return _mouse_position_y;
    }

    INT GetMouseDeltaX() const
    {
        return _mouse_delta_x;
    }

    INT GetMouseDeltaY() const
    {
        return _mouse_delta_y;
    }

    ID3D11RenderTargetView* GetRenderTargetView() const
    {
        return _rtv;
    }

    ID3D11DepthStencilView* GetDepthStencilView() const
    {
        return _dsv;
    }

    void BeginRender(ID3D11DeviceContext* context);
    void EndRender(ID3D11DeviceContext* context);


    void SetCaption(const char* caption)
    {
        ::SetWindowTextA(_handle, caption);
    }
protected:
    ID3D11Device*           _device;
    IDXGISwapChain*         _swapChain;
    IDXGIFactory1*          _dxgiFactory;
    ID3D11RenderTargetView* _rtv;
    ID3D11DepthStencilView* _dsv;
    ID3D11Texture2D*        _dsb;

    ::HWND                  _handle;
    bool                    _active;
    bool                    _closed;
    bool                    _resizing;
    bool                    _minimized;
    bool                    _maximized;
    int                     _clientWidth;
    int                     _clientHeight;

    bool                    _mouse_lbd;
    bool                    _mouse_mbd;
    bool                    _mouse_rbd;

    INT                     _mouse_position_x;
    INT                     _mouse_position_y;
    INT                     _mouse_delta_x;
    INT                     _mouse_delta_y;

protected:
    bool TryRegisterWindowClass();
    bool Create(UINT width, UINT height);
    bool Destroy();
    bool CreateResources();
    bool DestroyResources();

    void OnSize();
    void OnClose();

protected:
    static LRESULT CALLBACK WindowProcedure(::HWND handle, ::UINT message, ::WPARAM w, ::LPARAM l);
};
//------------------------------------------------------------------------------------------------//
}
//------------------------------------------------------------------------------------------------//
#endif /* _Prototype_RenderWindow_H__ */