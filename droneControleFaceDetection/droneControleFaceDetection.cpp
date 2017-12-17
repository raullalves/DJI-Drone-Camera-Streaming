/*
Developed by Raul Lima Alves
LinkedIn: https://www.linkedin.com/in/raul-alves-8b090228/
Github: https://github.com/raullalves
*/

#include "VideoSocket.h"

int main(int argc, char *argv[])
{
	if (argc < 2) {
         std::cout<<"Usage: ./drone androidIp"<<std::endl;
         exit(1);
    }
	std::string ip = argv[1];
	
	std::cout<<"Connecting to ip "<<ip<<std::endl;

	VideoSocket * videoSocket = new VideoSocket(ip);
	videoSocket->getVideo();
    return 0;
}

