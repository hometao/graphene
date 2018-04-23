#include <graphene/gl/Post_processing.h>

#include <graphene/macros.h>

namespace graphene
{
namespace gl
{

Post_processing::Post_processing() :
    width_(8),
    height_(8),
    current_effect_(post_processing_effects::NONE),
    configured_effect_(post_processing_effects::NONE),
    quad_(1.0f),
    source_framebuffer_(NULL)
{
    effects_menu_.push_back("None");
    effects_menu_.push_back("Ambient Occlusion");
}

Post_processing::~Post_processing()
{
    for (unsigned int i=0; i < framebuffers_.size(); ++i)
    {
        delete framebuffers_[i];
    }
}

void Post_processing::init(int width, int height)
{
    width_ = width;
    height_ = height;


    quad_.init();
}

void Post_processing::resize(int width, int height)
{
    width_ = width;
    height_ = height;
    for (unsigned int i=0; i < framebuffers_.size(); ++i)
    {
        framebuffers_[i]->resize(width, height);
    }
}

void Post_processing::bind()
{
    if (source_framebuffer_ == NULL)
    {
        source_framebuffer_ = new Framebuffer_multisample(width_, height_);
    }

    source_framebuffer_->bind();
}

const std::vector<std::string> &Post_processing::get_effects_menu()
{
    return effects_menu_;
}


void Post_processing::set_effect(const std::string &effect_name)
{
    if (effect_name == "None")
    {
        current_effect_ = post_processing_effects::NONE;
    }
    else if (effect_name == "Ambient Occlusion")
    {
        current_effect_ = post_processing_effects::AMBIENT_OCCLUSION;
    }
    else
    {
        current_effect_ = post_processing_effects::NONE;
    }

    if (current_effect_ != configured_effect_)
    {
        configure_current_effect();
    }
}

std::string& Post_processing::get_current_effect_as_string()
{
    return effects_menu_[current_effect_];
}

post_processing_effects::Effect Post_processing::get_current_effect()
{
    return current_effect_;
}


void Post_processing::apply(GL_state* gl)
{
    glBindFramebuffer(GL_READ_FRAMEBUFFER, source_framebuffer_->get_fbo_id());
    Framebuffer* fb = framebuffers_[0];
    glBindFramebuffer(GL_DRAW_FRAMEBUFFER, fb->get_fbo_id());

    glBlitFramebuffer(
                0, 0, width_, height_,
                0, 0, width_, height_,
                GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT, GL_NEAREST);

    switch (current_effect_)
    {
        case post_processing_effects::AMBIENT_OCCLUSION:
            ambient_occlusion(gl);
            break;

        default:
            // avoid warning
            break;
    }
}

void Post_processing::configure_current_effect()
{
    switch (current_effect_)
    {
    case post_processing_effects::AMBIENT_OCCLUSION:
        while (framebuffers_.size() < 2)
        {
            framebuffers_.push_back(new Framebuffer_singlesample(width_, height_));
        }

        Framebuffer* fb;
        //first fb has rgba color
        fb = framebuffers_[0];
        fb->set_color_attachment(fragdata_locations::COLOR, GL_RGBA8);
        fb = framebuffers_[1];
        fb->set_color_attachment(fragdata_locations::COLOR, GL_RGBA8);


        configured_effect_ = post_processing_effects::AMBIENT_OCCLUSION;

        break;
    default:
        break;
    }
}

void Post_processing::ambient_occlusion(GL_state* gl)
{

    glDisable(GL_MULTISAMPLE);

    Shader* zlinearizer = gl->set_active_shader(PP_LINEARIZE_Z_SHADER);
    zlinearizer->bindTexture("depth_texture", framebuffers_[0]->get_depth_texture_id(), GL_TEXTURE_2D, 3);
    zlinearizer->set_uniform("near", gl->near_);
    zlinearizer->set_uniform("far", gl->far_);
    framebuffers_[1]->bind();
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    quad_.draw();


    Shader* ao_shader = gl->set_active_shader(PP_AMBIENTOCCLUSION_SHADER);
    ao_shader->bindTexture("depth_texture", framebuffers_[1]->get_depth_texture_id(), GL_TEXTURE_2D, 3);
    ao_shader->set_uniform("inv_res", Vec2f(1.0f/width_, 1.0f/height_));
    ao_shader->set_uniform("resolution", Vec2f(width_, height_));
    framebuffers_[0]->bind();
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    quad_.draw();


    Shader* blur_shader = gl->set_active_shader(PP_BLUR_SHADER);
    blur_shader->bindTexture("source_texture", framebuffers_[0]->get_color_texture_id(fragdata_locations::COLOR), GL_TEXTURE_2D, 2);
    blur_shader->bindTexture("depth_texture", framebuffers_[0]->get_depth_texture_id(), GL_TEXTURE_2D, 3);
    blur_shader->set_uniform("inv_res", Vec2f(1.0f/width_, 1.0f/height_));
    blur_shader->set_uniform("direction", 0);
    framebuffers_[1]->bind();
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    quad_.draw();

    blur_shader->bindTexture("source_texture", framebuffers_[1]->get_color_texture_id(fragdata_locations::COLOR), GL_TEXTURE_2D, 2);
    blur_shader->bindTexture("depth_texture", framebuffers_[1]->get_depth_texture_id(), GL_TEXTURE_2D, 3);
    blur_shader->set_uniform("direction", 1);
    framebuffers_[0]->bind();
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    quad_.draw();

    glEnable(GL_MULTISAMPLE);

    Shader* final_shader = gl->set_active_shader(PP_FINAL_SHADER);
    final_shader->bindTexture("color_texture", source_framebuffer_->get_color_texture_id(fragdata_locations::COLOR), GL_TEXTURE_2D_MULTISAMPLE, 2);
    final_shader->bindTexture("depth_texture", source_framebuffer_->get_depth_texture_id(), GL_TEXTURE_2D_MULTISAMPLE, 3);
    final_shader->bindTexture("effect_texture", framebuffers_[0]->get_color_texture_id(fragdata_locations::COLOR), GL_TEXTURE_2D, 4);



    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    quad_.draw();



}


} //namespace gl
} //namespace graphene
