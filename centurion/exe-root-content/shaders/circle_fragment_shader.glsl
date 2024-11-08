#version 460

out vec4 FragColor;
uniform vec4 uBorderColor;
void main()
{
    FragColor = uBorderColor;
}