#version 330 core

out vec4 outColor;

in vec2 texCoord;
in vec3 normal;
in vec3 fragPos;

uniform sampler2D texClouds;
uniform sampler2D texCloudsN;

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

uniform float unitime;

float diffuse(vec3 lightDirec, vec3 normal)
{
	return max(dot(-lightDirec, normal), 0.0) * 1;
}

void main(void)
{
	vec3 diff = vec3(0, 0, 0);

	vec4 vtex = texture(texClouds, texCoord);

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

				diff += diffuse(lightDirec, mNormal) * lightColor[i];

				break;
			}
			case POINT_LIGHT_TYPE: //point + attenuation
			{
				vec3 lightDirec = normalize(fragPos - lightPos[i]);
				vec3 viewDir = normalize(fragPos - viewPos);

				vec3 localDiff = diffuse(lightDirec, mNormal) * lightColor[i];
				
				// attenuation
				float Kc = 1.0, Kl = 0.0014, Kq = 0.000007;
				float dis = length(fragPos - lightPos[i]);
				float intensity = 1.0 / ( Kc + Kl * dis + Kq * dis * dis);

				diff += intensity * localDiff;

				break;
			}
			case SPOT_LIGHT_TYPE: //spot + attenuation
			{
				vec3 lightDirec = normalize(fragPos - lightPos[i]);
				vec3 viewDir = normalize(fragPos - viewPos);

				vec3 localDiff = diffuse(lightDirec, mNormal) * lightColor[i];

				// spot limited
				float theta = dot(lightDirec, normalize(-lightDir[i]));
				float intensity = clamp((theta - radians(60.0)) / (radians(40.0) - radians(60.0)), 0.0, 1.0);  

				// attenuation
				float Kc = 1.0, Kl = 0.0014, Kq = 0.000007;
				float dis = length(fragPos - lightPos[i]);
				intensity = 1.0 / ( Kc + Kl * dis + Kq * dis * dis);

				diff += intensity * localDiff;

				break;
			}
			default:
				break;
		}
	}

    
    // noise world coordinate
	vec2 worldCoords = fragPos.xz * 0.001;
	worldCoords.y += unitime;
	float noiseVal = texture(texClouds, worldCoords).r * 40 - 20;
	
	// color
	worldCoords = fragPos.xz * 0.001;
	worldCoords.x += unitime;
	vec3 cloudColor = vec3(1, 1, 1) * clamp(texture(texClouds, worldCoords).r + 0.8, 0.0, 1.0);

	// alpha blending
	float height = fragPos.y;

	float alphaRamp1 = 40.0 + noiseVal;
    float alphaRamp2 = 50.0 + noiseVal;
    float alphaRamp3 = 90.0 + noiseVal;	

	float alp;

    if(height < alphaRamp1)
    {
        alp = 0.0;
		cloudColor *= 0.8;
    }
    else if(height < alphaRamp2)
    {
        alp = (height - alphaRamp1) / (alphaRamp2 - alphaRamp1);

		cloudColor *= 0.8 + 0.2 * alp;
    }
    else if(height < alphaRamp3)
    {
        alp = (alphaRamp3 -  height) / (alphaRamp3 - alphaRamp2);
    }
    else
    {
        alp = 0.0;
    }

	// outcolor
	outColor = vec4(diff, 1.0) * vec4(cloudColor, 1.0);

    outColor.a = alp * vtex.r * 1.2;
}
