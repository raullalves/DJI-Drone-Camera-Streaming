#include "stdafx.h"
#include "ReconhecedorFace.h"

using namespace std;
using namespace cv;


void ReconhecedorFace::salvarImagem(Mat img) {
	contador++;
	string endereco = "faces\\"+to_string(contador)+".jpg";
	cout << endereco;
	imwrite(endereco, img);
}

Rect ReconhecedorFace::detectarFace(Mat img) {
	Mat img_temp = img.clone();
	double w = ((double)img_temp.cols);
	double h = ((double)img_temp.rows);
	vector<Rect> faces;
	face_cascade.detectMultiScale(img_temp, faces, 1.1, 2, 0 | CV_HAAR_SCALE_IMAGE, Size(w / 5, h / 5));
	for (unsigned int i = 0; i < faces.size();i++) {
		cout << "FACE" << endl;
		Rect roi = faces[i];
		salvarImagem(img_temp(roi));
		return roi;
	}
	return Rect(0, 0, img_temp.cols, img_temp.rows);
}