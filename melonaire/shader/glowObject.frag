#version 330 core

out vec4 outColor;

uniform vec3 glowColor;

void main(void)
{
	outColor = vec4(glowColor, 1.0);
}
