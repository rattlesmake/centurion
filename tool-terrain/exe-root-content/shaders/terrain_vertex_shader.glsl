#version 460

layout (location = 0) in vec2 vertexPosition;
layout (location = 1) in vec2 vertexUVs;

uniform mat4 uView;            
uniform mat4 uProjection;  

uniform float vertices_gap_px;

out float tcsColor;
out vec2 tcsUVs;

void main()
{    
    float vertexColor;
    if (mod(vertexPosition.x, 2.f) == 0 || vertexPosition.x == vertexPosition.y)
    {
        vertexColor = 0.f;
    }
    else
    {
        vertexColor = 1.f;
    }
    tcsColor = vertexColor;
    tcsUVs = vertexUVs;
    gl_Position = uProjection * uView * vec4(vertexPosition.x * vertices_gap_px, vertexPosition.y * vertices_gap_px, 0.0, 1.0);
}
