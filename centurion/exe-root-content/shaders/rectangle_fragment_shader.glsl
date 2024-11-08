#version 460

// Uniform variables
uniform vec4 uBorderColor;

// Output variables
out vec4 FragColor;

void main()
{
    FragColor = uBorderColor;
}
