#version 330 core

in vec2 TexCoord;  // Texture coordinates passed from the vertex shader
out vec4 FragColor;  // Final fragment color

uniform sampler2D groundTexture;  // The texture of the water
uniform float time;  // Time uniform to animate the texture

void main() {
    // Speed and frequency of the sine wave movement
    float speed = 0.1;  // Adjust this for the speed of the wave
    float frequency = 1.0;  // Adjust this for the frequency of the wave oscillation

    // Create a sine wave effect on the x-axis using time
    float sineOffset = sin(time * frequency) * 0.1;  // The sine wave oscillates over time, adjust the amplitude with 0.1

    // Modify the texture coordinates to create the sine wave movement
    vec2 animatedTexCoord = TexCoord + vec2(sineOffset, 0.0f);

    // Sample the texture at the modified coordinates
    FragColor = texture(groundTexture, animatedTexCoord);
}
