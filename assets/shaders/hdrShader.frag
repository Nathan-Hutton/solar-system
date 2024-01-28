#version 330 core

out vec4 fragColor;
in vec2 texCoords;

// This is everything we've rendered in the scene in the framebuffer
uniform sampler2D theTexture;

void main()
{ 
    fragColor = texture(theTexture, texCoords);
}
