#pragma once

#include <cmath>

//CPUでGPUと同等の事を実行する関数群
namespace cpu{
	// 演算
	template <typename INPUT, typename OUTPUT>
	void run(unsigned int num_loop, INPUT *input, OUTPUT *output) {
		for (int i = 0; i < num_loop; i++) {
			for (int j = 0; j < num_loop*2; j++) {
				output->b[i] = std::sin((float)input->a[i]);
			}
		}
	}
}