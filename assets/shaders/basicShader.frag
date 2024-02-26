#version 460 core

out vec4 fragColor;
in vec2 texCoords;

// This is everything we've rendered in the scene in the framebuffer
uniform sampler2D theTexture;

void main()
{ 
    const float gamma = 2.2;
    vec3 color = texture(theTexture, texCoords).rgb;

    fragColor = vec4(texture(theTexture, texCoords).rgb, 1.0);
}
