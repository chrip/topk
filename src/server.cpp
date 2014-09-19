/*
 * Server.cpp
 *
 *      example from http://www.linuxhowtos.org/C_C++/socket.htm
 */

#include "server.h"
#include "query_vector.h"
#include "top_k_queue_nrd.h"
#include "util.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <fstream>
#include <iostream>
#include <sstream>

Server::Server(const std::string &pathToVectorSpaceData, int portNr) :
	_sockFd(0),
	_newSockFd(0),
    _topKQueueNRD()
{
    std::ifstream input0(pathToVectorSpaceData);
    int i = 0;
    for( std::string line; getline( input0, line ); i++){
        vec vector = vec();
        util::stringToVec(line,vector);
        _topKQueueNRD.addVector(util::getId(line),vector);
    }
    _topKQueueNRD.buildIndex(_compressionBlockRows, _compressionBlockCols, _compressionLevels);
    std::cout << _topKQueueNRD.getVectorSpaceSize() << " vectors of length " << _topKQueueNRD.getVectorSpace().getVectorSpacePyramide()[0][0].size() << std::endl;

    struct sockaddr_in serv_addr;
    _sockFd = socket(AF_INET, SOCK_STREAM, 0);
    if (_sockFd < 0) {
       std::cout << "ERROR opening socket" << std::endl;
    }
    bzero((char *) &serv_addr, sizeof(serv_addr));
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
    socklen_t clilen;
    char buffer[64];
	struct sockaddr_in cli_addr;
	clilen = sizeof(cli_addr);
    ssize_t n; /* byte count or error */
	_newSockFd = accept(_sockFd,
				(struct sockaddr *) &cli_addr,
				&clilen);
	if (_newSockFd < 0) {
		std::cout << "ERROR on accept" << std::endl;
	}
    while(1) {
    	memset(buffer, 0, 64); // clear buffer
		n = read(_newSockFd,buffer,63);
		if (n < 0) {
			std::cout << "ERROR reading from socket" << std::endl;
			break;
		}

		std::string b(buffer);
        vec v = vec();
        util::stringToVec(b, v);

        QueryVector qv = QueryVector(v, _compressionBlockRows, _compressionLevels);
        _topKQueueNRD.executeTopK(qv, 20);
        std::string result = _topKQueueNRD.toString();

        n = write(_newSockFd, result.c_str(), result.size());
		if (n < 0) {
			std::cout << "ERROR writing to socket" << std::endl;
			break;
		}
    }
}

Server::~Server() {
    close(_newSockFd);
    close(_sockFd);
}
