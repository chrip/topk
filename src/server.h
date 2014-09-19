#ifndef SERVER_H
#define SERVER_H

#include "top_k_queue_nrd.h"

class Server {
public:
    Server(const std::string& pathToVectorSpaceData, int portNr);
	virtual ~Server();
	void communicate();
private:
    int _sockFd;
    int _newSockFd;
    TopKQueueNRD _topKQueueNRD;
    size_t _compressionBlockRows = 3;
    size_t _compressionBlockCols = 1;
    size_t _compressionLevels = 2;
};

#endif // SERVER_H
