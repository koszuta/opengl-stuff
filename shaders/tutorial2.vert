#version 460

layout(location = 0) in vec3 in_Position;
layout(location = 1) in vec3 in_Normal;

uniform mat4 mvp;
uniform mat4 model;
uniform vec3 color;
uniform vec3 lightDir;

// out vec3 ambient;
// out vec3 diffuse;
// out vec3 specular;
out vec3 norm;

void main() {
    gl_Position = mvp * vec4(in_Position, 1.0);

    // ambient = 0.2 * color;

    // vec3 norm = normalize(vec3(model * vec4(in_Normal, 1.0)));
    // diffuse = dot(norm, sunPos) * color;

    // specular = vec3(0.0);
    norm = normalize(vec3(model * vec4(in_Normal, 1.0)));
}
