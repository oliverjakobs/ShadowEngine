#version 330 core

layout(location = 0) out vec4 f_Color;

#define PI 3.14

//inputs from vertex shader
in vec2 v_TexCoord;

//uniform values
uniform sampler2D u_Texture;
uniform float u_Resolution;

//alpha threshold for our occlusion map
const float THRESHOLD = 0.75;

void main()
{
	float d = 1.0;

	float theta = PI * (2 * v_TexCoord.s + 0.5);

	for (float y = 0.0; y < u_Resolution; y += 1.0)
	{
		//the current distance is how far from the top we've come
		float r = y / u_Resolution;

		//rectangular to polar filter
		
		//coord which we will sample from occlude map
		vec2 coord = vec2(-r * sin(theta), -r * cos(theta))/2.0 + 0.5;
		
		//sample the occlusion map
		vec4 data = texture2D(u_Texture, coord);
		
		//if we've hit an opaque fragment (occluder), then get new distance
		//if the new distance is below the current, then we'll use that for our ray
		float caster = data.a;
		if (caster > THRESHOLD) 
		{
			d = min(d, r); //NOTE: we could probably use "break" or "return" here
		}
	} 

	f_Color = vec4(vec3(d), 1.0);
}