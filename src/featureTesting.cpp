/*
 * @file SURF_FlannMatcher
 * @brief SURF detector + descriptor + FLANN Matcher
 * @author A. Huaman
 */
#include <stdio.h>
#include <iostream>
#include <stdio.h>
#include <iostream>
#include "opencv2/core.hpp"
#include "opencv2/features2d.hpp"
#include "opencv2/imgcodecs.hpp"
#include "opencv2/highgui.hpp"
#include "opencv2/xfeatures2d.hpp"

#include <map>

using namespace std;
using namespace cv;
using namespace cv::xfeatures2d;
void readme();
float euklid_distance(KeyPoint& keypoint_1,KeyPoint& keypoint_2);
int find_max(float * histogram_x, int length); //, float& dev, float& mean);
/*
 * @function main
 * @brief Main function
 */
int main( int argc, char** argv )
{
  if( argc < 4 )
  { readme(); return -1; }
  Mat img_1 = imread( argv[1], IMREAD_GRAYSCALE );
  Mat img_2 = imread( argv[2], IMREAD_GRAYSCALE );
  if( !img_1.data || !img_2.data )
  { std::cout<< " --(!) Error reading images " << std::endl; return -1; }
  //-- Step 1: Detect the keypoints using SURF Detector, compute the descriptors
  int minHessian = 400;
  Ptr<SURF> detector = SURF::create(atoi(argv[3]));
  detector->setHessianThreshold(atoi(argv[3]));
  std::vector<KeyPoint> keypoints_1, keypoints_2;
  Mat descriptors_1, descriptors_2;
  detector->detectAndCompute( img_1, Mat(), keypoints_1, descriptors_1 );
  detector->detectAndCompute( img_2, Mat(), keypoints_2, descriptors_2 );

  //-- Step 2: Matching descriptor vectors using FLANN matcher
  FlannBasedMatcher matcher;
  std::vector< DMatch > matches;
  matcher.match( descriptors_1, descriptors_2, matches );
  double max_dist = 0; double min_dist = 100;
  //-- Quick calculation of max and min distances between keypoints
  for( int i = 0; i < descriptors_1.rows; i++ )
  { double dist = matches[i].distance;
    if( dist < min_dist ) min_dist = dist;
    if( dist > max_dist ) max_dist = dist;
  }
  printf("-- Max dist : %f \n", max_dist );
  printf("-- Min dist : %f \n", min_dist );
  //-- Draw only "good" matches (i.e. whose distance is less than 2*min_dist,
  //-- or a small arbitary value ( 0.02 ) in the event that min_dist is very
  //-- small)
  //-- PS.- radiusMatch can also be used here.
  std::vector< DMatch > good_matches;
  for( int i = 0; i < descriptors_1.rows; i++ )
  { if( matches[i].distance <= max(2*min_dist, 0.02) )
    { good_matches.push_back( matches[i]); }
  }



    // create histogram, find mean, std. dev from distances
    //std::cout << "image width " << img_1.size().width << " " << '\n';
    int int_start = 5;
    int int_end = 20;
    if( argc >= 6 ){
        int_start = atoi(argv[4]);
        int_end = atoi(argv[5]);
    }
    int length = 2*img_1.size().width;
    int shift = img_1.size().width;
    float histogram_x[length];
    for (int m = 0; m < length; ++m) {
        histogram_x[m] = 0;
    }
    float dev = 0;
    float mean = 0;
    int distances_length = good_matches.size();
    float distances_x[distances_length];
    std::cout << "distances" <<'\n';
    for (int k = 0; k < good_matches.size(); ++k) {
        distances_x[k] = keypoints_1[good_matches[k].queryIdx].pt.x - keypoints_2[good_matches[k].trainIdx].pt.x;
        //std::cout << "distance[" << k << "] = " << distances_x[k] << " " << '\n';
        std::cout << distances_x[k] << " ";
        dev+= pow(distances_x[k],2);
        mean += distances_x[k];
        int dist = (int) round(distances_x[k]) + shift;
        int increment = 1;
        for (int i = dist-int_start; i < dist; ++i) {
            if( (i < 0) || (i>length) ) continue;
            histogram_x[i] += increment;
            increment++;
        }
        for (int i = dist; i <= dist+int_end; ++i) {
            if( (i < 0) || (i>length) ) continue;
            histogram_x[i] += increment;
            if(increment > 1 ) increment--;
        }
    }
    std::cout << '\n';
    mean /= distances_length;
    dev = sqrt( (dev/distances_length) - pow(mean,2) );
    //std::cout << "dev = " << dev << '\n';
    //std::cout << "mean = " << mean << '\n';
    // print histogram
    std::cout << "histogram" <<'\n';
    for (int j = 0; j < length; ++j) {
        /*if(histogram_x[j] != 0) {
            std::cout << "histogram[" << j << "] = " << histogram_x[j] << '\n';
        }*/
        std::cout << histogram_x[j] << " ";
    }
    std::cout << '\n';

    // find max value of histogram
    int max_ind = find_max(histogram_x, length);
    //std::cout << "max value is: histogram[" << max_ind << "] = " << histogram_x[max_ind] << '\n';
    int max = max_ind - shift;
    //std::cout << "max = " << max <<  '\n';


    // find outliers
    std::vector< DMatch > correct_matches;
    std::vector< DMatch > outliers_matches;
    int outlier = dev;
    if( argc >= 7 ){
        outlier = atoi(argv[6]);
    }
    for (int n = 0; n < distances_length; n++) {
        if(abs(distances_x[n]-max) > outlier){
            outliers_matches.push_back(good_matches[n]);
        } else {
            correct_matches.push_back(good_matches[n]);
        }
    }
    std::cout << "stats" <<'\n';
    std::cout << keypoints_1.size() << " " << keypoints_2.size() << " " << max << " " << mean << " " << dev << " " << correct_matches.size() << " " << outliers_matches.size() << " " << atoi(argv[8]) << '\n';


    /*int outliers = 3;
    if( argc == 7 ){
        outliers = atoi(argv[6]);
    }
    dev = outliers*dev;
    std::cout << "dev = " << dev << '\n';
    for (int n = 0; n < distances_length; n++) {
        if( (distances_x[n] > (mean + dev) ) || (distances_x[n] < (mean - dev)) ){
            std::cout << "distances_x[" << n << "] = " << distances_x[n] << '\n';
            outliers_matches.push_back(good_matches[n]);
        } else {
            correct_matches.push_back(good_matches[n]);
        }
    }*/

    //std::cout << "good matches size = " << good_matches.size() << '\n';
    float out_per = ((float) outliers_matches.size()/ (float) good_matches.size())*100;
    float cor_per = ( (float) correct_matches.size()/ (float) good_matches.size())*100;
    //std::cout << "outliers = " << outliers_matches.size() << " => " << out_per << "%" << '\n';
    //std::cout << "correct = " << correct_matches.size() << " => " << cor_per << "%" << '\n';
    /*float distances[length];

    for (int n = 0; n < length; n++) { //good_matches.size()
        distances[n] = abs(n-max_ind); //abs( histogram_x[n] - histogram_x[max_ind] );
        if(distances[n] < dev){
            std::cout << "distances[" << n << "] = " << distances[n] << '\n';
            outliers_matches.push_back(good_matches[n]);
        } else {
            correct_matches.push_back(good_matches[n]);
        }
    }
    printf("size: %i\n", outliers_matches.size());*/

    //-- Draw only "good" matches
    Mat img_matches;
    /*drawMatches( img_1, keypoints_1, img_2, keypoints_2,
                 good_matches, img_matches, Scalar::all(-1), Scalar::all(-1),
                 vector<char>(), DrawMatchesFlags::NOT_DRAW_SINGLE_POINTS );*/
    // draw matches
    Mat img_matches_good;
    drawMatches( img_1, keypoints_1, img_2, keypoints_2,
                 correct_matches, img_matches, Scalar(255,0,0), Scalar(0,255,0),
                 vector<char>(), 0 );
    Mat img_matches_bad;
    drawMatches( img_1, keypoints_1, img_2, keypoints_2,
                 outliers_matches, img_matches,  Scalar(0,0,255), Scalar(0,0,255),
                 vector<char>(), 3 );

    if(atoi(argv[7])) {
        //-- Show detected matches
        imshow("All Matches", img_matches);
        //imshow("Correct Matches", img_matches_good);
        //imshow("Outliers Matches", img_matches_bad);
    }
   /* for( int i = 0; i < (int)good_matches.size(); i++ )
    { printf( "-- Good Match [%d] Keypoint 1: %d  -- Keypoint 2: %d  \n", i, good_matches[i].queryIdx, good_matches[i].trainIdx ); }
   */
    waitKey(0);

    return 0;
}
/*
 * @function readme
 */
void readme()
{ std::cout << " Usage: ./SURF_FlannMatcher <img1> <img2>" << std::endl; }

float euklid_distance(KeyPoint& keypoint_1,KeyPoint& keypoint_2)
{
    return sqrt( pow(keypoint_1.pt.x - keypoint_2.pt.x,2) + pow(keypoint_1.pt.x - keypoint_2.pt.x,2) );
}

int find_max(float * histogram_x, int length)
{
    int ind = -1;
    float max = 0;
    int count = 0;

    float dev = 0;
    float mean = 0;
    float sum = 0;

    // i represents pixels = distances = how the camera was moved
    for (int i = 0; i < length; ++i) {
        dev+= pow(i,2)*histogram_x[i];
        mean += i*histogram_x[i];
        sum += histogram_x[i];
        if(histogram_x[i] > max){
            max = histogram_x[i];
            ind = i;
            count = 0;
        } else if (histogram_x[i] == max){
            count++;
        }
    }

    mean /= sum;
    dev = sqrt( (dev/sum) - pow(mean,2) );

    // if there are more indexes with max, then take the center
    if(count > 0){
        ind+= (int) round(count/2);
    }

    return ind;
}
