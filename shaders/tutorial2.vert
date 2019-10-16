#version 450

layout(location = 0) in vec3 in_Position;
layout(location = 1) in vec3 in_Normal;

uniform mat4 mvp;
uniform vec3 color;

out vec3 ex_Color;

void main() {
    gl_Position = mvp * vec4(in_Position, 1.0);

    ex_Color = color * cos(dot(vec3(1.0, 0.0, 0.0), in_Normal));
}
