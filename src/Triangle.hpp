#pragma once

#include "Program.hpp"

class Triangle {
	public:
		Triangle();
		~Triangle(){};
		Triangle(Triangle&&) = default;
		Triangle& operator=(Triangle&&) = default;

		void draw();
	private:
		GL::Program sh_triangle;
		GL::VAO v_triangle;
		GL::Buffer b_triangle;
};
