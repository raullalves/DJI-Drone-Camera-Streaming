#pragma once

#define _WIN32_WINNT 0x0A00

#define _WINSOCK_DEPRECATED_NO_WARNINGS
#define UDP_PACK_SIZE 4096 
#define ENCODE_QUALITY 80
#define TAMANHO_BUFFER 65540
#include <Ws2tcpip.h>
#include <windows.h>
#include <winsock2.h>
#include <process.h>
#include <iostream>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <fstream>
#include <iomanip>
#include <string>
#include <sstream>
#include "opencv/cv.h"
#include "opencv/highgui.h"

#include "opencv2/core.hpp"
#include "opencv2/face.hpp"
#include "opencv2/highgui.hpp"
#include "opencv2/objdetect.hpp"
#include "opencv2/imgproc.hpp"
#include "opencv2/videoio.hpp"
#include "ReconhecedorFace.h"
#include "stdafx.h"
class Decodificador {

	int contadorJanelas = 0;
	std::map<std::string, int> windows;
	int height = 720;
	int width = 1280;
	
public:

	void decodificadorYUVAndroid(unsigned char*, int);
	Decodificador(void){
		//reconhecedorFace = new ReconhecedorFace();
	};
private:
	
	void novaWindow(std::string, cv::Mat);

};