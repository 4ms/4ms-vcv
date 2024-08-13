#include "buffer.hh"
#include <cassert>
#include <iostream>

template<typename T>
class WavWriter {
	FILE *fp;

public:
	WavWriter(const char *filename, uint32_t size, uint16_t num_channels, uint32_t sample_rate) {
		fp = fopen(filename, "wb");
		if (fp == NULL) {
			std::cerr << "output file \"" << filename << "\" cannot be created" << std::endl;
			exit(1);
			;
		}

		fwrite("RIFF", 4, 1, fp);
		uint32_t len = 44 - 8 + size * num_channels * sizeof(T); // file length in bytes
		fwrite(&len, 4, 1, fp);
		fwrite("WAVEfmt ", 8, 1, fp);
		uint32_t bs = 0x10; // block size
		fwrite(&bs, 4, 1, fp);
		uint16_t fmt = 0x1; // format: 1=PCM
		fwrite(&fmt, 2, 1, fp);
		uint16_t chan = num_channels; // channel nr
		fwrite(&chan, 2, 1, fp);
		uint32_t freq = sample_rate; // sample rate
		fwrite(&freq, 4, 1, fp);
		uint16_t bpsamples = 8 * sizeof(T) / num_channels;			 // bits per samples
		uint32_t bps = sample_rate * (bpsamples / 8) * num_channels; // bytes per second
		fwrite(&bps, 4, 1, fp);
		uint16_t bpb = num_channels * (bpsamples / 8); // bytes per blocks
		fwrite(&bpb, 2, 1, fp);
		fwrite(&bpsamples, 2, 1, fp);
		fwrite("data", 4, 1, fp);
		uint32_t datasize = size * (bpsamples / 8) * num_channels;
		fwrite(&datasize, 4, 1, fp);
	}
	// samples from channels in output are interlaced
	template<int size>
	void Write(Buffer<T, size> &output) {
		fwrite(output.data(), sizeof(T), size, fp);
	}
};

class WavReader {
	FILE *fp;
	uint32_t size_; // nr of samples
	uint16_t num_channels_;
	uint32_t sample_rate_;

public:
	WavReader(const char *filename) {
		fp = fopen(filename, "r");
		if (fp == NULL) {
			std::cerr << "input file \"" << filename << "\" not found" << std::endl;
			exit(1);
			;
		}

		char data[] = "\0\0\0\0\0\0\0\0";

		if (!(fread(data, 1, 4, fp) == 4 && data[0] == 'R' && data[1] == 'I' && data[2] == 'F' && data[3] == 'F')) {
			std::cerr << "bad WAV format (RIFF)" << std::endl;
			exit(1);
		}

		uint32_t len; // file size - 8 bytes
		fread(&len, 4, 1, fp);

		if (!(fread(data, 1, 8, fp) == 8 && data[0] == 'W' && data[1] == 'A' && data[2] == 'V' && data[3] == 'E' &&
			  data[4] == 'f' && data[5] == 'm' && data[6] == 't' && data[7] == ' '))
		{
			std::cerr << "bad WAV format (WAVEfmt)" << std::endl;
			exit(1);
		}

		uint32_t bs; // size of header section
		fread(&bs, 4, 1, fp);
		if (bs != 0x10) {
			std::cerr << "bad WAV format (bs)" << std::endl;
			exit(1);
		}

		uint16_t fmt; // 1=PCM
		fread(&fmt, 2, 1, fp);
		if (fmt != 0x1) {
			std::cerr << "bad WAV format (fmt)" << std::endl;
			exit(1);
		}

		fread(&num_channels_, 2, 1, fp);
		fread(&sample_rate_, 4, 1, fp);

		uint32_t bps; // bytes per second
		fread(&bps, 4, 1, fp);

		uint16_t bpb; // bytes per blocks
		fread(&bpb, 2, 1, fp);

		if (bps != sample_rate_ * bpb) {
			std::cerr << "bad WAV format (bpb)" << std::endl;
			exit(1);
		}

		uint16_t bpsamples; // bits per samples
		fread(&bpsamples, 2, 1, fp);

		if (!(fread(data, 1, 4, fp) == 4 && data[0] == 'd' && data[1] == 'a' && data[2] == 't' && data[3] == 'a')) {
			std::cerr << "bad WAV format (data)" << std::endl;
			exit(1);
		}

		uint32_t datasize;
		fread(&datasize, 4, 1, fp);
		size_ = datasize * 8 / num_channels_ / bpsamples;
	}

	template<class T, int size>
	bool Read(Buffer<T, size> &input) {
		return fread(input.begin().repr(), sizeof(T), size, fp) == size;
	}

	int size() {
		return size_;
	}
	int sample_rate() {
		return sample_rate_;
	}
	int num_channels() {
		return num_channels_;
	}
};
