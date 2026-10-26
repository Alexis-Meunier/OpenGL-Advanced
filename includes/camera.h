#ifndef CAMERA_H
#define CAMERA_H

#include "matrix.h"

class Camera {
    public:
        Camera();
        Camera(const float& yaw, const float& pitch, const float& dist);

        void update_proj(int width, int height);
        void update_camera();

        matrix4& get_view_matrix();
        matrix4& get_proj_matrix();

        float yaw;
        float pitch;
        float dist;
    private:
        matrix4 _view;
        matrix4 _proj;
        vec3 _pos;
};

#endif // CAMERA_H
