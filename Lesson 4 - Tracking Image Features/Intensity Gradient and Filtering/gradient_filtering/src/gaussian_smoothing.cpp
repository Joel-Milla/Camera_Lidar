#include <iostream>
#include <numeric>
#include <opencv2/core.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/imgproc.hpp>

using namespace std;

void gaussianSmoothing1()
{
    // load image from file
    cv::Mat img;
    img = cv::imread("../images/img1gray.png"); // grayscale image, with one channle cv detects it
    // cv::imshow("image before gaussian", img);
  
    // create filter kernel
    float gauss_data[25] = {1, 4, 7, 4, 1,
                            4, 16, 26, 16, 4,
                            7, 26, 41, 26, 7,
                            4, 16, 26, 16, 4,
                            1, 4, 7, 4, 1};
    cv::Mat kernel = cv::Mat(5, 5, CV_32F, gauss_data);
  
  	// TODO: Divide each element of the kernel by the sum of all the values in the kernel.
    double sum = cv::sum(kernel)[0];
    std::cout << sum << std::endl;

    // auto normalize_value = [&](double value) { return (value / sum); };
    // std::transform(kernel.begin<double>(), kernel.end<double>(), kernel.begin<double>(), normalize_value);

    kernel /= sum; // cv supports each element gets divided directly

    // apply filter
    cv::Mat result;
    cv::filter2D(img, result, -1, kernel, cv::Point(-1, -1), 0, cv::BORDER_DEFAULT); // takes image, where to stored image, depth of image, kernel, then anchro point (-1,-1) is default anchor in center, then delta and borde

    // show result
    string windowName = "Gaussian Blurring";
    cv::namedWindow(windowName, 1); // create window
    cv::imshow(windowName, result);
    cv::waitKey(0); // wait for keyboard input before continuing
}

int main()
{
    gaussianSmoothing1();
}