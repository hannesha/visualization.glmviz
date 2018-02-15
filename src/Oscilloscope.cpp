/*
 *	Copyright (C) 2016  Hannes Haberl
 *
 *	This file is part of GLMViz.
 *
 *	GLMViz is free software: you can redistribute it and/or modify
 *	it under the terms of the GNU General Public License as published by
 *	the Free Software Foundation, either version 3 of the License, or
 *	(at your option) any later version.
 *
 *	GLMViz is distributed in the hope that it will be useful,
 *	but WITHOUT ANY WARRANTY; without even the implied warranty of
 *	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *	GNU General Public License for more details.
 *
 *	You should have received a copy of the GNU General Public License
 *	along with GLMViz.  If not, see <http://www.gnu.org/licenses/>.
 */

#include "Oscilloscope.hpp"
#include "Buffer.hpp"

#define GLM_FORCE_RADIANS
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <vector>
#include <iostream>

Oscilloscope::Oscilloscope(const Module_Config::Oscilloscope& config, const unsigned o_id): size(0), id(o_id){
	init_crt();

	configure(config);
}

void Oscilloscope::draw(unsigned start, unsigned len){
	sh_crt.use();
	v_crt.bind();

	//opengl vertex id starts with start index!!!! <= why tho
	//GLint i_start = sh_crt.get_uniform("start");
	//glUniform1i(i_start, start);

	resize_x_buffer(len);
	glActiveTexture(GL_TEXTURE0);
	t_osc.bind(GL_TEXTURE_BUFFER);

	glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
	GL::get_error("Render");

	GL::Texture::unbind(GL_TEXTURE_BUFFER);
}

void Oscilloscope::init_crt(){
	const char* vert_code =
	#include "shader/osc.vert"
	;

	GL::Shader vert(vert_code, GL_VERTEX_SHADER);
	GL::get_error("vertex shader");

//	const char* geom_code =
//	#include "shader/osc.geom"
//	;
//
//	GL::Shader geom(geom_code, GL_GEOMETRY_SHADER);


	const char* frag_code =
	#include "shader/osc.frag"
	;

	GL::Shader frag(frag_code, GL_FRAGMENT_SHADER);
	GL::get_error("fragment shader");

	try{
		sh_crt.attach(vert, frag);
		//glBindAttribLocation(sh_crt.get_id(), 0, "pos");
		GL::get_error("attach");
		glLinkProgram(sh_crt.get_id());
		GL::get_error("link");
		sh_crt.check_link_status();
		sh_crt.detach(vert, frag);

	}
	catch(std::invalid_argument& e){
		std::cerr << "Can't link oscilloscope shader!" << std::endl << e.what() << std::endl;
	}

	v_crt.bind();
	GL::get_error("bind vao");

	const float vert_data[] = {
			-1.0f,  1.0f, 0.0f, 0.0f, // Top-left
			1.0f,  1.0f, 1.0f, 0.0f, // Top-right
			-1.0f, -1.0f, 0.0f, 1.0f, // Bottom-right
			1.0f, -1.0f, 1.0f, 1.0f  // Bottom-left
	};

	b_crt_y.bind();
	glBufferData(GL_ARRAY_BUFFER, sizeof(vert_data), vert_data, GL_STATIC_DRAW);
	GL::get_error("bind vbo");
	GLint arg_y = sh_crt.get_attrib("pos");
	GL::get_error("get attrib");
	std::cout << "index: "<< arg_y << std::endl;
	glVertexAttribPointer(arg_y, 2, GL_FLOAT, GL_FALSE, 4*sizeof(float), nullptr);
	GL::get_error("set attrib pointer");
	glEnableVertexAttribArray(arg_y);
	GLint arg_uv = sh_crt.get_attrib("uvcoords");
	glVertexAttribPointer(arg_uv, 2, GL_FLOAT, GL_FALSE, 4*sizeof(float), (void*)(2*sizeof(float)));
	glEnableVertexAttribArray(arg_uv);
	GL::get_error("enable attrib pointer");


	GL::VAO::unbind();

	b_crt_tex.bind(GL_TEXTURE_BUFFER);
	//glBufferData(GL_TEXTURE_BUFFER, 1000 * sizeof(float), nullptr, GL_DYNAMIC_DRAW);
	GL::Buffer::unbind(GL_TEXTURE_BUFFER);

	//b_crt_tex.bind(GL_TEXTURE_BUFFER);
	t_osc.bind(GL_TEXTURE_BUFFER);
	GL::get_error("bind");
	glTexBuffer(GL_TEXTURE_BUFFER, GL_R32F, b_crt_tex.id);
	std::cout << "buffer id: " << b_crt_tex.id << std::endl;
	std::cout << "tex id: " << t_osc.id << std::endl;
	GL::get_error("assign texture");
	GL::Texture::unbind(GL_TEXTURE_BUFFER);
	GL::get_error("create tex buffer");
	//GL::get_error("bind tex");
	//glTexImage1D(GL_TEXTURE_1D, 0, GL_RED, 1, 0, GL_RED, GL_FLOAT, nullptr);


	//GL::get_error("bind tex");

}

