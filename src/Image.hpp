#pragma once

#include <string>
#include <vector>

// Image data RAII wrapper
class Image{
	public:
		Image(const std::string&);
		Image(const std::vector<unsigned char>&);
		~Image();

		unsigned char* data;
		int width, height, channels;
};
