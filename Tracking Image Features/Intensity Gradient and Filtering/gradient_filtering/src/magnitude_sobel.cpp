#include <iostream>
#include <numeric>
#include <opencv2/core.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/imgproc.hpp>


using namespace std;

void magnitudeSobel()
{
    // load image from file
    cv::Mat img;
    img = cv::imread("../images/img1gray.png");

    // convert image to grayscale
    cv::Mat imgGray;
    cv::cvtColor(img, imgGray, cv::COLOR_BGR2GRAY);

    // apply smoothing using the GaussianBlur() function from the OpenCV
    // ToDo : Add your code here
    cv::Mat gaussian_smoothing;
    cv::GaussianBlur(imgGray, gaussian_smoothing, cv::Size(5, 5), 2.0);

    // create filter kernels using the cv::Mat datatype both for x and y
    // ToDo : Add your code here
    cv::Mat filterx;
    cv::Mat filtery;
    
    float sobel_x[9] = {-1, 0, +1,
                        -2, 0, +2, 
                        -1, 0, +1};
    cv::Mat kernel_x = cv::Mat(3, 3, CV_32F, sobel_x);
    
    float sobel_y[9] = {-1, -2, -1,
                        0, 0, 0, 
                        1, 2, 1};
    cv::Mat kernel_y = cv::Mat(3, 3, CV_32F, sobel_y);

    // apply filter using the OpenCv function filter2D()
    // ToDo : Add your code here
    cv::filter2D(gaussian_smoothing, filterx, -1, kernel_x, cv::Point(-1, -1), 0, cv::BORDER_DEFAULT );
    cv::filter2D(gaussian_smoothing, filtery, -1, kernel_y, cv::Point(-1, -1), 0, cv::BORDER_DEFAULT );
    

    // compute magnitude image based on the equation presented in the lesson 
    // ToDo : Add your code here
    cv::Mat magnitude = imgGray.clone();
    for (int rndx = 0; rndx < magnitude.rows; rndx++) {
        for (int cndx = 0; cndx < magnitude.cols; cndx++) {
            magnitude.at<unsigned char>(rndx, cndx) = 
                sqrt(
                    (filterx.at<unsigned char>(rndx, cndx) * filterx.at<unsigned char>(rndx, cndx))
                    +
                    (filtery.at<unsigned char>(rndx, cndx) * filtery.at<unsigned char>(rndx, cndx))
                );
        }
    }

    // show result
    string windowName = "Gaussian Blurring";
    // cv::namedWindow(windowName, 1); // create window
    // cv::imshow(windowName, magnitude);
    // cv::imshow("gaussian", gaussian_smoothing);
    // cv::waitKey(0); // wait for keyboard input before continuing
    // cv::imshow("sobelx", filterx);
    // cv::waitKey(0); // wait for keyboard input before continuing
    // cv::imshow("sobely", filtery);
    // cv::waitKey(0);
    cv::imshow("magnitude", magnitude);
    cv::waitKey(0);
}

int main()
{
    magnitudeSobel();
}