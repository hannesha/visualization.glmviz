#include "TexSprite.hpp"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

TexSprite::TexSprite(){

	const char* vert_code = 
	"#version 150\n"
	"in vec2 pos;"
	"in vec2 UVcoords;"
	"out vec2 v_UVcoords;"
	"uniform float width;"
	"uniform float height;"
	"uniform mat4 model;"
	"uniform mat4 rot;"
	"uniform mat4 proj;"
	"void main(){"
	"  v_UVcoords = UVcoords;"
	"  gl_Position = proj * rot * model * vec4(pos, 0.0, 1.0);"
	"}";

	GL::Shader vert(vert_code, GL_VERTEX_SHADER);

	const char* frag_code =
	#include "shader/Texsprite.frag"
	;

	GL::Shader frag(frag_code, GL_FRAGMENT_SHADER);

	sh_sprite.link(vert, frag);



	const float quad_pos[] = {
	//x    y
	-1,  1,
	 1,  1,
	-1, -1,
	 1, -1
	};

	const float UVcoords[] = {
	0., 0.,
	1., 0.,
	0., 1.,
	1., 1.
	};

	v_sprite.bind();

	b_quad.bind();
	glBufferData(GL_ARRAY_BUFFER, sizeof(quad_pos), quad_pos, GL_STATIC_DRAW);

	GLint arg_pos = sh_sprite.get_attrib("pos");
	glVertexAttribPointer(arg_pos, 2, GL_FLOAT, GL_FALSE, 0, nullptr);
	glEnableVertexAttribArray(arg_pos);

	b_UV.bind();
	glBufferData(GL_ARRAY_BUFFER, sizeof(UVcoords), UVcoords, GL_STATIC_DRAW);

	GLint arg_uvcoords = sh_sprite.get_attrib("UVcoords");
	glVertexAttribPointer(arg_uvcoords, 2, GL_FLOAT, GL_FALSE, 0, nullptr);
	glEnableVertexAttribArray(arg_uvcoords);

	GL::VAO::unbind();

	sh_sprite();

	GLuint u_tex = sh_sprite.get_uniform("tex");
	glUniform1i(u_tex, 0);

	set_screen_ratio(1, 1);
	//Image img(file);
	//update_texture(img);
	t_sprite(GL_TEXTURE_2D);

	const unsigned char blank_data[] = {0, 0, 0, 0};
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, 1, 1, 0, GL_RGBA, GL_UNSIGNED_BYTE, blank_data);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	GL::Texture::unbind(GL_TEXTURE_2D);

	//adjust_img_ratio();

	rotate(0);
}

void TexSprite::rotate(const float angle){
	sh_sprite();

	glm::mat4 model = glm::mat4(1.f);
	model = glm::rotate(
		model,
		angle * glm::radians(180.0f),
		glm::vec3(0.0f, 0.0f, -1.0f)
	);

	GLint uniModel = sh_sprite.get_uniform("rot");
	glUniformMatrix4fv(uniModel, 1, GL_FALSE, glm::value_ptr(model));
}

void TexSprite::update(const float value){
	sh_sprite();
	GLint u_offset = sh_sprite.get_uniform("tex_offset");
	glUniform1f(u_offset, value);
}

void TexSprite::update_texture(const Image& img){
	t_sprite(GL_TEXTURE_2D);
	// allow arbitrary widths
	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

	switch(img.channels){
		case 4:
			glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, img.width, img.height, 0, GL_RGBA, GL_UNSIGNED_BYTE, img.data);
			break;
		case 3:
			glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, img.width, img.height, 0, GL_RGB, GL_UNSIGNED_BYTE, img.data);
			break;
		default:
			glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, img.width, img.height, 0, GL_RED, GL_UNSIGNED_BYTE, img.data);
	}
	GL::get_error("update Texture");

	width = img.width;
	height = img.height;
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	GL::get_error("set Texture parameters");

	GL::Texture::unbind(GL_TEXTURE_2D);	

	adjust_img_ratio();
}

void TexSprite::adjust_img_ratio(){
	sh_sprite();

	float ratio = get_ratio();
	std::cout << "Image ratio: " << ratio << std::endl;
	glm::mat4 model = glm::ortho(-ratio, ratio, -1.f, 1.f);
	// image is smaller than screen
	if(ratio < screen_ratio){
		model = glm::ortho(-ratio/screen_ratio, ratio/screen_ratio, -1.f/screen_ratio, 1.f/screen_ratio);
		std::cout << "ratio inverted" << std::endl;
	}

	GLuint u_model = sh_sprite.get_uniform("model");
	glUniformMatrix4fv(u_model, 1, GL_FALSE, glm::value_ptr(model));
}

void TexSprite::set_screen_ratio(const int w, const int h){
	screen_ratio = static_cast<float>(w)/h;
	std::cout << "screen ratio:" << screen_ratio << std::endl;
	GLuint u_proj = sh_sprite.get_uniform("proj");
	glm::mat4 model = glm::ortho(-screen_ratio, screen_ratio, -1.f, 1.f);
	glUniformMatrix4fv(u_proj, 1, GL_FALSE, glm::value_ptr(model));

	adjust_img_ratio();
}

void TexSprite::draw(){
	sh_sprite();
	v_sprite.bind();

	glActiveTexture(GL_TEXTURE0);
	t_sprite(GL_TEXTURE_2D);

	glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);

	GL::VAO::unbind();
	GL::Texture::unbind(GL_TEXTURE_2D);
}
