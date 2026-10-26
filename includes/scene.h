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

        void render() const;

        static void window_resize(int width, int height);

        static void on_mouse(int button, int state, int x, int y);
        static void on_motion(int x, int y);
        static void on_wheel(int w, int d, int x, int y);
        static void handleKeyboard(unsigned char c, int x, int y);

    private:
        std::vector<Object> _objects;
        std::vector<std::shared_ptr<Material>> _programs;

        static Camera _camera;

        static bool _dragging;
        static float _lastX;
        static float _lastY;
};

#endif // SCENE_H
