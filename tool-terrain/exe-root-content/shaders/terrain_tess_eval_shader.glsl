#version 460

layout (quads, equal_spacing, ccw) in;

in float tesColor[];
out float geoColor;

in vec2 tesUVs[];
out vec2 geoUVs;

void main()
{
	
	float u = gl_TessCoord.x;
	float v = gl_TessCoord.y;

	vec4 p0 = gl_in[0].gl_Position;
	vec4 p1 = gl_in[1].gl_Position;
	vec4 p2 = gl_in[2].gl_Position;
	vec4 p3 = gl_in[3].gl_Position;

	vec4 p = p0 * (1 - u) * (1 - v) + p1 * u * (1 - v) + p2 * u * v + p3 * (1 - u) * v;
	
	gl_Position = p;

	float uc = float(u > 0.5);
	float vc = float(v > 0.5); //float(gl_TessCoord.y > 0.5);
	float c0 = tesColor[0];
	float c1 = tesColor[1];
	float c2 = tesColor[2];
	float c3 = tesColor[3];
	geoColor = c0 * (1 - uc) * (1 - vc) + c1 * uc * (1 - vc) + c2 * uc * vc + c3 * (1 - uc) * vc;

	float un = u; //float(u > 0.5);
	float vn = v; //float(v > 0.5); //float(gl_TessCoord.y > 0.5);

	// ----------------------------------------------------------------------
    // retrieve control point texture coordinates
    vec2 t00 = tesUVs[0];
    vec2 t01 = tesUVs[1];
    vec2 t11 = tesUVs[2];
    vec2 t10 = tesUVs[3];

    // bilinearly interpolate texture coordinate across patch
    vec2 t0 = (t01 - t00) * u + t00;
    vec2 t1 = (t11 - t10) * u + t10;
	geoUVs = (t1 - t0) * v + t0;
}
