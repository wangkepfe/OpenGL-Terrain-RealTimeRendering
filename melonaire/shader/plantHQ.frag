#version 330 core

out vec4 outColor;

in vec3 fragPos;
in vec3 normal;
in vec2 texCoord;

uniform sampler2D tex;

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

// plants have unique lighting

float diffuse(vec3 lightDirec, vec3 normal, vec3 viewDir)
{
	if(dot(lightDirec, normal) * dot(viewDir, normal) > 0)
	{
		// same direction
		return abs(dot(-lightDirec, normal)) * 0.8;
	}
	else
	{
		// different direction
		return abs(dot(-lightDirec, normal)) * 1.2;
	}
}

float specular(vec3 lightDirec, vec3 normal, vec3 viewDir, int ex)
{
	if(dot(lightDirec, normal) * dot(viewDir, normal) > 0)
	{
		// same direction
		// blinn
		return pow (max( dot( normal , normalize( - lightDirec - viewDir) ), 0.0), ex * 2);
	}
	else
	{
		// different direction
		return 0.0;
	}
}

void main(void)
{
	vec4 outTex = texture(tex, texCoord);
	if(outTex.a < 0.5)
	{
		discard;
	}

    vec3 diff = vec3(0, 0, 0);
	vec3 spec = vec3(0, 0, 0);

	vec3 mNormal = normal;

    int i = 0;
	for(; i < lightNum; i++)
	{
		switch(lightType[i])
		{
			case AMBIENT_LIGHT_TYPE: //ambient
			{
				diff += lightColor[i];
				break;
			}
			case DIRECTIONAL_LIGHT_TYPE: //directional
			{
				vec3 lightDirec = normalize(lightDir[i]);
				vec3 viewDir = normalize(fragPos - viewPos);

				diff += diffuse(lightDirec, mNormal, viewDir) * lightColor[i];
				spec += specular(lightDirec, mNormal, viewDir, phongExponetial) * lightColor[i];

				break;
			}
			case POINT_LIGHT_TYPE: //point + attenuation
			{
				vec3 lightDirec = normalize(fragPos - lightPos[i]);
				vec3 viewDir = normalize(fragPos - viewPos);

				vec3 localDiff = diffuse(lightDirec, mNormal, viewDir) * lightColor[i];
				vec3 localSpec = specular(lightDirec, mNormal, viewDir, phongExponetial) * lightColor[i];
				
				// attenuation
				float Kc = 1.0, Kl = 0.0014, Kq = 0.000007;
				float dis = length(fragPos - lightPos[i]);
				float intensity = 1.0 / ( Kc + Kl * dis + Kq * dis * dis);

				diff += intensity * localDiff;
				spec += intensity * localSpec;

				break;
			}
			case SPOT_LIGHT_TYPE: //spot + attenuation
			{
				vec3 lightDirec = normalize(fragPos - lightPos[i]);
				vec3 viewDir = normalize(fragPos - viewPos);

				vec3 localDiff = diffuse(lightDirec, mNormal, viewDir) * lightColor[i];
				vec3 localSpec = specular(lightDirec, mNormal, viewDir, phongExponetial) * lightColor[i];

				// spot limited
				float theta = dot(lightDirec, normalize(-lightDir[i]));
				float intensity = clamp((theta - radians(60.0)) / (radians(40.0) - radians(60.0)), 0.0, 1.0);  

				// attenuation
				float Kc = 1.0, Kl = 0.0014, Kq = 0.000007;
				float dis = length(fragPos - lightPos[i]);
				intensity = 1.0 / ( Kc + Kl * dis + Kq * dis * dis);

				diff += intensity * localDiff;
				spec += intensity * localSpec;

				break;
			}
			default:
				break;
		}
	}

	outColor = vec4(diff, 1.0) * outTex + vec4(spec, 1.0);
}
