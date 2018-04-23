#include "Framebuffer.h"


#include <graphene/gl/Post_processing.h>
#include <graphene/macros.h>



namespace graphene
{
namespace gl
{

//=SINGLESAMPLE================================================================


Framebuffer_singlesample::Framebuffer_singlesample(int width, int height) :
    Framebuffer(width, height)
{
    glGenFramebuffers(1, &fbo_id_);

    //set one default color attachment
    set_color_attachment(fragdata_locations::COLOR, GL_RGBA8);

    //set default depth component
    set_depth_attachment(GL_DEPTH_COMPONENT);



    glBindFramebuffer(GL_FRAMEBUFFER, fbo_id_);

    GLenum status = glCheckFramebufferStatus(GL_FRAMEBUFFER);
    if (status != GL_FRAMEBUFFER_COMPLETE) {
        LOG(Log_error) << "Framebuffer incomplete!" << std::endl;
    }
    glBindFramebuffer(GL_FRAMEBUFFER, 0);


}

Framebuffer_singlesample::~Framebuffer_singlesample()
{
    if (depth_attachment_)
        delete depth_attachment_;

    Attachment* a = 0;
    for (unsigned int i=0; i < color_attachments_.size(); ++i)
    {
        a = color_attachments_[i];
        if (a)
        {
            delete a;
        }
    }

    glDeleteFramebuffers(1, &fbo_id_);
}


void Framebuffer_singlesample::bind()
{
    glBindFramebuffer(GL_FRAMEBUFFER, fbo_id_);
}

void Framebuffer_singlesample::resize(int width, int height)
{
    width_ = width;
    height_ = height;

    Attachment* a;
    for (unsigned int i=0; i < color_attachments_.size(); ++i)
    {
        a = color_attachments_[i];
        if (a)
        {
            glBindTexture(GL_TEXTURE_2D, a->texture_id_);
            glTexImage2D(GL_TEXTURE_2D, 0, a->internal_format_, width_, height_, 0, GL_RGBA, GL_FLOAT, 0);
        }
    }


    glBindTexture(GL_TEXTURE_2D, depth_attachment_->texture_id_);
    glTexImage2D(GL_TEXTURE_2D, 0, depth_attachment_->internal_format_,
                 width_, height_, 0, GL_DEPTH_COMPONENT, GL_FLOAT, 0);
}

GLuint Framebuffer_singlesample::get_color_texture_id(int attachment)
{
    return color_attachments_[attachment]->texture_id_;
}

GLuint Framebuffer_singlesample::get_depth_texture_id()
{
    return depth_attachment_->texture_id_;
}

void Framebuffer_singlesample::set_color_attachment(int location, GLint internal_format)
{
    Attachment* a = color_attachments_[location];
    //already up and running => just set texture values
    if (a)
    {
        a->internal_format_ = internal_format;
        glBindTexture(GL_TEXTURE_2D, a->texture_id_);
        glTexImage2D(GL_TEXTURE_2D, 0, a->internal_format_, width_, height_, 0, GL_RGBA, GL_FLOAT, 0);
    }
    //not active => create new and add to drawbuffers and framebuffer
    else
    {
        a = new Attachment(GL_COLOR_ATTACHMENT0 + location,
                           internal_format);

        color_attachments_[location] = a;

        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, a->texture_id_);
        glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glTexImage2D(GL_TEXTURE_2D, 0, a->internal_format_, width_, height_, 0, GL_RGBA, GL_FLOAT, 0);


        glBindFramebuffer(GL_FRAMEBUFFER, fbo_id_);
        //bind texture to framebuffer
        glFramebufferTexture2D(GL_FRAMEBUFFER, a->attachment_id_, GL_TEXTURE_2D, a->texture_id_, 0);

        //collect active attachments
        std::vector<GLenum> attachments;
        for (unsigned int i=0; i < color_attachments_.size(); ++i)
        {
            a = color_attachments_[i];
            if (a) {
                attachments.push_back(a->attachment_id_);
            }
        }

        glDrawBuffers(attachments.size(), &attachments[0]);
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
    }
}

void Framebuffer_singlesample::set_depth_attachment(GLint format)
{
    if (depth_attachment_)
    {
        depth_attachment_->internal_format_ = format;
        glBindTexture(GL_TEXTURE_2D, depth_attachment_->texture_id_);
        glTexImage2D(GL_TEXTURE_2D, 0, depth_attachment_->internal_format_, width_, height_, 0, GL_DEPTH_COMPONENT, GL_FLOAT, 0);
    }
    else
    {
        glActiveTexture(GL_TEXTURE0);

        depth_attachment_ = new Attachment(-1,//depth does not have fragdata location
                           format);
        glBindTexture(GL_TEXTURE_2D, depth_attachment_->texture_id_);
        glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glTexImage2D(GL_TEXTURE_2D, 0, depth_attachment_->internal_format_, width_, height_, 0, GL_DEPTH_COMPONENT, GL_FLOAT, 0);

        //bind texture to framebuffer
        glBindFramebuffer(GL_FRAMEBUFFER, fbo_id_);
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, depth_attachment_->texture_id_, 0);
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
    }

}
//=SINGLESAMPLE_END============================================================



