#version 460

// Uniform variables
uniform int uMapWidth;
uniform int uMapHeight;
uniform int uCloudsHeight;
uniform mat4 uProjection;
uniform mat4 uView;

// Input variables
layout (location = 0) in vec2 pos;
layout (location = 1) in vec2 tex;

// Output variables
out vec2 FragTex;

float x1, y1;

void main()
{   
	x1 = pos.x * uMapWidth;
    y1 = pos.y * uMapHeight;
	FragTex = tex;

    gl_Position = uProjection * uView * vec4(x1, y1, - y1 + uCloudsHeight, 1.0);
}  