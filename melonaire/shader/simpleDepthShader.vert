#version 330 core
in vec3 inPosition;

uniform mat4 modelMatrix;
uniform mat4 lightingSpace;

void main()
{
    gl_Position = lightingSpace * modelMatrix * vec4(inPosition, 1.0);
}  