#version 460

// Input variables
in vec2 FragTex;
in float game_time;

// Output variables
out vec4 FragColor;

vec4 GenerateWater1()
{
    vec2 uv = 2 * vec2(FragTex.x * 20, FragTex.y * 40);
   
    vec4 texture_color = vec4(0.192156862745098, 0.6627450980392157, 0.9333333333333333, 1.0);
    
    vec4 k = vec4(game_time)*0.8;
	k.xy = uv * 7.0;
    float val1 = length(0.5-fract(k.xyw*=mat3(vec3(-2.0,-1.0,0.0), vec3(3.0,-1.0,1.0), vec3(1.0,-1.0,-1.0))*0.5));
    float val2 = length(0.5-fract(k.xyw*=mat3(vec3(-2.0,-1.0,0.0), vec3(3.0,-1.0,1.0), vec3(1.0,-1.0,-1.0))*0.2));
    float val3 = length(0.5-fract(k.xyw*=mat3(vec3(-2.0,-1.0,0.0), vec3(3.0,-1.0,1.0), vec3(1.0,-1.0,-1.0))*0.5));
    vec4 color = vec4 ( pow(min(min(val1,val2),val3), 7.0) * 3.0)+texture_color;
    return vec4(color.xyz, 0.6f);
}

/*
    main function
*/

void main()
{
    FragColor = GenerateWater1();
}