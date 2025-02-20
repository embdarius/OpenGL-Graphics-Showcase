#version 410 core

layout(location = 0) in vec3 vertexPosition;  // Vertex position input
layout(location = 1) in vec3 vertexColour;   // Vertex color input

uniform mat4 model;      // Model matrix (for object transformations)
uniform mat4 view;       // View matrix (camera transformation)
uniform mat4 projection; // Projection matrix (perspective)

uniform vec3 lightColor; // Light color (this will affect the color of the light)
out vec3 color;          // Output color of the fragment

float ambientStrength = 0.1;  // Strength of the ambient light (control how much the light affects the color)

void main() {
    // Compute the ambient light component based on the light color and ambient strength
    vec3 ambient = ambientStrength * lightColor;

    // Combine the ambient light with the vertex color (scaled)
    color = min(ambient + vertexColour, 1.0);  // Ensure the color doesn't exceed (1.0, 1.0, 1.0)

    // Compute the final vertex position after transformations (from model, view, and projection matrices)
    gl_Position = projection * view * model * vec4(vertexPosition, 1.0);
}
