#include <iostream>
#include <numeric>
#include <opencv2/core.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/features2d.hpp>

using namespace std;

int getMaxValue(const cv::Mat& img, int windows_size, int indx, int jndx) {
    int local_maximum = img.at<int>(indx, jndx);

    std::vector<pair<int,int>> movements = {{-1,-1},{-1,0},
                                                {-1,1},{0,1},
                                                {1,1},{1,0},
                                                {1,-1},{0,-1}};

    for (int count = 1; count <= windows_size; count++) {
        for (const pair<int,int>& movement : movements) {
            int new_indx = indx + movement.first;
            int new_jndx = jndx + movement.second;

            int compared_value = img.at<int>(new_indx, new_jndx);
            if (compared_value > local_maximum)
                local_maximum = compared_value;
        }
    }
    
    return local_maximum;
}

void cornernessHarris()
{
    // load image from file
    cv::Mat img;
    img = cv::imread("../images/img1.png");
    cv::cvtColor(img, img, cv::COLOR_BGR2GRAY); // convert to grayscale

    // Detector parameters
    int blockSize = 2;     // for every pixel, a blockSize × blockSize neighborhood is considered. This replaces the standar deviation that was explained, so the block size defines the number of neighboors that are considered for detecting the keypoints
    int apertureSize = 3;  // aperture parameter for Sobel operator (must be odd). Can control the size of the sobel operator. But larger makes it better for noise but also makes it less precise. 
    int minResponse = 100; // minimum value for a corner in the 8bit scaled response matrix
    double k = 0.04;       // Harris parameter (see equation for details)

    // Detect Harris corners and normalize output
    cv::Mat dst, dst_norm, dst_norm_scaled;
    dst = cv::Mat::zeros(img.size(), CV_32FC1);
    cv::cornerHarris(img, dst, blockSize, apertureSize, k, cv::BORDER_DEFAULT);
    cv::normalize(dst, dst_norm, 0, 255, cv::NORM_MINMAX, CV_32FC1, cv::Mat());
    cv::convertScaleAbs(dst_norm, dst_norm_scaled);

    // visualize results
    string windowName = "Harris Corner Detector Response Matrix";
    cv::namedWindow(windowName, 4);
    cv::imshow(windowName, dst_norm_scaled);
    cv::waitKey(0);

    // TODO: Your task is to locate local maxima in the Harris response matrix 
    // and perform a non-maximum suppression (NMS) in a local neighborhood around each maximum. The resulting coordinates shall be stored in a list of keypoints of the type `vector<cv::KeyPoint>`.
    cv::Mat result_image = cv::Mat::zeros(dst_norm_scaled.size(), CV_8U);
    for (int indx = 0; indx < img.rows; indx++) {
        for (int jndx = 0; jndx < img.cols; jndx++) {
            int curr_value = dst_norm_scaled.at<int>(indx, jndx);
            int max_local = getMaxValue(dst_norm_scaled, 1, indx, jndx);

            if (curr_value == max_local)
                result_image.at<int>(indx, jndx) = 255;
        }
    }

    cv::imshow(windowName, result_image);
    cv::waitKey(0);

    // save values in c_keypoints
    std::vector<cv::KeyPoint>  c_keypoints;
    for ( int indx = 0; indx < dst_norm_scaled.rows; indx++) {
        for (int jndx = 0; jndx < dst_norm_scaled.cols; jndx++) {
            int curr_value = result_image.at<int>(indx, jndx);
            if (curr_value == 0)
                continue;

            cv::Vec<float, 7> v = dst_norm_scaled.at< cv::Vec<float, 7> >(indx,jndx);
            cv::KeyPoint kp(v[0], v[1], v[2], v[3], v[4], (int)v[5], (int)v[6]);

            c_keypoints.push_back(kp);
        }

    };
}

int main()
{
    cornernessHarris();
}