// droneControleFaceDetection.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include "VideoSocket.h"
#include "VideoSocketTCP.h"

int main()
{
	VideoSocket * videoSocket = new VideoSocket();
	videoSocket->receberVideo2();
	//videoSocket->receberBytes();
	
	/*VideoSocketTCP * videoSocketTCP = new VideoSocketTCP();
	videoSocketTCP->receberBytes();*/
    return 0;
}

