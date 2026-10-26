#ifndef APP_H
#define APP_H

#include <memory>
#include <vector>

#include "camera.h"
#include "program.h"
#include "scene.h"
#include "shader.h"

class App {
    public:
        App() = delete;
        static App init(const uint& width = 512, const uint& height = 512);

        void set_render_loop(void (*func)());

        void load_new_scene(std::shared_ptr<Scene>& scene);

        void render();
        static void display();

    private:
        App(const uint& width = 512, const uint& height = 512);
        void init_glut(int &argc, char *argv[]);
        bool init_glew();
        void init_GL();

        static std::shared_ptr<Scene> _scene;

        static uint _width;
        static uint _height;
        std::vector<Program> _programs;
};

#endif // APP_H
