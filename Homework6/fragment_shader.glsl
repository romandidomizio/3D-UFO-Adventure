// fragment_shader.glsl
#version 120

varying vec3 FragPos;
varying vec3 Normal;
varying vec2 TexCoords;
varying vec3 TangentLightPos;
varying vec3 TangentViewPos;

uniform sampler2D diffuseMap;
uniform sampler2D normalMap;

void main()
{
    // Obtain normal from normal map, and transform from [0,1] to [-1,1]
    vec3 normal = texture2D(normalMap, TexCoords).rgb;
    normal = normalize(normal * 2.0 - 1.0);

    // If the normal map is not loaded correctly, the normal vector could be (0, 0, 0)
    // which would cause issues in lighting calculations. To debug, add a fallback.
    if (length(normal) < 0.1) {
        normal = vec3(0.0, 0.0, 1.0); // Default to pointing up if the normal map is incorrect
    }

    // Calculate lighting
    vec3 lightDir = normalize(TangentLightPos);
    vec3 viewDir = normalize(TangentViewPos);
    float diff = max(dot(normal, lightDir), 0.0);

    // Calculate ambient, diffuse and specular components
    vec3 diffuse = diff * texture2D(diffuseMap, TexCoords).rgb;

    // Combine results
    vec3 result = diffuse;
    gl_FragColor = vec4(result, 1.0);
}
