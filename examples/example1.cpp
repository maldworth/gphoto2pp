#include "helper_gphoto2.hpp"

#include <iostream>

int main(int argc, char* argv[]) {
	auto version = gphoto2pp::LibraryVersion();
	std::cout << "#############################" << std::endl;
	std::cout << "# Short Version - " << std::endl;
	std::cout << "#############################" << std::endl;
	std::cout << version << std::endl << std::endl;
	
	version = gphoto2pp::LibraryVersion(true);
	std::cout << "#############################" << std::endl;
	std::cout << "# Verbose Version - " << std::endl;
	std::cout << "#############################" << std::endl;
	std::cout << version << std::endl << std::endl;
}
