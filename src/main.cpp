#include "app.h"
#include "program.h"

int main(int argc, char **argv)
{
    (void)argc; (void)argv;
    App app = App::init();

    auto default_scene = Scene::default_scene();
    app.load_new_scene(default_scene);
    app.render();

    return 0;
}