#include "server.h"

int main(int argc, const char * argv[])
{
	if (argc < 3) {
		std::cout << "usage: pathToVectorSpaceData port" << std::endl;
		std::cout << "for example: ./topk_server /data/index.json 8081";
	}
	Server server(argv[1], static_cast<uint16_t>(atoi(argv[2])));
	server.communicate();

	return 0;

}
