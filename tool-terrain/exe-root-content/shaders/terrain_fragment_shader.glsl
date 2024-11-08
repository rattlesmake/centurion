#version 460

noperspective in vec3 edgeDistance;

uniform float lineWidth;
uniform int wireframe;

out vec4 finalColor;
in float fragColor;
in vec2 fragUVs;

uniform sampler2D grass;
uniform sampler2D road;

void main()
{
    if (wireframe == 1.f)
    {
        finalColor = vec4(1.0);
    }
    else
    {
        finalColor = mix(texture(grass, fragUVs), texture(road, fragUVs), fragColor);
    }
}
