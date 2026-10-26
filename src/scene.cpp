#include "scene.h"

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
        // case '7':
        //     middleDivisions1 += 1;
        //     div_location = glGetUniformLocation(program_id, "middle_divisions1");TEST_OPENGL_ERROR();
        //     glUniform1f(div_location, middleDivisions1);TEST_OPENGL_ERROR();
        //     break;
        // case '8':
        //     if (middleDivisions1 > 0)
        //       middleDivisions1 -= 1;
        //     div_location = glGetUniformLocation(program_id, "middle_divisions1");TEST_OPENGL_ERROR();
        //     glUniform1f(div_location, middleDivisions1);TEST_OPENGL_ERROR();
        //     break;
        // case '4':
        //     middleDivisions2 += 1;
        //     div_location = glGetUniformLocation(program_id, "middle_divisions2");TEST_OPENGL_ERROR();
        //     glUniform1f(div_location, middleDivisions2);TEST_OPENGL_ERROR();
        //     break;
        // case '5':
        //     if (middleDivisions2 > 0)
        //       middleDivisions2 -= 1;
        //     div_location = glGetUniformLocation(program_id, "middle_divisions2");TEST_OPENGL_ERROR();
        //     glUniform1f(div_location, middleDivisions2);TEST_OPENGL_ERROR();
        //     break;
      // if `escape` then close window
        case 27: exit(0);
    }
    glutPostRedisplay();
}

void Scene::render()
{
    for (const auto& obj : _objects)
        obj.render(_camera.get_view_matrix(), _camera.get_proj_matrix(), _camera.get_pos());
}

std::shared_ptr<Scene> Scene::default_scene()
{
    auto scene = std::make_shared<Scene>();
    auto base_material = std::make_shared<Material>("vertex.shd", "fragment.shd");
    Object rabbit(bunny_vertices, bunny_normals, base_material);
    scene->_objects.push_back(rabbit);
    scene->_programs.push_back(base_material);

    return scene;
}
