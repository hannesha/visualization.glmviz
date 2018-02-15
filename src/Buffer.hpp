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

#include <vector>
#include <cstdint>
#include <mutex>
#include <memory>

template<typename T>
class Buffer {
	public:
		Buffer(const size_t);
		Buffer(const Buffer& b) = delete;
		Buffer(Buffer&& b): v_buffer(std::move(b.v_buffer)), size(std::move(b.size)){};
		Buffer& operator=(Buffer&& b){ v_buffer = std::move(b.v_buffer); size = std::move(b.size);  return *this; };

		std::vector<T> v_buffer;
		bool new_data;
		size_t size;

		std::unique_lock<std::mutex> lock();
		void write(const T buf[], const size_t);
		void write(const std::vector<T>& buf);
		void write_offset(const T buf[], const size_t, const size_t, const size_t);
		void write_offset(const std::vector<T>& buf, const size_t, const size_t);
		void resize(const size_t);
		float rms();

		unsigned get_age() const{ return age; }
		void increment_age(){ age++; }
		void get_slice(unsigned steps, unsigned slice_length, unsigned& length, unsigned& start);

	private:
		std::mutex m;

		std::vector<T> ibuf; // intermediate buffer for interleaved writes
		void i_write(const T buf[], const size_t);
		void i_write(const std::vector<T>&, const size_t);
		unsigned age; // replaces new_data
};

struct Buffers{
	using Ptr = std::shared_ptr<Buffers>;
	std::vector<Buffer<int16_t>> bufs;
	std::mutex mut;

	Buffers():bufs(), mut(){};
};

template class Buffer<float>;
template class Buffer<int16_t>;
