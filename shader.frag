#version 330 core

out vec4 FragColor;
in vec2 TexCoord;
in vec3 Normal;
in vec3 FragPos;

uniform vec3 color; // Barva nastavená v programu

void main() {
    // Simulace základního osvětlení - barva + trocha stínu podle normály
    float lighting = max(dot(Normal, vec3(0.0, 0.0, 1.0)), 0.2);
    FragColor = vec4(color * lighting, 1.0);
}
