#include "stdafx.h"
#include "VideoSocket.h"

using namespace std;

using namespace cv;
//construtor
VideoSocket::VideoSocket(void) {

	//char pointer para armazenar ip conectado
	ipConectado = new char*[QTD_MAXIMA];

	//WSAStartup
	WSAStartup(MAKEWORD(2, 2), &wsaVideo);

	//criação do socket
	socketVideo = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);

	Receber_addr_video.sin_family = AF_INET;
	Receber_addr_video.sin_port = htons(PORT);
	Receber_addr_video.sin_addr.s_addr = INADDR_ANY;

	char broadcast = 'a';

	if (setsockopt(socketVideo, SOL_SOCKET, SO_BROADCAST, &broadcast, sizeof(broadcast)) < 0)
	{
		cout << "Erro 1" << endl;
		closesocket(socketVideo);

	}


	if (bind(socketVideo, (sockaddr*)&Receber_addr_video, sizeof(Receber_addr_video)) < 0)
	{
		cout << "Erro 2" << endl;
		closesocket(socketVideo);
		return;
	}

	//informações do endereço a ser conectado
	Receber_addr_video.sin_family = AF_INET;
	Receber_addr_video.sin_port = htons(PORT);

	//obter ip que conectou
	char* ip = "192.168.25.106";
	Receber_addr_video.sin_addr.s_addr = inet_addr(ip);

	//envia ack comunicando sucesso na conexao
	if (enviarACK() < 0) {

		cout << "Erro ao enviar ACK" << endl;
		return;

	}


}

//destrutor
VideoSocket::~VideoSocket(void) {

}

void VideoSocket::receberBytes() {
	unsigned char buffer[TAMANHO_BUFFER];
	
	Decodificador * decodificador = new Decodificador();
	
	int tamanhoMensagem;
	
	while (1) {

		do {

			tamanhoMensagem = recvfrom(socketVideo, (char*)buffer, TAMANHO_BUFFER, 0, (sockaddr *)&Receber_addr_video, &len);

		} while (tamanhoMensagem > sizeof(int));
		//int x = *(int *)buffer;
		int tamanho_pacote = (buffer[0] << 24) | (buffer[1] << 16) | (buffer[2] << 8) | (buffer[3]);
		enviarResposta(tamanho_pacote);
		unsigned char * buffer_video = new unsigned char[UDP_PACK_SIZE*(tamanho_pacote)];
		int flag = 1;
		//cout << "Tamanho recebido = " << tamanho_pacote << endl;
		for (int i = 0; i < tamanho_pacote && flag; i++) {

			tamanhoMensagem = recvfrom(socketVideo, (char*)buffer, TAMANHO_BUFFER, 0, (sockaddr *)&Receber_addr_video, &len);
			// enviarResposta(i + 1);
			if (tamanhoMensagem != UDP_PACK_SIZE) {
				int tamanho_pacote2 = (buffer[0] << 24) | (buffer[1] << 16) | (buffer[2] << 8) | (buffer[3]);
				cout << "perdeu pacotes " << i <<"de "<< tamanho_pacote2 << endl;
				flag = 0;
				break;
				//continue;
			}

			memcpy(&buffer_video[UDP_PACK_SIZE*i], (void*)buffer, UDP_PACK_SIZE);
			//enviarResposta(i+1);
		}
		
		if (!flag) continue;
		decodificador->decodificadorYUVAndroid(buffer_video, tamanho_pacote);
		
	}
}

int VideoSocket::enviarResposta(int valor) {
	char msgACK[1024];
	string msg = to_string(valor);
	strcpy(msgACK, msg.c_str());
	if (sendto(socketVideo, msgACK, strlen(msgACK), 0, (sockaddr *)&Receber_addr_video, sizeof(Receber_addr_video)) < 0) {

		closesocket(socketVideo);
		return -1;

	}
	//cout << "enviou " << valor << endl;
	return 1;
}

//envia ACK de resposta
int VideoSocket::enviarACK(void) {

	char msgACK[16];

	strcpy(msgACK, mensagemACK.c_str());

	if (sendto(socketVideo, msgACK, strlen(msgACK) + 1, 0, (sockaddr *)&Receber_addr_video, sizeof(Receber_addr_video)) < 0) {

		closesocket(socketVideo);
		return -1;

	}

	cout << "ACK Enviado" << endl;

	return 0;
}

