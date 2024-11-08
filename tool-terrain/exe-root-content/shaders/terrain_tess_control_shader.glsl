#version 460

layout (vertices = 4) out;

uniform float outerSegs;
uniform float innerSegs;

in vec2 tcsUVs[];
out vec2 tesUVs[];

in float tcsColor[];  // must be an array
out float tesColor[]; // must be an array

void main()
{
	
	gl_out[gl_InvocationID].gl_Position = gl_in[gl_InvocationID].gl_Position;

	gl_TessLevelOuter[0] = outerSegs;
	gl_TessLevelOuter[1] = outerSegs;
	gl_TessLevelOuter[2] = outerSegs;
	gl_TessLevelOuter[3] = outerSegs;

	gl_TessLevelInner[0] = innerSegs;
	gl_TessLevelInner[1] = innerSegs;

	tesColor[gl_InvocationID] = tcsColor[gl_InvocationID];
	tesUVs[gl_InvocationID] = tcsUVs[gl_InvocationID];
}