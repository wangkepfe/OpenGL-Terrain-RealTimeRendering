#version 330 core

out vec4 outColor;

in vec2 texCoord;
in vec3 normal;
in vec3 fragPos;
in vec3 mTangent;
in vec3 mBitangent;

uniform sampler2D tex;
uniform sampler2D texNormal;

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
	return max(dot(-lightDirec, normal), 0.0);
}

float specular(vec3 lightDirec, vec3 normal, vec3 viewDir, int ex)
{
	float spec;
	if(useBlinnPhong)
	{
		spec = pow (max( dot( normal , normalize( - lightDirec - viewDir) ), 0.0), ex * 2) * 2;
	}
	else
	{
		spec = pow (max( dot( normalize( reflect( lightDirec, normal ) ), -viewDir), 0.0), ex) * 2;
	}
	return spec;
}

void main(void)
{
	vec3 diff = vec3(0, 0, 0);
	vec3 spec = vec3(0, 0, 0);

    vec3 nMapTex = normalize( texture(texNormal, texCoord).rgb * 2.0 - 1.0);
    vec3 mNormal = nMapTex.r * 0.05 * mTangent + nMapTex.g * 0.05 * mBitangent + nMapTex.b * normal;
	//vec3 mNormal = normal;

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

				diff += diffuse(lightDirec, mNormal) * lightColor[i];
				spec += specular(lightDirec, mNormal, viewDir, phongExponetial) * lightColor[i];

				break;
			}
			case POINT_LIGHT_TYPE: //point + attenuation
			{
				vec3 lightDirec = normalize(fragPos - lightPos[i]);
				vec3 viewDir = normalize(fragPos - viewPos);

				vec3 localDiff = diffuse(lightDirec, mNormal) * lightColor[i];
				vec3 localSpec = specular(lightDirec, normalize(normal), viewDir, phongExponetial) * lightColor[i];
				
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

				vec3 localDiff = diffuse(lightDirec, mNormal) * lightColor[i];
				vec3 localSpec = specular(lightDirec, normalize(normal), viewDir, phongExponetial) * lightColor[i];

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

	vec4 outTex = texture(tex, texCoord);

	outColor = vec4(diff, 1.0) * outTex + vec4(spec, 1.0);
}
