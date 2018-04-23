#ifndef GRAPHENE_FRAMEBUFFER_GL_H
#define GRAPHENE_FRAMEBUFFER_GL_H

#include "gl_includes.h"
#include <vector>
#include <graphene/gl/Shader.h>

namespace graphene
{
namespace gl
{


struct Attachment
{
    GLenum attachment_id_;

    GLint internal_format_;

    GLuint texture_id_;

    Attachment(GLenum id, GLint internal_format) :
        attachment_id_(id), internal_format_(internal_format), texture_id_(0)
    {
        glGenTextures(1, &texture_id_);
    }

    ~Attachment()
    {
        if (texture_id_) glDeleteTextures(1, &texture_id_);
    }
};

class Framebuffer
{
protected:
    GLuint fbo_id_;

    Attachment* depth_attachment_;

    std::vector<Attachment*> color_attachments_;

    int width_;
    int height_;

public:
    Framebuffer(int width, int height) :
        fbo_id_(0), depth_attachment_(NULL), color_attachments_(fragdata_locations::MAX, 0),
        width_(width), height_(height)
    {  }

    virtual ~Framebuffer() {}
    
    virtual void bind() {}

    virtual void resize(int width, int height) {}

    virtual GLuint get_color_texture_id(int attachment) { return 0; }

    virtual GLuint get_depth_texture_id() { return 0; }

    ///if the attachment at "location" is already active this just changes the texture values
    ///else it adds a new attachment
    virtual void set_color_attachment(int location, GLint internal_format) {}

    virtual void set_depth_attachment(GLint format) {}

    GLuint get_fbo_id()
    {
        return fbo_id_;
    }
};


class Framebuffer_singlesample : public Framebuffer
{
private:

public:
    Framebuffer_singlesample(int width, int height);

    ~Framebuffer_singlesample();

    void bind();

    void resize(int width, int height);

    GLuint get_color_texture_id(int attachment);

    GLuint get_depth_texture_id();

    void set_color_attachment(int location, GLint internal_format);

    void set_depth_attachment(GLint format);


};

class Framebuffer_multisample : public Framebuffer
{
private:

public:
    Framebuffer_multisample(int width, int height);

    ~Framebuffer_multisample();

    void bind();

    void resize(int width, int height);

    GLuint get_color_texture_id(int attachment);

    GLuint get_depth_texture_id();

    void set_color_attachment(int location, GLint internal_format);

    void set_depth_attachment(GLint format);

};

} // namespace gl
} // namespace graphene

#endif //GRAPHENE_FRAMEBUFFER_GL_H