void VideoSocket::receberVideo2(void) {

	//cria window
	namedWindow("Imagem do Drone - IVISION!", CV_WINDOW_AUTOSIZE);
	cout << "Janela criada";
	//buffer
	char buffer[TAMANHO_BUFFER];
	int tamanhoMensagem;


	while (1) {
		
		//recebe inicialmente o tamanho do frame
		do {
			
			tamanhoMensagem = recvfrom(socketVideo, buffer, TAMANHO_BUFFER, 0, (sockaddr *)&Receber_addr_video, &len);
			
		} while (tamanhoMensagem > sizeof(int));
		
		cout << "Recebeu " << ++contador << endl;
		//int tamanho_pacote = ((int *)buffer)[0];
		int tamanho_pacote = buffer[tamanhoMensagem - 1];
		//cout << "Recebeu o tamanho de " << tamanho_pacote << endl;
		char * buffer_video = new char[UDP_PACK_SIZE*tamanho_pacote];
		int flag = 1;
		//recebe os dados
		for (int i = 0; i < tamanho_pacote && flag; i++) {
			//cout << "Recebendo packet " << i << "de " << tamanho_pacote << endl;
			tamanhoMensagem = recvfrom(socketVideo, buffer, TAMANHO_BUFFER, 0, (sockaddr *)&Receber_addr_video, &len);
			
			if (tamanhoMensagem != UDP_PACK_SIZE) {
				//cout << "Recebeu pacote menor do que o tamanho UDP"<<endl <<"tamanho = "<<tamanhoMensagem<<endl;
				//flag = 0;
				continue;
			}
			memcpy(&buffer_video[UDP_PACK_SIZE*i], buffer, UDP_PACK_SIZE);
		}
		//if (!flag) continue;

		//cout << "Criando o MAT com a imagem recebida" << endl;
		//cria MAT para armazenar
		Mat matVideo = Mat(1, UDP_PACK_SIZE*tamanho_pacote, CV_8UC1, buffer_video);
		if (matVideo.empty()) {
			cout << "Mat Vazio"<<endl;
			continue;
		}
		Mat frame_video = imdecode(matVideo, CV_LOAD_IMAGE_COLOR);

		//caso tenha perda de dados
		if (frame_video.empty()) {
			//cout << "Frame vazio!" << endl;
			continue;
		}
		//cout << "Frame exibido!" << endl;
		//mostra
		imshow("Imagem do Drone - IVISION!", frame_video);

		waitKey(1);

		free(buffer_video);

	}

}

//add ip conectado
void VideoSocket::addIP(char * ip) {

	ipConectado[0] = ip;

}

void VideoSocket::receberVideo(void) {
	cout << "Chamando o objeto de receber video atraves do decodificador " << endl;

}

//get ip conectado
char* VideoSocket::getIPConectado(void) {

	return ipConectado[0];

}

//procura conexao
int VideoSocket::buscarConexao(void) {

	//buffer
	char recvBuff[50];
	int recvBuffLen = 50;
	char from_ip[1024];

	//struct temporária para obter informações de conexao
	struct sockaddr_in Recv_addr;
	cout << "Aguardando uma nova conexao..." << endl;
	//aguarda receber string
	recvfrom(socketVideo, recvBuff, recvBuffLen, 0, (struct sockaddr*)&Recv_addr, &len);
	cout << "Recebeu alguma coisa" << endl;

	//obtem dados da conexão
	getnameinfo((struct sockaddr*)&Recv_addr, sizeof(Recv_addr), from_ip, 1024, NULL, NULL, 0);
	struct hostent *host = gethostbyname(from_ip);
	struct in_addr addr;

	if (host->h_addr_list[0] == 0) {
		cout << "Host vazio" << endl;
		return -1;
	}

	//cópia das informações
	memcpy(&addr, host->h_addr_list[0], sizeof(struct in_addr));
	struct sockaddr_in ipt;
	ipt.sin_family = AF_INET;
	ipt.sin_port = htons(PORT);
	ipt.sin_addr.s_addr = inet_addr(inet_ntoa(addr));

	//obtem o ip
	char *connected_ip = inet_ntoa(ipt.sin_addr);
	string con_ip(connected_ip);

	cout << "Recebeu string " << recvBuff << endl;
	cout << "Esperando string " << msgAdmissao << endl;
	if (strcmp(recvBuff, msgAdmissao) == 0) {

		char* temp_ip = new char[con_ip.length() + 1];
		strcpy(temp_ip, con_ip.c_str());
		addIP(temp_ip);
		cout << temp_ip << endl;
		return 0;

	}

	return -1;

}