#ifndef GRAPHENE_POSTPROCESSING_GL_H
#define GRAPHENE_POSTPROCESSING_GL_H

#include "GL_state.h"
#include "Quad.h"
#include "Framebuffer.h"

namespace graphene
{
namespace gl
{

namespace post_processing_effects
{
    enum Effect {
        NONE = 0,
        AMBIENT_OCCLUSION
    };
}


class Post_processing
{
private:
    int width_;
    int height_;

    post_processing_effects::Effect current_effect_;
    post_processing_effects::Effect configured_effect_;

    Quad quad_;

    Framebuffer_multisample* source_framebuffer_;

    std::vector<Framebuffer*> framebuffers_;

    std::vector<std::string> effects_menu_;

public:
    Post_processing();

    ~Post_processing();

    void init(int width = 800, int height = 600);

    void resize(int width, int height);

    void bind();

    const std::vector<std::string>& get_effects_menu();
    void set_effect(const std::string& effect_name);
    std::string& get_current_effect_as_string();
    post_processing_effects::Effect get_current_effect();

    void apply(GL_state* gl);

private:
    void configure_current_effect();

    void ambient_occlusion(GL_state* gl);
};

} // namespace gl
} // namespace graphene

#endif //GRAPHENE_POSTPROCESSING_GL_H
