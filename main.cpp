#include "Engine/_Core/window.h"

int main(int argc, char *argv[])
{
    std::cout<<"Hello World"<<std::endl; 

	{
 	   Window m_window = Window();
		m_window.run();
	}

	return 0;
}