#version 150

out vec4 outColor;
out vec4 outDepth;

in vec4 clipPosition;

in vec2 texCoord;
in vec3 fragPos;

in vec3 transformedNormal;

uniform sampler2D tex0;
uniform sampler2D tex1;
uniform sampler2D tex2;
uniform sampler2D tex3;
uniform sampler2D texBump;
uniform sampler2D texs;

uniform vec3 viewpos;

uniform float heightRamp[6];

uniform int objType;
/*
 * 0 : skybox
 * 1 : terrain
 * 2 : only diffuse
 * 3 : diffuse and specular
 * 4 : water
 * 5 : with glow map
 */
uniform int useBumpMapping;
vec2 bumpMappingTexCoord;

uniform vec3 lightColorsArr[4];
uniform vec3 lightPosArr[4];
uniform vec3 lightDirArr[4];
uniform float lightStrengthArr[4];
uniform float lightDistanceArr[4];
uniform int lightTypeArr[4];

uniform float time;
uniform vec3 skyColor;
float waterAlpha;
uniform int useFog;
uniform int enviromentType;

vec3 bumpMapping()
{
    // this is world coord bump
    vec4 texcolor = texture(texBump, bumpMappingTexCoord);
    return vec3(texcolor.r * 2.0f - 1.0f, texcolor.b * 2.0f - 1.0f, texcolor.g * 2.0f - 1.0f);
}

vec3 ambientLight(vec3 color, float strength)
{
    return strength * color;
}

vec3 diffuseShading(vec3 color, vec3 pos, float strength, int type)
{
    vec3 lightDir;
    vec3 normal = normalize(transformedNormal);
    
    if(useBumpMapping == 1)
    {
        normal += bumpMapping();
        normal = normalize(normal);
    }
    
    if(type == 2)//directional
        lightDir = -normalize(pos);
    else
        lightDir = normalize(pos - fragPos);
    
    return max(dot(normal, lightDir), 0) * color * strength;
}
vec3 specularShading(vec3 color, vec3 pos, float expo, float strength, int type)
{
    vec3 normal = normalize(transformedNormal);
    
    if(useBumpMapping == 1)
    {
        normal += bumpMapping();
        normal = normalize(normal);
    }
    
    //reflectDir
    vec3 lightDir;
    if(type == 2)//directional
        lightDir = normalize(pos);
    else
        lightDir = normalize(pos - fragPos);
    vec3 reflectDir = lightDir - 2.0f * dot(lightDir, normal) * normal;
    
    //viewDir
    vec3 viewDir = normalize(viewpos - fragPos);
    
    //phong
    return pow(max(dot(viewDir, reflectDir), 0.0f), expo) * strength * color;
}
//point light
float pointLightAttenuate(vec3 lightPos, float lightDistance)
{
    float dis = distance(lightPos, fragPos);
    if(dis > lightDistance)
        return 0;
    
    float Kconst = 1.0f, Klinear = 0.007f, Kquad = 0.0002f;//for 1000
    return 1.f / (Kconst + Klinear * dis + Kquad * dis * dis);
    
}
//spot light
float spotLightAttenuate(vec3 lightPos, float lightDistance, vec3 direction, float angle)
{
    float dis = distance(lightPos, fragPos);
    if(dis > lightDistance)
        return 0;
    
    float innerCut = cos(radians(angle - 5.0f));
    float outerCut = cos(radians(angle + 5.0f));
    
    vec3 apexToTestPoint = fragPos - lightPos;
    float cosTheta = dot(apexToTestPoint, direction) / length(apexToTestPoint) / length(direction);//test point
    
    if(cosTheta > innerCut)
        return pointLightAttenuate(lightPos, lightDistance);
    else if(cosTheta < outerCut)
        return 0;
    else
        //return (cosTheta - outerCut) / (innerCut - outerCut) * pointLightAttenuate(lightPos, lightDistance);
        return pointLightAttenuate(lightPos, lightDistance) * (cosTheta - outerCut) / (innerCut - outerCut);
}
//fog
vec4 fogMix(vec4 originalColor)
{
    float density = 0.01f;
    float gradient = 0.5f;
    vec3 fogColor = skyColor;
    
    float dis = distance(fragPos, viewpos);
    float visibility = exp( - pow((dis * density), gradient));
    visibility = clamp(visibility, 0.0f, 1.0f);
    
    return mix(vec4(fogColor, 1.0f), originalColor, visibility);
}

