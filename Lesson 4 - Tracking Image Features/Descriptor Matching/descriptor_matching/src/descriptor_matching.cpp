#include <iostream>
#include <numeric>
#include <opencv2/core.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/features2d.hpp>

#include "structIO.hpp"

using namespace std;

void matchDescriptors(cv::Mat &imgSource, cv::Mat &imgRef, vector<cv::KeyPoint> &kPtsSource, vector<cv::KeyPoint> &kPtsRef, cv::Mat &descSource, cv::Mat &descRef,
                      vector<cv::DMatch> &matches, string descriptorType, string matcherType, string selectorType)
{

    // configure matcher
    bool crossCheck = false; // crosscheck is for computing matches of keys1 to img2 and viceversa. and just match the ones that both have the same match. cannot be activated with KNN because it only works with normal match
    cv::Ptr<cv::DescriptorMatcher> matcher; // general matcher. tell if use brute force matcher or flann (kd tree) match

    if (matcherType.compare("MAT_BF") == 0)
    {

        int normType = descriptorType.compare("DES_BINARY") == 0 ? cv::NORM_HAMMING : cv::NORM_L2;
        matcher = cv::BFMatcher::create(normType, crossCheck);
        cout << "BF matching cross-check=" << crossCheck;
    }
    else if (matcherType.compare("MAT_FLANN") == 0)
    {
        if (descSource.type() != CV_32F)
        { // OpenCV bug workaround : convert binary descriptors to floating point due to a bug in current OpenCV implementation
            descSource.convertTo(descSource, CV_32F);
            descRef.convertTo(descRef, CV_32F);
        }

        // TODO : implement FLANN matching
        matcher = cv::FlannBasedMatcher::create();
        cout << "FLANN matching";
    }

    // perform matching task
    // in here we tell if want nearest neighbor (to keep only best match) or knn (keeping best k keypoints per match) and then computing the ratio between all of them and at the end choose the best one. this helps reduce ambiguos points where some keypoints could be very similar, reduces FP. 
    if (selectorType.compare("SEL_NN") == 0)
    { // nearest neighbor (best match)

        double t = (double)cv::getTickCount();
        matcher->match(descSource, descRef, matches); // Finds the best match for each descriptor in desc1
        t = ((double)cv::getTickCount() - t) / cv::getTickFrequency();
        cout << " (NN) with n=" << matches.size() << " matches in " << 1000 * t / 1.0 << " ms" << endl;
    }
    else if (selectorType.compare("SEL_KNN") == 0)
    { // k nearest neighbors (k=2)

        double t = (double)cv::getTickCount();
        // TODO : implement k-nearest-neighbor matching
        int k = 2;
        vector<vector<cv::DMatch>> knnMatches;
        matcher->knnMatch(descSource, descRef, knnMatches, 2);


        // TODO : filter matches using descriptor distance ratio test
        float threshold = 0.8;
        int discarded = 0;
        for (int indx = 0; indx < knnMatches.size(); indx++) {
            const vector<cv::DMatch>& match = knnMatches[indx];

            float firstDist = match[0].distance;
            float secondDist = match[1].distance;
            float ratio = firstDist / secondDist;
            bool valid_number = firstDist < threshold * secondDist;

            if (valid_number)
                matches.push_back(match[0]);
            else
                discarded++;
            
        }
        t = ((double)cv::getTickCount() - t) / cv::getTickFrequency();
        cout << " (KNN) with n=" << matches.size() << " matches in " << 1000 * t / 1.0 << " ms " << endl;

        cout << knnMatches.size() - matches.size() << " discarded points" << endl;
    }

    // visualize results
    cv::Mat matchImg = imgRef.clone();
    cv::drawMatches(imgSource, kPtsSource, imgRef, kPtsRef, matches,
                    matchImg, cv::Scalar::all(-1), cv::Scalar::all(-1), vector<char>(), cv::DrawMatchesFlags::DRAW_RICH_KEYPOINTS);

    string windowName = "Matching keypoints between two camera images (best 50)";
    cv::namedWindow(windowName, 7);
    cv::imshow(windowName, matchImg);
    cv::waitKey(0);
}

/*
RESULTS
1) Brute force + Nearest neighboors
-BF matching cross-check=0 (NN) with n=2896 matches in 14.6868 ms
-BF matching cross-check=1 (NN) with n=1705 matches in 23.1417 ms

-Takes longer in general but less matches made.

2) Brute force + KNearest - in two datasets
- BF matching cross-check=0 (KNN) with n=1578 matches in 19.8127 ms 
1318 discarded points
- BF matching cross-check=0 (KNN) with n=55 matches in 3.02166 ms 
45 discarded points

Rougly half of the points where discarded

3) BF vs Flann
** BRISK - USING HAMMING BECAUS BINARY
- FLANN matching (NN) with n=2896 matches in 61.0417 ms
- FLANN matching (KNN) with n=1177 matches in 45.4164 ms 
1719 discarded points

** SIFT - USING L2 NORM
- FLANN matching (NN) with n=2896 matches in 45.4864 ms
- FLANN matching (KNN) with n=1177 matches in 45.5844 ms 
1719 discarded points

- BF matching cross-check=0 (NN) with n=1890 matches in 15.1509 ms

In general flann is much sower than previous algorihtms, taking more miliseconds to compute
*/

int main()
{
    cv::Mat imgSource = cv::imread("../images/img1gray.png");
    cv::Mat imgRef = cv::imread("../images/img2gray.png");

    // vector<cv::KeyPoint> kptsSource, kptsRef; 
    // readKeypoints("../dat/C35A5_KptsSource_BRISK_large.dat", kptsSource);
    // readKeypoints("../dat/C35A5_KptsRef_BRISK_large.dat", kptsRef);

    // cv::Mat descSource, descRef; 
    // readDescriptors("../dat/C35A5_DescSource_BRISK_large.dat", descSource);
    // readDescriptors("../dat/C35A5_DescRef_BRISK_large.dat", descRef);
    
    // vector<cv::KeyPoint> kptsSource, kptsRef; 
    // readKeypoints("../dat/C35A5_KptsSource_BRISK_small.dat", kptsSource);
    // readKeypoints("../dat/C35A5_KptsRef_BRISK_small.dat", kptsRef);

    // cv::Mat descSource, descRef; 
    // readDescriptors("../dat/C35A5_DescSource_BRISK_small.dat", descSource);
    // readDescriptors("../dat/C35A5_DescRef_BRISK_small.dat", descRef);

    vector<cv::KeyPoint> kptsSource, kptsRef; 
    readKeypoints("../dat/C35A5_KptsSource_SIFT.dat", kptsSource);
    readKeypoints("../dat/C35A5_KptsRef_SIFT.dat", kptsRef);

    cv::Mat descSource, descRef; 
    readDescriptors("../dat/C35A5_DescSource_SIFT.dat", descSource);
    readDescriptors("../dat/C35A5_DescRef_SIFT.dat", descRef);


    vector<cv::DMatch> matches;
    string matcherType = "MAT_BF"; 
    string descriptorType = 0 ? "DES_BINARY" : ""; 
    string selectorType = "SEL_NN";
    matchDescriptors(imgSource, imgRef, kptsSource, kptsRef, descSource, descRef, matches, descriptorType, matcherType, selectorType);
}