R"(
#version 330

//in float x;
layout(location = 0) in float y;

uniform vec4 bot_color;
uniform vec4 top_color;

// switch gradient, 0:full range per bar, 1:0dB has top_color
uniform float gradient;
uniform float length_1;

out vec4 v_bot_color;
out vec4 v_top_color;

const float pi_2 = radians(360.);
void main () {
	float y_clamp = clamp(y, -0.5, 0.5);

	float id = float(gl_VertexID);
	// calculate x coordinates
	float x = mix(-1., 1., (id + 0.5) * length_1);
	
	//gl_Position = vec4(x, y_clamp, 0.0, 1.0);
	v_bot_color = bot_color;

	// calculate normalized top color	
	y_clamp = mix(1.0, y_clamp, gradient);
	v_top_color = mix(bot_color, top_color, y_clamp + 0.5);

	float t = id * length_1;
	vec2 circle_pos = vec2(-sin(pi_2 * t), cos(pi_2 * t));
	gl_Position = vec4(circle_pos, y_clamp + 0.5, 1.0);
}
)"
