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

void ReconhecedorFace::detectarFace(Mat img) {
	Mat temp = img.clone();
	Mat gray;
	cvtColor(temp, gray, CV_RGB2GRAY);
	Mat lbp = LBP(gray);
	double** his = getHistograma(lbp);
	//for (int i = 1; i < 256; i++) cout << his[0][i] << endl;
	Mat hist_mat = Mat(Size(1,256),CV_64F,his[0]);
	//cout << hist_mat << endl;
	delete[] his[0];
	delete[] his;
	//cout << his[0] << endl;
	
	
	/*Mat img_temp = img.clone();
	double w = ((double)img_temp.cols);
	double h = ((double)img_temp.rows);
	vector<Rect> faces;
	face_cascade.detectMultiScale(img_temp, faces, 1.1, 2, 0 | CV_HAAR_SCALE_IMAGE, Size(w / 5, h / 5));
	for (unsigned int i = 0; i < faces.size();i++) {
		cout << "FACE" << endl;
		Rect roi = faces[i];
		salvarImagem(img_temp(roi));
		//return roi;
	}*/
	//return Rect(0, 0, img_temp.cols, img_temp.rows);
}

Mat ReconhecedorFace::LBP(Mat img) {
	
	Mat dst = Mat::zeros(img.rows - 2, img.cols - 2, CV_8UC1);
	for (int i = 1;i<img.rows - 1;i++) {
		for (int j = 1;j<img.cols - 1;j++) {
			uchar center = img.at<uchar>(i, j);
			unsigned char code = 0;
			unsigned char code_menor = 300;
			for (int k = 0; k < 8; k++) {

				if (k + 7 > 7)
					code |= ((img.at<uchar>(i - 1, j - 1)) > center) << k - 1;
				else
					code |= ((img.at<uchar>(i - 1, j - 1)) > center) << k + 7;

				if (k + 6 > 7)
					code |= ((img.at<uchar>(i - 1, j)) > center) << k - 2;
				else
					code |= ((img.at<uchar>(i - 1, j)) > center) << k + 6;

				if (k + 5 > 7)
					code |= ((img.at<uchar>(i - 1, j + 1)) > center) << k - 3;
				else
					code |= ((img.at<uchar>(i - 1, j + 1)) > center) << k + 5;

				if (k + 4 > 7)
					code |= ((img.at<uchar>(i, j + 1)) > center) << k - 4;
				else
					code |= ((img.at<uchar>(i, j + 1)) > center) << k + 4;

				if (k + 3 > 7)
					code |= ((img.at<uchar>(i + 1, j + 1)) > center) << k - 5;
				else
					code |= ((img.at<uchar>(i + 1, j + 1)) > center) << k + 3;

				if (k + 2 > 7)
					code |= ((img.at<uchar>(i + 1, j)) > center) << k - 6;
				else
					code |= ((img.at<uchar>(i + 1, j)) > center) << k + 2;

				if (k + 1 > 7)
					code |= ((img.at<uchar>(i + 1, j - 1)) > center) << k - 7;
				else
					code |= ((img.at<uchar>(i + 1, j - 1)) > center) << k + 1;

				code |= ((img.at<uchar>(i, j - 1)) > center) << k;

				if (code < code_menor)
					code_menor = code;
			}
			dst.at<uchar>(i - 1, j - 1) = code_menor;
		}
	}
	return dst;
}

double** ReconhecedorFace::getHistograma(Mat img) {
	double** hist = 0;
	hist = new double*[1];
	for (int h = 0; h < 1; h++)
	{
		hist[h] = new double[256];
		for (int w = 0; w < 256; w++)
		{
			hist[h][w] = 0;
		}
	}
	Mat img_temp = img.clone();
	for (int i = 0; i < img_temp.rows; i++) {

		for (int j = 0; j < img_temp.cols; j++)
		{
			try {
				hist[0][static_cast<int>(img_temp.at<uchar>(i, j))] = hist[0][static_cast<int>(img_temp.at<uchar>(i, j))] + 1; //canal único
			}
			catch (Exception e) {
				std::cout << "\n erro na coluna " << j << " de um total de " << img_temp.cols << " na linha " << i << " de um total de " << img_temp.rows << " com valor de" << static_cast<int>(img_temp.at<uchar>(j, i));
			}
		}
	}
	for (int h = 0; h < 1; h++)
		for (int w = 0; w < 256; w++) {
			hist[h][w] = hist[h][w] / (img_temp.rows*img_temp.cols);
			//cout << hist[h][w] << endl;
			if (!isfinite(hist[h][w])) hist[h][w] = 0;
		}
	return hist;
}

double ReconhecedorFace::cosineDistance(double *A, double *B, int size_array) {
	double a_a = 0.0, b_b = 0.0, soma = 0.0;
	for (int i = 0; i < size_array; i++) {
		a_a += A[i] * A[i];
		b_b += B[i] * B[i];
	}
	for (int i = 0; i < size_array; i++) {
		A[i] /= sqrt(a_a);
		B[i] /= sqrt(b_b);
		soma += A[i] * B[i];
	}
	return 1 - soma;
}
