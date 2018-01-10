#version 330 core

out vec4 outColor;

in vec3 normal;
in vec3 fragPos;

uniform samplerCube cubeTex;

uniform vec3 viewPos;

void main(void)
{
    vec3 mNormal = normalize(normal);

    // reflection
    vec3 I = normalize(fragPos - viewPos);
    vec3 R = reflect(I, mNormal);
    vec4 reflection = texture(cubeTex, R);

    // refraction
    float ratio = 1.00 / 1.52;
    //vec3 I = normalize(fragPos - viewPos);
    vec3 R2 = refract(I, mNormal, ratio);
    vec4 refraction = texture(cubeTex, R2);

    // frensil effect mix factor
    float factor = dot(I, mNormal);
    factor = pow(factor, 1.5);

    vec4 mixture = mix(reflection, refraction, factor);

    outColor = reflection;
    //outColor = refraction;
    //outColor = mixture;
}
