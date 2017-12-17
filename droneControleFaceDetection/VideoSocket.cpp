/*
Developed by Raul Lima Alves
LinkedIn: https://www.linkedin.com/in/raul-alves-8b090228/
Github: https://github.com/raullalves
*/

#include "VideoSocket.h"
using namespace std;
using namespace cv;

void VideoSocket::error(const char *msg)
{
    perror(msg);
    exit(1);
}

int VideoSocket::enviarResposta(int valor) {
	char msgACK[1024];
	string msg = to_string(valor);
	strcpy(msgACK, msg.c_str());
	if (sendto(socketVideo, msgACK, strlen(msgACK), 0, (sockaddr *)&Receber_addr_video, sizeof(Receber_addr_video)) < 0) {
		error("ERROR sending answer to android");
	}
	return 1;
}

int VideoSocket::enviarACK(void) {
	char msgACK[16];
	strcpy(msgACK, mensagemACK.c_str());
	if (sendto(socketVideo, msgACK, strlen(msgACK) + 1, 0, (sockaddr *)&Receber_addr_video, sizeof(Receber_addr_video)) < 0) {
		error("ERROR sending ACK");
	}
	return 0;
}

void VideoSocket::getVideo(void) {

	Decodificador * decodificador = new Decodificador();
	unsigned char buffer[TAMANHO_BUFFER];
	int tamanhoMensagem;
	while (1) {
		do {
			cout << "Waiting for frame..." << endl;
			tamanhoMensagem = recvfrom(socketVideo, (char*)buffer, TAMANHO_BUFFER, 0, (sockaddr *)&Receber_addr_video, &len);
		} while (tamanhoMensagem > sizeof(int));
		int tamanho_pacote = (buffer[0] << 24) | (buffer[1] << 16) | (buffer[2] << 8) | (buffer[3]);
		cout << tamanho_pacote << endl;
		enviarResposta(tamanho_pacote);
		unsigned char * buffer_video = new unsigned char[UDP_PACK_SIZE*tamanho_pacote];
		int flag = 1;
		for (int i = 0; i < tamanho_pacote && flag; i++) {
			tamanhoMensagem = recvfrom(socketVideo, (char*)buffer, TAMANHO_BUFFER, 0, (sockaddr *)&Receber_addr_video, &len);
			if (tamanhoMensagem != UDP_PACK_SIZE) {
				flag = 0;
				break;
			}
			memcpy(&buffer_video[UDP_PACK_SIZE*i], buffer, UDP_PACK_SIZE);
		}
		decodificador->decodificadorMat(buffer_video, tamanho_pacote);
	}
}