#pragma once
#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <memory>
#include "vec.h"

class Film
{
public:
	//Film public Method
	Film() {};
	Film(const std::string &loaddirname,
		const std::string &loadfilename);
private:
	bool canload_binary(const std::string &filename, std::vector<Vec> &pixels);

public:
	int width, height;
	std::vector<Vec> pixels;
};

