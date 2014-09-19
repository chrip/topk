#include "server.h"

int main(int argc, const char * argv[])
{

Server server(argv[1], 8081);
server.communicate();

std::cout << "exit main" << std::endl;
return 0;

}
