#version 330 core
layout (location = 0) in vec3 aPos;

out vec3 TexCoords;

uniform mat4 viewProjection;
uniform mat4 modelMatrix;

void main()
{
    TexCoords = vec3( modelMatrix * vec4(aPos, 1.0) );
    gl_Position = viewProjection * vec4(aPos, 1.0);
}  