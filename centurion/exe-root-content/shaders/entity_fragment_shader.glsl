#version 460

// input variables
in vec2 frag_texture;
in float frag_player_color_id;
in float frag_shadow;
in float frag_layer_has_mask;
in float frag_level;
in float frag_is_selected;
in float frag_is_placeable;
in float frag_picking_id;

// uniforms
uniform sampler2D uTexture0; // normal OR shadow
uniform sampler2D uTexture1; // mask
uniform ivec4 uTextureFlags; 
uniform int u_minimap;
uniform int u_editor;
uniform int u_picking;
uniform int u_alpha;

uniform float   hueAdjust;

// Output variables
out vec4 out_Color;

// constants
float DRAWMODE_NORMAL = 0.f;
float DRAWMODE_SHADOW = 1.f;
float DRAWMODE_COLOR = 2.f;
int LEVEL_THRESHOLD_1 = 6;
int LEVEL_THRESHOLD_2 = 12;
int LEVEL_THRESHOLD_3 = 18;
vec3 LEVEL_COLOR_1 = vec3(0.557f, 0.569f, 0.561f); //iron - base
vec3 LEVEL_COLOR_2 = vec3(0.690f, 0.510f, 0.231f); //bronze
vec3 LEVEL_COLOR_3 = vec3(0.900f, 0.900f, 0.900f); //fine iron
vec3 LEVEL_COLOR_4 = vec3(0.850f, 0.850f, 0.000f); // gold

vec3 decode_float_to_vec3(float f) 
{
    vec3 data;
    data.b = floor(f / 65536.0);
    data.g = floor((f - data.b * 65536.0) / 256.0);
    data.r = floor(f - data.b * 65536.0 - data.g * 256.0);
    return data;
}
vec3 player_color = decode_float_to_vec3(frag_player_color_id) / 255.0; // player color converted from float

vec4 merge_with_player_color(vec4 color)
{
    return vec4(color.r * player_color.r, color.g * player_color.g, color.b * player_color.b, color.a);
}
vec4 generate_output_color(vec4 color)
{
    vec4 outputColor;
    if (frag_is_selected == 1.f)
    {
        outputColor = color + vec4(0.2f, 0.2f, 0.2f, 0.0f);
    }
    if (frag_is_selected == 0.f) outputColor = color;   
    if (frag_is_placeable == 0.f) outputColor = vec4(color.r * 1.f, 0.f, 0.f, color.a);
    
    return outputColor;
}

vec4 get_color_from_picking_id(float f) 
{
    vec4 color = vec4(decode_float_to_vec3(f), 255.f);
    return color / 255.0;
}
vec4 render_picking(vec4 color)
{
	if (color.a <= 0.1) discard;

	vec4 picking_color = get_color_from_picking_id(frag_picking_id);
	return vec4(picking_color.rgb, 1.f);
}
vec4 render_shadow(vec4 color)
{
	return color;
}

vec4 apply_mask(vec4 color, vec4 mask)
{
	if (mask.a == 1.f)
		return mask;
	
	else if (mask.a == 0.f)
		return color;
	
	return vec4(mix(color, mask, mask.a).xyz, color.a);
}

vec4 render(vec4 baseTexture)
{
    if (frag_layer_has_mask == 1.f)
    {
        vec4 layers_mask = texture(uTexture1, frag_texture);

        // controlla il pixel che stiamo colorando nel fragment
        // R = color
        // G = level
        // B = skin
        // dobbiamo trovare l'intensita di ciascun elemento 
        // p.s. questa procedura e' una idea che ho avuto e puo essere rivista!
        float playercolor_intensity = layers_mask.r;
        float level_intensity = layers_mask.g;
        float skin_intensity = layers_mask.b;

        // colore
        vec4 playercolor = merge_with_player_color(baseTexture);

        // livello
        vec4 level;        
        if (frag_level < LEVEL_THRESHOLD_1)
            level = baseTexture * vec4(LEVEL_COLOR_1, baseTexture.a);
        else if (frag_level >= LEVEL_THRESHOLD_1 && frag_level < LEVEL_THRESHOLD_2)
            level = baseTexture * vec4(LEVEL_COLOR_2, baseTexture.a);
        else if (frag_level >= LEVEL_THRESHOLD_2 && frag_level < LEVEL_THRESHOLD_3)
            level = baseTexture * vec4(LEVEL_COLOR_3, baseTexture.a);
        else
            level = baseTexture * vec4(LEVEL_COLOR_4, baseTexture.a);

        // skin
        vec4 light_pink = vec4 (0.941,0.875,0.839, baseTexture.a); //light_pink
        vec4 pink = vec4 (0.773,0.518,0.361, baseTexture.a); //pink
        vec4 light_dark  = vec4 (0.639,0.455,0.267, baseTexture.a); //fine light_dark
        vec4 dark =  vec4 (0.235,0.18,0.157, baseTexture.a); // dark
        vec4 skin = baseTexture * light_pink;

        // merge all
        baseTexture = mix(mix(mix(baseTexture, playercolor, playercolor_intensity), level, level_intensity), skin, skin_intensity);
    }

  //---------------------------------------------------

  // Picking
  if (u_picking == 1)
  {
    return render_picking(baseTexture);
  }

  if (u_alpha == 1)
  {
    baseTexture.a = baseTexture.a / 2.f;
  }
  if (u_minimap == 1)
  {  
    //if (baseTexture.a <= 0.1f) discard;
    return baseTexture;
  }
  else
  {
    return generate_output_color(baseTexture);
  }
}

void main()
{
	// generate base texture
	vec4 color = texture(uTexture0, frag_texture);
	
	if (frag_shadow == 1.f)
	{
		out_Color = render_shadow(color);
	}
	else
	{
		out_Color = render(color);
	}
}
