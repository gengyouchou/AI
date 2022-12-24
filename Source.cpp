#include <iostream>
#include <opencv2/opencv.hpp>
#include <opencv/highgui.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <math.h>
#include <fstream>
#include <thread>
#include <algorithm>
#include <iostream>
#include <string>
#include <ctime>
#include <sapi.h>
//#include "caffe/caffe.hpp"
#include <Python.h>
#include <numpy/ndarrayobject.h>
#define NPY_NO_DEPRECATED_API NPY_1_7_API_VERSION // remove warnings
const int D =2;
const int R_H_1 = 0;
const int R_H_2 = 10;
const int R_H_3 = 200;
const int R_H_4 = 255;
const int R_L_1 = 30;
const int R_L_2 = 204;
const int R_S_1 = 26;
const int R_S_2 = 255;
const int B_H_1 = 136;
const int B_H_2 = 198;
const int B_L_1 = 56;
const int B_L_2 = 127;
const int B_S_1 = 71;
const int B_S_2 = 255;
void Resize(cv::Mat &img) {
	cv::resize(img, img, cv::Size(960, 540), 0, 0, CV_INTER_AREA);
}
void put_text(cv::Mat &, char, int);
cv::Mat fetchPixel_RedBlue(cv::Mat &, cv::Mat &);
int filter(std::vector<std::vector<cv::Point>> &, cv::Mat &, cv::Mat &, cv::Mat &);
double filter2(cv::Mat &, cv::Mat &, int &, int &, int &, int &);
int dtb(cv::Mat &, int, int, int, int, int);
cv::Mat warpAffine(cv::Mat &);
void voice(int);
void onMouse(int event, int x, int y, int flags, void*param);

