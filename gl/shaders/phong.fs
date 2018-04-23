in vec3 v2f_normal;
in vec3 v2f_color;
in vec2 v2f_texcoords;

// light sources
uniform vec3 light_directions[8];
uniform vec3 light_colors[8];
uniform int  num_active_lights = 1;

// material
uniform vec3      front_color = vec3(0.5, 0.55, 0.6);
uniform vec3      back_color  = vec3(0.5, 0.0, 0.0);
uniform vec4      material    = vec4(0.1, 0.8, 1.0, 100.0);
uniform sampler1D texture1D;

// switches
uniform bool use_texture     = false;
uniform bool use_vertexcolor = false;
uniform bool use_lighting    = true;

// out
out vec4 out_color;

// tmp
vec3 color_tmp;


//-----------------------------------------------------------------------------


void phong_lighting(const vec3  normal,
                    const vec4  material,
                    const vec3  color,
                    inout vec3  result)
{
    // ambient component
    result = material[0] * color;

    vec3  L, R;
    const vec3 V = vec3(0.0, 0.0, -1.0);
    float NL, RV;

    // for each light source
    for (int i=0; i < num_active_lights; ++i)
    {
        L = light_directions[i];

        // diffuse
        NL = dot(normal, L);
        if (NL > 0.0)
        {
            result += NL * material[1] * color * light_colors[i];

            // specular
            R  = normalize(reflect(L, normal));
            RV = dot(R, V);
            if (RV > 0.0)
            {
                result += vec3(material[2] * pow(RV, material[3]));
            }
        }
    }

    result = clamp(result, 0.0, 1.0);
}


//-----------------------------------------------------------------------------


void main()
{
    vec3 N = normalize(v2f_normal);

    if (use_lighting)
    {
        vec3 color = front_color;
        if (use_vertexcolor)
            color = v2f_color;

        if (gl_FrontFacing)
        {
            phong_lighting(N, material, color, color_tmp);
        }
        else
        {
            phong_lighting(-N, material, back_color, color_tmp);
        }
    }
    else
    {
        if (use_vertexcolor)
        {
            color_tmp = v2f_color;
        }
        else
        {
            color_tmp = front_color;
        }
    }

    if (use_texture)
    {
        color_tmp *= texture(texture1D, v2f_texcoords.x).xyz;
    }

    color_tmp = clamp(color_tmp, 0.0, 1.0);
    out_color = vec4(color_tmp, 1.0);
}
