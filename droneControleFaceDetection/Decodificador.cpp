/*
Developed by Raul Lima Alves
LinkedIn: https://www.linkedin.com/in/raul-alves-8b090228/
Github: https://github.com/raullalves
*/

#include "Decodificador.h"
using namespace std;
using namespace cv;

void Decodificador::novaWindow(string nome, Mat imagem) {

	map<string, int>::iterator it;
	it = windows.find(nome);
	if (it == windows.end())
	{
		namedWindow(nome, CV_WINDOW_AUTOSIZE);
		windows[nome] = contadorJanelas++;
	}
	if (!imagem.empty()) {
		imshow(nome, imagem);
		waitKey(1);
	}
}

void Decodificador::decodificadorMat(unsigned char* buffer_video, int tamanho_pacote) {
	Mat matVideo = Mat(1, UDP_PACK_SIZE*tamanho_pacote, CV_8UC1, buffer_video);
	if (matVideo.empty()) {
		cout << "Mat Vazio" << endl;
		return;
	}
	Mat frame_video = imdecode(matVideo, CV_LOAD_IMAGE_COLOR);
	if (frame_video.empty()) {
		cout << "Frame vazio!" << endl;
		return;
	}
	novaWindow("Decodificador MAT", frame_video);
	free(buffer_video);
}