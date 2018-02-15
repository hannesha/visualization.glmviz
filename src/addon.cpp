#include <kodi/addon-instance/Visualization.h>
#include <iostream>
#include <cmath>
#include "Oscilloscope.hpp"
#include "Triangle.hpp"
#include "Spectrum.hpp"

class CGLMViz : public kodi::addon::CAddonBase,
						 public kodi::addon::CInstanceVisualization{
public:
	CGLMViz();
	~CGLMViz() override;

	ADDON_STATUS Create() override;
	bool Start(int channels, int samplesPerSec, int bitsPerSample, std::string songName) override;
	void Stop() override;
	void AudioData(const float* audioData, int audioDataLength, float* freqData, int freqDataLength) override;
	void Render() override;
	void GetInfo(bool& wantsFreq, int& syncDelay) override;
	float ratio(){return static_cast<float>(Width())/Height();}

	bool GetPresets(std::vector<std::string>&) override;

private:
	//std::vector<float> audiol, audior;
	std::vector<Buffer<float>> baudio;
	std::vector<Oscilloscope> osc;
	std::vector<Spectrum> spectra;
	std::vector<FFT> ffts;
	//std::vector<float> fft_old, fft_avg;
	std::vector<Triangle> t;
	unsigned buffer_age = 0;
	unsigned kodi_buffer_len = 1;
};

CGLMViz::CGLMViz(){
	std::cout << "constructed!" << std::endl;
}

CGLMViz::~CGLMViz(){
	std::cout << "Destroyed!" << std::endl;
}

ADDON_STATUS CGLMViz::Create(){
	const unsigned fft_size = 4096;
	const unsigned buffer_size = 2204;

	const unsigned data_size = std::min(fft_size, buffer_size);
	//std::cout << "samples:" << samplesPerSec << std::endl;
	std::cout << glGetString(GL_VERSION) << std::endl;
	std::cout << "create!" << std::endl;
	Module_Config::Oscilloscope tmp;
	tmp.scale = 1.5;
	baudio.emplace_back(buffer_size);
	osc.emplace_back(tmp, 0);
	t.emplace_back();

	//fft_old.resize(4069);
	ffts.emplace_back(fft_size);

	Module_Config::Spectrum spec;
	spec.scale = 1.f/data_size;
	spec.gravity = 7;
	spec.top_color = {1., 1., 1., 1.};
	spec.bot_color = {0.5, 0.5, 0.5, 1.};
	spec.output_size = 100;
	// set aspect ratio
	spec.pos.Xmin = -ratio();
	spec.pos.Xmax = ratio();

	float max_n = -10 * 0.05;
	float min_n = -60 * 0.05;
	spec.slope = -2.0 / (min_n - max_n);
	spec.offset = 1.0 - spec.slope * max_n;

	spectra.emplace_back(spec, 0);
	//std::cout << osc.size() << std::endl;
	return ADDON_STATUS_OK;
}

bool CGLMViz::GetPresets(std::vector<std::string>& presets){
	presets.emplace_back("Test");
	return true;
}

void CGLMViz::GetInfo(bool& wantsFreq, int& syncDelay){
	wantsFreq = false;
	syncDelay = 0;
}

bool CGLMViz::Start(int channels, int samplesPerSec, int bitsPerSample, std::string songName){
	std::cout << "starting!" << std::endl;
	return true;
}

void CGLMViz::Stop(){

	std::cout << "stopping!" << std::endl;
}

void CGLMViz::AudioData(const float* audioData, int audioDataLength, float* freqData, int freqDataLength){
	// Stereo test
//	float lmax = 0;
//	float rmax = 0;
//
//	for(int i = 0; i < audioDataLength -1; i++){
//		if(i%2){
//			lmax = std::max(lmax, audioData[i]);
//		}else{
//			rmax = std::max(rmax, audioData[i]);
//		}
//	}
//std::cout << "L: " << 20*std::log10(lmax) << " R: " << 20*std::log10(rmax) << std::endl;

//	if(audiol.size() != (audioDataLength / 2)){
//		audiol.resize(static_cast<unsigned>(audioDataLength / 2));
//	}
//	for(int i = 0; i < (audioDataLength /2); i++){
//		audiol[i] = audioData[i*2];
//	}
	baudio[0].write_offset(audioData, audioDataLength, 2, 0);

	// Store old fft data
//	if(fft_old.size() != 2048){
//		fft_old.resize(2048);
//	}
//	for(int i = 0; i < fft_old.size(); i +=2){
//		fft_old[i] = ffts[0].output[i/2][0];
//		fft_old[i+1] = ffts[0].output[i/2][1];
//	}
	//ffts[0].calculate(audiol, 0, audiol.size());

	// Buffer hacks
	buffer_age = 0;
	kodi_buffer_len = audioDataLength;
}

void CGLMViz::Render(){
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	t[0].draw();
	//auto lock = baudio[0].lock();
	const unsigned step_interval = 3; // slice buffer 3 times per update
	if(baudio[0].get_age() > step_interval){
		// audio buffer is too old
		//std::cout << "Buffer age :" << buffer_age << std::endl;
	}

	// slice buffer
	unsigned start, buf_length;
	baudio[0].get_slice(step_interval, kodi_buffer_len/2, start, buf_length);
	//unsigned step = (kodi_buffer_len/2)/step_interval;
	//unsigned start = std::min(step * baudio[0].get_age(), step * (step_interval -1));
	//std::cout << "start :" <<start << std::endl;

	//unsigned draw_len = baudio[0].size - (step * (step_interval - 1));
//	if(draw_len + start > baudio[0].size){
//		std::cout << "Buffer too large" << std::endl;
//	}

//	if(buffer_age == 0){
//		osc[0].update_buffer(baudio[0]);
//	}
//
//
//	osc[0].draw(start, draw_len);


	// FFT smoothing (for future use)
//	float mix = std::min((float)(buffer_age+1)/(float)step_interval, 1.0f);
//	float mix1 = 1. - mix;
//	if(fft_avg.size() != 50*2){
//		fft_avg.resize(50*2);
//	}
//	for(int i = 0; i<fft_avg.size(); i+=2){
//		fft_avg[i] = ffts[0].output[i/2][0] * mix + fft_old[i] * mix1;
//		fft_avg[i+1] = ffts[0].output[i/2][1] * mix + fft_old[i+1] * mix1;
//	}

	// update buffer slice
	//auto lock = baudio[0].lock();
	ffts[0].calculate(baudio[0].v_buffer, start, buf_length);

	spectra[0].update_fft(ffts[0]);

	spectra[0].draw(0.016);

	// increment buffer age for the next frame
	baudio[0].increment_age();
}

ADDONCREATOR(CGLMViz)
