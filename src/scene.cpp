#include "scene.h"

#include <cmath>

#include "bunny.h"

void Scene::update_camera()
{
    _camera.update_camera();
}

void Scene::window_resize(int width, int height) {
    glViewport(0, 0, width, height);
    _camera.update_proj(width, height);
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
        // case 's': lightX += 0.2; lightY += 0.0; lightZ += 0.0; break;
        // case 'w': lightX -= 0.2; lightY -= 0.0; lightZ -= 0.0; break;
        // case 'd': lightX -= 0.0; lightY -= 0.0; lightZ -= 0.2; break;
        // case 'a': lightX += 0.0; lightY += 0.0; lightZ += 0.2; break;
        // case 'q': lightY += 0.2; break;
        // case 'e': lightY -= 0.2; break;
        // case '+':
        //     nbDivisions += 1;
        //     div_location = glGetUniformLocation(program_id, "nb_divisions");TEST_OPENGL_ERROR();
        //     glUniform1f(div_location, nbDivisions);TEST_OPENGL_ERROR();
        //     break;
        // case '-':
        //     if (nbDivisions > 0)
        //       nbDivisions -= 1;
        //     div_location = glGetUniformLocation(program_id, "nb_divisions");TEST_OPENGL_ERROR();
        //     glUniform1f(div_location, nbDivisions);TEST_OPENGL_ERROR();
        //     break;
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
        case 27: exit(0);
    }
    
    glutPostRedisplay();
}

void Scene::animate(float dt)
{
    _anim_time += dt;
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
    sun.color = vec3(1.0f + (1.0f - 1.0f) * t,
                    0.30f + 0.66f * t,
                    0.08f + 0.82f * t);

    float f = std::min(h / 0.15f, 1.f);
    sun.intensity = 1.2f * f * f * (3.f - 2.f * f);
}

void Scene::advance_time(float dh)
{
    set_time_of_day(_time_of_day + dh);
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
    std::vector<GLfloat> v(n * n * 4 * 3, 0.f);
    for (size_t i = 1; i < v.size(); i += 3) v[i] = 1.f;
    return v;
}

std::vector<GLfloat> make_plane(int n)
{
    std::vector<GLfloat> v;
    float s = 1.f / n;
    for (int j = 0; j < n; j++)
        for (int i = 0; i < n; i++)
        {
            float x = -0.5f + i * s, z = -0.5f + j * s;
            float q[4][2] = { {x, z}, {x, z + s}, {x + s, z + s}, {x + s, z} };
            for (auto& c : q) { v.push_back(c[0]); v.push_back(0.f); v.push_back(c[1]); }
        }
    return v;
}

void Scene::add_tesselation_wave(const vec3& translation, const float& scale)
{
    auto tess_material = std::make_shared<Material>(Pipeline{"wave/vertex.shd", "wave/tess_control.shd", "wave/tess_evaluation.shd", "", "wave/fragment.shd", ""});

    Object wave(make_plane(scale), make_plane_normals(scale), tess_material);
    wave.translate(translation);
    wave.scale(scale);

    _objects.push_back(wave);
    _programs.push_back(tess_material);
}

std::shared_ptr<Scene> Scene::default_scene()
{
    auto scene = std::make_shared<Scene>();

    scene->add_bunny({0.0}, {0.2});
    scene->add_tesselation_wave({0.0}, 10.0);

    scene->_sun_idx = scene->_lights.size();
    scene->_lights.push_back(Light::directional(vec3(0.f, 1.f, 0.f), vec3(1.f, 1.f, 1.f), 1.f));
    scene->set_time_of_day(10.f);

    return scene;
}
