#version 330 core

precision highp float;

layout (location = 0) out vec4 fragColor;
in vec3 inSunCoord;

uniform blobSettings {
	vec4 color;
	vec2 lightPos;
	float size;
};

void main()
{
	float intensity = size / length(inSunCoord.xy - lightPos);
	fragColor = color * intensity;
}