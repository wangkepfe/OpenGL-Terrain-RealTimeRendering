#version 330 core

out vec4 outColor;

in vec2 texCoord;
in vec3 normal;
in vec3 fragPos;

uniform vec3 viewPos;

uniform sampler2D tex;

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
	return max(dot(-lightDirec, normal), 0.0) * 5;
}

void main(void)
{
    vec3 diff = vec3(0, 0, 0);

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

    float cloudiness = 3.0;// the bigger , the clearer
    
    vec2 myCoord1 = texCoord;
    myCoord1 *= 5;
    myCoord1.y += unitime;

    vec2 myCoord2 = texCoord;
    myCoord2 *= 10;
    myCoord2.x += unitime;

    vec2 myCoord3 = texCoord;
    myCoord3 *= 20;
    myCoord3.x += unitime;
    myCoord3.y += unitime;

    float noiseVal = texture(tex, myCoord1).r + texture(tex, myCoord2).r / 2 + texture(tex, myCoord3).r / 4;

    noiseVal = clamp((noiseVal / 1.75 - (1.0 - 1.0 / cloudiness)) * cloudiness, 0.0, 1.0);

	float shade = clamp(1.4 - noiseVal * 0.65, 0.0, 1.0);

	diff *= shade;

    //float average = 0.2126 * diff.r + 0.7152 * diff.g + 0.0722 * diff.b;
	float average = (diff.r + diff.g + diff.b) / 3;
    vec3 aveColor = vec3(average);

	diff = mix(diff, aveColor, 0.5);

    outColor = vec4(diff, noiseVal);
}