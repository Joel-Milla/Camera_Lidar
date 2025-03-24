#include <opencv2/core.hpp>
#include <opencv2/highgui.hpp>


using namespace std;

void createMatrix1()
{
    // create matrix
    int nrows = 480, ncols = 640;
    cv::Mat m1_8u;
    m1_8u.create(nrows, ncols, CV_8UC1); // two-channel matrix with 8bit unsigned elements. Has nrows, ncols, and each element is cv with 8bit (0-255 numbers) unsigned (only positive) char, with 1 depth (menaing one color)
    m1_8u.setTo(cv::Scalar(255)); // Set all colors to 255, which is white

    // STUDENT TASK :
    // Create a variable of type cv::Mat* named m3_8u which has three channels with a
    cv::Mat m3_8u;
    m3_8u.create(nrows, ncols, CV_8UC3); // now with three channels
    // depth of 8bit per channel. Then, set the first channel to 255 and display the result.
    m3_8u.setTo(cv::Scalar(255,0,0));


    // show result
    string windowName = "First steps in OpenCV (m1_8u)";
    cv::namedWindow(windowName, 1); // create window
    // cv::imshow(windowName, m1_8u);
    cv::imshow(windowName, m3_8u);
    cv::waitKey(0); // wait for keyboard input before continuing

    // STUDENT TASK :
    // Display the results from the STUDENT TASK above

}


int main()
{
    createMatrix1();
    return 0;
}