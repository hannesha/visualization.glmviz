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

#pragma once

#include "FFT.hpp"
#include "Module_Config.hpp"
#include "Program.hpp"
#include <memory>

class Spectrum {
	public:
		Spectrum(const Module_Config::Spectrum&);
		// disable copy construction
		Spectrum(const Spectrum&) = delete;
		Spectrum(Spectrum&&) = default;
		Spectrum& operator=(Spectrum&&) = default;
		~Spectrum(){};

		void draw(const float);
		void update_bars(std::vector<float>&);
		void configure(const Module_Config::Spectrum&);

	private:
		GL::Program sh_bars, sh_lines;
		GL::VAO v_lines;
		GL::VAO v_bars;

		GL::Buffer b_bars, b_lines;
		size_t output_size;
		bool draw_lines;
		unsigned id, channel;


		void init_bar_shader();
		void init_bars();
		void init_line_shader();
		void init_lines();

		void resize_bar_buffer(const size_t);
		void resize(const size_t);
		void set_transformation(const Module_Config::Transformation&);
};
