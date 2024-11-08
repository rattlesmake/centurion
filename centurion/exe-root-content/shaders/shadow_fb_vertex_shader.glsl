#version 460

layout (location = 0) in vec2 pos;
layout (location = 1) in vec2 tex;

// Uniform variables
uniform mat4 uProjection;
uniform mat4 uView;
uniform ivec2 uSize;

// to fragment
out vec2 FragTex;

/*
	main function
*/

void main()
{   
	gl_Position = uProjection * uView * vec4(pos.x * uSize.x, pos.y * uSize.y, 0.0, 1.0);
	FragTex = vec2(tex.x, 1-tex.y);    
}
