#version 460

layout (location = 0) in vec2 pos;
layout (location = 1) in vec2 tex;

layout(std140) uniform sea_ubo_data
{
    mat4 uProjection;
    mat4 uView;
};

uniform vec4 sea_data_1; // uMapWidth, uMapHeight, uSeaHeight, game_time

// to fragment
out vec2 FragTex;
out float game_time;

/*
	main function
*/

void main()
{   
    float uMapWidth = sea_data_1.x;
    float uMapHeight = sea_data_1.y;
    float uSeaHeight = sea_data_1.z;

	float x1 = pos.x * uMapWidth;
    float y1 = pos.y * uMapHeight;
	FragTex = tex;
    game_time = sea_data_1.w;
    gl_Position = uProjection * uView * vec4(x1, y1, - y1 + uSeaHeight, 1.0);
}
