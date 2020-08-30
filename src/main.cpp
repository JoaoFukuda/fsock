#include "fsock.hpp"

int main(int argc, char** argv)
{
	if (argc != 2) {
		return -1;
	}

	if (argv[1][0] == 's') {
		fsock::server server(3000);

		auto client = server.accept();

		client << "Hello, world!\n";
		std::cout << client << std::flush;
	} else if (argv[1][0] == 'c') {
		fsock::client c("127.0.0.1", 3000);

		c << "Hello, world!\n";
		std::cout << c << std::flush;
	}
}

