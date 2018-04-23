
in vec2 v2f_texcoords;

uniform float near;
uniform float far;

uniform sampler2D depth_texture;

void main() 
{
    float depth = texture(depth_texture, v2f_texcoords).x;
    if (depth >= 1.0) 
    {
        discard;
    }
    
    gl_FragDepth = (2 * near) / (far + near - depth * (far - near));
}