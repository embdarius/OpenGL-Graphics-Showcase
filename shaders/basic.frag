#version 410 core

in vec3 fPosition;
in vec3 fNormal;
in vec2 fTexCoords;

in vec4 fPosEye;
in vec4 fragPosLightSpace;

out vec4 fColor;

// Matrices
uniform mat4 model;
uniform mat4 view;
uniform mat3 normalMatrix;

// Directional Light
uniform vec3 lightDir;     // Direction for the directional light source
uniform vec3 lightColor;   // Color for the directional light
uniform sampler2D shadowMap;

// Point Light
uniform vec3 pointLightPosition;  // Position of the point light
uniform vec3 pointLightColor;     // Color of the point light
uniform float pointLightIntensity; // Intensity of the point light
uniform float pointLightRadius;    // Effective radius of the point light

uniform float thunderIntensity;
uniform int fogEnabled;

// Textures
uniform sampler2D diffuseTexture;
uniform sampler2D specularTexture;

// Components
vec3 ambient;
float ambientStrength = 0.2f;
vec3 diffuse;
vec3 specular;
float specularStrength = 0.5f;
float shininess = 32.0f;

// Hardcoded fog parameters
vec3 fogColor = vec3(0.8f, 0.8f, 0.8f);    // Light gray fog color
float fogStart = 4.0f;                     // Fog starts at 4 units distance
float fogEnd = 50.0f;                      // Fog fully visible after 50 units distance


// 4 point lights
// Uniform arrays for point lights
uniform vec3 pointLightPositions[4];   // Array of point light positions (4 lights)
uniform vec3 pointLightColors[4];       // Array of point light colors (4 lights)
uniform float pointLightIntensities[4]; // Array of point light intensities (4 lights)
uniform float pointLightRadiuses[4];      // Array of point light radii (4 lights)
uniform vec3 pointLightDirections[4];


void computeDirectionalLightComponents() {
    vec3 cameraPosEye = vec3(0.0f); // Camera position (in eye space, at origin)
    
    // Transform normal
    vec3 normalEye = normalize(fNormal);    
    
    // Compute light direction
    vec3 lightDirN = normalize(lightDir);
    
    // Compute view direction 
    vec3 viewDirN = normalize(cameraPosEye - fPosEye.xyz);
        
    // Compute ambient light
    ambient = ambientStrength * lightColor;
    
    // Compute diffuse light
    diffuse = max(dot(normalEye, lightDirN), 0.0f) * lightColor * thunderIntensity;
    
    // Compute specular light
    vec3 reflection = reflect(-lightDirN, normalEye);
    float specCoeff = pow(max(dot(viewDirN, reflection), 0.0f), shininess);
    specular = specularStrength * specCoeff * lightColor;
}

vec3 computePointLight(vec3 fragPos, vec3 normal) {
    vec3 totalLight = vec3(0.0f);

    // Iterate through all point lights
    for (int i = 0; i < 4; ++i) {
        // Direction from fragment to the light source
        vec3 lightDir = normalize(pointLightDirections[i]);

        // Distance from the fragment to the light source
        float distance = length(pointLightPositions[i] - fragPos);

        // Compute attenuation based on the light's radius
        float attenuation = clamp(1.0 - distance / pointLightRadiuses[i], 0.0, 1.0);

        // Diffuse shading
        float diff = max(dot(normal, lightDir), 0.0);
        vec3 diffuse = diff * pointLightColors[i] * pointLightIntensities[i];

        // Specular shading (Phong model)
        vec3 viewDir = normalize(-fragPos); // Assuming the camera is at (0,0,0) in eye space
        vec3 reflectDir = reflect(-lightDir, normal);
        float spec = pow(max(dot(viewDir, reflectDir), 0.0), shininess);
        vec3 specular = spec * pointLightColors[i] * specularStrength;

        // Combine diffuse and specular, modulated by attenuation
        totalLight += attenuation * (diffuse + specular);
    }
    return totalLight;
}

float computeShadow() { 
    vec3 normalizedCoords = fragPosLightSpace.xyz / fragPosLightSpace.w;
    normalizedCoords = normalizedCoords * 0.5 + 0.5;
    float closestDepth = texture(shadowMap, normalizedCoords.xy).r;
    float currentDepth = normalizedCoords.z;
    float bias = max(0.05f * (1.0f - dot(fNormal, lightDir)), 0.005f);
    float shadow = currentDepth - bias > closestDepth ? 1.0f : 0.0f;
    if (normalizedCoords.z > 1.0f)
        return 0.0f;
    return shadow;
}

// Function to calculate the fog factor based on distance
float calculateFogFactor(float distance) {
    float fogFactor = (distance - fogStart) / (fogEnd - fogStart);
    fogFactor *= 2.0f;
    fogFactor = clamp(fogFactor, 0.0f, 1.0f); 
    return fogFactor;
}

void main() {
    // Initialize lighting components
    computeDirectionalLightComponents();
    
    // Compute point light contribution
    vec3 pointLight = computePointLight(fPosEye.xyz, normalize(fNormal));
    
    // Combine the light sources
    float shadow = computeShadow();
    vec3 totalLight = ambient + (1.0f - shadow) * (diffuse + specular) + pointLight;
    
    // Apply textures to lighting components
    vec3 baseColor = vec3(0.9f, 0.35f, 0.0f); // Example base color (orange)
    totalLight *= texture(diffuseTexture, fTexCoords).rgb;
    
    // Calculate distance from the camera to the fragment
    float distance = length(fPosEye.xyz);  // Assuming fPosEye is in eye space

    // Compute the fog factor based on the distance
    float fogFactor = calculateFogFactor(distance);

    // Interpolate between object color and fog color
    vec3 finalColor = mix(totalLight, fogColor, fogFactor);

    // Output the final color
    if (fogEnabled == 1){
        fColor = vec4(finalColor, 1.0f);
    }
    else{
        fColor = vec4(totalLight, 1.0f);
    }
     
    
}
