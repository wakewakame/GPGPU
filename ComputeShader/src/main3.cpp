#include "gpgpu2.h"
#include "RUN.h"
#include <iostream>
#include <windows.h>

void main()
{
	test::cpu::init();
	test::gpu::init();

	std::string text = "";
	for (int i = 0; i < 100; i++) {
		//std::cout << "cpu:" << std::fixed << test::cpu::run() << "sec,gpu:" << test::gpu::run() << "sec" << std::endl;
		std::string cpu = std::to_string(test::cpu::run());
		std::string gpu = std::to_string(test::gpu::run());
		//std::cout << cpu << "\n" << gpu << "\n";
		text += cpu + "\n" + gpu + "\n";
	}

	std::ofstream outputfile("result.txt");
	outputfile << text;
	outputfile.close();

	test::cpu::exit();
	test::gpu::exit();
	return;
}