vec4 fogSkyBlend(vec4 originalColor)
{
    vec3 fogColor = skyColor;
    
    float blendFactor = fragPos[1];
    blendFactor = clamp(blendFactor, 0.0, 1.0);
    
    return mix(vec4(fogColor, 1.0), originalColor, blendFactor);
}

vec4 waterEffect()
{
    // depth part
    vec2 texCoord1 = (clipPosition.xy / clipPosition.w + 1.0) * 0.5;// original texcoords
    vec2 texCoord2 = vec2(texCoord1.x, -texCoord1.y);
    
    float near = 0.2;
    float far = 1500.0;
    vec4 depthColor = texture(tex3, texCoord1);
    float floorDistance = depthColor.r;
    float waterDistance = distance(fragPos, viewpos) * 0.002;
    float waterDepth = floorDistance - waterDistance;
    
    waterAlpha = clamp(exp(waterDepth+3) - exp(3), 0.0, 1.0);
    float distortAttenuate = clamp(exp(waterDepth+3) - exp(3), 0.0, 1.0);
    
    // du dv distort
    const float waveScale = 0.1f;
    const float waveSpeed = 0.03f;
    vec2 distort = texture(tex2, vec2(texCoord.x / waveScale + time * waveSpeed, texCoord.y / waveScale) ).rg * distortAttenuate;
    
    // normal part
    const float normalDisLength = 0.1f;
    
    vec2 normalDistort = distort * normalDisLength * distortAttenuate;//range x:(0~, wave~) y:(0, wave)
    vec2 distortedCoordforNormal = vec2(texCoord.x / waveScale + normalDistort.x,  texCoord.y / waveScale + normalDistort.y + time * waveSpeed); // x range (0,1) + (0~, wave~) // y (0,1) + (0,wave) + ~~~
    bumpMappingTexCoord = distortedCoordforNormal;
    
    // tex part 
    const float texDisLength = 0.03;
    vec2 texdistort = distort * 2.0 - 1.0;// range (0,1) to (-1,-1)
    texdistort *= texDisLength * distortAttenuate;// range (-wave, +wave)
    
    vec2 distexCoord1 = clamp( texCoord1 + texdistort, 0.001, 0.999);// distorted texcoords
    vec2 distexCoord2 = texCoord2 + texdistort;
    
    distexCoord2.x = clamp( distexCoord2.x, 0.001, 0.999);
    distexCoord2.y = clamp( distexCoord2.y, -0.999, -0.001);
    
    vec4 refrationColor = texture(tex0, distexCoord1);// get rendered textures
    vec4 reflectionColor = texture(tex1, distexCoord2);
    
    float factor = dot(normalize(fragPos - viewpos), normalize(bumpMapping() + vec3(0,1,0)));// frensil mix factor
    factor = pow(factor, 0.6);
    
    // final part
    vec4 final;
    /*if(reflectionColor.r > 0.5
       && reflectionColor.r < 0.6
       && reflectionColor.g > 0.5
       && reflectionColor.g < 0.6
       && reflectionColor.b > 0.5
       && reflectionColor.b < 0.6)
    {
        reflectionColor = vec4(0.01,0.1,0.06,1);
    }*/
    refrationColor = mix(refrationColor, vec4(0.0, 0.05, 0.03, 1.0), waterAlpha * 0.2);
    final = mix(reflectionColor, refrationColor, factor);
    final = mix(final, vec4(0.0, 0.5, 0.3, 1.0), 0.1);// tint
    //final = vec4(vec3(clamp(exp(waterDepth+3) - exp(3), 0.0, 1.0)),1);
    //final = vec4(vec3(clamp(waterDepth * 10, 0.0, 1.0)),1);
    return final;
}

