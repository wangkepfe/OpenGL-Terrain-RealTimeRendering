#version 330 core

out vec4 outColor;

in vec2 texCoord;
in vec3 normal;
in vec3 fragPos;
in vec3 mTangent;
in vec3 mBitangent;
in vec4 fragPosLightingSpace;

uniform sampler2D texDirt;
uniform sampler2D texDirtN;
uniform sampler2D texDirtS;

uniform sampler2D texRock;
uniform sampler2D texRockN;
uniform sampler2D texRockS;

uniform sampler2D texSand;
uniform sampler2D texSandN;
uniform sampler2D texSandS;

uniform sampler2D texGrass;
uniform sampler2D texNoise;

uniform sampler2D texShadowMap;

uniform vec3 viewPos;

const int maxLightNum = 10;
uniform int lightNum;
uniform int lightType[maxLightNum];
uniform vec3 lightPos[maxLightNum];
uniform vec3 lightDir[maxLightNum];
uniform vec3 lightColor[maxLightNum];

#define AMBIENT_LIGHT_TYPE 0
#define POINT_LIGHT_TYPE 1
#define DIRECTIONAL_LIGHT_TYPE 2
#define SPOT_LIGHT_TYPE 3

const int phongExponetial = 8;
const bool useBlinnPhong = true;

float diffuse(vec3 lightDirec, vec3 normal)
{
	return max(dot(-lightDirec, normal), 0.0) * 1.0;
}

float specular(vec3 lightDirec, vec3 normal, vec3 viewDir, int ex)
{
	float spec;
	if(useBlinnPhong)
	{
		spec = pow (max( dot( normal , normalize( - lightDirec - viewDir) ), 0.0), ex * 2) * 0.5;
	}
	else
	{
		spec = pow (max( dot( normalize( reflect( lightDirec, normal ) ), -viewDir), 0.0), ex) * 0.5;
	}
	return spec;
}

float shadowCalculation()
{
	// perform perspective divide
    vec3 projCoords = fragPosLightingSpace.xyz / fragPosLightingSpace.w;
    // transform to [0,1] range
    projCoords = projCoords * 0.5 + 0.5;
    // get closest depth value from light's perspective (using [0,1] range fragPosLight as coords)
    float closestDepth = texture(texShadowMap, projCoords.xy).r; 
    // get depth of current fragment from light's perspective
    float currentDepth = projCoords.z;
    // check whether current frag pos is in shadow
	//float bias = max(0.05 * (1.0 - dot(normal, -lightDirec)), 0.005);
	float bias = 0.000;
    float shadow = currentDepth - bias > closestDepth  ? 1.0 : 0.0;

    return shadow;
}

