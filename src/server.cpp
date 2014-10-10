
#include "server.h"
#include "query_vector.h"
#include "top_k_queue_nrd.h"
#include "util.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fstream>
#include <iostream>
#include <sstream>

Server::Server(const std::string &pathToVectorSpaceData, uint16_t portNr) :
	_sockFd(0),
	_newSockFd(0),
    _topKQueueNRD(),
	_compressionBlockRows(3),
    _compressionBlockCols(1),
    _compressionLevels(2)
{
    std::ifstream input0(pathToVectorSpaceData);
	std::cout << "Using index file " << pathToVectorSpaceData << std::endl;
    int i = 0;
    for( std::string line; getline( input0, line ); i++){
        vec vector = vec();
        util::stringToVec(line,vector);
        _topKQueueNRD.addVector(util::getJsonInt(line, "id"),vector);
    }
    _topKQueueNRD.buildIndex(_compressionBlockRows, _compressionBlockCols, _compressionLevels);
    std::cout << _topKQueueNRD.getVectorSpaceSize() << " vectors of length " << _topKQueueNRD.getVectorSpace().getVectorSpacePyramide()[0][0].size() << std::endl;

    struct sockaddr_in serv_addr;
#ifdef _WIN32
	/* Initialisiere TCP für Windows ("winsock"). */
	WORD wVersionRequested;
	WSADATA wsaData;
	wVersionRequested = MAKEWORD(1, 1);
	if (WSAStartup(wVersionRequested, &wsaData) != 0){
		fprintf(stderr, "%s: %d\n", "Fehler beim Initialisieren von Winsock", WSAGetLastError());
	}
#endif
    _sockFd = socket(AF_INET, SOCK_STREAM, 0);
    if (_sockFd < 0) {
       std::cout << "ERROR opening socket" << std::endl;
    }
	memset( &serv_addr, 0, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = INADDR_ANY;
    serv_addr.sin_port = htons(portNr);
    if (bind(_sockFd, (struct sockaddr *) &serv_addr, sizeof(serv_addr)) < 0) {
        std::cout << "ERROR on binding" << std::endl;
        return;
    }

    listen(_sockFd,5);
    std::cout << "Listening on port " << portNr << std::endl;
}

void Server::communicate() {
	char chunk[BUFFER_SIZE];
	struct sockaddr_in cli_addr;
#ifdef _WIN32
	int clilen;
#else
	uint32_t clilen;
#endif
	clilen = sizeof(cli_addr);
	_newSockFd = accept(_sockFd, (struct sockaddr *) &cli_addr, &clilen);
	if (_newSockFd < 0) {
		std::cout << "ERROR on accept" << std::endl;
	}
    while(1) {
		std::string resultJson = "";
		int totalSize = 0;
		int n = 0;
		while (1) {
			memset(chunk, 0, BUFFER_SIZE); // clear buffer
			int n = recv(_newSockFd, chunk, BUFFER_SIZE - 1, 0);
			if (n < 0) {
				break;
			}
			totalSize += n;
			resultJson += chunk;
			// check if end of msg
			if (totalSize < 2 || resultJson.substr(totalSize - 2) == "]}") {
				// std::cout << "recv msg complete" << std::endl;
				break;
			}		   
		}
		if (n < 0 || resultJson == "") {
			std::cout << "The other side closed the socket, exit" << std::endl;
			break;
		}

		int64_t startTs = util::getTimestampInMilliseconds();

        vec v = vec();
		util::stringToVec(resultJson.substr(0, resultJson.size()-1), v);

        QueryVector qv = QueryVector(v, _compressionBlockRows, _compressionLevels);

		int limit = util::getJsonInt(resultJson, "limit");
		_topKQueueNRD.executeTopK(qv, limit);
        std::string resultArray = _topKQueueNRD.toString() + "\n"; // newline at the end because the corresponding side read the socket by ".readLine()"


		n = send(_newSockFd, resultArray.c_str(), resultArray.size(), 0);
		if (n != static_cast<int>(resultArray.size())) {
			std::cout << "ERROR writing to socket" << std::endl;
			break;
		}
		std::cout << (util::getTimestampInMilliseconds() - startTs) << "ms" << std::endl;
    }
}

Server::~Server() {
#ifdef _WIN32
	closesocket(_newSockFd);
	closesocket(_sockFd);
#else
	close(_newSockFd);
	close(_sockFd);
#endif

}
