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

#include "Spectrum2.hpp"

#define GLM_FORCE_RADIANS
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <vector>
#include <iostream>

Spectrum::Spectrum(const Module_Config::Spectrum& config): output_size(0){
	init_bar_shader();
	init_line_shader();

	configure(config);

	init_bars();
	init_lines();
}

void Spectrum::draw(const float dt){
	/* render lines */
	if(draw_lines){
		sh_lines.use();
		v_lines.bind();
		glDrawArrays(GL_LINES, 0, 18);
	}

	/* render bars */
	sh_bars();
	v_bars.bind();
	glDrawArrays(GL_POINTS, 0, output_size);

	// unbind VAOs
	GL::VAO::unbind();
}

void Spectrum::update_bars(std::vector<float>& fft_data){
	b_bars();
	glBufferSubData(GL_ARRAY_BUFFER, 0, output_size * sizeof(float), fft_data.data());
	GL::Buffer::unbind();
	GL::get_error("update fft buffer");
}

void Spectrum::resize_bar_buffer(const size_t size){
	b_bars();
	glBufferData(GL_ARRAY_BUFFER, output_size * sizeof(float), 0, GL_DYNAMIC_DRAW);
	GL::Buffer::unbind();
	GL::get_error("resize fft buffer");
}

void Spectrum::configure(const Module_Config::Spectrum& scfg){
	//const Config::Spectrum& scfg = cfg.spectra[id];
	GL::Program& sh = sh_bars;
	sh();
	// Post compute specific uniforms
	GLint i_width = sh.get_uniform("width");
	glUniform1f(i_width, scfg.bar_width/(float)scfg.output_size);

	// set bar color gradients
	GLint i_top_color = sh.get_uniform("top_color");
	glUniform4fv(i_top_color, 1, scfg.top_color.rgba);

	GLint i_bot_color = sh.get_uniform("bot_color");
	glUniform4fv(i_bot_color, 1, scfg.bot_color.rgba);

	GLint i_gradient = sh.get_uniform("gradient");
	glUniform1f(i_gradient, scfg.gradient);

	GLint i_length = sh.get_uniform("length_1");
	glUniform1f(i_length, 1./scfg.output_size);

	GLint i_slope = sh.get_uniform("slope");
	glUniform1f(i_slope, scfg.slope * 0.5);

	GLint i_offset = sh.get_uniform("offset");
	glUniform1f(i_offset, scfg.offset * 0.5);

	sh_lines.use();
	// set dB line specific arguments
	i_offset = sh_lines.get_uniform("offset");
	glUniform1f(i_offset, scfg.offset);

	i_slope = sh_lines.get_uniform("slope");
	glUniform1f(i_slope, scfg.slope);

	GLint i_line_color = sh_lines.get_uniform("line_color");
	glUniform4fv(i_line_color, 1, scfg.line_color.rgba);


	resize(scfg.output_size);
	set_transformation(scfg.pos);
	draw_lines = scfg.dB_lines;
	// limit number of channels
	channel = scfg.channel;
}

void Spectrum::resize(const size_t size){
	if(size != output_size){
		output_size = size;
		resize_bar_buffer(size);
	}
}

void Spectrum::set_transformation(const Module_Config::Transformation& t){
	// apply simple ortho transformation
	glm::mat4 transformation = glm::ortho(t.Xmin, t.Xmax, t.Ymin, t.Ymax);

	sh_bars();
	GLint i_trans = sh_bars.get_uniform("trans");
	glUniformMatrix4fv(i_trans, 1, GL_FALSE, glm::value_ptr(transformation));

	sh_lines.use();
	i_trans = sh_lines.get_uniform("trans");
	glUniformMatrix4fv(i_trans, 1, GL_FALSE, glm::value_ptr(transformation));
}

void Spectrum::init_bar_shader(){
	const char* vertex_shader =
	#include "shader/bar2.vert"
	;
	GL::Shader vs(vertex_shader, GL_VERTEX_SHADER);

	const char* fragment_shader =
	#include "shader/simple.frag"
	;
	GL::Shader fs(fragment_shader, GL_FRAGMENT_SHADER);

	// geometry shader
	// draw bars
	const char* geometry_shader =
	#include "shader/bar.geom"
	;
	GL::Shader gs(geometry_shader, GL_GEOMETRY_SHADER);

	// link shaders
	try{
		sh_bars.link(fs, vs, gs);
	}
	catch(std::invalid_argument& e){
		std::cerr << "Can't link bar shaders!" << std::endl << e.what() << std::endl;
	}
}

void Spectrum::init_bars(){
	v_bars();

	b_bars.bind();
	GLint arg_y = sh_bars.get_attrib("y");
	glVertexAttribPointer(arg_y, 1, GL_FLOAT, GL_FALSE, sizeof(float), nullptr);
	glEnableVertexAttribArray(arg_y);

	GL::Buffer::unbind();
	GL::VAO::unbind();
}

void Spectrum::init_line_shader(){
	// fragment shader
	const char* fragment_shader =
	#include "shader/simple.frag"
	;
	GL::Shader fs(fragment_shader, GL_FRAGMENT_SHADER);

	const char* vs_lines_code =
	#include "shader/lines.vert"
	;
	GL::Shader vs_lines(vs_lines_code, GL_VERTEX_SHADER);

	try{
		sh_lines.link(fs, vs_lines);
	}
	catch(std::invalid_argument& e){
		std::cerr << "Can't link dB line shader!" << std::endl << e.what() << std::endl;
	}
}

void Spectrum::init_lines(){
	v_lines.bind();

	const float dB_lines[36] = {
		-1.0,  0.0, 1.0,  0.0, //   0dB
		-1.0, -0.5, 1.0, -0.5, // -10dB
		-1.0, -1.0, 1.0, -1.0, // -20dB
		-1.0, -1.5, 1.0, -1.5, // -30dB
		-1.0, -2.0, 1.0, -2.0, // -40dB
		-1.0, -2.5, 1.0, -2.5, // -50dB
		-1.0, -3.0, 1.0, -3.0, // -60dB
		-1.0, -3.5, 1.0, -3.5, // -70dB
		-1.0, -4.0, 1.0, -4.0  // -80dB
	};

	b_lines.bind();
	glBufferData(GL_ARRAY_BUFFER, sizeof(dB_lines), dB_lines, GL_STATIC_DRAW);

	GLint arg_line_vert = sh_lines.get_attrib("pos");
	glVertexAttribPointer(arg_line_vert, 2, GL_FLOAT, GL_FALSE, 0, nullptr);
	glEnableVertexAttribArray(arg_line_vert);

	GL::VAO::unbind();
}
