//=============================================================================
// Copyright (C) Graphics & Geometry Processing Group, Bielefeld University
//=============================================================================


//== INCLUDES =================================================================


#include "GL_state.h"


//=============================================================================


namespace graphene {
namespace gl {


//=============================================================================


GL_state::GL_state() :
    model_(Mat4f::identity()),
    view_(Mat4f::identity()),
    proj_(Mat4f::identity()),
    modelviewproj_(Mat4f::identity()),
    shaders_(SHADER_MAX, NULL),
    shader_type_(PHONG_SHADER)
{
}


//-----------------------------------------------------------------------------


GL_state::~GL_state()
{
    unsigned int i;
    for (i=0; i < shaders_.size(); ++i)
    {
        if (shaders_[i]) delete shaders_[i];
    }
}


//-----------------------------------------------------------------------------


bool GL_state::init(const char *argv)
{
    if (!load_shaders(argv))
    {
        return false;
    }

    return true;
}


//-----------------------------------------------------------------------------


Shader* GL_state::set_active_shader(Shader_type shader_type)
{
    shader_type_ = shader_type;
    shaders_[shader_type_]->use();
    return get_active_shader();
}


//-----------------------------------------------------------------------------


Shader* GL_state::get_active_shader()
{
    return shaders_[shader_type_];
}


//-----------------------------------------------------------------------------


Shader_type GL_state::get_active_shader_type()
{
    return shader_type_;
}


//-----------------------------------------------------------------------------


void GL_state::update_matrices()
{
    modelview_     = view_*model_;
    modelviewproj_ = proj_*modelview_;
    normal_        = inverse(transpose(Mat3f(modelview_)));
}


//-----------------------------------------------------------------------------


void GL_state::upload_matrices()
{
    get_active_shader()->set_uniform("modelview_projection_matrix", modelviewproj_);
    get_active_shader()->set_uniform("modelview_matrix", modelview_);
    get_active_shader()->set_uniform("normal_matrix", normal_);
}


//-----------------------------------------------------------------------------


void GL_state::bind_texture(const char *sampler_name,
                            GLuint texture_id,
                            GLint texture_type,
                            GLint texture_unit)
{
    glActiveTexture(GL_TEXTURE0 + texture_unit);
    glBindTexture(texture_type, texture_id);

    GLint id = glGetUniformLocation(Shader::get_active()->get_shaderid(), sampler_name);
    //if (id == -1) std::cerr << "Error: Invalid texture name!" << std::endl;
    glUniform1i(id, texture_unit);

    glActiveTexture(GL_TEXTURE0);

}


//-----------------------------------------------------------------------------


void GL_state::setup_lights(const std::vector<Light>& _lights)
{
    // store light sources
    lights_ = _lights;


    // let's accept only 8 light sources
    if (lights_.size() > 8)
    {
        std::cerr << "GL_state: only 8 light sources possible\n";
        lights_.resize(8);
    }


    // setup data to be used in shaders
    std::vector<Vec3f> light_dirs;
    std::vector<Vec3f> light_colors;
    for (unsigned int i=0; i<lights_.size(); ++i)
    {
        light_dirs.push_back(normalize(-lights_[i].position_));
        light_colors.push_back(lights_[i].color_);
    }


    // upload light data to each shader
    Shader* shader;
    for (unsigned int i=0; i < SHADER_WITH_LIGHT_MODEL_MAX; ++i)
    {
        shader = shaders_[i];
        if (shader)
        {
            shader->use();
            shader->set_uniform("light_directions", light_dirs);
            shader->set_uniform("light_colors", light_colors);
            shader->set_uniform("num_active_lights", (int)light_dirs.size());
        }
    }
}


//-----------------------------------------------------------------------------


bool GL_state::load_shaders(const char *argv)
{
    // path to shader directory
    std::string shader_path(argv);
#ifdef _WIN32
    shader_path = shader_path.substr(0, shader_path.find_last_of('\\')+1);
#elif __APPLE__
    shader_path = shader_path.substr(0, shader_path.find_last_of('/')+1);
    shader_path += "../";
#else
    shader_path = shader_path.substr(0, shader_path.find_last_of('/')+1);
#endif
    shader_path += "shaders/";


    // Phong shader
    shaders_[PHONG_SHADER] = load_shader((shader_path + "phong.vs").c_str(),
                                         (shader_path + "phong.fs").c_str());

    // sphere shader
    shaders_[SPHERE_SHADER] = load_shader((shader_path + "sphere.vsh").c_str(),
                                          (shader_path + "sphere.fsh").c_str());

    // cylinder shader
    shaders_[CYLINDER_SHADER] = load_shader((shader_path + "cylinder.vsh").c_str(),
                                            (shader_path + "cylinder.fsh").c_str());

    //post processing ao shader
    shaders_[PP_AMBIENTOCCLUSION_SHADER] = load_shader((shader_path + "pp_ortho.vsh").c_str(),
                                                    (shader_path + "pp_ambientocclusion.fsh").c_str());

    //post processing blur shader
    shaders_[PP_BLUR_SHADER] = load_shader((shader_path + "pp_ortho.vsh").c_str(),
                                        (shader_path + "pp_blur.fsh").c_str());

    //post processing z linearizer
    shaders_[PP_LINEARIZE_Z_SHADER] = load_shader((shader_path + "pp_ortho.vsh").c_str(),
                                               (shader_path + "pp_linearize_z.fsh").c_str());

    //post processing finalize shader
    shaders_[PP_FINAL_SHADER] = load_shader((shader_path + "pp_ortho.vsh").c_str(),
                                        (shader_path + "pp_final.fsh").c_str());

    return true;
}

//-----------------------------------------------------------------------------


Shader* GL_state::load_shader(const char *vs_file,
                              const char *fs_file,
                              const char *gs_file)
{
    Shader* shader = new Shader;

    if (shader->load(vs_file, fs_file, gs_file))
    {
        return shader;
    }
    else
    {
        std::cerr << "glState::compileShader: ERROR: Could not compile Shader with filenames \""
                  << vs_file << "\" and \"" << fs_file << "\". " << std::endl;
        delete shader;
        return NULL;
    }
}


//=============================================================================
} // namespace gl
} // namespace graphene
//=============================================================================
