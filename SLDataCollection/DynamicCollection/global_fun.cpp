#include "global_fun.h"

int ErrorHandling(std::string message)
{
	std::cout << "An Error Occurs:" << message << std::endl;
	system("PAUSE");
	return 0;
}
