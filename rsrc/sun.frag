#version 330 core

layout (location = 0) out vec4 color;

uniform vec4 colour;
uniform vec2 light_pos;
uniform float size;

in vec4 pos;

void main()
{
	float intensity = size / length(pos.xy - light_pos);
	color = colour * intensity;
}