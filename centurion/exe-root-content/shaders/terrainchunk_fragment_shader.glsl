#version 460

/*  
    ----- FRAGMENT SHADER -----
    Here we can render colors, textures, lights, shadows ...
    Vertices come all together: here we can do nothing to manipulate
    the position or other properties of EACH vertex.
*/

layout(std140) uniform terrain_ubo_data
{
    mat4 uProjection;
    mat4 uView;
    vec2 uHeightMinMax;
    vec2 uSize;
    ivec3 uTracingWireframeTerrainsnumber;
    vec3 uHeights; // uMinGrassHeight, uSeaPassableHeight, uSeaNotPassableHeight
};

// texture
uniform sampler2DArray uSampleTex;

// Input variables
in float FragTracingColor;
in float FragTerrainType;
in float FragTerrainWeight[20];
in vec2 FragTex;
in vec3 FragNorm;
in float FragZ;

vec4 terrainTexture;
float lightDiffuseIntensity = 1.45f;
float lightAmbientIntensity = 0.f;
vec3 lightColour = vec3(1, 1, 1);
vec3 lightDirection = vec3(0, 0, 1);

out vec4 outColor;

float CalculateAlphaFromHeight(float z)
{
    float uSeaPassableHeight = uHeights.y;
    float uSeaNotPassableHeight = uHeights.z;

    //return 1.f;
	if (z >= uSeaPassableHeight) return 1.f;
	
	return (z-uSeaNotPassableHeight)/(uSeaPassableHeight-uSeaNotPassableHeight);
}

/*
	main function
*/

void main()
{      
    int tracing = uTracingWireframeTerrainsnumber.x;
    int wireframe = uTracingWireframeTerrainsnumber.y;
    int terrains_number = uTracingWireframeTerrainsnumber.z;

	vec4 ambientColor = vec4(lightColour, 1.0f) * lightAmbientIntensity;    
    float diffuseFactor = max(dot(FragNorm, normalize(lightDirection)), 0.0f);
    vec4 diffuseColor = vec4(lightColour, 1.0f) * lightDiffuseIntensity * diffuseFactor;
	
    terrainTexture = vec4(0.f);
	for (int i = 0; i < terrains_number; i++)
    {
        vec4 tex = texture(uSampleTex, vec3(FragTex, i));
		terrainTexture +=  tex * (ambientColor + diffuseColor) * FragTerrainWeight[i];
	}
	
	float alpha = CalculateAlphaFromHeight(FragZ);	
	terrainTexture = vec4(terrainTexture.xyz, alpha);

    if (wireframe == 1)
    {
        outColor = vec4(1.0f);
    }
    else 
    {
        if (tracing == 0)
        {
            outColor = terrainTexture;
        }
        else
        {
            outColor = vec4(FragTracingColor,FragTracingColor,FragTracingColor,1.0);
        }
    }
}