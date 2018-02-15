#include "Triangle.hpp"

Triangle::Triangle(){
	const char* vert_code = 
	"#version 150\n"
	"in vec2 pos;"
	"out vec4 color;"
	"void main(){"
	"  color = vec4(pos + vec2(0.5), 1.0, 0.7);"
	"  gl_Position = vec4(pos, 0.0, 1.0);"
	"}";

	GL::Shader vert(vert_code, GL_VERTEX_SHADER);

	const char* frag_code =
	#include "shader/simple.frag"
	;

	GL::Shader frag(frag_code, GL_FRAGMENT_SHADER);

	sh_triangle.link(vert, frag);

	sh_triangle();

	const float triangle_verts[] = {
	//x    y    z
	-0.5, -0.5,
	 0.0, 0.5,
	 0.5, -0.5,
	};

	v_triangle.bind();

	b_triangle.bind();
	glBufferData(GL_ARRAY_BUFFER, sizeof(triangle_verts), triangle_verts, GL_STATIC_DRAW);

	GLint arg_pos = sh_triangle.get_attrib("pos");
	glVertexAttribPointer(arg_pos, 2, GL_FLOAT, GL_FALSE, 0, nullptr);
	glEnableVertexAttribArray(arg_pos);
}

void Triangle::draw(){
	sh_triangle();
	v_triangle.bind();
	glDrawArrays(GL_TRIANGLES, 0, 3);
}
