#version 330 core

out vec4 fragColor;
in vec2 texCoords;

// This is everything we've rendered in the scene in the framebuffer
uniform sampler2D theTexture;
uniform float gamma;

void main()
{ 
    vec3 fragment = texture(theTexture, texCoords).rgb;

    vec3 toneMapped = fragment / (fragment + vec3(1.0));
    toneMapped = pow(toneMapped, vec3(1.0 / gamma));
    fragColor = vec4(toneMapped, 1.0);
}
