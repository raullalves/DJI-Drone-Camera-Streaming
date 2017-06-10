#pragma once

#define _WIN32_WINNT 0x0A00

#define _WINSOCK_DEPRECATED_NO_WARNINGS
#define UDP_PACK_SIZE 4096 
#define TCP_PACK_SIZE  4096*4
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
	ReconhecedorFace * r;
	
public:

	void decodificadorYUVAndroid(unsigned char*, int);
	void decodificadorYUVAndroidTCP(unsigned char*, int);
	void decodificadorYUVAndroidMetade(unsigned char*, int);
	void decodificadorMat(unsigned char*, int);
	int contador = 1;

	Decodificador(void){
		r = new ReconhecedorFace();
	};
private:
	void salvarMat(cv::Mat);
	void novaWindow(std::string, cv::Mat);

};