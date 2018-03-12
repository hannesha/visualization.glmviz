#include "Image.hpp"
#include <fstream>
#include <iterator>
#include <exception>
#include <sstream>
#include <iostream>

#include <SOIL/SOIL.h>

Image::Image(const std::string& path){
	std::vector<unsigned char> imagedata;
	try{
		std::ifstream file(path, std::ios::binary);
		if(!file.good()){
			throw std::ios_base::failure("");
		}
		file.exceptions(std::ifstream::failbit);

		std::ostringstream os;
		os << file.rdbuf();
		std::string str = os.str();
		imagedata.assign(str.begin(), str.end());

	}
	catch(const std::ios_base::failure& e){
		std::string msg("Can't load file: " + path + "!");
		throw std::runtime_error(msg);
	}

	data = SOIL_load_image_from_memory(imagedata.data(), imagedata.size(), &width, &height, &channels, SOIL_LOAD_AUTO);
	if (data == nullptr){
		std::string msg = "File: " + path + " doesn't contain valid image data!";
		throw std::runtime_error(msg);
	}
}

Image::Image(const std::vector<unsigned char>& imagedata){
	data = SOIL_load_image_from_memory(imagedata.data(), imagedata.size(), &width, &height, &channels, SOIL_LOAD_AUTO);
	if (data == nullptr){
		std::string msg = "Image doesn't contain valid image data!";
		throw std::runtime_error(msg);
	}
}

Image::~Image(){
	SOIL_free_image_data(data);
}
