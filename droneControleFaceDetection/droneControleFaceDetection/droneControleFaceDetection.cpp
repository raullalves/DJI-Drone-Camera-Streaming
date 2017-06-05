// droneControleFaceDetection.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include "VideoSocket.h"

int main()
{
	VideoSocket * videoSocket = new VideoSocket();
	//videoSocket->receberVideo2();
	videoSocket->receberBytes();
    return 0;
}