int ttmp = 900;
int main()
{

	cv::VideoWriter writer;
	writer.open("VideoTest.avi", CV_FOURCC('D', 'I', 'V', 'X'), 30, cv::Size(1280,720));





	int fk = ttmp;

	// initialize python
	Py_InitializeEx(1);
	cv::namedWindow("Binarization_image", 0);
	cv::namedWindow("Original_image", 0);
	/*---------輸入行車紀錄器影片--------*/
	cv::VideoCapture video("C:\\Users\\61412\\Desktop\\a1.mp4");
	cv::Mat Original_image;
	video >> Original_image;
	int frame = 0;
	int tmp = 0;
	long nFrame = static_cast<long>(video.get(CV_CAP_PROP_FRAME_COUNT));
	cv::createTrackbar("pos", "Original_image", &frame, nFrame);

	while (true)
	{
		video >> Original_image;
		//Resize(Original_image);
		if (frame != tmp)
		{
			tmp = frame;
			video.set(CV_CAP_PROP_POS_FRAMES, frame);
		}
		/*-----------------------------------*/

		/*--------由Web Cam作為影像來源-------*/
	//cv::VideoCapture video(0);
	//if (!video.isOpened())
	//{
	//	return -1;
	//}
	//cv::Mat Original_image;
	//video >> Original_image;
	//while (true) {
	//	video >> Original_image;
		/*---------------------------------------*/

		/*----------輸入多張圖片-------------*/
//int tmp = 0;
//while (1)
//{

//	tmp++;
//	char str[50] = "C:\\Users\\61412\\Desktop\\1\\";
//	char str2[50] = { 0 };
//	sprintf(str2, "%d", tmp);
//	std::strcat(str, str2);
//	std::strcat(str, ".jpg");
//	cv::Mat Original_image = cv::imread(str);
	/*-----------------------------------*/

//cv::Mat Original_image = cv::imread("C:\\Users\\61412\\Desktop\\abcdd2.jpg");
		cv::medianBlur(Original_image, Original_image, 3);
		cv::Mat Hsv_image;
		cv::cvtColor(Original_image, Hsv_image, CV_BGR2HLS_FULL);
		cv::Mat Binarization_image = fetchPixel_RedBlue(Original_image, Hsv_image);	//Hsv_image->原圖之紅藍點(二值化)
		std::vector<std::vector<cv::Point>> contours;
		std::vector<cv::Vec4i> hierarchy;
		cv::findContours(Binarization_image, contours, hierarchy, CV_RETR_EXTERNAL, CV_CHAIN_APPROX_NONE);
		cv::drawContours(Binarization_image, contours, -1, cv::Scalar(255), 1);
		filter(contours, Binarization_image, Original_image, Hsv_image);
		cv::imshow("Binarization_image", Binarization_image);
		cv::imshow("Original_image", Original_image);
		//writer.write(Original_image);
		writer << Original_image;
		//cv::waitKey(10);
		if (cv::waitKey(10) == 27) {
			writer.release();
			return 0;
		}
		/*if (ttmp != 900 && (ttmp % 100) == 0)
		{
			cv::waitKey(0);
		}*/
	}
	return 0;
}
cv::Mat fetchPixel_RedBlue(cv::Mat &Original_image, cv::Mat &Hsv_image)
{
	//假設isContinuous
	int cols = Original_image.cols*Original_image.rows;
	//返回第一列的指標
	uchar *Original_image_data = Original_image.ptr<uchar>(0);
	uchar *Hsv_image_data = Hsv_image.ptr<uchar>(0);
	//紅、藍->=二值化圖
	cv::Mat Binarization_image(Original_image.rows, Original_image.cols, CV_8UC1);
	uchar *Binarization_image_data = Binarization_image.ptr<uchar>(0);
	int j = 0;
	for (int i = 0; i < cols * 3; i = i + 3, j++)
	{
		//避免飽和度亮度過大過小
		if (((abs(*(Original_image_data + i) - *(Original_image_data + i + 1)) + abs(*(Original_image_data + i + 1) - *(Original_image_data + i + 2)) + abs(*(Original_image_data + i) - *(Original_image_data + i + 2))) / (3 * D)) > 1)
		{

			//判斷是否符合L、S範圍
			/*if (*(Original_image_data + i + 2) > (*(Original_image_data + i + 1) + 5) && *(Original_image_data + i + 2) > (*(Original_image_data + i) + 5))
			{*/
			/*if ((*(Hsv_image_data + i + 1) > R_L_1 && *(Hsv_image_data + i + 1) < R_L_2) && (*(Hsv_image_data + i + 2) > R_S_1 && *(Hsv_image_data + i + 2) < R_S_2))
			{*/
				//若符合H之條件
				if ((*(Hsv_image_data + i) >= R_H_1 && *(Hsv_image_data + i) <= R_H_2) || (*(Hsv_image_data + i) >= R_H_3 &&*(Hsv_image_data + i) <= R_H_4))
				{
					*(Binarization_image_data + j) = 255;
				}
				else
				{
					*(Binarization_image_data + j) = 0;
				}
			}
			/*else
			{
				*(Binarization_image_data + j) = 0;
			}*/
			//}
			//else
			//{
			//	*(Binarization_image_data + j) = 0;
			//}
		//}
		else
		{
			*(Binarization_image_data + j) = 0;
		}
	}
	return Binarization_image;
}
int filter(std::vector<std::vector<cv::Point>> &contours, cv::Mat &Binarization_image, cv::Mat &Original_image, cv::Mat &Hsv_image)
{
	// import module
	static PyObject* caffe_moudle = PyImport_ImportModule("caffe_module");
	static PyObject* predict_signs = PyObject_GetAttrString(caffe_moudle, "predict_signs");
	static PyObject* ro;
	int result = 0;
	// initialize numpy array library
	import_array1(-1); // returns -1 on failure
					   // create a new numpy array
					   // array dimensions
	static npy_intp dim[] = { 80,80,3 };
	int i = 0;
	int j = 0;
	int maxX = 0;
	int minX = 0;
	int maxY = 0;
	int minY = 0;
	static int tmp = 0;
	int *tmp2 = NULL;
	auto itc = contours.begin();

	while (itc != contours.end())
	{
		if ((itc->size()) < 100 || (itc->size()) > 2000)
		{
			itc = contours.erase(itc);
		}
		else
		{
			itc++;
		}
	}
	for (i = 0; i < contours.size(); i++) {
		itc = contours.begin();
		maxX = contours[i][0].x;
		minX = contours[i][0].x;
		maxY = contours[i][0].y;
		minY = contours[i][0].y;

		for (j = 0; j < contours[i].size(); j++) {
			if (contours[i][j].x > maxX) {
				maxX = contours[i][j].x;
			}
			if (contours[i][j].x < minX) {
				minX = contours[i][j].x;
			}
			if (contours[i][j].y > maxY) {
				maxY = contours[i][j].y;
			}
			if (contours[i][j].y < minY) {
				minY = contours[i][j].y;
			}
		}
		int dX = maxX - minX + 1;//寬
		int dY = maxY - minY + 1;//高
		double ddx = dX;
		double ddy = dY;
		int x = 0;
		int y = 0;

		cv::Mat roi_image(dY, dX, CV_8UC1);
		cv::Mat roi_image2(dY, dX, CV_8UC3);
		//長或寬不大於0.3倍原圖
		if ((dY > 0.8*Binarization_image.rows) || (dX > 0.8*Binarization_image.cols))
		{
			itc = contours.erase(itc);
			i--;
		}
		//長寬比
		else if ((ddx / ddy) > 1.3 || (ddx / ddy) < 0.7)
		{
			itc = contours.erase(itc);
			i--;
		}
		else if ((filter2(Original_image, Hsv_image, maxX, maxY, minX, minY)) > 65)
		{
			itc = contours.erase(itc);
			i--;
		}
		//圓形
		else if (dtb(Binarization_image, maxX, maxY, minX, minY, tmp) == 1)
		{


			//取出條件限制後的物件
			for (x = 0; x < dX; x++) {
				for (y = 0; y < dY; y++) {
					roi_image.at<uchar>(y, x) = Binarization_image.at<uchar>(minY + y, minX + x);
					roi_image2.at<cv::Vec3b>(y, x) = Original_image.at<cv::Vec3b>(minY + y, minX + x);
				}
			}
		/*	char str[5];
			sprintf(str, "%d", tmp);
			cv::namedWindow(str, 0);
			cv::imshow(str, roi_image2);
			tmp++;*/

			cv::resize(roi_image2, roi_image2, cv::Size(80, 80), 0, 0, CV_INTER_AREA);
			/*cv::namedWindow("Binarization", 0);
			cv::imshow("Binarization", roi_image2);*/
			cv::waitKey(1);
			uchar *p_data = roi_image2.ptr<uchar>(0);
			PyObject* array_2d = PyArray_SimpleNewFromData(3, dim, NPY_UBYTE, p_data);
			ro = PyObject_CallFunction(predict_signs, "O", array_2d);
			result = PyInt_AsLong(ro);

			/*static char c1[] = "C:\\Users\\61412\\Desktop\\PIC3\\";
			static char c2[10] = { 0 };
			sprintf(c2, "%d", ttmp);
			std::strcat(c1, c2);
			std::strcat(c1, ".jpg");
			cv::imwrite(c1, roi_image2);
			std::strcpy(c1, "C:\\Users\\61412\\Desktop\\PIC3\\");
			ttmp++;*/

			cv::rectangle(Original_image, cv::Point(minX, minY), cv::Point(maxX, maxY), cvScalar(0, 255, 0), 1);
			put_text(Original_image, 'c', result);
		}
		//三角形
		else if (1)
		{
			//取出條件限制後的物件
			for (x = 0; x < dX; x++)
			{
				for (y = 0; y < dY; y++)
				{
					roi_image.at<uchar>(y, x) = Binarization_image.at<uchar>(minY + y, minX + x);
					roi_image2.at<cv::Vec3b>(y, x) = Original_image.at<cv::Vec3b>(minY + y, minX + x);
				}
			}
			/*	cv::namedWindow("roi_image", 0);
				cv::imshow("roi_image", roi_image);*/
			roi_image = warpAffine(roi_image);
			//cv::namedWindow("roi_image_warpAffine", 0);
			//cv::imshow("roi_image_warpAffine", roi_image);
			if (dtb(roi_image, maxX - minX, maxY - minY, 0, 0, tmp) == 2)
			{
				//char str[5];
				//sprintf(str, "%d", tmp);
				//cv::namedWindow(str, 0);
				//cv::imshow(str, roi_image);
				//tmp++;
				cv::resize(roi_image2, roi_image2, cv::Size(80, 80), 0, 0, CV_INTER_AREA);
				uchar *p_data = roi_image2.ptr<uchar>(0);
				PyObject* array_2d = PyArray_SimpleNewFromData(3, dim, NPY_UBYTE, p_data);
				ro = PyObject_CallFunction(predict_signs, "O", array_2d);
				result = PyInt_AsLong(ro);
				//std::cout << result;

				/*static char c1[] = "C:\\Users\\61412\\Desktop\\PIC3\\";
				static char c2[10] = { 0 };
				sprintf(c2, "%d", ttmp);
				std::strcat(c1, c2);
				std::strcat(c1, ".jpg");
				cv::imwrite(c1, roi_image2);
				std::strcpy(c1, "C:\\Users\\61412\\Desktop\\PIC3\\");
				ttmp++;*/

				cv::rectangle(Original_image, cv::Point(minX, minY), cv::Point(maxX, maxY), cvScalar(0, 255, 0), 1);
				put_text(Original_image, 't', result);
			}
		}
		//經過dtb與圓形、三角形、方形差距過大
		else if (dtb(Binarization_image, maxX, maxY, minX, minY, tmp) == 0)
		{
			itc = contours.erase(itc);
			i--;
		}
	}


}
void put_text(cv::Mat &Original_image, char s, int signs)
{

	switch (s)
	{
	case 'c':
		cv::putText(Original_image, "circle", cv::Point(20, 40), cv::FONT_HERSHEY_DUPLEX, 1, cv::Scalar(255, 255, 0));
		break;
	case 't':
		cv::putText(Original_image, "triangle", cv::Point(20, 80), cv::FONT_HERSHEY_DUPLEX, 1, cv::Scalar(255, 255, 0));
		break;
	}
	switch (signs)
	{
	case 0:
		cv::putText(Original_image, "NO LEFT TURN", cv::Point(340, 40), cv::FONT_HERSHEY_DUPLEX, 1, cv::Scalar(255, 255, 0));
		break;
	case 1:
		cv::putText(Original_image, "SPEED LIMIT 60", cv::Point(340, 40), cv::FONT_HERSHEY_DUPLEX, 1, cv::Scalar(255, 255, 0));
		break;
	case 2:
		cv::putText(Original_image, "SLIPPERY", cv::Point(340, 40), cv::FONT_HERSHEY_DUPLEX, 1, cv::Scalar(255, 255, 0));
		break;
	case 3:
		cv::putText(Original_image, "FALLING ROCKS", cv::Point(340, 40), cv::FONT_HERSHEY_DUPLEX, 1, cv::Scalar(255, 255, 0));
		break;
	case 4:
		cv::putText(Original_image, "SPEED CAMERAS", cv::Point(340, 40), cv::FONT_HERSHEY_DUPLEX, 1, cv::Scalar(255, 255, 0));
		break;
	case 5:
		cv::putText(Original_image, "NO PARKING", cv::Point(340, 40), cv::FONT_HERSHEY_DUPLEX, 1, cv::Scalar(255, 255, 0));
		break;
	case 6:
		cv::putText(Original_image, "NO RIGHT TURN", cv::Point(340, 40), cv::FONT_HERSHEY_DUPLEX, 1, cv::Scalar(255, 255, 0));
		break;
	case 7:
		cv::putText(Original_image, "NO PEDESTRIAN", cv::Point(340, 40), cv::FONT_HERSHEY_DUPLEX, 1, cv::Scalar(255, 255, 0));
		break;
	case 8:
		cv::putText(Original_image, "BEWARE RAMP", cv::Point(340, 40), cv::FONT_HERSHEY_DUPLEX, 1, cv::Scalar(255, 255, 0));
		break;
	case 9:
		cv::putText(Original_image, "SLOW", cv::Point(340, 40), cv::FONT_HERSHEY_DUPLEX, 1, cv::Scalar(255, 255, 0));
		break;
	case 10:
		cv::putText(Original_image, "BEWARE PEDESTRIAN", cv::Point(340, 40), cv::FONT_HERSHEY_DUPLEX, 1, cv::Scalar(255, 255, 0));
		break;
	case 11:
		cv::putText(Original_image, "NO ENTRY", cv::Point(340, 40), cv::FONT_HERSHEY_DUPLEX, 1, cv::Scalar(255, 255, 0));
		break;
	case 12:
		cv::putText(Original_image, "SEPARATE WAY", cv::Point(340, 40), cv::FONT_HERSHEY_DUPLEX, 1, cv::Scalar(255, 255, 0));
		break;
	case 13:
		cv::putText(Original_image, "NO RETURN", cv::Point(340, 40), cv::FONT_HERSHEY_DUPLEX, 1, cv::Scalar(255, 255, 0));
		break;
	}

}
double filter2(cv::Mat &Original_image, cv::Mat &Hsv_image, int &maxX, int &maxY, int &minX, int &minY)
{
	int i = 0, j = 0;
	int red = 0;
	int backgroud = 0;

	for (i = minY; i < maxY; i++)
	{
		for (j = minX; j < maxX; j++)
		{
			if ((Hsv_image.at<cv::Vec3b>(i, j)[0] >= R_H_1 && Hsv_image.at<cv::Vec3b>(i, j)[0] <= R_H_2) || (Hsv_image.at<cv::Vec3b>(i, j)[0] >= R_H_3 &&Hsv_image.at<cv::Vec3b>(i, j)[0] <= R_H_4))
			{
				if (Hsv_image.at<cv::Vec3b>(i, j)[1] > R_L_1 &&Hsv_image.at<cv::Vec3b>(i, j)[1]<R_L_2 &&Hsv_image.at<cv::Vec3b>(i, j)[2]>R_S_1 &&Hsv_image.at<cv::Vec3b>(i, j)[2] < R_S_2)
				{
					red++;
				}
				else
				{
					backgroud++;
				}
			}
			else
			{
				backgroud++;
			}
		}
	}

	double percent = red / (double)(red + backgroud) * 100;

	return percent;
}
int dtb(cv::Mat &Binarization_image, int maxX, int maxY, int minX, int minY, int tmp)
{
	int i = 0;
	int j = 0;
	int k = 0;
	int temp = 0;
	int count = 0;
	double DTB[4][10] = { 0 };
	double Dstep = 0;
	double step = 0;
	int Istep = 0;
	//-------------------左邊往右邊掃--------------------//
	step = (double)(maxY - minY) / 9 - (maxY - minY) / 9;
	Dstep = step;
	Istep = (maxY - minY) / 9;
	for (i = minY; i <= maxY;)
	{
		for (j = minX; j <= maxX; j++)
		{
			if (Binarization_image.at<uchar>(i, j) > 0)
			{
				break;
			}
			temp++;
		}
		DTB[0][count] = (double)temp / (double)(maxX - minX);
		temp = 0;

		count++;
		Dstep = Dstep + step;
		if (i == maxY)
		{
			break;
		}
		else if (i + Istep + 1 >= maxY)
		{
			i = maxY;
		}
		else if (Dstep < 1)
		{
			i = i + Istep;
		}
		else if (Dstep >= 1)
		{
			Dstep = Dstep - 1;
			i = i + Istep + 1;
		}
	}
	//-------------------右邊往左邊掃--------------------//
	Dstep = step;
	count = 0;
	for (i = minY; i <= maxY;)
	{
		for (j = maxX; j >= minX; j--)
		{
			if (Binarization_image.at<uchar>(i, j) > 0)
			{
				break;
			}

			temp++;
			/*		Binarization_image.at<uchar>(i, j) = 128;
					cv::namedWindow("a", 0);
					cv::imshow("a", Binarization_image);
					cv::waitKey(60);*/
		}
		DTB[1][count] = (double)temp / (double)(maxX - minX);
		temp = 0;

		count++;
		Dstep = Dstep + step;
		if (i == maxY)
		{
			break;
		}
		else if (i + Istep + 1 >= maxY)
		{
			i = maxY;
		}
		else if (Dstep < 1)
		{
			i = i + Istep;
		}
		else if (Dstep >= 1)
		{
			Dstep = Dstep - 1;
			i = i + Istep + 1;
		}
	}
	//---------------------------------------------------//
	//-------------------上面往下面掃--------------------//
	step = (double)(maxX - minX) / 9 - (maxX - minX) / 9;
	Dstep = step;
	Istep = (maxX - minX) / 9;
	count = 0;
	for (i = minX; i <= maxX;)
	{
		for (j = minY; j <= maxY; j++)
		{
			if (Binarization_image.at<uchar>(j, i) > 0)
			{
				break;
			}
			temp++;
		}
		DTB[2][count] = (double)temp / (double)(maxY - minY);
		temp = 0;

		count++;
		Dstep = Dstep + step;
		if (i == maxX)
		{
			break;
		}
		else if (i + Istep + 1 >= maxX)
		{
			i = maxX;
		}
		else if (Dstep < 1)
		{
			i = i + Istep;
		}
		else if (Dstep >= 1)
		{
			Dstep = Dstep - 1;
			i = i + Istep + 1;
		}
	}
	//---------------------------------------------------//
	//-------------------下面往上面掃--------------------//
	Dstep = step;
	count = 0;
	for (i = minX; i <= maxX;)
	{
		for (j = maxY; j >= minY; j--)
		{
			if (Binarization_image.at<uchar>(j, i) > 0)
			{
				break;
			}
			temp++;
		}
		DTB[3][count] = (double)temp / (double)(maxY - minY);
		temp = 0;

		count++;
		Dstep = Dstep + step;
		if (i == maxX)
		{
			break;
		}
		else if (i + Istep + 1 >= maxX)
		{
			i = maxX;
		}
		else if (Dstep < 1)
		{
			i = i + Istep;
		}
		else if (Dstep >= 1)
		{
			Dstep = Dstep - 1;
			i = i + Istep + 1;
		}
	}
	//--------------------------------------------------//
	//------------形狀特徵(右、左、上、下)----------------------//
	double cir_tri_sqr[3][4][10] = { { { 0.393162,0.17094,0.0769231,0.017094,0,0,0.017094,0.0769231,0.17094,0.393162 }\
		,{ 0.393162,0.17094,0.0769231,0.017094,0,0,0.017094,0.0769231,0.17094,0.393162 }\
		,{ 0.393162,0.17094,0.0769231,0.017094,0,0,0.017094,0.0769231,0.17094,0.393162 }\
		,{ 0.393162,0.17094,0.0769231,0.017094,0,0,0.017094,0.0769231,0.17094,0.393162 } }\

		,{ { 0.487179,0.43589,0.376068,0.324786,0.26495,0.205128,0.153846,0.0940171,0.042735,0 }\
		,{ 0.478632,0.42735,0.367521,0.316239,0.25641,0.205128,0.153846,0.0940171,0.042735,0 }\
		,{ 0.965812,0.74359,0.521368,0.307692,0.08547,0.068376,0.290598,0.5128210,0.743590,0.96581 }\
		,{ 0.02    ,    0.02  ,     0.02  ,     0.02  ,    0.02  ,    0.02   ,    0.02   ,   0.02     ,   0.02    ,0.02 } }\

		,{ { 0    ,    0  ,     0  ,     0  ,    0  ,    0   ,    0   ,   0     ,   0    ,0 }\
		,{ 0    ,    0  ,     0  ,     0  ,    0  ,    0   ,    0   ,   0     ,   0    ,0 }\
		,{ 0    ,    0  ,     0  ,     0  ,    0  ,    0   ,    0   ,   0     ,   0    ,0 }\
		,{ 0    ,    0  ,     0  ,     0  ,    0  ,    0   ,    0   ,   0     ,   0    ,0 } } };
	//----------------------------------------------------------//

	//----------------使用DTB計算號誌差距--------------------//
	int cts = 2;//形狀
	int borader = 4;//邊
	int feature = 10;//特徵數量
	double gap[2] = { 0 };//結果差距
	for (i = 0; i < cts; i++)
	{
		for (j = 0; j < borader; j++)
		{
			for (k = 0; k < feature; k++)
			{
				gap[i] = gap[i] + pow((DTB[j][k] - cir_tri_sqr[i][j][k]), 2);
			}
		}
	}
	//-----------------------------------------------------//
	i = 0;
	if (gap[i] < gap[i + 1])
	{
		if (gap[i] <=0.5 && gap[i] > 0.000001)
		{
			return 1;//圓形
		}
	}
	else
	{
		if (gap[i + 1] <= 0.1 && gap[i + 1] > 0.000001)
		{
			return 2;//三角形
		}
	}

	return	0;
}
cv::Mat warpAffine(cv::Mat &roi_image)
{
	double a = 0;
	double angle = 0;
	int count = 0;
	int rows = roi_image.rows;
	int cols = roi_image.cols;

	cv::Point center = cv::Point(0, 0);
	uchar *roi_image_data = roi_image.ptr<uchar>(0);
	for (int i = 0; i < cols; i++)
	{
		if (*(roi_image_data + i) == 255)
		{
			break;
		}
		count++;
	}
	a = (double(cols / 2) - count);
	angle = atan(a / (rows / 2));

	angle = angle * 180 / 3.14;

	if ((double(cols / 2) - count) > 0)
	{
		center = cv::Point(0, rows - 1);
		angle = angle - 2;

	}
	else
	{
		center = cv::Point(cols - 1, rows - 1);
	}

	cv::Mat rot_mat = cv::getRotationMatrix2D(center, -angle, 1);
	if (angle < -5 || angle >5)
	{
		cv::warpAffine(roi_image, roi_image, rot_mat, roi_image.size());
	}

	return roi_image;
}
void voice(int shape)
{
	ISpVoice * pVoice = NULL;
	::CoInitialize(NULL);
	HRESULT hr = CoCreateInstance(CLSID_SpVoice, NULL, CLSCTX_ALL, IID_ISpVoice, (void **)&pVoice);
	std::wstring in = L"";
	switch (shape)
	{
	case 1:
		in = L"circle";
		hr = pVoice->Speak((LPCWSTR)in.c_str(), NULL, NULL);
		break;
	case 2:
		in = L"triangle";
		hr = pVoice->Speak((LPCWSTR)in.c_str(), NULL, NULL);
		break;
	case 3:
		in = L"square";
		hr = pVoice->Speak((LPCWSTR)in.c_str(), NULL, NULL);
		break;
	}
}
void onMouse(int event, int x, int y, int flags, void*param)
{
	cv::Mat *ptrimage = (cv::Mat*)param;
	if (event == CV_EVENT_LBUTTONDOWN) {
		std::cout << "x=" << x << ",y=" << y;
		std::cout << ",R=" << (int)(*ptrimage).at<cv::Vec3b>(y, x)[2];
		std::cout << ",G=" << (int)(*ptrimage).at<cv::Vec3b>(y, x)[1];
		std::cout << ",B=" << (int)(*ptrimage).at<cv::Vec3b>(y, x)[0];
		cv::cvtColor(*ptrimage, *ptrimage, CV_BGR2HLS_FULL);
		std::cout << ",H=" << (int)(*ptrimage).at<cv::Vec3b>(y, x)[0];
		std::cout << ",L=" << (int)(*ptrimage).at<cv::Vec3b>(y, x)[1];
		std::cout << ",S=" << (int)(*ptrimage).at<cv::Vec3b>(y, x)[2] << std::endl;
	}
}