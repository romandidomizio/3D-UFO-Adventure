#version 120

varying vec3 FragPos;
varying vec3 Normal;
varying vec2 TexCoords;
varying vec3 TangentLightPos;
varying vec3 TangentViewPos;

attribute vec3 tangent;

uniform mat4 model;
uniform mat4 modelInverseTranspose;
uniform mat4 view;
uniform mat4 projection;
uniform vec3 lightPos;
uniform vec3 viewPos;

void main()
{
    // Pass texture coordinates
    TexCoords = gl_MultiTexCoord0.xy;
    
    // Calculate vertex position in world space
    vec4 worldPosition = model * gl_Vertex;
    FragPos = worldPosition.xyz;

    // Calculate normal and tangent vectors in world space
    Normal = mat3(modelInverseTranspose) * gl_Normal;
    vec3 T = normalize(mat3(model) * tangent);

    // Calculate the bitangent
    vec3 B = cross(Normal, T);

    // Create TBN matrix
    mat3 TBN = transpose(mat3(T, B, Normal));

    // Transform light position and view position to tangent space
    TangentLightPos = TBN * (lightPos - FragPos);
    TangentViewPos = TBN * (viewPos - FragPos);

    // Final vertex position in clip space
    gl_Position = projection * view * worldPosition;
}
