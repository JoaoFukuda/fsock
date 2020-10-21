#pragma once

#include <iostream>
#include <string>

namespace cstd {
	#include <arpa/inet.h>
	#include <sys/socket.h>
	#include <unistd.h>
}

using namespace cstd;

namespace fsock {
	enum class family {
		ipv4 = AF_INET,
		ipv6 = AF_INET6
	};

	enum class protocol {
		tcp = SOCK_STREAM,
		udp = SOCK_DGRAM
	};

	enum class layer {
		socket = SOL_SOCKET
	};

	enum class option {
		reuse_addr = SO_REUSEADDR,
		reuse_port = SO_REUSEPORT
	};

	class connection {
	private:
		int sock;
		family sock_family;

	public:
		connection(family f, protocol p);
		connection(int socket);
		~connection();

		void	set_option(layer l, option o, int v) const;
		void	connect_to(const std::string & addr, const int port) const;
		void	make_server(int port, int listeners = 5) const;
		int	accept_connection() const;
		void	send(const std::string & data) const;
		const	std::string recv() const;

		const connection & operator<<(const std::string & data) const;
		const connection & operator>>(std::string & data) const;
	};

	class server {
	private:
		connection sock;

	public:
		server(const int port, const family f = family::ipv4, const protocol p = protocol::tcp)
			: sock(f, p)
		{
			sock.set_option(layer::socket, option::reuse_addr, 1);
			sock.make_server(port);
		};

		const connection accept() const
		{
			return sock.accept_connection();
		}
	};

	class client {
	private:
		connection sock;

	public:
		client(const std::string & addr, const int port, const family f = family::ipv4, const protocol p = protocol::tcp)
			: sock(f, p)
		{
			sock.connect_to(addr, port);
		}

		void send(const std::string & data) const
		{
			sock.send(data);
		}

		const std::string recv() const
		{
			return sock.recv();
		}

		const client & operator<<(const std::string & data) const
		{
			sock << data;
			return *this;
		}

		const client & operator>>(std::string & data) const
		{
			sock >> data;
			return *this;
		}
	};

	inline std::ostream & operator<<(std::ostream &out, const fsock::connection & conn)
	{
		return out << conn.recv();
	}

	inline std::ostream & operator<<(std::ostream &out, const fsock::client & client)
	{
		return out << client.recv();
	}
}

