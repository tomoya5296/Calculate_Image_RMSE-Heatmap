#include <iostream>
#include <vector>
#include <fstream>
#include <memory>
#include <algorithm>
#include "vec.h"
#include "film.h"



inline double clamp(double x) {
	if (x < 0.0)
		return 0.0;
	if (x > 1.0)
		return 1.0;
	return x;
}

inline int to_int(double x) {
	return int(pow(clamp(x), 1 / 2.2) * 255 + 0.5);
}

// PPMファイルの保存
void save_ppm_file(const std::string &filename, const Color *image, const int width, const int height) {
	std::ofstream writer(filename.c_str(), std::ios::out);
	writer << "P3" << std::endl;
	writer << width << " " << height - 3 << std::endl;
	writer << 255 << std::endl;
	for (int i = 0; i < width * height; i++) {
		const int r = to_int(image[i].x);
		const int g = to_int(image[i].y);
		const int b = to_int(image[i].z);
		writer << r << " " << g << " " << b << " ";
	}
	writer.close();
}

int main(int argc, char **argv) {
	//第一引数がdir,  第二がref, 第三がresult
	const std::string DIRECTORY = argv[1];
	const std::string REFERENCEIMAGEBIN = argv[2];
	const std::string RESULTIMAGEBIN = argv[3];

	//正解画像と生成画像のロード
	const Film ref(DIRECTORY, REFERENCEIMAGEBIN);
	const Film result(DIRECTORY, RESULTIMAGEBIN);
	const int num_pixels = result.pixels.size();
	std::vector<float> rmses(num_pixels);

	//RMSE値の計算
	for (int i = 0; i < num_pixels; i++) {
		const Vec &refPixel = ref.pixels[i];
		const Vec &resultPixel = result.pixels[i];
		rmses[i] = std::sqrt((std::pow((refPixel.x - resultPixel.x), 2.0) +
			std::pow((refPixel.y - resultPixel.y), 2.0) +
			std::pow((refPixel.z - resultPixel.z), 2.0)) / 3.0);
	}

	//エラーを0~1にclampするために1の値を適当な値で決める
	float rmse_max = 0.0;
	{
		std::vector<float> rmses_index = rmses;
		std::sort(rmses_index.begin(), rmses_index.end());
		rmse_max = rmses_index[0.95 * num_pixels];
		if (rmse_max <= 0.0) {
			fprintf(stderr, "rmse_max <= 0.0 !");
		}
	}

	//ヒートマップの生成
	auto heatmap = std::make_unique<Color[]>(num_pixels);
	for (int i = 0; i < num_pixels; i++) {
		float rmse_clamped = rmses[i] / rmse_max;
		if (rmse_clamped <= 0.5) {
			heatmap[i].y = rmse_clamped * 2.0;
			heatmap[i].z = (1.0 - rmse_clamped * 2.0);
		}
		else {
			heatmap[i].x = (rmse_clamped - 0.5) * 2.0;
			heatmap[i].y = (1.0 - (rmse_clamped - 0.5) * 2.0);
		}
	}

	//画像の保存
	save_ppm_file(DIRECTORY + "heatmap.ppm", heatmap.get(), result.width, result.height);

}