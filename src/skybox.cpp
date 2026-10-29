#include "skybox.h"

Skybox::Skybox()
{
    _material = std::make_shared<Material>(
        Pipeline{"skybox/vertex.shd", "", "", "", "skybox/fragment.shd", ""});
}

void Skybox::render(const matrix4& view, const matrix4& proj,
                    const vec3& sun_dir, const vec3& sun_color) const
{
    glDisable(GL_DEPTH_TEST);
    glDisable(GL_CULL_FACE);

    _material->bind();
    _material->set_mat4("view", view);
    _material->set_mat4("proj", proj);
    _material->set_vec3("sunDir", sun_dir);
    _material->set_vec3("sunColor", sun_color);

    glDrawArrays(GL_TRIANGLES, 0, 3);

    glEnable(GL_CULL_FACE);
    glEnable(GL_DEPTH_TEST);
}
