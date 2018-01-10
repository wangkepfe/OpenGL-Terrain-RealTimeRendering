#version 330 core

in vec3 inPosition;
in vec3 inNormal;
in vec2 inTexCoord;
in vec3 inTangent;
in vec3 inBitangent;
in mat4 instanceModelMatrix;

out vec2 texCoord;
out vec3 fragPos;

uniform mat4 vp;

uniform vec4 clippingPlane;

void main(void)
{
    texCoord = inTexCoord;
    
    vec4 fragPos4 = instanceModelMatrix * vec4(inPosition, 1.0);
	fragPos = vec3(fragPos4);
      
    gl_ClipDistance[0] = dot(clippingPlane, fragPos4);
    gl_Position = vp * instanceModelMatrix * vec4(inPosition, 1.0);
}