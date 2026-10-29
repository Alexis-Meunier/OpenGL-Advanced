#include "texture.h"

#include "material.h"

Texture Texture::generate_heightmap(int size)
{
    Texture tex;

    glGenTextures(1, &tex.id);
    glBindTexture(GL_TEXTURE_2D, tex.id);
    glTexStorage2D(GL_TEXTURE_2D, 1, GL_R32F, size, size);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

    glBindImageTexture(1, tex.id, 0, GL_FALSE, 0, GL_WRITE_ONLY, GL_R32F);

    Material compute(Pipeline{"", "", "", "", "", "heightmap.compute.shd"});
    compute.bind();
    glDispatchCompute((size + 15) / 16, (size + 15) / 16, 1);

    glMemoryBarrier(GL_TEXTURE_FETCH_BARRIER_BIT);
    return tex;
}