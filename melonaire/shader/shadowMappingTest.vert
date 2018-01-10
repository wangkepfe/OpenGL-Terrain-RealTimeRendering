#version 330 core

in vec3 inPosition;
in vec3 inNormal;
in vec2 inTexCoord;
in vec3 inTangent;
in vec3 inBitangent;

out vec2 texCoord;
out vec3 fragPos;
out vec4 fragPosLightingSpace;
out vec3 normal;

uniform mat4 mvp;
uniform mat4 modelMatrix;
uniform mat3 normalMatrix;
uniform mat4 lightingSpace;

void main(void)
{
    texCoord = inTexCoord;
    fragPos = vec3(modelMatrix * vec4(inPosition, 1.0));
    fragPosLightingSpace = lightingSpace * vec4(fragPos, 1.0);

    normal = normalMatrix * inNormal;
    
    gl_Position = mvp * vec4(inPosition, 1.0);
    //gl_Position = fragPosLightingSpace;
}
