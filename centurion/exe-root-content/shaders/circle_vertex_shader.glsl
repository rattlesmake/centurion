#version 460

layout (location = 0) in vec2 pos;
uniform mat4 uProjection;
uniform mat4 uView;
void main()
{   
    gl_Position = uProjection * uView * vec4(pos.x, pos.y, 0.0, 1.0);   
}