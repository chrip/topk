#include "server.h"

int main(int argc, const char * argv[])
{
	if (argc < 3) {
		std::cout << "usage: pathToVectorSpaceData port" << std::endl;
		std::cout << "for example: ./topk_server /data/index.json 8081";
	}
	Server server(argv[1], atoi(argv[2]));
	std::cout << "service startet with index file " << argv[1] << " listening on port " << argv[2] << std::endl;
	server.communicate();

	std::cout << "exit main" << std::endl;
	return 0;

}
