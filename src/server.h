#ifndef SERVER_H
#define SERVER_H

#include "top_k_queue_nrd.h"

#ifdef _WIN32
/* Headerfiles für Windows */
#include <winsock.h>
#include <io.h>
#else
/* Headerfiles für UNIX/Linux */
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#endif

#define BUFFER_SIZE 4096

class Server {
public:
	Server(const std::string& pathToVectorSpaceData, uint16_t portNr);
	virtual ~Server();
	void communicate();
private:
#ifdef _WIN32
#pragma comment(lib, "Ws2_32.lib")
	SOCKET _sockFd;
	SOCKET _newSockFd;
#else
	int _sockFd;
	int _newSockFd;
#endif
    TopKQueueNRD _topKQueueNRD;
    size_t _compressionBlockRows;
    size_t _compressionBlockCols;
    size_t _compressionLevels;
};

#endif // SERVER_H
