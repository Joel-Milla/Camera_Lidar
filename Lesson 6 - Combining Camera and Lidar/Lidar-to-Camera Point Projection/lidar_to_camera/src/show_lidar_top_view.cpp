#include <opencv2/core.hpp>
#include <opencv2/core/mat.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/imgproc.hpp>

#include "structIO.hpp"

using namespace std;

void showLidarTopview() {
  std::vector<LidarPoint> lidarPoints;
  readLidarPts("../dat/C51_LidarPts_0000.dat", lidarPoints);

  cv::Size worldSize(10.0, 20.0); // width and height of sensor field in m
  cv::Size imageSize(1000, 2000); // corresponding top view image in pixel

  // create topview image
  cv::Mat topviewImg(imageSize, CV_8UC3, cv::Scalar(0, 0, 0));

  // plot Lidar points into image
  for (auto it = lidarPoints.begin(); it != lidarPoints.end(); ++it) {
    float xw = (*it).x; // world position in m with x facing forward from sensor
    float yw = (*it).y; // world position in m with y facing left from sensor
    float yz = (*it).z;
    float limitz = -1.40;

    // When less than limit, then continue and not compute the point
    if (yz < limitz)
      continue;

    // Convert the coordinates to x,y in the iamge position
    // Positive yw value is the left part. right most will have yw of negative, thus want to change value
    int y = imageSize.height - (xw * imageSize.height / worldSize.height);
    int x = (-yw * imageSize.width / worldSize.width) + (imageSize.width / 2.0);

    //* Max val is the reference point used to calculate where is the green, and
    // then its half is where we have yellow, and opposite to that is the red
    //(closer to car)
    int max_val = worldSize.height; // This is the window that we are
                                    // considering to control red,yellow, green
                                    // colors. afar from here we have only green
    int red_intensity = 0;
    int green_intensity = 255;
    if (xw >= 0 && xw <= max_val) {
      red_intensity = 255 - (xw / max_val) * 255;
      green_intensity = (xw / max_val) * 255;
    }

    cv::circle(topviewImg, cv::Point(x, y), 5,
               cv::Scalar(0, green_intensity, red_intensity), -1);
  }

  /*
  Long explanation
  // imageSize.height / worldSize.height: tells how much pixels to move per
  meter
      // i * lineSpacing returns coordinate in meters in the axis
      // multiplying both maps the meters into corresponding pixel
      // summing height is the same as doing= y=meters*(pixels per meter). what
  this is doing is getting linespace and substracting that to height
  */
  // Plot distance markers
  float lineSpacing = 2.0; // gap between distance markers
  int nMarkers = floor(worldSize.height / lineSpacing);
  for (size_t i = 0; i < nMarkers; ++i) {
    int y = (-(i * lineSpacing) * imageSize.height / worldSize.height) +
            imageSize.height;
    // Blue line that delimits the distance
    cv::line(topviewImg, cv::Point(0, y), cv::Point(imageSize.width, y),
             cv::Scalar(255, 0, 0), 2);
  }

  // display image
  string windowName = "Top-View Perspective of LiDAR data";
  // cv::Mat rotatedImage;
  // cv::rotate(topviewImg, rotatedImage, cv::ROTATE_90_CLOCKWISE);
  // Window normal allows the resizeWindow to work
  cv::namedWindow(windowName, cv::WINDOW_NORMAL);
  cv::resizeWindow(windowName, 250, 1000);
  cv::imshow(windowName, topviewImg);
  cv::waitKey(0); // wait for key to be pressed
}

int main() { showLidarTopview(); }