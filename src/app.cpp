#include "app.h"

#include "matrix.h"
#include <cmath>

std::shared_ptr<Scene> App::_scene = nullptr;
uint App::_width  = 512;
uint App::_height = 512;

void App::display()
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    _scene->render();
    glutSwapBuffers();
    glutPostRedisplay();
}

void App::load_new_scene(std::shared_ptr<Scene>& scene)
{
    _scene = scene;
}

void App::render()
{
    glutDisplayFunc(App::display);
    glutMainLoop();
}

void App::init_GL() {
    GLuint vao;
    glGenVertexArrays(1, &vao);
    glBindVertexArray(vao);

    glEnable(GL_DEPTH_TEST);
    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    glEnable(GL_CULL_FACE);
    glClearColor(0.4,0.4,0.4,1.0);
    glPatchParameteri(GL_PATCH_VERTICES, 4);
}

void App::init_glut(int &argc, char *argv[]) {
    //glewExperimental = GL_TRUE;
    glutInit(&argc, argv);
    glutInitContextVersion(4,5);
    glutInitContextProfile(GLUT_CORE_PROFILE | GLUT_DEBUG);
    glutInitDisplayMode(GLUT_RGBA|GLUT_DOUBLE|GLUT_DEPTH);
    glutInitWindowSize(_width, _height);
    glutInitWindowPosition (100, 100);
    glutCreateWindow("POGLA Project");

    glEnable(GL_PROGRAM_POINT_SIZE);

    glutMouseFunc(Scene::on_mouse);
    glutMotionFunc(Scene::on_motion);
    glutMouseWheelFunc(Scene::on_wheel);
    glutKeyboardFunc(Scene::handleKeyboard);

    glutReshapeFunc(Scene::window_resize);
}

bool App::init_glew() {
    glewExperimental = GL_TRUE;
    if (glewInit()) {
        std::cerr << " Error while initializing glew";
        return false;
    }
    glGetError();
    return true;
}

void App::set_render_loop(void (*func)())
{
    glutDisplayFunc(func);
}

App::App(const uint& width, const uint& height)
{
    _width = width;
    _height = height;

    int argc = 1;
    char arg0[] = "./main";
    char *argv[] = { arg0, nullptr };
    init_glut(argc, argv);
    init_glew();
    init_GL();
    Scene::window_resize(_width, _height);
}

App App::init(const uint& width, const uint& height)
{
    App app(width, height);
    return app;
}
