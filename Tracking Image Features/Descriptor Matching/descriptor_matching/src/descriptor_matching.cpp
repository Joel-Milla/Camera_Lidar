#include <iostream>
#include <opencv2/core.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/features2d.hpp>

#include "structIO.hpp"

using namespace std;

/**
 * @brief Match keypoints
 * 
 * @param imgSource 
 * @param imgRef 
 * @param kPtsSource 
 * @param kPtsRef 
 * @param descSource 
 * @param descRef 
 * @param matches 
 * @param descriptorType 
 * @param matcherType 
 * @param selectorType 
 * @param crossCheck 
 */
void matchDescriptors(cv::Mat &imgSource, cv::Mat &imgRef, vector<cv::KeyPoint> &kPtsSource, vector<cv::KeyPoint> &kPtsRef, cv::Mat &descSource, cv::Mat &descRef, vector<cv::DMatch> &matches, string descriptorType, string matcherType, string selectorType, bool crossCheck) {
    // crosscheck is for computing matches of keys1 to img2 and viceversa. and just match the ones that both have the same match. cannot be activated with KNN because it only works with normal match. Both BF/Flann happen at the same time. BF + crossCheck only keep match if both are mutual best match. Crosscheck + knn cannot be applied because knn fundamentally returns multiple values

    cv::Ptr<cv::DescriptorMatcher> matcher; // general matcher. tell if use brute force matcher or flann (kd tree) match

    // Brute force approach where compare all keypoints and compares all distances
    if (matcherType.compare("MAT_BF") == 0) {

        int normType = descriptorType.compare("DES_BINARY") == 0 ? cv::NORM_HAMMING : cv::NORM_L2;
        matcher = cv::BFMatcher::create(normType, crossCheck);
        cout << "BF matching cross-check=" << crossCheck;
    }
    // Constructs a kdTree to traverse all distances and returns the matching pairs
    else if (matcherType.compare("") == 0) {
        if (descSource.type() != CV_32F) { 
            // OpenCV bug workaround : convert binary descriptors to floating point due to a bug in current OpenCV implementation
            descSource.convertTo(descSource, CV_32F);
            descRef.convertTo(descRef, CV_32F);
        }

        //* Implement FLANN matching
        matcher = cv::FlannBasedMatcher::create();
        cout << "FLANN matching";
    }

    // perform matching task
    // in here we tell if want nearest neighbor (to keep only best match) or knn (keeping best k keypoints per match and then compute distnace ratio to know if discard it or not). Using implementation above, if chooose Bf then compute all distances and then choose best. If choose flann, then that returns closes above certain threshold but faster than BF
    if (selectorType.compare("SEL_NN") == 0) { 
        // nearest neighbor (best match)
        double t = (double)cv::getTickCount();
        matcher->match(descSource, descRef, matches); // Finds the best match for each descriptor in desc1
        t = ((double)cv::getTickCount() - t) / cv::getTickFrequency();
        cout << " (NN) with n=" << matches.size() << " matches in " << 1000 * t / 1.0 << " ms" << endl;
    }
    // knn (keeping best k keypoints per match) and then computing the ratio between all of them and at the end choose the best one. this helps reduce ambiguos points where some keypoints could be very similar, reduces FP. If BF, then compute all distances and from list returned then performed Knn where compared the first two closest and compare distance ratio. For each match, it returns the two closes. Our work is compute disntace ratio and decide which to show
    else if (selectorType.compare("SEL_KNN") == 0) {
        double t = (double)cv::getTickCount();
        //* k-nearest-neighbor matching
        int k = 2;
        vector<vector<cv::DMatch>> knnMatches;
        matcher->knnMatch(descSource, descRef, knnMatches, 2);


        //* Filter matches using descriptor distance ratio test
        float threshold = 0.8;
        int discarded = 0;
        for (int indx = 0; indx < knnMatches.size(); indx++) {
            const vector<cv::DMatch>& match = knnMatches[indx];

            float firstDist = match[0].distance;
            float secondDist = match[1].distance;
            float ratio = firstDist / secondDist;
            bool valid_match = ratio < threshold;

            if (valid_match)
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
    
    cout << endl;
}

/**
 * @brief Recieves parameters and files to read, that will be used when calling matchDescriptor (to match keypoints)
 * 
 * @param brisk Use brisk dataset?
 * @param small Use small dataset?
 * @param crossCheck Use crosscheck to discard keypoints? when not using knn
 * @param descriptorType Using binary descriptors or HOG for sift
 * @param matcherType Using flann or brute force approach
 * @param selectorType Using NN (nearest neighbor) to choose keypoints or kNN to discard similar keypoints
 */
void callMethod(bool brisk, bool small, bool crossCheck, string descriptorType, string matcherType, string selectorType) {
    cv::Mat imgSource = cv::imread("../images/img1gray.png");
    cv::Mat imgRef = cv::imread("../images/img2gray.png");

    vector<cv::KeyPoint> kptsSource, kptsRef; 
    cv::Mat descSource, descRef;
    string dataset;

    if (brisk && !small) {
        dataset = "Brisk large";
        readKeypoints("../dat/C35A5_KptsSource_BRISK_large.dat", kptsSource);
        readKeypoints("../dat/C35A5_KptsRef_BRISK_large.dat", kptsRef);

        readDescriptors("../dat/C35A5_DescSource_BRISK_large.dat", descSource);
        readDescriptors("../dat/C35A5_DescRef_BRISK_large.dat", descRef);
    }

    if (brisk && small) {
        dataset = "Brisk small";
        readKeypoints("../dat/C35A5_KptsSource_BRISK_small.dat", kptsSource);
        readKeypoints("../dat/C35A5_KptsRef_BRISK_small.dat", kptsRef);

        readDescriptors("../dat/C35A5_DescSource_BRISK_small.dat", descSource);
        readDescriptors("../dat/C35A5_DescRef_BRISK_small.dat", descRef);
    }    

    if (!brisk) {
        dataset = "Sift";
        readKeypoints("../dat/C35A5_KptsSource_SIFT.dat", kptsSource);
        readKeypoints("../dat/C35A5_KptsRef_SIFT.dat", kptsRef);
        
        readDescriptors("../dat/C35A5_DescSource_SIFT.dat", descSource);
        readDescriptors("../dat/C35A5_DescRef_SIFT.dat", descRef);
    }


    vector<cv::DMatch> matches;
    cout << dataset << endl;
    matchDescriptors(imgSource, imgRef, kptsSource, kptsRef, descSource, descRef, matches, descriptorType, matcherType, selectorType, crossCheck);
}

int main() {

    //* First task compare brisk small with corssCheck turned off vs on
    callMethod(true, true, false, "DES_BINARY", "MAT_BF", "SEL_NN");

    callMethod(true, true, true, "DES_BINARY", "MAT_BF", "SEL_NN");

    //* Second task, brisk small vs large using KNN
    callMethod(true, true, false, "DES_BINARY", "MAT_BF", "SEL_KNN");

    callMethod(true, false, false, "DES_BINARY", "MAT_BF", "SEL_NN");
    callMethod(true, false, false, "DES_BINARY", "MAT_BF", "SEL_KNN");

    //* Third task, use brute force vs flann on brisk large and sift
    callMethod(true, false, false, "DES_BINARY", "", "SEL_KNN");

    callMethod(false, false, false, "", "MAT_BF", "SEL_KNN");

    callMethod(false, false, false, "", "", "SEL_KNN");
}