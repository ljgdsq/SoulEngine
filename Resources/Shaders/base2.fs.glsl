#version 330 core
in vec3 ourColor;

out vec4 color;
uniform vec4 u_Color;
void main()
{
    color = vec4(ourColor, 1.0) * u_Color;
}