#version 330 core
out vec4 fragColor;

uniform sampler2D theTexture;

void main()
{
    vec2 vTexCoord = gl_FragCoord.xy*2 *(1.0/textureSize(theTexture, 0));

    // need to use textureOffset here
    vec3 col0 = textureOffset(theTexture, vTexCoord, ivec2( -2, 0 ) ).xyz;
    vec3 col1 = textureOffset(theTexture, vTexCoord, ivec2( 2, 0 ) ).xyz;
    vec3 col2 = textureOffset(theTexture, vTexCoord, ivec2( 0, -2 ) ).xyz;
    vec3 col3 = textureOffset(theTexture, vTexCoord, ivec2( 0, 2 ) ).xyz;

    vec3 col = (col0+col1+col2+col3) * 0.25;

    fragColor = vec4( col.xyz, 1.0 );
}
