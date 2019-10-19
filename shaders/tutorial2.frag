#version 460
// It was expressed that some drivers required this next line to function properly
// precision highp float;

in vec3 norm;

uniform mat4 mvp;
uniform mat4 model;
uniform vec3 color;
uniform vec3 lightDir;
uniform float lightMag;
uniform float ambientMag;

void main() {
    vec3 ambient = ambientMag * color;

    vec3 diffuse = lightMag * dot(norm, lightDir) * color;

    vec3 specular = vec3(0.0);
    
    vec3 finalColor = ambient + diffuse + specular;
    gl_FragColor = min(vec4(finalColor, 1.0), 1.0);
}
