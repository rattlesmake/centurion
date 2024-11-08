#version 460

// input variables
layout (location = 0) in vec2 pos;
layout (location = 1) in vec2 tex;
layout (location = 2) in vec4 data1;
layout (location = 3) in vec4 data2;
layout (location = 4) in vec4 data3;
layout (location = 5) in vec4 data4;

layout(std140) uniform entity_ubo_data
{
    mat4 uProjection;
    mat4 uView;
};

// output variables
out vec2 frag_texture;
out float frag_shadow;
out float frag_level;
out float frag_player_color_id;
out float frag_layer_has_mask;
out float frag_is_selected;
out float frag_is_placeable;
out float frag_picking_id;

float x1, y1;

void main()
{
	float uPosX = data1.x;
	float uPosY = data1.y;
	float uSizeW = data1.z;
	float uSizeH = data1.w;
	
	float uFrames = data2.x;
	float uDirections = data2.y;
	float uCurrentFrame = data2.z;
	float uCurrentDirection = data2.w;
	
	frag_level = data3.x;
	frag_player_color_id = data3.y;
	frag_layer_has_mask = data3.z;
	frag_shadow = data3.w;
	
	frag_is_selected = data4.x;
	frag_is_placeable = data4.y;
	frag_picking_id = data4.z;
	
	float width = uSizeW / uFrames;
    float height = uSizeH / uDirections;

	x1 = pos.x * width + uPosX;
    y1 = pos.y * height + uPosY;
	gl_Position = uProjection * uView * vec4(x1, y1, 1.0, 1.0);
	frag_texture = vec2(tex.x / uFrames + uCurrentFrame / uFrames, tex.y / uDirections + uCurrentDirection / uDirections);
}