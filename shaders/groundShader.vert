#version 330 core

layout(location = 0) in vec3 position;  // Position attribute
layout(location = 1) in vec3 color;     // Color attribute
layout(location = 2) in vec2 texCoord;  // Texture coordinate attribute

out vec3 ourColor;  // Pass the color to the fragment shader
out vec2 TexCoord;  // Pass the texture coordinates to the fragment shader

uniform mat4 model;       // Model matrix (transformation from object space to world space)
uniform mat4 view;        // View matrix (camera's transformation)
uniform mat4 projection;  // Projection matrix (perspective or orthographic projection)
uniform mat3 normalMatrix;  // Normal matrix (if needed for lighting)

void main() {
    // Apply model, view, and projection transformations to the vertex position
    gl_Position = projection * view * model * vec4(position, 1.0f);
    
    // Pass the color attribute to the fragment shader
    ourColor = color;  
    
    // Pass the texture coordinates to the fragment shader
    TexCoord = texCoord;  
}
