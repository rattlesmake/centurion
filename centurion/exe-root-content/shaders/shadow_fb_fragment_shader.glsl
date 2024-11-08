#version 460

// Input variables
in vec2 FragTex;
uniform sampler2D uTexture1;

// Output variables
out vec4 out_Color;

/*
    main function
*/

void main()
{
	vec4 color = texture(uTexture1, FragTex);
	
	if (color.r == 1.f)
		discard;
	
	out_Color = vec4(color.rgb, (1.f - color.r) * 0.5f);
}