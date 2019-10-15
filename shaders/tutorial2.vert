#version 150
// in_Position was bound to attribute index 0 and in_Normal was bound to attribute index 1
in vec3 in_Position;
in vec3 in_Normal;

uniform mat4 view;
uniform mat4 proj;

// We output the ex_Color variable to the next shader in the chain
out vec3 ex_Color;
void main(void) {
    // GLSL allows shorthand use of vectors, this is equivelent to:
    // gl_Position = vec4(in_Position.x, in_Position.y, in_Position.z, 1.0);

    gl_Position = proj * view * vec4(in_Position, 1.0);

    // We're simply passing the color through unmodified
    ex_Color = in_Color;
}
