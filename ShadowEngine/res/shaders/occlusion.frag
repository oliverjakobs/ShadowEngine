#version 330 core

layout(location = 0) out vec4 f_Color;

in vec2 v_TexCoord;

uniform sampler2D u_Texture;

void main()
{
	f_Color = vec4(0.0, 0.0, 0.0, texture(u_Texture, v_TexCoord).a);
}