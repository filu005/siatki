#version 330 core
in vec3 outColor;

in vec3 Normal;
in vec3 FragPos;

out vec4 color;

void main()
{
	vec3 lightPos = vec3(-3.0, 2.0, -0.5);
	vec3 lightColor = vec3(1.0, 1.0, 1.0);
	// Diffuse
    vec3 norm = normalize(Normal);
    vec3 lightDir = normalize(lightPos - FragPos);
    float diff = max(dot(norm, lightDir), 0.0);
    vec3 diffuse = diff * lightColor;

    vec3 result = diffuse * outColor;

    color = vec4(result, 1.0);
}
