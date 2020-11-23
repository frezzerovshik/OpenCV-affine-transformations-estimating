#include <opencv2/opencv.hpp>

using namespace cv;
using namespace std;

int main()
{
	Mat image = imread("image_3_10.bmp", IMREAD_UNCHANGED);
	int xc{}, yc{};
	int x_numerator{}, x_denominator{}, y_numerator{}, y_denominator{};
	for (int x = 0; x < image.cols; ++x)
	{
		for (int y = 0; y < image.rows; ++y)
		{
			x_numerator += x * image.at<uchar>(y, x);
			x_denominator += image.at<uchar>(y, x);
			y_numerator += y * image.at<uchar>(y, x);
			y_denominator += image.at<uchar>(y, x);
		}
	}
	xc = round(x_numerator / x_denominator); 
	yc = round(y_numerator / y_denominator);
	double T{}, B{}, C{}, D{}, compress_direction, compress_amount, nx{}, ny{};
	for (double x = 0; x < image.cols; ++x)
	{
		for (double y = 0; y < image.rows; ++y)
		{
			T = image.at<uchar>(y, x);
			nx = x - xc; ny = y - yc;
			B += T * (nx*nx - ny*ny);
			C += T * 2 * nx * ny;
			D += T * (nx*nx + ny*ny);
		}
	}
	compress_amount = sqrt((D - sqrt(C*C+ B*B)) / (D + sqrt(C*C + B*B)));
	compress_direction = 0.5 * atan(C / B);
	float M{};
	float m_numerator{}, m_denominator{};
	for (int x = 0; x < image.cols; ++x)
	{
		for (int y = 0; y < image.rows; ++y)
		{
			m_numerator += image.at<uchar>(y, x) * sqrt(pow(x - xc, 2) + pow(y - yc, 2));
			m_denominator += image.at<uchar>(y, x);
		}
	}
	m_denominator *= 10;
	M = m_numerator / m_denominator;
	Point2f center(xc, yc);
	Mat rotate_plus = getRotationMatrix2D(center, compress_direction * (180 / CV_PI), 1);
	Mat rotate_minus = getRotationMatrix2D(center, -compress_direction * (180 / CV_PI), 1);
	Mat compress = getRotationMatrix2D(center, 0, 1 / compress_amount);
	Mat scale = getRotationMatrix2D(center, 0, 1 / M);
	if (atan(C / B) < 1)
	{
		if (B < C)
		{
			compress.at<double>(1, 1) = 1;
			compress.at<double>(1, 2) = 1;
		}
		else
		{
			compress.at<double>(0, 0) = 1;
			compress.at<double>(0, 2) = 1;
		}
	}
	else
	{
		if (B > C)
		{
			compress.at<double>(1, 1) = 1;
			compress.at<double>(1, 2) = 1;
		}
		else
		{
			compress.at<double>(0, 0) = 1;
			compress.at<double>(0, 2) = 1;
		}
	}
	Size size = Size(image.cols, image.rows);
	warpAffine(image, image, rotate_plus, size);
	warpAffine(image, image, compress, size);
	warpAffine(image, image, rotate_minus, size);
	warpAffine(image, image, scale, size);
	imshow("image", image);
	waitKey(0);
	return 0;
}