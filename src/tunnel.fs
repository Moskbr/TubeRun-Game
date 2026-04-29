#version 330 core
in vec3 FragPos;
in vec3 Normal;
in vec2 TexCoords;

out vec4 FragColor;

uniform vec3 lightPos;    // Position of the light source (ex: camera)
uniform vec3 viewPos;  // Color of the light source (ex: white vec3(1.0, 1.0, 1.0))

void main()
{
    // Tunnel color (can be a texture or a solid color)
    vec3 color = vec3(0.4 + 0.6*sin(TexCoords.y*10.0), 0.8, 0.5);
    // Ambient
    float ambientStrength = 0.2; // Adjust as needed
    vec3 ambient = ambientStrength * color;

    // Diffuse (based on Lambert's cosine law, due to the inclination of the tunnel walls)
    vec3 norm = normalize(Normal); // if illumination is inverted, use -Normal
    vec3 lightDir = normalize(lightPos - FragPos);

    float diff = max(dot(norm, lightDir), 0.0);
    vec3 diffuse = diff * color;

    // Combine results
    vec3 result = ambient + diffuse; // or times color if you want to modulate the color by the lighting
    // mix UVs to visualize error aligment(optional)
    // result = mix(result, vec3(TexCoords, 1.0), 0.1);
    FragColor = vec4(result, 1.0);
}