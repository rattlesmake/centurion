#version 460

layout (triangles) in;
layout (triangle_strip, max_vertices = 3) out;

noperspective out vec3 edgeDistance;

in float geoColor[];
out float fragColor;

in vec2 geoUVs[];
out vec2 fragUVs;

void main()
{

	// points on screen
	vec2 a = vec2(gl_in[0].gl_Position);
	vec2 b = vec2(gl_in[1].gl_Position);
	vec2 c = vec2(gl_in[2].gl_Position);

	// side lengths of triangle
	float cb = length(c - b);
	float ac = length(c - a);
	float ab = length(b - a);

	// angles
	float alpha = acos((ac * ac + ab * ab - cb * cb) / (2.0 * ac * ab));
	float beta = acos((cb  * cb + ab * ab - ac * ac) / (2.0 * cb * ab));

	// heights off the sides
	float ha = abs(ab * sin(beta));
	float hb = abs(ab * sin(alpha));
	float hc = abs(ac * sin(alpha));

	edgeDistance = vec3(ha, 0, 0);
	fragColor = geoColor[0];
	fragUVs = geoUVs[0];
	gl_Position = gl_in[0].gl_Position;
	EmitVertex();

	edgeDistance = vec3(0, hb, 0);
	fragColor = geoColor[1];
	fragUVs = geoUVs[1];
	gl_Position = gl_in[1].gl_Position;
	EmitVertex();

	edgeDistance = vec3(0, 0, hc);
	fragColor = geoColor[2];
	fragUVs = geoUVs[2];
	gl_Position = gl_in[2].gl_Position;
	EmitVertex();

	EndPrimitive();
}