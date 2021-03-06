#include <iostream>
#include <iomanip>
#include <boost/lexical_cast.hpp>

#include "../src/utils/matrixutils.h"

void test_rotate270() {
	std::cout << "test matrixutils tools" << std::endl;
	Resolution vr = {.width = 9, .height = 16};

	std::string res[vr.height][vr.width];
	
	std::cout << "原始图像栅格" << std::endl;
	for (int x = 0; x < vr.width; ++x) {
		for (int y = 0; y < vr.height; ++y) {
			MES in = {.x = x, .y = y};
			MES out = rotateMatrix270(in, vr);
			std::cout << x << "," << y << " ";
			res[out.x][out.y] = boost::lexical_cast<std::string>(x) + "," + boost::lexical_cast<std::string>(y);
		}

		std::cout << std::endl; 	
	}

	std::cout << std::endl;

	std::cout << "转换270度后图像栅格" << std::endl;
	for (int x = 0; x < vr.height; ++x) {
		for (int y = 0; y < vr.width; ++y) {
			std::cout << std::setw(4) << std::setfill(' ') << res[x][y] << " ";
		}

		std::cout << std::endl;
	}
}

void test_rotate90() {
	std::cout << "test matrixutils tools" << std::endl;
	Resolution vr = {.width = 9, .height = 16};

	std::string res[vr.height][vr.width];
	
	std::cout << "原始图像栅格" << std::endl;
	for (int x = 0; x < vr.width; ++x) {
		for (int y = 0; y < vr.height; ++y) {
			MES in = {.x = x, .y = y};
			MES out = rotateMatrix90(in, vr);
			std::cout << x << "," << y << " ";
			res[out.x][out.y] = boost::lexical_cast<std::string>(x) + "," + boost::lexical_cast<std::string>(y);
		}

		std::cout << std::endl; 	
	}

	std::cout << std::endl;

	std::cout << "转换90度后图像栅格" << std::endl;
	for (int x = 0; x < vr.height; ++x) {
		for (int y = 0; y < vr.width; ++y) {
			std::cout << std::setw(4) << std::setfill(' ') << res[x][y] << " ";
		}

		std::cout << std::endl;
	}
}

int main(int argc, const char *argv[]) {
	test_rotate270();
	test_rotate90();
	return 0;
}