void main(void)
{
	vec3 diff = vec3(0, 0, 0);
	vec3 spec = vec3(0, 0, 0);

	vec4 vtex;
	vec4 vtexN;
	vec4 vtexS;

	vec4 vtexDirt = texture(texDirt, texCoord);
	vec4 vtexDirtN = texture(texDirtN, texCoord);
	vec4 vtexDirtS = texture(texDirtS, texCoord);

	vec4 vtexRock = texture(texRock, texCoord);
	vec4 vtexRockN = texture(texRockN, texCoord);
	vec4 vtexRockS = texture(texRockS, texCoord);

	vec4 vtexSand = texture(texSand, texCoord);
	vec4 vtexSandN = texture(texSandN, texCoord);
	vec4 vtexSandS = texture(texSandS, texCoord);

	vec4 vtexGrass = texture(texGrass, texCoord);

	float noise_1 = texture(texNoise, vec2(fragPos.x, fragPos.z) * 0.4).r;
	float noise_2 = texture(texNoise, vec2(fragPos.x, fragPos.z + 5.5) * 0.5).r;

	float noise_3 = texture(texNoise, vec2(fragPos.x, fragPos.z) * 0.1).r;
	float noise_4 = texture(texNoise, vec2(fragPos.x, fragPos.z + 5.5) * 0.2).r;

	float height = fragPos.y;
	float heightStep1 = -0.5 + noise_1 * 0.3;
	float heightStep2 = 0 + noise_2 * 0.3;
	float heightStep3 = 0.75 + noise_1 * 0.5;
	float heightStep4 = 1.5 + noise_2 * 0.5;

	// grassify the dirt
	float grassiness = 1.2;// the bigger , the clearer
	float factor = clamp(((noise_3 + noise_4) / 3.0 - (1.0 - 1.0 / grassiness)) * grassiness, 0.0, 1.0);
	vtexDirt = mix(vtexDirt, vtexGrass, factor);
	

	if(height < heightStep1)
	{
		vtex = vtexSand;
		vtexN = vtexSandN;
		vtexS = vtexSandS;
	}
	else if(height < heightStep2)
	{
		float factor = (height - heightStep1) / (heightStep2 - heightStep1);
		vtex = mix(vtexSand, vtexDirt, factor);
		vtexN = mix(vtexSandN, vtexDirtN, factor);
		vtexS = mix(vtexSandS, vtexDirtS, factor);
	}
	else if(height < heightStep3)
	{
		vtex = vtexDirt;
		vtexN = vtexDirtN;
		vtexS = vtexDirtS;
	}
	else if(height < heightStep4)
	{
		float factor = (height - heightStep3) / (heightStep4 - heightStep3);
		vtex = mix(vtexDirt, vtexRock, factor);
		vtexN = mix(vtexDirtN, vtexRockN, factor);
		vtexS = mix(vtexDirtS, vtexRockS, factor);
	}
	else
	{
		vtex = vtexRock;
		vtexN = vtexRockN;
		vtexS = vtexRockS;
	}

    vec3 nMapTex = normalize( vtexN.rgb * 2.0 - 1.0);
    vec3 mNormal = nMapTex.r * mTangent + nMapTex.g * mBitangent + nMapTex.b * normal;

	mNormal = mix(normal, mNormal, 0.005);
	mNormal = normalize(mNormal);

	float specularStrength = clamp((vtexS.r - 0.5) * 2, 0.0, 1.0);

	// shadow mapping
	float shadowFactor = 1;
	int i = 0;
	/*for(; i < lightNum; i++)
	{
		if( lightType[i] == DIRECTIONAL_LIGHT_TYPE )
		{
			vec3 lightDirec = normalize(lightDir[i]);
			shadowFactor = shadowCalculation(mNormal, lightDirec);
		}
	}*/
	shadowFactor = 1 - shadowCalculation();

	i = 0;
	for(; i < lightNum; i++)
	{
		switch(lightType[i])
		{
			case AMBIENT_LIGHT_TYPE: //ambient
			{
				diff += lightColor[i] * shadowFactor;
				break;
			}
			case DIRECTIONAL_LIGHT_TYPE: //directional
			{
				vec3 lightDirec = normalize(lightDir[i]);
				vec3 viewDir = normalize(fragPos - viewPos);

				diff += diffuse(lightDirec, mNormal) * lightColor[i] * shadowFactor;
				spec += specular(lightDirec, mNormal, viewDir, phongExponetial) * lightColor[i] * shadowFactor;

				break;
			}
			case POINT_LIGHT_TYPE: //point + attenuation
			{
				vec3 lightDirec = normalize(fragPos - lightPos[i]);
				vec3 viewDir = normalize(fragPos - viewPos);

				vec3 localDiff = diffuse(lightDirec, mNormal) * lightColor[i];
				vec3 localSpec = specular(lightDirec, mNormal, viewDir, phongExponetial) * lightColor[i];
				
				// attenuation
				float Kc = 1.0, Kl = 0.0014, Kq = 0.000007;
				float dis = length(fragPos - lightPos[i]);
				float intensity = 1.0 / ( Kc + Kl * dis + Kq * dis * dis);

				diff += intensity * localDiff * shadowFactor;
				spec += intensity * localSpec * shadowFactor;

				break;
			}
			case SPOT_LIGHT_TYPE: //spot + attenuation
			{
				vec3 lightDirec = normalize(fragPos - lightPos[i]);
				vec3 viewDir = normalize(fragPos - viewPos);

				vec3 localDiff = diffuse(lightDirec, mNormal) * lightColor[i];
				vec3 localSpec = specular(lightDirec, mNormal, viewDir, phongExponetial) * lightColor[i];

				// spot limited
				float theta = dot(lightDirec, normalize(-lightDir[i]));
				float intensity = clamp((theta - radians(60.0)) / (radians(40.0) - radians(60.0)), 0.0, 1.0);  

				// attenuation
				float Kc = 1.0, Kl = 0.0014, Kq = 0.000007;
				float dis = length(fragPos - lightPos[i]);
				intensity = 1.0 / ( Kc + Kl * dis + Kq * dis * dis);

				diff += intensity * localDiff * shadowFactor;
				spec += intensity * localSpec * shadowFactor;

				break;
			}
			default:
				break;
		}
	}

	vec4 outTex = vtex;

	outColor = vec4(diff, 1.0) * outTex + vec4(spec, 1.0) * specularStrength;
}
