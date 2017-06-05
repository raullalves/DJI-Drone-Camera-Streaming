#include "stdafx.h"
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
typedef struct {
	Mat img;
}t;

void tempTask(void *param)
{
	ReconhecedorFace * reconhecedorFace = new ReconhecedorFace();
	Mat *args = (Mat*)param;
	
	reconhecedorFace->detectarFace(*args);
	//free(args);
}

void Decodificador::decodificadorYUVAndroid(unsigned char* buffer_video, int tamanho_pacote) {

	unsigned char * y = new unsigned char[width * height];
	unsigned char * u = new unsigned char[width * height / 4];
	unsigned char * v = new unsigned char[width * height / 4];
	unsigned char * nu = new unsigned char[width * height / 4];
	unsigned char * nv = new unsigned char[width * height / 4];
	//System.arraycopy(yuvFrame, 0, y, 0, y.length);

	memcpy((void*)y, (void*)buffer_video, width * height);
	for (int i = 0; i < width * height / 4; i++) {
		v[i] = buffer_video[width * height + 2 * i];
		u[i] = buffer_video[width * height + 2 * i + 1];
	}
	//cout << u << endl;
	int uvWidth = width / 2;
	int uvHeight = height / 2;
	for (int j = 0; j < uvWidth / 2; j++) {
		for (int i = 0; i < uvHeight / 2; i++) {
			unsigned char uSample1 = u[i * uvWidth + j];
			unsigned char uSample2 = u[i * uvWidth + j + uvWidth / 2];
			unsigned char vSample1 = v[(i + uvHeight / 2) * uvWidth + j];
			unsigned char vSample2 = v[(i + uvHeight / 2) * uvWidth + j + uvWidth / 2];
			nu[2 * (i * uvWidth + j)] = uSample1;
			nu[2 * (i * uvWidth + j) + 1] = uSample1;
			nu[2 * (i * uvWidth + j) + uvWidth] = uSample2;
			nu[2 * (i * uvWidth + j) + 1 + uvWidth] = uSample2;
			nv[2 * (i * uvWidth + j)] = vSample1;
			nv[2 * (i * uvWidth + j) + 1] = vSample1;
			nv[2 * (i * uvWidth + j) + uvWidth] = vSample2;
			nv[2 * (i * uvWidth + j) + 1 + uvWidth] = vSample2;
		}

	}
	unsigned char * bytes = new unsigned char[UDP_PACK_SIZE*(tamanho_pacote)];
	memcpy((void*)bytes, (void*)y, width * height);

	for (int i = 0; i <width * height / 4; i++) {
		bytes[width * height + (i * 2)] = nv[i];
		bytes[width * height + (i * 2) + 1] = nu[i];
	}

	Mat myuv(height + height / 2, width, CV_8UC1, bytes);

	Mat picBGR(height, width, CV_8UC4);

	cvtColor(myuv, picBGR, CV_YUV2BGRA_NV21);

	_beginthread(tempTask, 0, (void*)&picBGR);

	//Rect roi = reconhecedorFace->detectarFace(picBGR);
	novaWindow("decodificadorYUVAndroid", picBGR);

	free(y);
	free(u);
	free(v);
	free(nv);
	free(nu);
	free(bytes);
	free(buffer_video);
	

}
