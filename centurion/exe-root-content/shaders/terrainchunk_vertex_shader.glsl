#version 460

/*  
    ----- VERTEX SHADER -----
    Here we can work on the position of EACH single vertex;
    Moreover, it's possible to prepare stuff for the fragment shader
    which is compiled AFTER the vertex shader.
    For example, we prepare normals and texture coordinates.
    the 'layout( location = .)' variables come directly from the.cpp code
    during mesh creation(look for glBindVertexArrayand glVertexAttribPointer)
*/

layout(location = 0) in vec2 pos;
layout(location = 1) in vec2 tex;
layout(location = 2) in float height_texture; // height + texture packed in a single float
layout(location = 3) in vec3 norm;
layout(location = 4) in vec2 offset;

layout(std140) uniform terrain_ubo_data
{
    mat4 uProjection;
    mat4 uView;
    vec2 uHeightMinMax;
    vec2 uSize;
    ivec3 uTracingWireframeTerrainsnumber;
    vec3 uHeights; // uMinGrassHeight, uSeaPassableHeight, uSeaNotPassableHeight
};

// to fragment
out float FragTerrainType;
out float FragTerrainWeight[20];
out float FragTracingColor;
out vec2 FragTex;
out vec3 FragNorm;
out float FragZ;

/*
	main function
*/

vec2 DecodeHeightTexture(float f)
{
    float m1 = 64000.f;
    vec2 outputVec;
    outputVec.y = floor(f / m1);
    outputVec.x = floor(f - outputVec.y * m1);
    return outputVec;
}

void main()
{
    // decode
    vec2 decodedHTData = DecodeHeightTexture(height_texture);
    float height = decodedHTData.x;
    float terrainType = decodedHTData.y;
    float uMinZ = uHeightMinMax.x;
    float uMaxZ = uHeightMinMax.y;
    float uMinGrassHeight = uHeights.x;

	float mapX = pos.x * uSize.x + offset.x;
	float mapY = pos.y * uSize.y + offset.y;
	gl_Position = uProjection * uView * vec4(mapX, mapY + height - uMinGrassHeight, (-1.f) * mapY + height, 1.0);

    FragTerrainWeight = float[](0.f, 0.f, 0.f, 0.f, 0.f, 0.f, 0.f, 0.f, 0.f, 0.f, 0.f, 0.f, 0.f, 0.f, 0.f, 0.f, 0.f, 0.f, 0.f, 0.f);
	FragTerrainWeight[int(terrainType)] = 1.f;

	FragTex = tex;
	FragNorm = norm;
	FragTracingColor = (height - uMinZ) / (uMaxZ - uMinZ); // in (0-1)
	FragTerrainType = terrainType;
	FragZ = height;
}
