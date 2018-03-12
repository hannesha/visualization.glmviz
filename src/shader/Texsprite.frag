R"(
#version 150
in vec2 v_UVcoords;

out vec4 f_color;

uniform sampler2D tex;
uniform float tex_offset;

void main () {
	//fix float rounding errors
	vec4 bg = texture(tex, v_UVcoords);
	float offset_n = max(tex_offset, 0.);
	vec2 loc = clamp(v_UVcoords - (v_UVcoords - 0.5) * offset_n, 0., 1.);
	vec4 fg = texture(tex, loc);
	f_color = mix(bg, fg, 0.3);
}
)"
