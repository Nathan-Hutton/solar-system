#version 330 core

out vec4 fragColor;
in vec2 texCoords;

// This is everything we've rendered in the scene in the framebuffer
uniform sampler2D theTexture;
uniform sampler2D blurTexture;

void main()
{ 
    const float gamma = 2.2;
    vec3 hdrColor = texture(theTexture, texCoords).rgb;
    vec3 bloomColor = texture(blurTexture, texCoords).rgb;
    hdrColor += bloomColor; // Add the blur on top of the default color

    vec3 toneMapped = hdrColor / (hdrColor + vec3(1.0));
    toneMapped = pow(toneMapped, vec3(1.0 / gamma));
    fragColor = vec4(toneMapped, 1.0);
}
