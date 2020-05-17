#shader vertex
#version 330 core

layout(location=0) in vec4 position;

void main()
{
    gl_Position = position;
}
// note: cannot write semicolon after main function in the separate shader file

#shader fragment
#version 330 core

layout(location=0) out vec4 color;

uniform vec4 u_Color;

void main()
{
    color = u_Color;
   }