void Oscilloscope::configure(const Module_Config::Oscilloscope& ocfg){
	sh_crt();

	GLint i_scale = sh_crt.get_uniform("scale");
	glUniform1f(i_scale, ocfg.scale);
	std::cout << "i_scale: " << i_scale << std::endl;
	GL::get_error("set uniform scale");

	GLint i_color = sh_crt.get_uniform("line_color");
	std::cout << "i_color: " << i_color << std::endl;
	glUniform4fv(i_color, 1, ocfg.color.rgba);
	GL::get_error("set uniform color");

	GLint i_tex = sh_crt.get_uniform("tex");
	glUniform1i(i_tex, 0);

//	GLint i_width = sh_crt.get_uniform("width");
//	glUniform1f(i_width, ocfg.width);
//
//	GLint i_sigma = sh_crt.get_uniform("sigma");
//	glUniform1f(i_sigma, ocfg.sigma);
//
//	GLint i_sigma_c = sh_crt.get_uniform("sigma_coeff");
//	glUniform1f(i_sigma_c, ocfg.sigma_coeff);
//
//	set_transformation(ocfg.pos);

//	channel = ocfg.channel;
	GL::get_error("conf failed");
}

void Oscilloscope::resize_x_buffer(const size_t size){
	sh_crt();
	GLint i_length = sh_crt.get_uniform("length");
	glUniform1f(i_length, (float)size);
	//std::cout << "i_length: " << size << std::endl;
	GL::get_error("set uniform length");
}

void Oscilloscope::set_transformation(const Module_Config::Transformation& t){
	glm::mat4 transformation = glm::ortho(t.Xmin, t.Xmax, t.Ymin, t.Ymax);

	sh_crt();

	GLint i_trans = sh_crt.get_uniform("trans");
	glUniformMatrix4fv(i_trans, 1, GL_FALSE, glm::value_ptr(transformation));
}

void Oscilloscope::update_buffer(Buffer<float>& buffer){
	auto lock = buffer.lock();

	// resize x coordinate buffer if necessary
	if(size != buffer.size){
		size = buffer.size;

		//resize_x_buffer(window_length);

		b_crt_tex.bind(GL_TEXTURE_BUFFER);
		glBufferData(GL_TEXTURE_BUFFER, size * sizeof(float), buffer.v_buffer.data(), GL_DYNAMIC_DRAW);
		GL::Buffer::unbind(GL_TEXTURE_BUFFER);
		GL::get_error("Fill buffer");
	}else{
		b_crt_tex.bind(GL_TEXTURE_BUFFER);
		glBufferSubData(GL_TEXTURE_BUFFER, 0, size * sizeof(float), buffer.v_buffer.data());
		GL::Buffer::unbind(GL_TEXTURE_BUFFER);
		GL::get_error("Update buffer");

	}
}

void Oscilloscope::update_buffer(std::vector<float>& buffer){
	//auto lock = buffer.lock();

	// resize x coordinate buffer if necessary
	if(size != buffer.size()){
		size = buffer.size();


		b_crt_tex.bind(GL_TEXTURE_BUFFER);
		glBufferData(GL_TEXTURE_BUFFER, size * sizeof(float), buffer.data(), GL_DYNAMIC_DRAW);
		GL::get_error("Fill buffer");
		GL::Buffer::unbind(GL_TEXTURE_BUFFER);
	}else{
		b_crt_tex.bind(GL_TEXTURE_BUFFER);
		glBufferSubData(GL_TEXTURE_BUFFER, 0, size * sizeof(float), buffer.data());
		GL::get_error("Update buffer");
		GL::Buffer::unbind(GL_TEXTURE_BUFFER);
	}
}
