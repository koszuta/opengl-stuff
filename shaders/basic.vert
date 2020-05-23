#version 460

layout(location = 0) in vec3 in_Position;
layout(location = 1) in vec3 in_Normal;

uniform mat4 mvp;
uniform mat4 model;
uniform vec3 eye;

uniform vec3 color;

uniform float ambientMag;
uniform vec3 lightDir;
uniform float lightMag;
uniform vec3 lightColor;
uniform vec3 halfway;
uniform float shininess;


out vec3 ambient;
out vec3 diffuse;
out vec3 specular;

void main() {
    gl_Position = mvp * vec4(in_Position, 1.0);
    vec3 norm = normalize(mat3(model) * in_Normal);

    ambient = ambientMag * color;

    diffuse = max(0.0, dot(norm, lightDir)) * color;

    specular = pow(dot(norm, halfway), shininess) * lightColor;
}
