#version 460

// Uniform variables
uniform int uOrigin;
uniform ivec4 uData;
uniform mat4 uProjection;
uniform mat4 uView;

// Input variables
layout (location = 0) in vec2 pos;

void main()
{
    int x = uData.r;
    int y = uData.g;
    int w = uData.b;
    int h = uData.a;

    float x1, y1;
    
    switch(uOrigin) {
        case 0: // bottom-left
            x1 = x;
            y1 = y;
            break;
        case 1: // top-left
            x1 = x;
            y1 = y - h;
            break;
        case 2: // center
            x1 = x - w/2.f;
            y1 = y - h/2.f;
            break;
        case 3: // top-right
            x1 = x - w;
            y1 = y - h;
            break;
        case 4: // bottom-right
            x1 = x - w;
            y1 = y;
            break;
    }
    gl_Position = uProjection * uView * vec4(pos.x * w + x1, pos.y * h + y1, 0.0, 1.0);
    
}