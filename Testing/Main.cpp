#include <stdio.h>
#include <iostream>
#include <opencv2/opencv.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/core/core.hpp>
#include <opencv2/features2d/features2d.hpp>
#include <opencv2/features2d.hpp>
#include <opencv2/xfeatures2d/nonfree.hpp>
#include <opencv2/calib3d/calib3d.hpp>
#include <opencv2/calib3d.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/xfeatures2d.hpp>
#include <vector>
#include <algorithm>

using namespace cv;
using namespace std;

void area(const Mat& img, double& black)
{
	// Check if every pixel color is non-zero (not black)
	black = 0;
	for (int col = 0; col < 1000; col++)
	{
		for (int row = 0; row < 1000; row++)
		{
			Vec3b color_img = img.at<Vec3b>(Point(col, row));
			if (norm(color_img) != 0)
				black++;
		}
	}
}
void stitch(Mat& canvas, const Mat& img, const Mat& H, int& flag)
{
	static int file = 0;
	Mat p_img;

	flag = 0;

	warpPerspective(img, p_img, H, Size(1000, 1000), INTER_LINEAR, BORDER_TRANSPARENT);
	imwrite(format("p%d.jpg", file), p_img);
	file++;

	static double black;
	double temp_black;
	// Calculate the amount of "black" in the transformed image
	area(p_img, temp_black);
	if (file == 1)
	{
		area(p_img, black);
	}
	if (temp_black <= 1.5 * black)
	{
		black = temp_black;
		flag = 1;
	}
	// If stitching is successful, blend the transformed image with the canvas
	if (flag == 1) {
		for (int col = 0; col < canvas.cols; col++) {
			for (int row = 0; row < canvas.rows; row++) {
				if (col < p_img.cols && row < p_img.rows) {
					Vec3b color_im1 = canvas.at<Vec3b>(Point(col, row));
					Vec3b color_im2 = p_img.at<Vec3b>(Point(col, row));

					// Blend colors based on their norms
					if (norm(color_im1) != 0 && norm(color_im2) != 0) {
						canvas.at<Vec3b>(Point(col, row)) = 0.1 * color_im2 + 0.9 * color_im1;
					}
					else if (norm(color_im1) == 0) {
						canvas.at<Vec3b>(Point(col, row)) = color_im2;
					}
				}
			}
		}
	}
}

void homography(const Mat& im1, const Mat& im2, Mat& H, int& flag)
{
	vector<KeyPoint> kp1, kp2;
	Mat ds1, ds2;
	flag = 0;

	Mat img1, img2;
	cvtColor(im1, img1, COLOR_RGB2GRAY);
	cvtColor(im2, img2, COLOR_RGB2GRAY);

	// Extract Features using SIFT
	Ptr<Feature2D> detector = SIFT::create();
	detector->detect(img1, kp1);
	detector->detect(img2, kp2);

	// Extract Detectors using SIFT
	Ptr<Feature2D> extractor = SIFT::create();
	extractor->compute(img1, kp1, ds1);
	extractor->compute(img2, kp2, ds2);

	// Find close matches using Brute Force
	BFMatcher matcher(NORM_L2, false);
	vector< DMatch > matches;
	matcher.match(ds1, ds2, matches);

	//Max and min distance calculation between keypoints
	double max_dist = 0;
	double min_dist = 100;
	for (int i = 0; i < ds1.rows; i++)
	{
		double dist = matches[i].distance;
		if (dist < min_dist) min_dist = dist;
		if (dist > max_dist) max_dist = dist;
	}
	vector< DMatch > good_matches;
	for (int i = 0; i < ds1.rows; i++)
	{
		if (matches[i].distance < 3 * min_dist)
		{
			good_matches.push_back(matches[i]);
		}
	}

	int ngood_match = int(good_matches.size());

	// Push_back Keypoints from good_matches 
	vector< Point2f > kp_1;
	vector< Point2f > kp_2;
	for (int i = 0; i < ngood_match; i++)
	{
		kp_1.push_back(kp1[good_matches[i].queryIdx].pt);
		kp_2.push_back(kp2[good_matches[i].trainIdx].pt);
	}

	Mat mask;
	if (ngood_match >= 4)
	{
		H = findHomography(kp_2, kp_1, cv::RANSAC, 0, mask);
		flag = 1;
	}

}
int main() {
	//Load images (hard-coded here)
	vector<string> imageFilenames = {
		"C:\\Users\\Satansu\\source\\repos\\Testing\\img1.jpg",
		"C:\\Users\\Satansu\\source\\repos\\Testing\\img2.jpg",
		"C:\\Users\\Satansu\\source\\repos\\Testing\\img3.jpg",
		"C:\\Users\\Satansu\\source\\repos\\Testing\\img4.jpg"};

	vector<Mat> imgs;
	for (const auto& filename : imageFilenames) {
		Mat img = imread(filename);
		if (img.empty()) {
			cout << "Unable to load image: " << filename << endl;
			return -1;
		}

		// Resize the image if necessary
		resize(img, img, Size(400, 400));

		// Store the image
		imgs.push_back(img);
	}

	// Initialize variables
	Mat H_0j = Mat::eye(3, 3, CV_64FC1);
	Mat H_ij = Mat::eye(3, 3, CV_64FC1);
	Mat canvas;

	// Process images
	int count = 0;
	for (int frIdx = 0; frIdx < 4; frIdx++) {
		if (frIdx == 0) {
			canvas = imgs[0].clone(); // Initialize canvas with the first image
		}
		else if (frIdx % 2 == 0 && frIdx >= 1) {
			count++;
			int flag = 0;
			Mat temp;
			int flag_s = 0;
			homography(imgs[count - 1], imgs[count], H_ij, flag);
			if (flag == 1) {
				gemm(H_0j, H_ij, 1, 0, 0, temp, 0);
				stitch(canvas, imgs[count], temp, flag_s);
				imshow("canvas", canvas);
				if (waitKey(1) == 27)
					break;
				if (flag_s == 1)
					H_0j = temp;
			}
		}
	}

	// Save canvas image
	imwrite("canvas.jpg", canvas);

	return 0;
}