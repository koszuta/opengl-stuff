#version 460
// It was expressed that some drivers required this next line to function properly
// precision highp float;

in vec3 ambient;
in vec3 diffuse;
in vec3 specular;

void main() {
    vec3 finalColor = ambient + diffuse + specular;
    gl_FragColor = vec4(finalColor, 1.0);
}
