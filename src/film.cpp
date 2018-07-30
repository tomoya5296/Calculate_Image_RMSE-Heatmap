#include "film.h"

Film::Film(const std::string &loaddirname,
	const std::string &loadfilename)
{
	canload_binary(loaddirname + loadfilename, pixels);
}

bool Film::canload_binary(const std::string &filename, std::vector<Vec> &pixels) {
	std::ifstream reader;
	reader.open(filename.c_str(), std::ios::binary);
	if (!reader.is_open()) {
		fprintf(stderr, "Failed to open file: %s\n", filename.c_str());
		return false;
	}

	reader.read((char*)&width, sizeof(int));
	reader.read((char*)&height, sizeof(int));
	auto buffer = std::make_unique<float[]>(width * 3);
	pixels.resize(width * height);
	for (int y = 0, index = 0; y < height; y++) {
		reader.read((char*)buffer.get(), sizeof(float) * width * 3);
		for (int x = 0; x < width; x++, index++) {
			const double r = buffer[x * 3 + 0];
			const double g = buffer[x * 3 + 1];
			const double b = buffer[x * 3 + 2];
			pixels[index] = Vec(r, g, b);
		}
	}
	reader.close();
	return true;
}