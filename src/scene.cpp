#include "scene.h"

#include <algorithm>
#include <cmath>

#include "bunny.h"
#include "texture.h"
#include "primitives.h"

static const float TERRAIN_AMP = 18.0f;
static const int WAVE_RES = 1024;

static GLuint make_wave_texture(int size)
{
    GLuint id;
    glGenTextures(1, &id);
    glBindTexture(GL_TEXTURE_2D, id);
    glTexStorage2D(GL_TEXTURE_2D, 1, GL_RGBA32F, size, size);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    return id;
}

void Scene::update_camera()
{
    _camera.update_camera();
}

void Scene::window_resize(int width, int height) {
    glViewport(0, 0, width, height);
    _camera.update_proj(width, height);
    _bloom.resize(width, height);
}

void Scene::begin_frame()
{
    if (display_bloom)
        _bloom.bind_scene();
    else
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

void Scene::render_light_bloom()
{
    if (display_bloom)
        _bloom.apply();
}

void Scene::on_mouse(int button, int state, int x, int y)
{
    if (button == GLUT_RIGHT_BUTTON) _dragging = (state == GLUT_DOWN);
    if (button == GLUT_LEFT_BUTTON)  _panning  = (state == GLUT_DOWN);
    _lastX = x; _lastY = y;
    glutPostRedisplay();
}

#define MAX_LIGHTS 16

void Scene::on_motion(int x, int y)
{
    if (_dragging)
    {
        _camera.yaw   -= (x - _lastX) * 0.005f;
        _camera.pitch += (y - _lastY) * 0.005f;
        if (_camera.pitch > 1.5f)  _camera.pitch = 1.5f;
        if (_camera.pitch < 0.05f) _camera.pitch = 0.05f;
        _camera.update_camera();
    }
    else if (_panning)
    {
        _camera.pan(x - _lastX, y - _lastY);
    }
    else return;

    _lastX = x; _lastY = y;
    glutPostRedisplay();
}

void Scene::on_wheel(int w, int d, int x, int y)
{
    (void)w; (void)x; (void)y;
    _camera.dist -= d * 0.2f;
    if (_camera.dist < 0.5f)  _camera.dist = 0.5f;
    if (_camera.dist > 20.0f) _camera.dist = 20.0f;

    _camera.update_camera();
    glutPostRedisplay();
}

void Scene::handle_keyboard(unsigned char c, int x, int y)
{
    (void)x; (void)y;
    switch (c)
    {
        case 'w': _target_speed = std::min(_target_speed + 0.5f, 12.f); break; // stronger wind
        case 's': _target_speed = std::max(_target_speed - 0.5f, 0.f);  break; // weaker wind
        case 'a': _target_angle -= 0.3f; break;                                // rotate wind
        case 'd': _target_angle += 0.3f; break;
        case '7':
            increment_inner_divisions();
            break;
        case '8':
            decrement_inner_divisions();
            break;
        case '4':
            increment_outer_divisions();
            break;
        case '5':
            decrement_outer_divisions();
            break;
        case '+': advance_time(0.25f); break;
        case '-': advance_time(-0.25f); break;
        case 'b':
            display_bloom = !display_bloom;
            break;
        case 27: exit(0);
    }
    
    glutPostRedisplay();
}

void Scene::animate(float dt)
{
    _anim_time += dt;
    wave_movement(dt);
    glutPostRedisplay();
}

void Scene::add_light(const Light& light)
{
    _lights.push_back(light);
    if (_lights.size() > MAX_LIGHTS)
    {
        _lights.erase(_lights.begin() + 1);
    }
}

void Scene::render()
{
    if (_skybox)
    {
        const Light& sun = _lights[_sun_idx];
        _skybox->render(_camera.get_view_matrix(), _camera.get_proj_matrix(), sun.dir, sun.color);
    }

    for (const auto& obj : _objects)
        obj.render(_camera.get_view_matrix(), _camera.get_proj_matrix(), _camera.get_pos(), _anim_time, _nb_divisions, _middle_divisions, _lights);
}

void Scene::increment_outer_divisions()
{
    if (_nb_divisions < 60)
        _nb_divisions++;
}

void Scene::increment_inner_divisions()
{
    if (_middle_divisions < 60)
    _middle_divisions++;
}

void Scene::decrement_outer_divisions()
{
    if (_nb_divisions > 1)
        _nb_divisions--;
}

void Scene::decrement_inner_divisions()
{
    if (_middle_divisions > 1)
        _middle_divisions--;
}

void Scene::set_time_of_day(float hours)
{
    Light& sun = _lights[_sun_idx];
    _time_of_day = std::fmod(hours + 24.f, 24.f);

    float a = (_time_of_day - 6.f) / 12.f * 3.14159265f;
    vec3 d = normalize(vec3(std::cos(a), std::sin(a), 0.3f));

    float h = std::max(d.y, 0.f);
    float t = h / 0.6f;
    t = std::min(t, 1.f);

    t = t * t * (3.f - 2.f * t);
    sun.dir = d;
    sun.color = vec3(1.0f,
                    0.30f + 0.66f * t,
                    0.08f + 0.82f * t);

    float f = std::min(h / 0.15f, 1.f);
    sun.intensity = 1.2f * f * f * (3.f - 2.f * f);
}

void Scene::advance_time(float dh)
{
    set_time_of_day(_time_of_day + dh);
}

void Scene::wave_movement(float dt)
{
    const float PI = 3.14159265f;

    // --- wind: ease toward the target so nothing snaps ---
    const float s = 1.f - std::exp(-dt / 3.f);
    _wind_angle += (_target_angle - _wind_angle) * s;
    _wind_speed += (_target_speed - _wind_speed) * s;
    _wind_phase += _wind_speed * 1.8f * dt;

    const vec3 wind_dir(std::cos(_wind_angle), 0.f, std::sin(_wind_angle));

    // --- grass reads the same wind ---
    for (auto& m : _grass_mats)
    {
        m->set_vec3("wind_dir", wind_dir);
        m->set_float("wind_speed", _wind_speed);
        m->set_float("wind_phase", _wind_phase);
    }

    if (!_wave_compute)
        return;

    // --- wave components derived from the wind speed ---
    static const float RATIO[N_WAVES]  = {1.0f, 0.72f, 0.5f, 0.35f, 0.24f, 0.16f, 0.11f, 0.07f};  // wavelength / peak
    static const float SPREAD[N_WAVES] = {0.0f, 0.45f, -0.55f, 0.9f, -0.95f, 0.3f, -0.25f, 0.7f};  // radians off the wind
    static const float WEIGHT[N_WAVES] = {1.0f, 0.8f, 0.7f, 0.5f, 0.4f, 0.25f, 0.15f, 0.08f};

    const float U = _wind_speed;
    const float lambda_peak = std::min(std::max(0.83f * U * U, 2.f), _wave_size * 0.5f);
    const float Hs = 0.0214f * U * U; // significant wave height

    float raw[N_WAVES], k[N_WAVES], sumsq = 0.f;
    for (int i = 0; i < N_WAVES; i++)
    {
        float c = std::cos(SPREAD[i]);
        raw[i] = WEIGHT[i] * c * c;            // waves far from the wind direction are weaker
        sumsq += raw[i] * raw[i];
        float lambda = std::max(lambda_peak * RATIO[i], 0.4f);
        k[i] = 2.f * PI / lambda;
    }
    const float scale = (Hs / 2.83f) / std::sqrt(sumsq); // so the total height matches Hs

    for (int i = 0; i < N_WAVES; i++)
    {
        float amp = std::min(raw[i] * scale, 0.4f / k[i]); // keep slopes below ~0.4
        _wave_phase[i] = std::fmod(_wave_phase[i] + std::sqrt(9.81f * k[i]) * dt, 2.f * PI);

        float ang = _wind_angle + SPREAD[i];
        std::string n = std::to_string(i);
        _wave_compute->set_vec3("wave_dk[" + n + "]", vec3(std::cos(ang), std::sin(ang), k[i]));
        _wave_compute->set_vec3("wave_ap[" + n + "]", vec3(amp, _wave_phase[i], 0.f));
    }
    _wave_compute->set_vec3("wind_dir", wind_dir);
    _wave_compute->set_float("anim_time", _anim_time);

    // --- run the compute pass: one thread per texel ---
    _wave_compute->bind();
    glBindImageTexture(2, _wave_tex, 0, GL_FALSE, 0, GL_WRITE_ONLY, GL_RGBA32F);
    glDispatchCompute((WAVE_RES + 7) / 8, (WAVE_RES + 7) / 8, 1);
    glMemoryBarrier(GL_TEXTURE_FETCH_BARRIER_BIT);
}

void Scene::add_skybox()
{
    _skybox = std::make_shared<Skybox>();
}

void Scene::add_bunny(const vec3& translation, const vec3& scale)
{
    auto base_material = std::make_shared<Material>(Pipeline{"bunny/vertex.shd", "", "", "", "bunny/fragment.shd", ""});

    Object rabbit(bunny_vertices, bunny_normals, base_material);
    rabbit.translate(translation);
    rabbit.scale(scale);

    _objects.push_back(rabbit);
    _programs.push_back(base_material);
}

std::vector<GLfloat> make_plane_normals(int n)
{
    // Height x Width x corners x vector_dim
    std::vector<GLfloat> v(n * n * 4 * 3, 0.f);
    for (size_t i = 1; i < v.size(); i += 3)
    {
        v[i] = 1.f; // Only set the middle to one because it's a plane
    }

    return v;
}

std::vector<GLfloat> make_plane(int n)
{
    std::vector<GLfloat> v;
    float s = 1.f / n;
    for (int j = 0; j < n; j++)
    {
        for (int i = 0; i < n; i++)
        {
            float x = -0.5f + i * s, z = -0.5f + j * s;
            float q[4][2] = { {x, z}, {x, z + s}, {x + s, z + s}, {x + s, z} };
            for (auto& c : q)
            {
                v.push_back(c[0]);
                v.push_back(0.f);
                v.push_back(c[1]);
            }
        }
    }

    return v;
}

void Scene::add_tesselation_wave(const vec3& translation, const float& scale)
{
    _wave_size = scale;
    _wave_tex = make_wave_texture(WAVE_RES);

    auto wave_mat = std::make_shared<Material>(Pipeline{"wave/vertex.shd", "wave/tess_control.shd", "wave/tess_evaluation.shd", "", "wave/fragment.shd", ""});
    wave_mat->add_texture("wave_tex", _wave_tex);
    wave_mat->set_float("plane_size", scale);

    Object wave(make_plane(scale), make_plane_normals(scale), wave_mat);
    wave.translate(translation);
    wave.scale(scale);

    // the compute material must be KEPT, otherwise it is destroyed when this function returns
    _wave_compute = std::make_shared<Material>(Pipeline{"", "", "", "", "", "wave/compute.shd"});
    _wave_compute->set_float("plane_size", scale);
    _wave_compute->set_int("nb_waves", N_WAVES);

    _objects.push_back(wave);
    _programs.push_back(wave_mat);
}

void Scene::add_terrain(const vec3& translation, const float& scale)
{
    _heightmap_id = Texture::generate_heightmap(1024).id;

    auto mat = std::make_shared<Material>(Pipeline{
        "terrain/vertex.shd", "terrain/tess_control.shd", "terrain/tess_evaluation.shd",
        "", "terrain/fragment.shd", ""});
    mat->add_texture("heightmap", _heightmap_id);
    mat->set_float("terrain_size", scale);
    mat->set_float("terrain_amp", TERRAIN_AMP);

    Object terrain(make_plane(scale), make_plane_normals(scale), mat);
    terrain.translate(translation);
    terrain.scale(scale);

    _objects.push_back(terrain);
    _programs.push_back(mat);
    _terrain_size = scale;
}

static std::vector<GLfloat> make_grass_tile(int n)
{
    std::vector<GLfloat> v;
    v.reserve(n * n * 3);
    for (int j = 0; j < n; j++)
        for (int i = 0; i < n; i++)
        {
            v.push_back(float(i) / n);
            v.push_back(0.f);
            v.push_back(float(j) / n);
        }
    return v;
}
 
void Scene::add_grass(const vec3& terrain_origin, const float& terrain_size)
{
    auto layer = [&](int cells, int grid, float tile,
                    float in_start, float in_end, // fade IN  (blades appear)
                    float out_start, float out_end, // fade OUT (blades thin out, then vanish)
                    float width_scale, float height_scale)
    {
        auto mat = std::make_shared<Material>(Pipeline{
            "grass/vertex.shd", "", "", "grass/geometry.shd", "grass/fragment.shd", ""});
        mat->draw_type = GL_POINTS;
        mat->instance_count = grid * grid;
        mat->cull_face = false;
        mat->add_texture("heightmap", _heightmap_id);
 
        mat->set_float("terrain_size", terrain_size);
        mat->set_float("terrain_amp", TERRAIN_AMP);
        mat->set_vec3("terrain_origin", terrain_origin);
 
        mat->set_int("grass_grid", grid);
        mat->set_float("grass_tile", tile);
        mat->set_float("grass_cells", cells);
 
        mat->set_float("fade_in_start", in_start);
        mat->set_float("fade_in_end", in_end);
        mat->set_float("fade_start", out_start);
        mat->set_float("fade_end", out_end);
        mat->set_float("width_scale", width_scale);
        mat->set_float("height_scale", height_scale);
 
        Object o(make_grass_tile(cells), std::vector<GLfloat>(cells * cells * 3, 0.f), mat);
        _objects.push_back(o);
        _programs.push_back(mat);
        _grass_mats.push_back(mat);
    };

    layer(40, 9, 4.0f, -1.f, 0.f, 5.f, 14.f, 1.0f, 1.0f); // near: dense, thin blades
    layer(40, 5, 16.0f, 8.f, 14.f, 20.f, 30.f, 2.5f, 1.8f); // far: sparse, big tufts
}

float Scene::terrain_height(float x, float z)
{
    const int n = 1024;
    if (_height_cpu.empty())
    {
        _height_cpu.resize(n * n);
        glMemoryBarrier(GL_ALL_BARRIER_BITS);
        glGetTextureImage(_heightmap_id, 0, GL_RED, GL_FLOAT,
                          n * n * sizeof(float), _height_cpu.data());
    }

    float u = std::min(std::max(x / _terrain_size + 0.5f, 0.f), 0.9999f);
    float v = std::min(std::max(z / _terrain_size + 0.5f, 0.f), 0.9999f);
    return _height_cpu[int(v * n) * n + int(u * n)] * TERRAIN_AMP;
}

void Scene::add_glowing_light(bool cube, float x, float z, float hover, float size,
                              const vec3& color, float glow, float light_intensity)
{
    float ground = std::max(terrain_height(x, z), 2.0f);
    vec3 pos(x, ground + hover, z);

    auto mat = std::make_shared<Material>(Pipeline{
        "emissive/vertex.shd", "", "", "", "emissive/fragment.shd", ""});
    mat->set_vec3("emissive_color", color);
    mat->set_float("emissive_intensity", glow);

    std::vector<GLfloat> verts, normals;
    if (cube)
        make_cube_mesh(verts, normals);
    else
        make_sphere_mesh(verts, normals, 16, 32);

    Object o(verts, normals, mat);
    o.translate(pos);
    o.scale(vec3(size, size, size));
    _objects.push_back(o);
    _programs.push_back(mat);

    add_light(Light(pos, color, light_intensity));
}

std::shared_ptr<Scene> Scene::default_scene()
{
    auto scene = std::make_shared<Scene>();

    scene->add_skybox();
    scene->add_terrain({0.0}, 50.0);
    // scene->add_bunny({0.0}, {0.2});
    scene->add_tesselation_wave({0.0, 2.0, 0.0}, 50.0);
    scene->add_grass({0.0}, 50.0);

    scene->_sun_idx = scene->_lights.size();
    scene->_lights.push_back(Light::directional(vec3(0.f, 1.f, 0.f), vec3(1.f, 1.f, 1.f), 1.f));
    scene->set_time_of_day(10.f);
    scene->add_glowing_light(false, 5.2f, 7.7f, 1.5f, 0.8f, vec3(1.0f, 0.45f, 0.1f), 3.f, 3.f);
    scene->add_glowing_light(true, -6.f, 8.2f, 1.0f, 0.9f, vec3(0.2f, 0.70f, 1.0f), 3.f, 6.f);
    scene->add_glowing_light(false, 0.f, -6.f, 2.0f, 0.5f, vec3(1.0f, 0.20f, 0.7f), 3.f, 2.f);
    scene->wave_movement(0.f);

    return scene;
}
