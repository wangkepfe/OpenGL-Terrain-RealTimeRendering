#version 330 core

out vec4 outColor;

in vec2 texCoord;
in vec3 normal;
in vec3 fragPos;

uniform sampler2D tex;

uniform vec3 viewPos;

const int maxLightNum = 10;
uniform int lightNum;
uniform int lightType[maxLightNum];
uniform vec3 lightPos[maxLightNum];
uniform vec3 lightDir[maxLightNum];
uniform vec3 lightColor[maxLightNum];


/*
const float K20[3] = {1.0, 0.22, 0.2};
const float K50[3] = {1.0, 0.09, 0.032};
const float K100[3] = {1.0, 0.22, 0.2};
const float K200[3] = {1.0, 0.022, 0.0019};
const float K600[3] = {1.0, 0.007, 0.0002};
const float K3250[3] = {1.0, 0.0014, 0.000007};
*/

float diffuse(vec3 lightDirec, vec3 normal)
{
	return max(dot(-normalize(lightDirec), normalize(normal)), 0.0);
}

float specular(vec3 lightDirec, vec3 normal, vec3 viewDir, int ex)
{
	return pow (max( dot( normalize( reflect( normalize(lightDirec), normalize(normal) ) ), -normalize( viewDir)), 0.0), ex);
}

void main(void)
{
	vec3 diff = vec3(0, 0, 0);
	vec3 spec = vec3(0, 0, 0);

	int i = 0;
	for(; i < lightNum; i++)
	{
		switch(lightType[i])
		{
			case 0: //ambient
			{
				diff += lightColor[i];
				break;
			}
			case 1: //directional
			{
				vec3 lightDirec = lightDir[i];
				vec3 viewDir = fragPos - viewPos;

				diff += diffuse(lightDirec, normal) * lightColor[i];
				spec += specular(lightDirec, normal, viewDir, 8) * lightColor[i];

				break;
			}
			case 2: //point + attenuation
			{
				vec3 lightDirec = fragPos - lightPos[i];
				vec3 viewDir = fragPos - viewPos;

				diff += diffuse(lightDirec, normal) * lightColor[i];
				spec += specular(lightDirec, normal, viewDir, 8) * lightColor[i];

				// attenuation
				float Kc = 1.0, Kl = 0.0014, Kq = 0.000007;
				float dis = length(fragPos - viewPos);

				float intensity = 1.0 / ( Kc + Kl * dis + Kq * dis * dis);

				diff *= intensity;
				spec *= intensity;

				break;
			}
			case 3: //spot + attenuation
			{
				vec3 lightDirec = fragPos - lightPos[i];
				vec3 viewDir = fragPos - viewPos;

				diff += diffuse(lightDirec, normal) * lightColor[i];
				spec += specular(lightDirec, normal, viewDir, 8) * lightColor[i];

				// spot limited
				float theta     = dot(lightDirec, normalize(-lightDir[i]));
				float intensity = clamp((theta - radians(60.0)) / (radians(40.0) - radians(60.0)), 0.0, 1.0);  

				// attenuation
				float Kc = 1.0, Kl = 0.0014, Kq = 0.000007;	
				float dis = length(fragPos - viewPos);

				intensity = intensity * 1.0 / ( Kc + Kl * dis + Kq * dis * dis);

				diff *= intensity;
				spec *= intensity;

				break;
			}
			default:
				break;
		}
	}

	vec4 outTex = texture(tex, texCoord);

	outColor = vec4(diff, 1.0) * outTex + vec4(spec, 1.0);
}
