#include <opencv2/core.hpp>
#include <opencv2/core/mat.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/imgproc.hpp>

#include "dataStructures.h"
#include "structIO.hpp"

using namespace std;

void showLidarTopview() {
  std::vector<LidarPoint> lidarPoints;
  readLidarPts("../dat/C51_LidarPts_0000.dat", lidarPoints);

  cv::Size worldSize(20.0, 10.0); // width and height of sensor field in m
  cv::Size imageSize(2000, 1000); // corresponding top view image in pixel

  // create topview image
  cv::Mat topviewImg(imageSize, CV_8UC3, cv::Scalar(0, 0, 0));


  // plot Lidar points into image
  for (auto it = lidarPoints.begin(); it != lidarPoints.end(); ++it) {
    float xw = (*it).x; // world position in m with x facing forward from sensor
    float yw = (*it).y; // world position in m with y facing left from sensor
    float yz = (*it).z;
    float r = (*it).r;
    float limitz = -1.40;

    // When less than limit, then continue and not compute the point. Points below this limit, mean points of the road
    if (yz < limitz)
      continue;

    // Convert the coordinates to x,y in the iamge position
    // Positive yw value is the left part. right most will have yw of negative, thus want to change value
    /*
    if yw = -2, then 300px. if yw = 2, then 700px
    */
    int y = (imageSize.height / 2.0) + (-yw * imageSize.height / worldSize.height);
    int x = (xw * imageSize.width / worldSize.width);
    // int y = imageSize.height - (xw * imageSize.height / worldSize.height);
    // int x = (-yw * imageSize.width / worldSize.width) + (imageSize.width / 2.0);

    //* Max val is the reference point used to calculate where is the green, and
    // then its half is where we have yellow, and opposite to that is the red
    //(closer to car)
    int max_val = worldSize.width; // This is the window that we are
                                    // considering to control red,yellow, green
                                    // colors. afar from here we have only green
    int red_intensity;
    int green_intensity;

    //* Based on distance
    if (xw >= 0 && xw <= max_val) {
      red_intensity = 255 - (xw / max_val) * 255;
      green_intensity = (xw / max_val) * 255;
    }

    //* Color based on reflectivity, which goes 0 <= r <= 1
    // max_val = 1;
    // if (r >= 0 && r <= max_val) {
    //     red_intensity = r * 255;
    //     green_intensity = 255 - (r * 255);
    // }

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
  int nMarkers = floor(worldSize.width / lineSpacing);
  for (size_t line_indx = 0; line_indx < nMarkers; line_indx++) {
    int x = ((line_indx * lineSpacing) * imageSize.width / worldSize.width);
    // Blue line that delimits the distance
    cv::line(topviewImg, cv::Point(x, 0), cv::Point(x, imageSize.height),
             cv::Scalar(255, 0, 0), 2);
  }

  // display image
  string windowName = "Top-View Perspective of LiDAR data";
  // cv::Mat rotatedImage;
  // cv::rotate(topviewImg, rotatedImage, cv::ROTATE_90_CLOCKWISE);
  // Window normal allows the resizeWindow to work
  cv::namedWindow(windowName, cv::WINDOW_NORMAL);

  cv::resizeWindow(windowName, 500, 250);
  cv::imshow(windowName, topviewImg);
  cv::waitKey(0); // wait for key to be pressed
}

int main() { showLidarTopview(); }