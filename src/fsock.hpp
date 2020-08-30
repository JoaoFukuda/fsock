#include <iostream>
#include <string>

#include <arpa/inet.h>
#include <sys/socket.h>
#include <unistd.h>

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
		connection(family f, protocol p)
			: sock_family(f)
		{
			sock = socket(static_cast<int>(f), static_cast<int>(p), 0);
		}

		connection(int socket)
			: sock(socket)
		{ }

		~connection()
		{
			close(sock);
		}

		void set_option(layer l, option o, int v) const
		{
			setsockopt(sock, static_cast<int>(l), static_cast<int>(o), &v, sizeof(v));
		}

		void connect_to(const std::string & addr, const int port) const
		{
			sockaddr_in host_info;
			host_info.sin_family = static_cast<int>(sock_family);
			host_info.sin_port = htons(port);
			inet_pton(static_cast<int>(sock_family), addr.c_str(), &host_info.sin_addr);

			connect(sock, reinterpret_cast<const sockaddr *>(&host_info), sizeof(host_info));
		}

		void make_server(int port, int listeners = 5) const
		{
			sockaddr_in host_info;
			host_info.sin_family = static_cast<int>(sock_family);
			host_info.sin_addr.s_addr = INADDR_ANY;
			host_info.sin_port = htons(port);

			bind(sock, reinterpret_cast<const sockaddr *>(&host_info), sizeof(host_info));

			listen(sock, listeners);
		}

		int accept_connection() const
		{
			sockaddr_in client_info;
			socklen_t client_len = sizeof(client_info);

			return accept(sock, reinterpret_cast<sockaddr *>(&client_info), &client_len);
		}

		void send_data(const std::string & data) const
		{
			send(sock, data.c_str(), data.size(), 0);
		}

		const connection & operator<<(const std::string & data) const
		{
			send_data(data);
			return *this;
		}

		const std::string receive_data(int buf_size = 1024) const
		{
			char buf[buf_size];
			recv(sock, buf, buf_size - 1, 0);
			return std::string(buf);
		}
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

		const connection & operator<<(const std::string & data) const
		{
			sock.send_data(data);
			return sock;
		}

		const std::string receive_data() const
		{
			return sock.receive_data();
		}
	};

	inline std::ostream & operator<<(std::ostream &out, const fsock::connection & conn)
	{
		return out << conn.receive_data();
	}

	inline std::ostream & operator<<(std::ostream &out, const fsock::client & client)
	{
		return out << client.receive_data();
	}
}

