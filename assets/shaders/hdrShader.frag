#version 330 core

out vec4 fragColor;
in vec2 texCoords;

// This is everything we've rendered in the scene in the framebuffer
uniform sampler2D theTexture;
uniform float gamma;

void main()
{ 
    vec4 fragment = texture(theTexture, texCoords);
    fragColor.rgb = pow(fragment.rgb, vec3(1.0f / gamma));
}
