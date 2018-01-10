#version 330 core
out vec4 out_color;
  
in vec2 pos;

uniform sampler2D dudvTex;
uniform sampler2D normalTex;
uniform sampler2D reflectionTex;
uniform sampler2D refractionTex;
uniform sampler2D depthTex;

uniform mat4 inverseProjection;
uniform mat4 inverseView;

uniform vec3 viewPos;

uniform float time;

uniform float waterLevel;

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

float specular(vec3 lightDirec, vec3 normal, vec3 viewDir, int ex)
{
	float spec;
	if(useBlinnPhong)
	{
		spec = pow (max( dot( normal , normalize( - lightDirec - viewDir) ), 0.0), ex * 2) * 0.3;
	}
	else
	{
		spec = pow (max( dot( normalize( reflect( lightDirec, normal ) ), -viewDir), 0.0), ex) * 0.3;
	}
	return spec;
}

vec3 getFragmentPos()
{
    // ray tracing
    vec4 ray_clip = vec4(pos, -1.0, 1.0);

    vec4 ray_eye = inverseProjection * ray_clip;
    ray_eye = vec4(ray_eye.xy, -1.0, 0.0);

    vec3 ray_world = (inverseView * ray_eye).xyz;
    ray_world = normalize(ray_world);

    // ray-plane intersection,    need improvement!!!

    // viewPos.y + ray.y * k = 0
    float k = - (viewPos.y - waterLevel) / ray_world.y;

    // viewPos.x + ray.x * k = x
    vec3 fragPos;
    fragPos.x = viewPos.x + ray_world.x * k;
    fragPos.y = waterLevel;
    fragPos.z = viewPos.z + ray_world.z * k;

    return fragPos;
}

vec3 addLighting(vec3 normal, vec3 fragPos)
{
	vec3 spec = vec3(0, 0, 0);

	int i = 0;
	for(; i < lightNum; i++)
	{
		switch(lightType[i])
		{
			case AMBIENT_LIGHT_TYPE: //ambient
			{
				break;
			}
			case DIRECTIONAL_LIGHT_TYPE: //directional
			{
				vec3 lightDirec = normalize(lightDir[i]);
				vec3 viewDir = normalize(fragPos - viewPos);

				spec += specular(lightDirec, normalize(normal), viewDir, 8) * lightColor[i];

				break;
			}
			case POINT_LIGHT_TYPE: //point + attenuation
			{
				vec3 lightDirec = normalize(fragPos - lightPos[i]);
				vec3 viewDir = normalize(fragPos - viewPos);

				vec3 localSpec = specular(lightDirec, normalize(normal), viewDir, 8) * lightColor[i];

				// attenuation
				float Kc = 1.0, Kl = 0.0014, Kq = 0.000007;
				float dis = length(fragPos - lightPos[i]);
				float intensity = 1.0 / ( Kc + Kl * dis + Kq * dis * dis);

				spec += intensity * localSpec;

				break;
			}
			case SPOT_LIGHT_TYPE: //spot + attenuation
			{
				vec3 lightDirec = normalize(fragPos - lightPos[i]);
				vec3 viewDir = normalize(fragPos - viewPos);

				vec3 localSpec = specular(lightDirec, normalize(normal), viewDir, 8) * lightColor[i];

				// spot limited
				float theta = dot(lightDirec, normalize(-lightDir[i]));
				float intensity = clamp((theta - radians(60.0)) / (radians(40.0) - radians(60.0)), 0.0, 1.0);  

				// attenuation
				float Kc = 1.0, Kl = 0.0014, Kq = 0.000007;
				float dis = length(fragPos - lightPos[i]);
				intensity = 1.0 / ( Kc + Kl * dis + Kq * dis * dis);

				spec += intensity * localSpec;

				break;
			}
			default:
				break;
		}
	}

	return spec;
}

void main()
{ 
    vec3 fragPos = getFragmentPos();
    vec2 texCoords;
    texCoords.x = fragPos.x / 200.0;
    texCoords.y = fragPos.z / 200.0;
    texCoords = (texCoords + 1.0) / 2.0;
    vec3 viewVec = viewPos - fragPos;

    vec2 screenTexCoord = (pos + 1.0) / 2.0;

	// depth calculation
	float depth = texture(depthTex, screenTexCoord).r;
	const float near = 0.2;
	const float far = 2000.0;
	float floorDistance = 2.0 * far * near / (far + near - (2.0 * depth - 1.0) * (far - near));
	depth = gl_FragCoord.z;
	float waterDistance = 2.0 * far * near / (far + near - (2.0 * depth - 1.0) * (far - near));
	float waterDepth = floorDistance - waterDistance;
	float alpha = clamp(waterDepth / 4.0 - 0.05, 0.0, 1.0);

    // distortion dudv map
    const float waveScale = 0.01;
    const float waveSpeed = 8.0;
    const float waveStrength = 0.03;
    vec2 distort = texture(dudvTex, vec2(texCoords.x / waveScale + time * waveSpeed, texCoords.y / waveScale) ).rg * 2.0 - 1.0;
    distort *= waveStrength * alpha;

    // get reflection & refraction texture
	vec2 reflectionTexCoord = screenTexCoord;
	reflectionTexCoord.y = 1.0 - reflectionTexCoord.y;
    vec4 reflection = texture(reflectionTex, reflectionTexCoord + distort);
    vec4 refraction = texture(refractionTex, screenTexCoord + distort);
	//vec4 reflection = texture(reflectionTex, reflectionTexCoord);
	//vec4 refraction = texture(refractionTex, screenTexCoord);
    refraction = mix(refraction, vec4(0, 0.3, 0.8, 1.0), 0.1);

    // fresnel effect mix factor
    float factor = dot(normalize(viewVec), vec3(0, 1, 0));
    factor = pow(factor, 0.6);

    // final mix
    vec4 fresnelMix = mix(reflection, refraction, clamp(factor, 0.0, 1.0));
	//vec4 fresnelMix = mix(reflection, refraction, 0.5);

    // normal map
    vec2 normalMapTexCoords = vec2(texCoords.x / waveScale, texCoords.y / waveScale + time * waveSpeed) + distort * 3.0;
	vec3 normalMap = texture(normalTex, normalMapTexCoords).rgb;
	vec3 normal = vec3(normalMap.r * 2.0 - 1.0, normalMap.b, normalMap.g * 2.0 - 1.0);

	vec3 lighting = addLighting(normal, fragPos);
    out_color = fresnelMix;
	out_color += vec4(lighting, 1.0);

	out_color.a = alpha;
}