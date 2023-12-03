#version 330

// input texture
uniform sampler2D theTexture;
uniform sampler2D brightTexture;


in vec4 vCol;
in vec2 texCoord;

out vec4 color;

void main()
{    
    // Sample the scene and bright textures
    vec3 sceneColor = texture(theTexture, texCoord).rgb;
    vec3 brightColor = texture(brightTexture, texCoord).rgb;

    // Combine the scene and bright textures
    vec3 finalColor = sceneColor + brightColor;

    // Output the final color
    color = vec4(finalColor, 1.0);

    //color = texture(theTexture, texCoord);
}
