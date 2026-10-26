#include "camera.h"

#include <cmath>

void Camera::update_proj(int width, int height)
{
    const GLfloat fov = 60.0f * M_PI / 180.0f;
    const GLfloat z_near = 0.1f, z_far = 100.0f;
    GLfloat aspect = float(width) / float(height > 0 ? height : 1);
    GLfloat top = z_near * std::tan(fov / 2.0f);
    GLfloat right = top * aspect;
    _proj = frustum(-right, right, -top, top, z_near, z_far);
}

void Camera::update_camera() {
    float ex = dist * std::cos(pitch) * std::sin(yaw);
    float ey = dist * std::sin(pitch);
    float ez = dist * std::cos(pitch) * std::cos(yaw);
    _view  = look_at(ex, ey, ez, 0,0,0, 0,1,0);
}

matrix4& Camera::get_view_matrix()
{
    return _view;
}

matrix4& Camera::get_proj_matrix()
{
    return _proj;
}

Camera::Camera()
    : yaw(0.6f), pitch(0.5f), dist(2.5f)
{
    _pos = { 0.0f, 0.3f, 0.2f };

    _view = look_at(_pos.x, _pos.y, _pos.z,     // eye
                    0.f, 0.3f, 0.f,             // center
                    0.f, 1.f, 0.f);             // up

    _proj = matrix4::identity();
}

Camera::Camera(const float& yaw, const float& pitch, const float& dist)
    : yaw(yaw), pitch(pitch), dist(dist)
{
    _pos = { 0.0f, 0.3f, 0.2f };

    _view = look_at(_pos.x, _pos.y, _pos.z,     // eye
                    0.f, 0.3f, 0.f,             // center
                    0.f, 1.f, 0.f);             // up

    _proj = matrix4::identity();
}
