in vec2 v2f_texcoords;

uniform sampler2D depth_texture;
uniform sampler2D source_texture;

uniform float radius = 5;
uniform float falloff = 0.05;
uniform float depth_awareness = 10000.0;
uniform float near;
uniform float far;
uniform vec2 inv_res;
uniform int direction = 0; //0 = horizontal blur, 1 = vertical blur

float center_depth;
float diff;
float sum;

out vec4 out_color;

float BlurFunction(vec2 uv, float r, float center_d, inout float w_total)
{
    float c = texture(source_texture, uv).x;
    float d = texture(depth_texture, uv).x;

    float ddiff = d - center_d;
    float w = exp(-r*r*falloff - ddiff*ddiff*depth_awareness);
    w_total += w;

    return w*c;
}

float blurHorizontal() {
	vec2 uv;
    float b = 0;
    float w_total = 0;
    float center_d = center_depth;
    for (float r = -radius; r <= radius; ++r)
    {
        uv = v2f_texcoords + vec2(r*inv_res.x, 0);
        b += BlurFunction(uv, r, center_d, w_total);	
	}
	return b/w_total;
}

float blurVertical() {
	vec2 uv;
    float b = 0;
    float w_total = 0;
    float center_d = center_depth;
    for (float r = -radius; r <= radius; ++r)
    {
        uv = v2f_texcoords + vec2(0, r*inv_res.y);
        b += BlurFunction(uv, r, center_d, w_total);	
	}
	return b/w_total;
}

void main() {


	center_depth = texture(depth_texture, v2f_texcoords).x;
	if (center_depth >= 0.999) {
		discard;
	}
	
	switch (direction) 
	{
		case 0:
			sum = blurHorizontal();
			break;
		case 1:
			sum = blurVertical();
			break;
	}
	
	out_color = vec4(vec3(sum),1.0);
	gl_FragDepth = center_depth;
}