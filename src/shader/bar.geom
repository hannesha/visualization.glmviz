R"(
#version 150

layout (points) in;
layout (triangle_strip, max_vertices = 4) out;

in vec4 v_bot_color[];
in vec4 v_top_color[];

out vec4 color;

uniform float width;
uniform mat4 trans;

//float min = 0.25;
float min = -1.0;

void main () {
	vec2 normal = vec2(-gl_in[0].gl_Position.y, gl_in[0].gl_Position.x);
	vec2 vwidth = normal * width;
	//float max =  gl_in[0].gl_Position.z * (1. - min) + min;
	float max =  gl_in[0].gl_Position.z;

	color = v_bot_color[0];
	//gl_Position = trans * vec4(gl_in[0].gl_Position.xy * min - vwidth, 0.0, 1.0);
	gl_Position = trans * vec4(gl_in[0].gl_Position.x - width, min, 0.0, 1.0);
	EmitVertex();

	color = v_top_color[0];
	//gl_Position = trans * vec4(gl_in[0].gl_Position.xy * max - vwidth, 0., 1.);
	gl_Position = trans * vec4(gl_in[0].gl_Position.x - width, max, 0.0, 1.0);
	EmitVertex();

	color = v_bot_color[0];
	//gl_Position = trans * vec4(gl_in[0].gl_Position.xy * min + vwidth, 0.0, 1.0);
	gl_Position = trans * vec4(gl_in[0].gl_Position.x + width, min, 0.0, 1.0);
	EmitVertex();

	color = v_top_color[0];
	//gl_Position = trans * vec4(gl_in[0].gl_Position.xy * max + vwidth, 0., 1.);
	gl_Position = trans * vec4(gl_in[0].gl_Position.x + width, max, 0.0, 1.0);
	EmitVertex();

	EndPrimitive();
}	
)"
