# FSock

Fukuda's Socket Library

Simple TCP socket library for linux

Main repository at [GitLab](https://gitlab.com/joao.fukuda/fsock)

## Using

This uses `CMake >= 3.5` for compiling and installing the program.

You have two methods:

### Method 1

Just include `fsock.hpp` to your C++ code and compile with `g++` using these flags:

* `path_to_fsock.cpp`: to compile the library together with your executable and;
* `-I<path_to_header_dir>`: to include the `fsock.hpp` header to your code (if the `fsock.hpp` is not yet located at your source directory or any of your include directories)

### Method 2

```
git clone https://gitlab.com/joao.fukuda/fsock.git &&\
cd fsock && mkdir build && cd build &&\
cmake .. &&\
make &&\
sudo make install
```

Include the library's header on your code:

```
#include <fsock.hpp>
//...
```

And compile it linking the library as static:

```
g++ source_code.cpp -lfsock
```

## Examples

Basic http client

`main.cpp`:
```
#include <fsock.hpp>
#include <iostream>
#include <string>

int main(int argc, char** argv) {
	if (argc != 3) {
		std::cout << "Usage: " << argv[0] << " <address> <port>\n";
		return 0;
	}

	std::string address = argv[1];
	int port = std::stoi(argv[2]);

	fsock::client server(address, port);

	server
		<< "GET / HTTP/1.1\r\n"
		<< "Host: " << address << "\r\n"
		<< "Accept: text/*\r\n"
		<< "Connection: close\r\n"
		<< "\r\n";

	std::string response;
	server >> response;
	std::cout << response;
}
```

```
g++ -o get main.cpp -lfsock
```

