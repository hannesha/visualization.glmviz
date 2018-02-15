R"(
#version 150

in float pos;
out vec4 color;

uniform float length_1; // 1/length
uniform int start;
uniform float scale;
uniform vec4 line_color;

const float pi_2 = radians(360.);
void main(){
    float id = float(gl_VertexID - start);
	// calculate x coordinates
	float x = mix(-1., 1., (float(gl_VertexID - start) + 0.5) * length_1);
	color = line_color;
	gl_Position = vec4(x, clamp(pos * scale, -1.0, 1.0), 0.0, 1.0);
	
	//float t = id * length_1;
	//vec2 circle_pos = vec2(cos(pi_2 * t), sin(pi_2 * t));


	//gl_Position = vec4(clamp(pos * scale * 0.5 + 0.5, 0., 1.0) * circle_pos, 0.0, 1.0);
}
)"
