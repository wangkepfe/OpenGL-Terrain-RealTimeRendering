#version 330 core

out vec4 outColor;

in vec2 texCoord;
in vec3 fragPos;
in vec4 fragPosLightingSpace;
in vec3 normal;

uniform float textureScale;

uniform sampler2D tex;
uniform sampler2D texShadowMap;
uniform vec3 viewPos;
uniform vec3 lightDir;

float diffuse(vec3 lightDirec, vec3 normal)
{
	return max(dot(-lightDirec, normal), 0.0) * 2.0;
}

float specular(vec3 lightDirec, vec3 normal, vec3 viewDir, int ex)
{
	return pow (max( dot( normal , normalize( - lightDirec - viewDir) ), 0.0), ex * 2) * 0.5;
}

float shadowCalculation()
{
	// perform perspective divide
    vec3 projCoords = fragPosLightingSpace.xyz / fragPosLightingSpace.w;

    float shadow = 0.0;

    if(projCoords.z > 1.0)
    {
        shadow = 0.0;
        return shadow;
    }

    // transform to [0,1] range
    projCoords = projCoords * 0.5 + 0.5;
    // get closest depth value from light's perspective (using [0,1] range fragPosLight as coords)
    //float closestDepth = texture(texShadowMap, projCoords.xy).r; 
    // get depth of current fragment from light's perspective
    float currentDepth = projCoords.z;
    // check whether current frag pos is in shadow
	float bias = max(0.05 * (1.0 - dot(normal, -lightDir)), 0.001);
	//float bias = 0.005;
    //float shadow = currentDepth - bias > closestDepth  ? 1.0 : 0.0;

    vec2 texelSize = 1.0 / textureSize(texShadowMap, 0);
    for(int x = -1; x <= 1; ++x)
    {
        for(int y = -1; y <= 1; ++y)
        {
            float pcfDepth = texture(texShadowMap, projCoords.xy + vec2(x, y) * texelSize).r; 
            shadow += currentDepth - bias > pcfDepth ? 1.0 : 0.0;        
        }    
    }
    shadow /= 9.0;

    //return closestDepth;
    return shadow;
}

void main(void)
{
    // ambient light
    vec3 diff = vec3(1, 1, 1) * 0.3;
    vec3 spec = vec3(0, 0, 0);

    // directional light
    float shadowFactor = 1 - shadowCalculation();
    //shadowFactor = 1;

    vec3 viewDir = normalize(fragPos - viewPos);

    diff += shadowFactor * diffuse(lightDir, normal);
    spec += shadowFactor * specular(lightDir, normal, viewDir, 8);

    vec4 texColor = texture(tex, texCoord * textureScale);
    outColor = texColor * vec4(diff, 1.0) + vec4(spec, 1.0);

    //float depth = gl_FragCoord.z;
    //outColor = vec4(vec3(depth),1.0);

    //outColor = vec4(vec3(shadowFactor),1.0);
}
