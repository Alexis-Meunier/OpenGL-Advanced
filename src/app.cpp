#include "app.h"

#include "matrix.h"
#include <cmath>

static App* g_app = nullptr;
static Scene* g_scene = nullptr;

static void cb_display()  { g_app->display(); }
static void cb_resize(int w, int h)                       { g_scene->window_resize(w, h); }
static void cb_mouse(int b, int s, int x, int y)          { g_scene->on_mouse(b, s, x, y); }
static void cb_motion(int x, int y)                       { g_scene->on_motion(x, y); }
static void cb_wheel(int w, int d, int x, int y)          { g_scene->on_wheel(w, d, x, y); }
static void cb_keyboard(unsigned char c, int x, int y)    { g_scene->handle_keyboard(c, x, y); }
static void cb_timer(int)
{
    if (g_scene) g_scene->animate(0.033f);
    glutTimerFunc(33, cb_timer, 0);
}

void App::display()
{
    g_scene->begin_frame();
    g_scene->render();
    g_scene->render_light_bloom();
    glutSwapBuffers();
    glutPostRedisplay();
}


void App::load_new_scene(std::shared_ptr<Scene>& scene)
{
    _scene = scene;
    g_scene = _scene.get();
    g_scene->window_resize(_width, _height);
}

void App::render()
{
    g_app = this;
    g_scene->update_camera();
    glutDisplayFunc(cb_display);
    glutMouseFunc(cb_mouse);
    glutMotionFunc(cb_motion);
    glutMouseWheelFunc(cb_wheel);
    glutKeyboardFunc(cb_keyboard);
    glutReshapeFunc(cb_resize);
    glutTimerFunc(33, cb_timer, 0);
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
}

App App::init(const uint& width, const uint& height)
{
    App app(width, height);
    return app;
}
