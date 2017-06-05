#pragma once

#define _WIN32_WINNT 0x0A00

#define _WINSOCK_DEPRECATED_NO_WARNINGS

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
#include "stdafx.h"
/*
#define FRAME_HEIGHT 720
#define FRAME_WIDTH 1280*/
#define FRAME_HEIGHT 480
#define FRAME_WIDTH 640
/*
#define FRAME_HEIGHT 2160
#define FRAME_WIDTH 3840*/

#define FRAME_INTERVAL (1000/30)
#define UDP_PACK_SIZE 4096 
#define ENCODE_QUALITY 80
#define TAMANHO_BUFFER 65540
#define QTD_MAXIMA 1
#pragma comment (lib, "Ws2_32.lib")
/*
extern "C" {
#include "libavutil/imgutils.h"
#include <libavcodec/avcodec.h>
#include <libswscale/swscale.h>
}*/
using namespace std;

using namespace cv;
#include "Decodificador.h"
class VideoSocket {

	WSADATA wsaVideo;
	SOCKET socketVideo;
	sockaddr_in Receber_addr_video;
	int len = sizeof(struct sockaddr_in);
	int PORT = 11006;
	string mensagemACK = "Recebido";
	
	string mensagemP2p = "EnviarVideo";
	
	char** ipConectado;
	char msgAdmissao[16];
	int contador = 0;
	

public:

	VideoSocket(void);
	~VideoSocket(void);

	int buscarConexao(void);

	void receberVideo(void);
	void receberVideo2(void);
	void receberBytes(void);

private:
	int enviarResposta(int);
	void addIP(char * ip);

	char* getIPConectado(void);

	int enviarACK(void);

	


};
