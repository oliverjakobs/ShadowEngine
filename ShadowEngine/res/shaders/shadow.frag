#version 330 core

layout(location = 0) out vec4 f_Color;

in vec2 v_TexCoord;

uniform sampler2D u_Texture;

uniform float u_Resolution;
uniform float u_SoftShadows;

uniform vec4 u_Color;

const float PI = 3.14;

// sample from the distance map
float sample(float x, float y, float r)
{
	return step(r, texture2D(u_Texture, vec2(x, y)).r);
}

void main(void)
{
	// rectangular to polar
	vec2 norm = v_TexCoord.st * 2.0 - 1.0;
	float theta = atan(norm.y, norm.x);
	float r = length(norm);
	
	// the tex coord to sample our 1D lookup texture (y is always 0.0)
	float coord = (theta + PI) / (2.0 * PI);
	
	// the center tex coord, which gives us hard shadows
	float center = sample(coord, 0.0, r);

	// we multiply the blur amount by our distance from center
	// this leads to more blurriness as the shadow "fades away"
	float blur = (1.0 / u_Resolution) * smoothstep(0.0, 1.0, r);
	
	// now we use a simple gaussian blur
	float sum = 0.0;
	
	sum += sample(coord - 4.0 * blur, 0.0, r) * 0.05;
	sum += sample(coord - 3.0 * blur, 0.0, r) * 0.09;
	sum += sample(coord - 2.0 * blur, 0.0, r) * 0.12;
	sum += sample(coord - 1.0 * blur, 0.0, r) * 0.15;
	
	sum += center * 0.16;
	
	sum += sample(coord + 1.0 * blur, 0.0, r) * 0.15;
	sum += sample(coord + 2.0 * blur, 0.0, r) * 0.12;
	sum += sample(coord + 3.0 * blur, 0.0, r) * 0.09;
	sum += sample(coord + 4.0 * blur, 0.0, r) * 0.05;
	
	// 1.0 -> in light, 0.0 -> in shadow
	float lit = mix(center, sum, u_SoftShadows);
	
	// multiply the summed amount by our distance, which gives us a radial falloff
	f_Color = vec4(u_Color.rgb, lit * smoothstep(1.0, 0.0, r));
}