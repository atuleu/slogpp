#include <iostream>

void someFunction(int foo) {
	for (int i = 0; i < foo; i++) {
		std::cerr << "Not in your dream" << std::endl;
	}
}

int main() {
	std::cout << "hello world, this is a very long message that will break the "
	             "fill-column width"
	          << std::endl;
	someFunction(2);
	return 0;
}
