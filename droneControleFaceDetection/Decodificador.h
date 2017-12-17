/*
Developed by Raul Lima Alves
LinkedIn: https://www.linkedin.com/in/raul-alves-8b090228/
Github: https://github.com/raullalves
*/
#define UDP_PACK_SIZE 4096 
#define TCP_PACK_SIZE  4096*4
#define ENCODE_QUALITY 80
#define TAMANHO_BUFFER 65540
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

class Decodificador {

	int contadorJanelas = 0;
	std::map<std::string, int> windows;
	int height = 720;
	int width = 1280;
	
public:
	void decodificadorMat(unsigned char*, int);
	int contador = 1;
private:
	void novaWindow(std::string, cv::Mat);
};