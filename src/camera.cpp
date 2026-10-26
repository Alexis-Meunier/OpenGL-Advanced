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

void Camera::update_camera()
{
    _pos.x = _target.x + dist * std::cos(pitch) * std::sin(yaw);
    _pos.y = _target.y + dist * std::sin(pitch);
    _pos.z = _target.z + dist * std::cos(pitch) * std::cos(yaw);
    _view = look_at(_pos.x, _pos.y, _pos.z,
                    _target.x, _target.y, _target.z,
                    0.f, 1.f, 0.f);
}

void Camera::pan(float dx, float dy)
{
    // camera right / up vectors, derived from yaw and pitch
    vec3 right = { std::cos(yaw), 0.f, -std::sin(yaw) };
    vec3 up    = { -std::sin(pitch) * std::sin(yaw),
                    std::cos(pitch),
                   -std::sin(pitch) * std::cos(yaw) };

    float k = dist * 0.001f;   // pan speed scales with zoom
    _target.x += (-dx * right.x + dy * up.x) * k;
    _target.y += (-dx * right.y + dy * up.y) * k;
    _target.z += (-dx * right.z + dy * up.z) * k;
    update_camera();
}

matrix4& Camera::get_view_matrix()
{
    return _view;
}

matrix4& Camera::get_proj_matrix()
{
    return _proj;
}

Camera::Camera() : yaw(0.6f), pitch(0.5f), dist(2.5f)
{
    _target = { 0.f, 0.3f, 0.f };
    _proj = matrix4::identity();
    update_camera();
}
// same for the second constructor

Camera::Camera(const float& yaw, const float& pitch, const float& dist) : yaw(yaw), pitch(pitch), dist(dist)
{
    _target = { 0.f, 0.3f, 0.f };
    _proj = matrix4::identity();
    update_camera();
}
