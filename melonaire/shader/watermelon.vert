#version 150

in vec3 inPosition;              out vec4 clipPosition;

in vec3 inNormal;
in vec2 inTexCoord;              out vec2 texCoord;

out vec3 fragPos;
out vec3 transformedNormal;

uniform int objType;

uniform vec4 ClipPlane;

uniform mat4 projMatrix;
uniform mat4 mdlMatrix;
uniform mat4 camMatrix;

void main(void)
{
	texCoord = inTexCoord;
    
	transformedNormal = mat3(mdlMatrix) * inNormal;
    
    vec4 worldPos = mdlMatrix * vec4(inPosition, 1.0);
    fragPos = vec3(worldPos);
    
    if(objType != 0)//skybox
        gl_ClipDistance[0] = dot(ClipPlane, worldPos);
    else
        gl_ClipDistance[0] = 1;
    
	gl_Position = clipPosition = projMatrix * camMatrix * worldPos;
}
