#ifndef SCENE_H
#define SCENE_H

#include <map>
#include <memory>
#include <vector>

#include "skybox.h"
#include "light.h"
#include "material.h"
#include "matrix.h"
#include "object.h"
#include "camera.h"
#include "bloom.h"

struct bloomMIP
{
    float size[2];
    int isize[2];
    GLuint texture_id;
};

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
        void animate(float dt);
        void begin_frame();
        void render_light_bloom();

        void increment_outer_divisions();
        void increment_inner_divisions();
        void decrement_outer_divisions();
        void decrement_inner_divisions();

        void add_light(const Light& light);

        void set_time_of_day(float hours);
        void advance_time(float dh);
        void add_bunny(const vec3& translation = vec3(0.0), const vec3& scale = vec3(0.0));
        void add_tesselation_wave(const vec3& translation = vec3(0.0), const float& scale = 1.0f);
        void add_skybox();
        void add_terrain(const vec3& translation, const float& scale);
        void add_grass(const vec3& terrain_origin, const float& terrain_size);

        void add_glowing_light(bool cube, float x, float z, float hover, float size,
                              const vec3& color, float glow, float light_intensity);
        float terrain_height(float x, float z);

    private:
        float _anim_time = 0.f;
        std::vector<Object> _objects;
        std::vector<std::shared_ptr<Material>> _programs;

        Camera _camera;
        bool _dragging = false;
        float _lastX = 0.0f;
        float _lastY = 0.0f;
        bool _panning = false;
        int _nb_divisions = 2;
        int _middle_divisions = 2;
        std::vector<Light> _lights;
        float _time_of_day = 12.f;
        size_t _sun_idx = 0;

        std::shared_ptr<Skybox> _skybox;
        GLuint _heightmap_id;

        Bloom _bloom;
        bool display_bloom = true;

        std::vector<float> _height_cpu;
        float _terrain_size;
};

#endif // SCENE_H
