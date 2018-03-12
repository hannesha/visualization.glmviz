#pragma once

#include "Program.hpp"
#include <string>
#include "Image.hpp"

class TexSprite{
	public:
		TexSprite();
		~TexSprite(){};
		TexSprite(TexSprite&&) = default;

		void draw();
		void rotate(const float);
		void update(const float);
		void update_texture(const Image&);
		void set_screen_ratio(int, int);
		float get_ratio(){return static_cast<float>(height)/width;}
		void adjust_img_ratio();
	private:
		GL::Program sh_sprite;
		GL::VAO v_sprite;
		GL::Buffer b_quad, b_UV;
		GL::Texture t_sprite;
		unsigned width = 1, height = 1;
		float screen_ratio;
};
 
