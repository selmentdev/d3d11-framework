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
#ifndef _Prototype_Camera_H__
#define _Prototype_Camera_H__
//------------------------------------------------------------------------------------------------//
#include "Headers.h"
//------------------------------------------------------------------------------------------------//
namespace Prototype
{
//------------------------------------------------------------------------------------------------//
enum CameraMovement : uint16_t
{
    CameraMovement_None                = 0,
    CameraMovement_Forward             = 1 << 0,
    CameraMovement_Backward            = 1 << 1,
    CameraMovement_StrafeLeft          = 1 << 2,
    CameraMovement_StrafeRight         = 1 << 3,
    CameraMovement_Up                  = 1 << 4,
    CameraMovement_Down                = 1 << 5,
};

enum ProjectionType : uint16_t
{
    ProjectionType_Perspective,
    ProjectionType_Orthogonal,
};

class Camera
{
public:
    Camera();

    DirectX::XMMATRIX GetViewMatrix() const
    {
        return _view;
    }

    DirectX::XMMATRIX GetProjectionMatrix() const
    {
        return _projection;
    }

    DirectX::XMVECTOR GetPosition() const
    {
        return _position;
    }

    DirectX::XMVECTOR GetTarget() const
    {
        return _target;
    }

    void SetTarget(DirectX::XMVECTOR target)
    {
        _target = target;
    }

    void SetPosition(DirectX::XMVECTOR position)
    {
        _position = position;
    }

    void SetAspectRatio(float ratio)
    {
        _aspect = ratio;
    }

    float GetFov() const
    {
        return _fov;
    }

    float GetAspect() const
    {
        return _aspect;
    }

    float GetNearZ() const
    {
        return _nearZ;
    }

    float GetFarZ() const
    {
        return _farZ;
    }

public:
    void UpdateProjection();

    void UpdateView();

    void Update(float delta);

    void Rotate(float delta_x, float delta_y);

    void Move(DirectX::XMVECTOR delta);

    void Move(CameraMovement movement, bool enable)
    {
        if (enable == true)
        {
            _movement |= movement;
        }
        else
        {
            _movement &= (~movement);
        }
    }

private:
    DirectX::XMMATRIX   _view;
    DirectX::XMMATRIX   _projection;
    DirectX::XMVECTOR   _position;
    DirectX::XMVECTOR   _target;
    DirectX::XMVECTOR   _direction;
    DirectX::XMVECTOR   _up;
    float               _fov;
    float               _aspect;
    float               _nearZ;
    float               _farZ;
    float               _rotation_x;
    float               _rotation_y;
    ProjectionType      _projectionType;
    uint32_t            _movement;
};
//------------------------------------------------------------------------------------------------//
}
//------------------------------------------------------------------------------------------------//
#endif /* _Prototype_Camera_H__ */