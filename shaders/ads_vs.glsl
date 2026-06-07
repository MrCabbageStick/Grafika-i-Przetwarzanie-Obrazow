#version 420

// === Gouraud ===

uniform vec3 Color;
uniform mat4 ModelMat;
uniform mat4 ViewMat;
uniform mat4 ProjectionMat;

// Light & material uniforms
uniform vec3 LightPos;       // world-space light position
uniform vec3 LightColor;
uniform vec3 AmbientColor;
uniform float Shininess;

layout(location=0) in vec3 VertexPosition;
layout(location=1) in vec3 VertexColor;
layout(location=2) in vec3 VertexNormal;

out vec4 v_color;

void main()
{
    mat4 MVP = ProjectionMat * ViewMat * ModelMat;
    vec4 worldPos = ModelMat * vec4(VertexPosition, 1.0);
    vec3 N = normalize(mat3(transpose(inverse(ModelMat))) * VertexNormal);
    vec3 L = normalize(LightPos - worldPos.xyz);
    vec3 V = normalize(-worldPos.xyz); // assumes camera at origin in view space
    vec3 R = reflect(-L, N);

    // ADS components
    vec3 ambient  = AmbientColor;
    vec3 diffuse  = LightColor * max(dot(N, L), 0.0);
    vec3 specular = LightColor * pow(max(dot(R, V), 0.0), Shininess);

    vec3 ads = (ambient + diffuse + specular) * Color * VertexColor;
    v_color = vec4(ads, 1.0);

    gl_Position = MVP * vec4(VertexPosition, 1.0);
}
