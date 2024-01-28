#version 330 core

out vec4 fragColor;
in vec2 texCoords;

uniform sampler2D screenTexture;

void main()
{ 
    vec3 hdrColor = texture(screenTexture, texCoords).rgb;
    //fragColor = vec4(hdrColor, 1.0);
    fragColor = texture(screenTexture, texCoords);
}
