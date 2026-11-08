#include "bloom.h"

#include <algorithm>
#include <iostream>

static GLuint make_texture(GLenum internal_format, int w, int h)
{
    GLuint tex;

    glGenTextures(1, &tex);
    glBindTexture(GL_TEXTURE_2D, tex);

    glTexStorage2D(GL_TEXTURE_2D, 1, internal_format, w, h);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

    return tex;
}

void Bloom::destroy_textures()
{
    if (_scene_tex)
        glDeleteTextures(1, &_scene_tex);
    if (_scene_depth)
        glDeleteRenderbuffers(1, &_scene_depth);
    for (auto& m : _mips)
        glDeleteTextures(1, &m.tex);

    _scene_tex = _scene_depth = 0;
    _mips.clear();
}

void Bloom::resize(int w, int h)
{
    if (w <= 0 || h <= 0)
        return;
    if (_scene_fbo && w == _w && h == _h)
        return;

    if (!_scene_fbo)
    {
        _down = std::make_shared<Material>(Pipeline{"", "", "", "", "", "bloom/downsample.comp.shd"});
        _up = std::make_shared<Material>(Pipeline{"", "", "", "", "", "bloom/upsample.comp.shd"});
        _composite = std::make_shared<Material>(Pipeline{"bloom/vertex.shd", "", "", "", "bloom/composite.shd", ""});
        _down->cull_face = false;
        _up->cull_face = false;
        _composite->cull_face = false;

        glGenFramebuffers(1, &_scene_fbo);
    }

    destroy_textures();
    _w = w;
    _h = h;

    _scene_tex = make_texture(GL_RGBA16F, w, h);

    glGenRenderbuffers(1, &_scene_depth);
    glBindRenderbuffer(GL_RENDERBUFFER, _scene_depth);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT24, w, h);

    glBindFramebuffer(GL_FRAMEBUFFER, _scene_fbo);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, _scene_tex, 0);
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, _scene_depth);
    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
        std::cerr << "Bloom: scene FBO is incomplete\n";

    int mw = w;
    int mh = h;
    for (int i = 0; i < N_MIPS; i++)
    {
        mw = std::max(1, mw / 2);
        mh = std::max(1, mh / 2);
        Mip m;
        m.w = mw;
        m.h = mh;
        m.tex = make_texture(GL_R11F_G11F_B10F, mw, mh);
        _mips.push_back(m);
    }

    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void Bloom::bind_scene() const
{
    glBindFramebuffer(GL_FRAMEBUFFER, _scene_fbo);
    glViewport(0, 0, _w, _h);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

static void dispatch_for(int w, int h)
{
    glDispatchCompute((w + 7) / 8, (h + 7) / 8, 1);
    glMemoryBarrier(GL_TEXTURE_FETCH_BARRIER_BIT | GL_SHADER_IMAGE_ACCESS_BARRIER_BIT);
}

void Bloom::apply()
{
    glActiveTexture(GL_TEXTURE0);

    _down->bind();
    _down->set_float("threshold", threshold);
    _down->set_float("knee", knee);

    GLuint src = _scene_tex;
    for (size_t i = 0; i < _mips.size(); i++)
    {
        const Mip& m = _mips[i];
        glBindTexture(GL_TEXTURE_2D, src);
        glBindImageTexture(0, m.tex, 0, GL_FALSE, 0, GL_WRITE_ONLY, GL_R11F_G11F_B10F);
        _down->set_int("first_pass", i == 0 ? 1 : 0);
        dispatch_for(m.w, m.h);
        src = m.tex;
    }

    _up->bind();
    _up->set_float("radius", radius);
    for (int i = (int)_mips.size() - 1; i > 0; i--)
    {
        const Mip& from = _mips[i];
        const Mip& to = _mips[i - 1];
        glBindTexture(GL_TEXTURE_2D, from.tex);
        glBindImageTexture(0, to.tex, 0, GL_FALSE, 0, GL_READ_WRITE, GL_R11F_G11F_B10F);
        dispatch_for(to.w, to.h);
    }

    glDisable(GL_DEPTH_TEST);
    glDisable(GL_CULL_FACE);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glViewport(0, 0, _w, _h);

    _composite->bind();
    _composite->set_int("scene", 0);
    _composite->set_int("bloom", 1);
    _composite->set_float("strength", strength);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, _scene_tex);
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, _mips[0].tex);
    draw_triangle();

    glActiveTexture(GL_TEXTURE0);
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);
}