void main(void)
{
    float depth = distance(fragPos, viewpos);
    outDepth = vec4(depth * 0.002, 0, 0, 1);
    
    if(objType == 0)//skybox
    {
        vec4 finalColor = texture(texs, texCoord);
        if(enviromentType == 0)
        {
            finalColor *= 1.3;
        }
        else if(enviromentType == 1)
        {
            finalColor = (finalColor + vec4(1,1,1,1)) * 0.5;//add grayty
        }
        else if(enviromentType == 2)
        {
            finalColor = (finalColor + vec4(1,1,1,1)) * 0.5;//add grayty
            finalColor *= 0.6;
            finalColor = finalColor * 2 - vec4(1,1,1,0);
        }
        outColor = fogSkyBlend(finalColor);
        //outColor = vec4(skyColor,1);
        return;
    }
    
    bool glow = false;
    
    // texture
    vec4 tex = vec4(0, 0, 0, 0);
    
    if(objType == 1)
    {
        //multi texture terrain
        if(fragPos[1] <= heightRamp[0])
        {
            tex += texture(tex0, texCoord);
        }
        else if(heightRamp[0] <= fragPos[1] && fragPos[1] < heightRamp[1])
        {
            tex += mix(texture(tex0, texCoord), texture(tex1, texCoord), (fragPos[1] -heightRamp[0]) / (heightRamp[1] - heightRamp[0]));
        }
        else if(heightRamp[1] <= fragPos[1] && fragPos[1] < heightRamp[2])
        {
            tex += texture(tex1, texCoord);
        }
        else if(heightRamp[2] <= fragPos[1] && fragPos[1] < heightRamp[3])
        {
            tex += mix(texture(tex1, texCoord), texture(tex2, texCoord), (fragPos[1] - heightRamp[2]) / (heightRamp[3] - heightRamp[2]));
        }
        else if(heightRamp[3] <= fragPos[1] && fragPos[1] < heightRamp[4])
        {
            tex += texture(tex2, texCoord);
            
        }
        else if(heightRamp[4] <= fragPos[1] && fragPos[1] < heightRamp[5])
        {
            tex += mix(texture(tex2, texCoord), texture(tex3, texCoord), (fragPos[1] - heightRamp[4]) / (heightRamp[5] - heightRamp[4]));
        }
        else if(heightRamp[5] <= fragPos[1])
        {
            tex += texture(tex3, texCoord);
        }
    }
    else if(objType == 4) // water
    {
        tex = waterEffect();
    }
    else if(objType == 5) // glow map
    {
        tex = texture(texs, texCoord);
        glow = texture(tex1, texCoord).r > 0.9;
    }
    else//object
    {
        tex = texture(texs, texCoord);
    }
    
    //lighting
    vec3 diffuseLight = vec3(0,0,0);
    vec3 specularLight = vec3(0,0,0);
    
    int i = 0;
    
    if(glow)
    {
        diffuseLight = vec3(1,1,1);
    }
    else
    {
        for(i = 0; i < 4; i++)
        {
            if(lightTypeArr[i] == 1)//ambient
            {
                if(objType == 4)
                    diffuseLight += vec3(1, 1, 1);
                else
                    diffuseLight += ambientLight(lightColorsArr[i], lightStrengthArr[i]);
                
            }
            
            if(lightTypeArr[i] == 2)//directional
            {
                if(objType != 4)
                    diffuseLight += diffuseShading(lightColorsArr[i], lightDirArr[i], lightStrengthArr[i], 2);
                
                if(objType == 3 || objType == 4 || objType == 5)//specular
                    specularLight += specularShading(lightColorsArr[i], lightDirArr[i], 8, lightStrengthArr[i] * 0.4f, 2);
            }
            
            if(lightTypeArr[i] == 3)//point
            {
                float at = pointLightAttenuate(lightPosArr[i], lightDistanceArr[i]);
                if(objType != 4)
                    diffuseLight += at * diffuseShading(lightColorsArr[i], lightPosArr[i], lightStrengthArr[i], 1);
                if(objType == 3 || objType == 4 || objType == 5)//specular
                    specularLight += at * specularShading(lightColorsArr[i], lightDirArr[i], 16, lightStrengthArr[i], 1);
            }
            
            if(lightTypeArr[i] == 4)//spot
            {
                float at = spotLightAttenuate(lightPosArr[i], lightDistanceArr[i], lightDirArr[i], 30);
                if(objType != 4)
                    diffuseLight += at * diffuseShading(lightColorsArr[i], lightPosArr[i], lightStrengthArr[i], 1);
                if(objType == 3 || objType == 4 || objType == 5)//specular
                    specularLight += at * specularShading(lightColorsArr[i], lightDirArr[i], 16, lightStrengthArr[i], 1);
            }
        }
    }
    
    // blend tex with lighting
    vec4 finalColor = vec4(diffuseLight, 1.0f) * tex + vec4(specularLight, 1.0f);
    
    // blend with fog
    if(useFog == 1 && objType != 4 && !glow)
        outColor = fogMix(finalColor);
    else
        outColor = finalColor;
    
    // alpha
    if(objType == 4)
    {
        //outColor.a = clamp(waterDepth * 10, 0.0, 1.0);
        outColor.a = waterAlpha;
        //outColor.a = 1;
    }
    
    //outColor = vec4(vec3(depth * 0.001), 1);
}
