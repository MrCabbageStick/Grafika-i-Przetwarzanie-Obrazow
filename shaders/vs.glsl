#version 420

uniform vec3 Color;
uniform mat4 ModelMat;
uniform mat4 ViewMat;
uniform mat4 ProjectionMat;

layout (location=0) in vec3 VertexPosition;
layout (location=1) in vec3 VertexColor;

out vec4 v_color;

void main()
{
    // Model View Projection matrix
    mat4 MVP = ProjectionMat * ViewMat * ModelMat;
    gl_Position = MVP * vec4(VertexPosition, 1.0);

    v_color = vec4(Color, 1.0) * vec4(VertexColor, 1.0);
}
