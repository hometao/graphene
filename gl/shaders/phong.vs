
// in
in vec4 vertex;
in vec3 normal;
in vec4 color;
in vec2 texcoords;

// out
out vec3 v2f_normal;
out vec3 v2f_color;
out vec2 v2f_texcoords;

// uniform
uniform mat4 modelview_projection_matrix;
uniform mat3 normal_matrix;


void main()
{
    v2f_normal    = normalize(normal_matrix * normal);
    v2f_color     = color.xyz;
    v2f_texcoords = texcoords;
    gl_Position   = modelview_projection_matrix * vertex;
}
