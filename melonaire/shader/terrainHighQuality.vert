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
out vec4 fragPosLightingSpace;

uniform mat4 mvp;
uniform mat4 modelMatrix;
uniform mat3 normalMatrix;
uniform mat4 lightingSpace;

uniform vec4 clippingPlane;

void main(void)
{
    texCoord = inTexCoord;
    vec4 fragPos4 = modelMatrix * vec4(inPosition, 1.0);
	fragPos = vec3(fragPos4);
    fragPosLightingSpace = lightingSpace * vec4(fragPos, 1.0);

	normal = normalMatrix * inNormal;
    mTangent = normalMatrix * inTangent;
    mBitangent = normalMatrix * inBitangent;
      
    gl_ClipDistance[0] = dot(clippingPlane, fragPos4);
    gl_Position = mvp * vec4(inPosition, 1.0);
}