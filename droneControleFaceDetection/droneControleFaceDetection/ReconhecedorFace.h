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
#include "stdafx.h"
class ReconhecedorFace {

	std::string lbpFaceDetector_location = "C:\\opencvCpp\\etc\\lbpcascades\\lbpcascade_frontalface.xml";
	cv::CascadeClassifier face_cascade;
	

public:
	int contador = 0;
	ReconhecedorFace() {
		face_cascade.load(lbpFaceDetector_location);
	}
	void detectarFace(cv::Mat);

private:
	double ** getHistograma(cv::Mat);
	double cosineDistance(double*, double*, int);
	cv::Mat LBP(cv::Mat);
	void salvarImagem(cv::Mat);

};