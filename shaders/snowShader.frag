#version 330 core

in vec2 TexCoord; // Texture coordinates from the vertex shader

out vec4 FragColor; // Final output color

uniform sampler2D snowTexture; // The texture sampler

void main()
{
    // Sample the texture
    vec4 texColor = texture(snowTexture, TexCoord);

    // If alpha is less than 0.1, discard the fragment
    if (texColor.a < 0.1) {
        discard; // Discard the fragment
    }

    // Otherwise, output the color
    FragColor = texColor;
}