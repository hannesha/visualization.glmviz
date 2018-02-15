R"(
#version 150

in vec2 pos;
in vec2 uvcoords;

out vec2 f_uvcoords;
//out vec4 color;

//uniform float length_1; // 1/length
//uniform int start;
//uniform float scale;
//uniform vec4 line_color;

//const float pi_2 = radians(360.);
void main(){
    f_uvcoords = uvcoords;
    gl_Position = vec4(pos, 0.0, 1.0);
}
)"
