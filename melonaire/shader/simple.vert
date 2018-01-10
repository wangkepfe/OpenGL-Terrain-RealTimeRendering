#version 330 core

in vec3 inPosition;
in vec3 inNormal;
in vec2 inTexCoord;
in vec3 inTangent;
in vec3 inBitangent;

out vec2 texCoord;

uniform mat4 mvp;

void main(void)
{
    texCoord = inTexCoord;
    
    gl_Position = mvp * vec4(inPosition, 1.0);
}
