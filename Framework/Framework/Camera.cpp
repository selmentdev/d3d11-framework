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

#include "Camera.h"
//------------------------------------------------------------------------------------------------//
namespace Prototype
{
//------------------------------------------------------------------------------------------------//
Camera::Camera()
{
    _fov = 35.0f;
    _aspect = 1.0f;
    _nearZ = 0.01f;
    _farZ = 250.0f;

    _rotation_x = 0.0f;
    _rotation_y = 0.0f;

    _projectionType = ProjectionType_Perspective;

    _position   = DirectX::XMVectorZero();
    _target     = DirectX::XMVectorSet(1.0f, 0.0f, 0.0f, 0.0f);
    _up         = DirectX::XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f);
}
//------------------------------------------------------------------------------------------------//
void Camera::UpdateProjection()
{
    if (_projectionType == ProjectionType_Perspective)
    {
        _projection = DirectX::XMMatrixPerspectiveFovLH(
                          DirectX::XMConvertToRadians(_fov),
                          _aspect,
                          _nearZ,
                          _farZ);
    }
    else
    {
        _projection = DirectX::XMMatrixOrthographicLH(
                          _aspect, 1.0f, _nearZ, _farZ);
    }
}
//------------------------------------------------------------------------------------------------//
void Camera::UpdateView()
{
    _view = DirectX::XMMatrixLookAtLH(_position, _target, _up);
}
//------------------------------------------------------------------------------------------------//
void Camera::Update(float delta)
{
    DirectX::XMVECTOR direction = DirectX::XMVectorZero();

    if (_movement & CameraMovement_Backward)
    {
        direction = DirectX::XMVectorAdd(direction, DirectX::XMVectorSet(0.0f, 0.0f, 1.0f, 0.0f));
    }
    else if (_movement & CameraMovement_Forward)
    {
        direction = DirectX::XMVectorAdd(direction, DirectX::XMVectorSet(0.0f, 0.0f, -1.0f, 0.0f));
    }
    else if (_movement & CameraMovement_StrafeLeft)
    {
        direction = DirectX::XMVectorAdd(direction, DirectX::XMVectorSet(-1.0f, 0.0f, 0.0f, 0.0f));
    }
    else if (_movement & CameraMovement_StrafeRight)
    {
        direction = DirectX::XMVectorAdd(direction, DirectX::XMVectorSet(1.0f, 0.0f, 0.0f, 0.0f));
    }
    else if (_movement & CameraMovement_Up)
    {
        direction = DirectX::XMVectorAdd(direction, DirectX::XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f));
    }
    else if (_movement & CameraMovement_Down)
    {
        direction = DirectX::XMVectorAdd(direction, DirectX::XMVectorSet(0.0f, -1.0f, 0.0f, 0.0f));
    }

    if (DirectX::XMVector4Greater(DirectX::XMVector3LengthSq(direction), DirectX::XMVectorZero()))
    {
        direction = DirectX::XMVector3Normalize(direction);
        direction = DirectX::XMVectorScale(direction, 64.0F);
        direction = DirectX::XMVectorScale(direction, delta);
        Move(direction);
    }
}
//------------------------------------------------------------------------------------------------//
void Camera::Rotate(float delta_x, float delta_y)
{
    delta_x *= 0.03f;
    delta_y *= 0.03f;

    _rotation_x += delta_x;
    _rotation_y += delta_y;

    _rotation_x = DirectX::XMScalarModAngle(_rotation_x);
    _rotation_y = DirectX::XMScalarModAngle(_rotation_y);

    DirectX::XMVECTOR x = DirectX::XMVectorSet(0.0f, 0.0f, 1.0f, 1.0f);
    DirectX::XMVECTOR q = DirectX::XMQuaternionRotationRollPitchYaw(_rotation_y, _rotation_x, 0.0f);
    _direction = DirectX::XMVector3Rotate(x, q);

    _target = DirectX::XMVectorAdd(_position, _direction);

    UpdateView();
}
//------------------------------------------------------------------------------------------------//
void Camera::Move(DirectX::XMVECTOR delta)
{
    delta = DirectX::XMVectorScale(delta, -0.15f);
    DirectX::XMVECTOR q = DirectX::XMQuaternionRotationRollPitchYaw(_rotation_y, _rotation_x, 0.0f);

    DirectX::XMVECTOR x = DirectX::XMVectorSet(0.0f, 0.0f, 1.0f, 1.0f);
    DirectX::XMVECTOR direction = DirectX::XMVector3Rotate(x, q);       // look direction
    DirectX::XMVECTOR distance = DirectX::XMVector3Rotate(delta, q);    // new position update

    _position = DirectX::XMVectorAdd(_position, distance);
    _target = DirectX::XMVectorAdd(_position, direction);

    UpdateView();
}
//------------------------------------------------------------------------------------------------//
}
//------------------------------------------------------------------------------------------------//