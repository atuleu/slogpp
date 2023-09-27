#include <iostream>

int main() {
	std::cout << "hello world, this is a very long message that will break the "
	             "fill-column width"
	          << std::endl;
	return 0;
}

void someFunction(int foo) {
	std::cerr << "Not in your dream" << std::endl;
}
