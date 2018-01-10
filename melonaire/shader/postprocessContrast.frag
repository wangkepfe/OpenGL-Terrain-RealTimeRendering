#version 330 core
out vec4 FragColor;
  
in vec2 TexCoords;

uniform sampler2D screenTexture;

const float contrast = 0.1;

void main()
{ 
    FragColor = texture(screenTexture, TexCoords);

    FragColor.rgb = (FragColor.rgb - 0.5) * (1.0 + contrast) + 0.5;
}