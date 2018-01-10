#version 330 core

in vec3 inPosition;
in vec3 inNormal;
in vec2 inTexCoord;
in vec3 inTangent;
in vec3 inBitangent;

out vec2 texCoord;
out vec3 normal;
out vec3 mTangent;
out vec3 mBitangent;
out vec3 fragPos;

uniform mat4 mvp;
uniform mat4 modelMatrix;
uniform mat3 normalMatrix;

void main(void)
{
    texCoord = inTexCoord;
	fragPos = vec3(modelMatrix * vec4(inPosition, 1.0));
    
	normal = normalMatrix * inNormal;
    
    gl_Position = mvp * vec4(inPosition, 1.0);
}