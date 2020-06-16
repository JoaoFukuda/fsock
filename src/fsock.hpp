#include <iostream>
#include <string>

#include <arpa/inet.h>
#include <sys/socket.h>
#include <unistd.h>

namespace fsock {
	enum family {
		f_ip4 = AF_INET,
		f_ip6 = AF_INET6
	};

	enum  protocol {
		p_tcp = SOCK_STREAM,
		p_udp = SOCK_DGRAM
	};

	enum layer {
		l_socket = SOL_SOCKET
	};

	enum option {
		reuse_addr = SO_REUSEADDR,
		reuse_port = SO_REUSEPORT
	};

	class connection {
	private:
		int sock, sock_family;

	public:
		connection(family f, protocol p)
			: sock_family(f)
		{
			sock = socket(f, p, 0);
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
			setsockopt(sock, l, o, &v, sizeof(v));
		}

		void make_server(int port) const
		{
			sockaddr_in host_info;
			host_info.sin_family = sock_family;
			host_info.sin_addr.s_addr = INADDR_ANY;
			host_info.sin_port = htons(port);

			bind(sock, reinterpret_cast<const sockaddr *>(&host_info), sizeof(host_info));

			listen(sock, 5);
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
		server(int port, family f = f_ip4, protocol p = p_tcp)
			: sock(f, p)
		{
			sock.set_option(l_socket, reuse_addr, 1);
			sock.make_server(port);
		};

		const connection get_client() const
		{
			return sock.accept_connection();
		}
	};
}

inline std::ostream & operator<<(std::ostream &out, const fsock::connection & conn)
{
	return out << conn.receive_data();
}