//=MULTISAMPLE=================================================================

Framebuffer_multisample::Framebuffer_multisample(int width, int height) :
    Framebuffer(width, height)
{
    glGenFramebuffers(1, &fbo_id_);

    //set one default color attachment
    set_color_attachment(fragdata_locations::COLOR, GL_RGBA8);

    //set default depth component
    set_depth_attachment(GL_DEPTH_COMPONENT);



    glBindFramebuffer(GL_FRAMEBUFFER, fbo_id_);


    GLenum status = glCheckFramebufferStatus(GL_FRAMEBUFFER);
    if (status != GL_FRAMEBUFFER_COMPLETE) {
        LOG(Log_error) << "Framebuffer incomplete!" << std::endl;
    }
    glBindFramebuffer(GL_FRAMEBUFFER, 0);


}

Framebuffer_multisample::~Framebuffer_multisample()
{
    if (depth_attachment_)
        delete depth_attachment_;

    Attachment* a = 0;
    for (unsigned int i=0; i < color_attachments_.size(); ++i)
    {
        a = color_attachments_[i];
        if (a)
        {
            delete a;
        }
    }

    glDeleteFramebuffers(1, &fbo_id_);
}


void Framebuffer_multisample::bind()
{
    glBindFramebuffer(GL_FRAMEBUFFER, fbo_id_);
}

void Framebuffer_multisample::resize(int width, int height)
{
    width_ = width;
    height_ = height;

    Attachment* a;
    for (unsigned int i=0; i < color_attachments_.size(); ++i)
    {
        a = color_attachments_[i];
        if (a)
        {
            glBindTexture(GL_TEXTURE_2D_MULTISAMPLE, a->texture_id_);
            glTexImage2DMultisample(GL_TEXTURE_2D_MULTISAMPLE, 4, a->internal_format_, width_, height_, GL_FALSE);
        }
    }


    glBindTexture(GL_TEXTURE_2D_MULTISAMPLE, depth_attachment_->texture_id_);
    glTexImage2DMultisample(GL_TEXTURE_2D_MULTISAMPLE, 4, depth_attachment_->internal_format_,
                 width_, height_, GL_FALSE);
}

GLuint Framebuffer_multisample::get_color_texture_id(int attachment)
{
    return color_attachments_[attachment]->texture_id_;
}

GLuint Framebuffer_multisample::get_depth_texture_id()
{
    return depth_attachment_->texture_id_;
}

void Framebuffer_multisample::set_color_attachment(int location, GLint internal_format)
{
    Attachment* a = color_attachments_[location];
    //already up and running => just set texture values
    if (a)
    {
        a->internal_format_ = internal_format;
        glBindTexture(GL_TEXTURE_2D_MULTISAMPLE, a->texture_id_);
        glTexImage2DMultisample(GL_TEXTURE_2D_MULTISAMPLE, 4, a->internal_format_, width_, height_, GL_FALSE);
    }
    //not active => create new and add to drawbuffers and framebuffer
    else
    {
        a = new Attachment(GL_COLOR_ATTACHMENT0 + location,
                           internal_format);

        color_attachments_[location] = a;

        glBindTexture(GL_TEXTURE_2D_MULTISAMPLE, a->texture_id_);
        glTexImage2DMultisample(GL_TEXTURE_2D_MULTISAMPLE, 4, a->internal_format_, width_, height_, GL_FALSE);


        glBindFramebuffer(GL_FRAMEBUFFER, fbo_id_);
        //bind texture to framebuffer
        glFramebufferTexture(GL_FRAMEBUFFER, a->attachment_id_, a->texture_id_, 0);

        //collect active attachments
        std::vector<GLenum> attachments;
        for (unsigned int i=0; i < color_attachments_.size(); ++i)
        {
            a = color_attachments_[i];
            if (a) {
                attachments.push_back(a->attachment_id_);
            }
        }

        glDrawBuffers(attachments.size(), &attachments[0]);
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
    }
}

void Framebuffer_multisample::set_depth_attachment(GLint format)
{
    if (depth_attachment_)
    {
        depth_attachment_->internal_format_ = format;
        glBindTexture(GL_TEXTURE_2D_MULTISAMPLE, depth_attachment_->texture_id_);
        glTexImage2DMultisample(GL_TEXTURE_2D_MULTISAMPLE, 4, depth_attachment_->internal_format_, width_, height_, GL_FALSE);
    }
    else
    {

        depth_attachment_ = new Attachment(-1,//depth does not have fragdata location
                           format);
        glBindTexture(GL_TEXTURE_2D_MULTISAMPLE, depth_attachment_->texture_id_);
        glTexImage2DMultisample(GL_TEXTURE_2D_MULTISAMPLE, 4, depth_attachment_->internal_format_, width_, height_, GL_FALSE);

        //bind texture to framebuffer
        glBindFramebuffer(GL_FRAMEBUFFER, fbo_id_);
        glFramebufferTexture(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, depth_attachment_->texture_id_, 0);
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
    }

}


//=MULTISAMPLE_END=============================================================

} //namespace gl
} //namespace graphene
