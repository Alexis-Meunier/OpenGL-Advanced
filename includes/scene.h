#ifndef SCENE_H
#define SCENE_H

#include <memory>
#include <vector>

#include "material.h"
#include "matrix.h"
#include "object.h"
#include "camera.h"

class Scene {
    public:
        Scene() = default;
        static std::shared_ptr<Scene> default_scene();

        void render();
        void update_camera();

        void window_resize(int width, int height);
        void on_mouse(int button, int state, int x, int y);
        void on_motion(int x, int y);
        void on_wheel(int w, int d, int x, int y);
        void handle_keyboard(unsigned char c, int x, int y);

    private:
        std::vector<Object> _objects;
        std::vector<std::shared_ptr<Material>> _programs;

        Camera _camera;
        bool _dragging = false;
        float _lastX = 0.0f;
        float _lastY = 0.0f;
        bool _panning = false;
};

#endif // SCENE_H
