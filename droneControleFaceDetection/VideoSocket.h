/*
Developed by Raul Lima Alves
LinkedIn: https://www.linkedin.com/in/raul-alves-8b090228/
Github: https://github.com/raullalves
*/
#define FRAME_HEIGHT 480
#define FRAME_WIDTH 640
#define FRAME_INTERVAL (1000/30)
#define UDP_PACK_SIZE 4096 
#define ENCODE_QUALITY 80
#define TAMANHO_BUFFER 65540
#define QTD_MAXIMA 1

#include <iostream>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <fstream>
#include <iomanip>
#include <string>
#include <sstream>
#include <sys/types.h> 
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include "opencv/cv.h"
#include "opencv/highgui.h"
#include "opencv2/core.hpp"
#include "opencv2/face.hpp"
#include "opencv2/highgui.hpp"
#include "opencv2/objdetect.hpp"
#include "opencv2/imgproc.hpp"
#include "opencv2/videoio.hpp"
#include "Decodificador.h"

class VideoSocket {
	int socketVideo;
	struct sockaddr_in Receber_addr_video;
	socklen_t len = sizeof(struct sockaddr_in);
	int PORT = 11006;
	std::string mensagemACK = "Recebido";
	std::string mensagemP2p = "EnviarVideo";
	char** ipConectado;
	char msgAdmissao[16];
	int contador = 0;
public:
	void error(const char *);
	VideoSocket(std::string ip) {
		ipConectado = new char*[QTD_MAXIMA];
		socketVideo = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
		if (socketVideo < 0) error("ERROR opening socket");
		bzero((char *) &Receber_addr_video, sizeof(Receber_addr_video));
		Receber_addr_video.sin_family = AF_INET;
     	Receber_addr_video.sin_addr.s_addr = inet_addr(ip.c_str());
     	Receber_addr_video.sin_port = htons(PORT);
		if (enviarACK() < 0) {
			std::cout << "Erro ao enviar ACK" << std::endl;
			return;
		}
	}
	void getVideo(void);
private:
	int enviarResposta(int);
	int enviarACK(void);
};
