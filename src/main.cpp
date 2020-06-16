#include "fsock.hpp"

int main()
{
	fsock::server server(3000);

	auto client = server.get_client();

	client << "Hello, world!" << "\n";
	std::cout << client << std::endl;
}

