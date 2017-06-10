#include "stdafx.h"
#include "VideoSocketTCP.h"
using namespace std;

using namespace cv;

VideoSocketTCP::VideoSocketTCP(void) {

	int iResult = WSAStartup(MAKEWORD(2, 2), &wsaVideo);
	if (iResult != 0) {
		cout << "Erro ao criar WSA" << endl;
		return;
	}

	ZeroMemory(&hints, sizeof(hints));
	hints.ai_family = AF_UNSPEC;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_protocol = IPPROTO_TCP;

	char* ip = "192.168.27.41";

	iResult = getaddrinfo(ip, PORTA, &hints, &result);
	if (iResult != 0) {
		cout << "Falha ao se conectar ao servidor" << endl;
		WSACleanup();
		return;
	}


	for (ptr = result; ptr != NULL;ptr = ptr->ai_next) {

		socketVideo = socket(ptr->ai_family, ptr->ai_socktype,
			ptr->ai_protocol);
		if (socketVideo == INVALID_SOCKET) {
			cout << "Falha na conexao com servidor" << endl;
			WSACleanup();
			return;
		}

		iResult = connect(socketVideo, ptr->ai_addr, (int)ptr->ai_addrlen);
		if (iResult == SOCKET_ERROR) {
			closesocket(socketVideo);
			socketVideo = INVALID_SOCKET;
			continue;
		}
		break;
	}

	freeaddrinfo(result);

	if (socketVideo == INVALID_SOCKET) {
		cout << "Nao conseguiu se conectar com o servidor" << endl;
		WSACleanup();
		return;
	}
	else cout << "Conexao estabelecida" << endl;

}

int VideoSocketTCP::enviarResposta(int valor) {

	char msgACK[1024];
	string msg = to_string(valor);
	strcpy(msgACK, msg.c_str());
	cout << "Enviou " << msgACK << endl;
	int iResult = send(socketVideo, msgACK, (int)strlen(msgACK), 0);
	if (iResult == SOCKET_ERROR) {
		cout << "Falha no envio" << endl;
		closesocket(socketVideo);
		WSACleanup();
		return -1;
	}

}

void VideoSocketTCP::receberBytes() {

	unsigned char buffer[TAMANHO_BUFFERTCP];

	Decodificador * decodificador = new Decodificador();

	int tamanhoMensagem;

	while (1) {
		int size;
		do {
			size = recv(socketVideo, (char*)buffer, TAMANHO_BUFFERTCP, 0);
		} while (size > 60);
		int tamanho_pacote = (buffer[0] << 24) | (buffer[1] << 16) | (buffer[2] << 8) | (buffer[3]);
		//int tamanho_pacote = (buffer[3] << 24) | (buffer[2] << 16) | (buffer[1] << 8) | (buffer[0]);
		cout << "Recebeu tamanho " << tamanho_pacote << endl;
		unsigned char * buffer_video = new unsigned char[tamanho_pacote];
		size = recv(socketVideo, (char*)buffer, TAMANHO_BUFFERTCP, 0);
		if (size > 60) {
			memcpy((void*)buffer_video, (void*)buffer, tamanho_pacote);
			decodificador->decodificadorYUVAndroidTCP(buffer_video, tamanho_pacote);
		}

	}
}

VideoSocketTCP::~VideoSocketTCP(void) {

}