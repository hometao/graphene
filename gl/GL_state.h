//=============================================================================
// Copyright (C) Graphics & Geometry Processing Group, Bielefeld University
//=============================================================================

#ifndef GRAPHENE_GLSTATE_GL_H
#define GRAPHENE_GLSTATE_GL_H

//== INCLUDES =================================================================

#include "Shader.h"
#include <graphene/geometry/Matrix3x3.h>
#include <graphene/geometry/Matrix4x4.h>
#include <graphene/geometry/Vector.h>
#include <vector>


//== NAMESPACE ================================================================

namespace graphene {
namespace gl {

//=============================================================================


enum Shader_type {
    PHONG_SHADER = 0,
    SPHERE_SHADER,
    CYLINDER_SHADER,

    //define new shader with calculation of a lightmodel above this entry and shaders without beneath this entry
    //setup_lights() method shows why
    SHADER_WITH_LIGHT_MODEL_MAX,

    PP_AMBIENTOCCLUSION_SHADER,
    PP_BLUR_SHADER,
    PP_LINEARIZE_Z_SHADER,
    PP_FINAL_SHADER,

    SHADER_MAX
};


//=============================================================================


struct Camera
{
    Vec3f eye_;
    Vec3f center_;
    Vec3f up_;

    Camera() :
        eye_(0.0f), center_(0.0f), up_(0.0f,1.0f,0.0f)
    {}
};


//=============================================================================


struct Light
{
    Light() {}
    Light(const Vec3f& _position,
          const Vec3f& _color) :
        position_(_position), color_(_color)
    {}

    Vec3f position_;
    Vec3f color_;
};


//=============================================================================


class GL_state
{
public:

    GL_state();
    ~GL_state();

    bool init(const char* argv);

    //makes 'shader_type' the current shader
    Shader* set_active_shader(Shader_type shader_type);
    //returns a pointer to the currently active shader
    Shader* get_active_shader();
    //get shader type of active shader
    Shader_type get_active_shader_type();

    //update mvp and normal matrix based on current model, view and projection matrices
    void update_matrices();
    //upload normal- and mvp matrix to currently active shader
    void upload_matrices();

    //bind texture to currently active shader
    void bind_texture(const char* sampler_name,
                      GLuint texture_id,
                      GLint texture_type,
                      GLint texture_unit);

    //set up static lights for all shaders
    void setup_lights(const std::vector<Light>& _lights);


private:

    bool load_shaders(const char* argv);
    Shader* load_shader(const char* vs_file,
                        const char* fs_file,
                        const char* gs_file=NULL);


public:

    //public matrices
    Mat4f model_;
    Mat4f view_;
    Mat4f proj_;

    // projection parameters
    float fovy_, ar_, near_, far_;

    //these matrices are only up to date after a call from update_matrices()!!!
    Mat4f modelviewproj_;
    Mat4f modelview_;
    Mat3f normal_;

    // light sources
    std::vector<Light> lights_;


private:

    //shaders
    std::vector<Shader*> shaders_;

    //saves the active shader type;
    Shader_type shader_type_;
};


//=============================================================================
} // namespace gl
} // namespace graphene
//=============================================================================
#endif // GRAPHENE_GLSTATE_GL_H
//=============================================================================
