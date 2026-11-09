#ifndef TEXTURE_H
#define TEXTURE_H

#include <GL/glew.h>
#include <GL/freeglut.h>

class Texture {
    public:
        static Texture generate_heightmap(int size);
        static Texture generate_wave_texture(int size);

        GLuint id;
    private:
        Texture() = default;
};

#endif // TEXTURE_H
