#ifndef _NETWORK_H_
#define _NETWORK_H_

#include "io.h"

#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>

#include <stdexcept>
#include <inttypes.h> //uint8_t
#include <map>
#include <list>
#include <string>
#include <iostream>
#include <sstream> //stringstream

namespace mavhub {
	typedef std::pair<std::string, uint16_t> string_addr_pair_t;
	std::ostream& operator <<(std::ostream &os, const string_addr_pair_t &string_addr_pair);
	std::istream& operator >>(std::istream &is, string_addr_pair_t &string_addr_pair);

	typedef std::pair<in_addr, uint16_t> num_addr_pair_t;

	/**
	 * \brief Union of sockaddr structs.
	 *
	 * To get rid of "dereferencing pointer ‘sa_in’ does break strict-aliasing rules"
	 * warnings use the members of the union instead of casting pointers.
	 */
	union sockaddr_u {
		struct sockaddr_storage storage;
		struct sockaddr_in in;
		struct sockaddr_in6 in6;
		struct sockaddr sockaddr;
	};

	class Socket : public cpp_io::IOInterface {
		public:
			virtual int open() = 0;
			std::string foreign_addr() const throw(const std::exception&);
			uint16_t foreign_port() const throw(const std::exception&);
			static const std::string& proto_to_name(const int type);

		protected:
			Socket(const int type,const  int protocol);
			Socket(const int socket_fd);
			~Socket();

			struct sockaddr_in si_self;
			mutable struct sockaddr_in si_other;
			
			void bind(const int port) throw(const std::exception&);
			int open(const int type, const int protocol);
		private:
			Socket(const Socket &socket);
			void operator=(const Socket &socket);
	};

	class UDPSocket : public Socket {
		public:
			UDPSocket(int port) throw(const std::exception&);
			virtual ~UDPSocket();

			virtual int open();

			/**
			 * @brief Receive data from any system
			 * @param buffer data to receive
			 * @param buf_len maximal length of data to receive
			 */
			int recv_any(void *buffer, int buf_len) const;

			/**
			 * @brief receive data from given system
			 * @param buffer data to receive
			 * @param buf_len maximal length of data to receive
			 * @param source_addr address of source system as a dots-and-number string
			 * @param source_port port number of source system in host byte order
			 */
			int recv_from(void *buffer, int buf_len, std::string &source_addr, uint16_t source_port) const throw(const std::exception&);

			/**
			 * @brief send data to given system
			 * @param buffer data to send
			 * @param buf_len length of data to send
			 * @param foreign_addr address of destination system as a dots-and-number string
			 * @param foreign_port port number of destination system in host byte order
			 * @return number of bytes actually sent
			 */
			int send_to(const void *buffer, int buf_len, const std::string &foreign_addr, uint16_t foreign_port) const throw(const std::exception&);
			/**
			 * @brief send data to given system
			 * @param buffer data to send
			 * @param buf_len length of data to send
			 * @param foreign_addr numeric address of destination system
			 * @param foreign_port port number of destination system in host byte order
			 * @return number of bytes actually sent
			 */
			int send_to(const void *buffer, int buf_len, in_addr foreign_addr, uint16_t foreign_port) const throw(const std::exception&);

		private:
	};

	class TCPSocket : public Socket {
		public:
			TCPSocket();
			TCPSocket(const std::string &foreign_addr, uint16_t foreign_port);
			virtual ~TCPSocket();

			virtual int open();

			void connect(const std::string &foreign_addr, uint16_t foreign_port);
			void disconnect();
			void send(const void *buffer, int buf_len);
			/**
			 * @brief Receive data from foreign system
			 * @param buffer data to receive
			 * @param buf_len maximal length of data to receive
			 */
			int receive(void *buffer, int buf_len) const;

		private:
			friend class TCPServerSocket;
			TCPSocket(int socket_fd);
	};
	
	class TCPServerSocket : public Socket {
		public:
			TCPServerSocket(uint16_t port, int connections) throw(const std::exception&);
// 			TCPServerSocket(const std::string &address, uint16_t port) throw(const std::exception&);
			virtual ~TCPServerSocket();

			virtual int open();

			TCPSocket* accept() throw(const std::exception&);

		
	};

	// ----------------------------------------------------------------------------
	// Socket
	// ----------------------------------------------------------------------------

	// ----------------------------------------------------------------------------
	// TCPSocket
	// ----------------------------------------------------------------------------
	inline int TCPSocket::open() {
		return -1;
	}
	// ----------------------------------------------------------------------------
	// TCPServerSocket
	// ----------------------------------------------------------------------------
	inline int TCPServerSocket::open() {
		return -1;
	}
} // namespace mavhub

#endif
