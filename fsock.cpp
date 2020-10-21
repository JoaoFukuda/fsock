#include <fsock.hpp>

#include <exception>
#include <regex>
#include <cstring>

namespace cstd {
#include <netdb.h>
}

const std::regex ip_re("[0-9]{1,3}(\\.[0-9]{1,3}){3}");

fsock::connection::connection(family f, protocol p)
	: sock_family(f)
{
	sock = socket(static_cast<int>(f), static_cast<int>(p), 0);
};

fsock::connection::connection(int socket)
	: sock(socket)
{ };

fsock::connection::~connection()
{
	close(sock);
};

void fsock::connection::set_option(layer l, option o, int v) const
{
	setsockopt(sock, static_cast<int>(l), static_cast<int>(o), &v, sizeof(v));
}

void fsock::connection::connect_to(const std::string & addr, const int port) const
{
	sockaddr_in host_info;
	host_info.sin_family = static_cast<int>(sock_family);
	host_info.sin_port = htons(port);
	if (std::regex_match(addr, ip_re)) {
		inet_pton(static_cast<int>(sock_family), addr.c_str(), &host_info.sin_addr);
	} else {
		cstd::hostent* he;
		in_addr** addr_list;
		int i;

		if ((he = cstd::gethostbyname(addr.c_str())) == NULL) throw std::runtime_error("Could not resolve domain " + addr);

		addr_list = reinterpret_cast<in_addr**>(he->h_addr_list);
		if (addr_list[0] == NULL) std::runtime_error("Could not find any address for domain " + addr);
		inet_pton(static_cast<int>(sock_family), cstd::inet_ntoa(*addr_list[0]), &host_info.sin_addr);
	}

	connect(sock, reinterpret_cast<const sockaddr *>(&host_info), sizeof(host_info));
}

void fsock::connection::make_server(int port, int listeners) const
{
	sockaddr_in host_info;
	host_info.sin_family = static_cast<int>(sock_family);
	host_info.sin_addr.s_addr = INADDR_ANY;
	host_info.sin_port = htons(port);

	bind(sock, reinterpret_cast<const sockaddr *>(&host_info), sizeof(host_info));

	listen(sock, listeners);
}

int fsock::connection::accept_connection() const
{
	sockaddr_in client_info;
	socklen_t client_len = sizeof(client_info);

	return accept(sock, reinterpret_cast<sockaddr *>(&client_info), &client_len);
}

void fsock::connection::send(const std::string & data) const
{
	cstd::send(sock, data.c_str(), data.size(), 0);
}

const std::string fsock::connection::recv() const
{
	std::string whole_data = "";
	int part = 0, part_size;
	char buf[1025]{0};
	while ((part_size = cstd::recv(sock, buf, 1024, 0)) != 0) {
		whole_data += buf;
		std::memset(buf, 0, sizeof(buf));
	}
	return whole_data;
}

const fsock::connection & fsock::connection::operator<<(const std::string & data) const
{
	send(data);
	return *this;
}

const fsock::connection & fsock::connection::operator>>(std::string & data) const
{
	data = recv();
	return *this;
}

