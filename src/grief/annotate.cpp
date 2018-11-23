//this tool can be used for automatic annotation of the datasets - it's undocumented
//however, use only as a reference (!), otherwise it might create a strong bias towards the method which is used to register the images in this annotation tool
//always make sure that you do the annotation manually (!)
//the current code was tailored to annotate sequences of the Nordland dataset 

#include <sys/time.h>
#include <stdlib.h>
#include <opencv2/opencv.hpp>
#include <opencv2/core/core.hpp>
#include <opencv2/features2d/features2d.hpp>
#include <opencv2/nonfree/features2d.hpp>
#include <stdio.h>
#include <opencv2/highgui/highgui.hpp>
#include <fstream>
#include "grief/grief.h"
#include <dirent.h>

#define VERTICAL_LIMIT 100 
float distance_factor = 1.0;

using namespace std;
using namespace cv;

int offsetX,offsetY;
int displayStyle = 2;
int key = 0;

const int granularity = 20;
const int width = 500;
const int height = 100;
int difference = 0;
FILE *output = NULL;		

int seasons = 0;
char season[1000][1000]; 
char dataset[1000];
int numLocations= 0;
int autoBestMatch= 0;
int autoLocation = 0;

FeatureDetector     *detector = NULL;
DescriptorExtractor *descriptor = NULL;

/*matching scheme*/
void distinctiveMatch(const Mat& descriptors1, const Mat& descriptors2, vector<DMatch>& matches)
{
	DescriptorMatcher *descriptorMatcher;
	vector<vector<DMatch> > allMatches1to2, allMatches2to1;

	descriptorMatcher = new BFMatcher(cv::NORM_HAMMING, false);
	descriptorMatcher->knnMatch(descriptors1, descriptors2, allMatches1to2, 2);
	descriptorMatcher->knnMatch(descriptors2, descriptors1, allMatches2to1, 2);
	for(unsigned int i=0; i < allMatches1to2.size(); i++)
	{
		if (allMatches1to2[i].size() == 2)
		{ 
			if (allMatches2to1[allMatches1to2[i][0].trainIdx].size() == 2)
			{
				if (allMatches1to2[i][0].distance < allMatches1to2[i][1].distance * distance_factor && allMatches2to1[allMatches1to2[i][0].trainIdx][0].distance < allMatches2to1[allMatches1to2[i][0].trainIdx][1].distance * distance_factor && allMatches1to2[i][0].trainIdx == allMatches2to1[allMatches1to2[i][0].trainIdx][0].queryIdx)
				{
					DMatch match = DMatch(allMatches1to2[i][0].queryIdx, allMatches1to2[i][0].trainIdx, allMatches1to2[i][0].distance);
					matches.push_back(match);
				}
			}
			else if (allMatches2to1[allMatches1to2[i][0].trainIdx].size() == 1)
				if (allMatches1to2[i][0].distance  < allMatches1to2[i][1].distance * distance_factor && allMatches1to2[i][0].trainIdx == allMatches2to1[allMatches1to2[i][0].trainIdx][0].queryIdx)
				{
					DMatch match = DMatch(allMatches1to2[i][0].queryIdx, allMatches1to2[i][0].trainIdx, allMatches1to2[i][0].distance);
					matches.push_back(match);
					cout << "ERROR" << endl;
				} 
		}
		else if (allMatches2to1[allMatches1to2[i][0].trainIdx].size() == 2)
		{
			if (allMatches2to1[allMatches1to2[i][0].trainIdx][0].distance < allMatches2to1[allMatches1to2[i][0].trainIdx][1].distance * distance_factor && allMatches1to2[i][0].trainIdx == allMatches2to1[allMatches1to2[i][0].trainIdx][0].queryIdx)
			{
				DMatch match = DMatch(allMatches1to2[i][0].queryIdx, allMatches1to2[i][0].trainIdx, allMatches1to2[i][0].distance);
				matches.push_back(match);
				cout << "ERROR" << endl;
			}
		}
		else if (allMatches1to2[i][0].trainIdx == allMatches2to1[allMatches1to2[i][0].trainIdx][0].queryIdx)
		{
			DMatch match = DMatch(allMatches1to2[i][0].queryIdx, allMatches1to2[i][0].trainIdx, allMatches1to2[i][0].distance);
			matches.push_back(match);
			cout << "ERROR" << endl;
		} 

	}
	delete descriptorMatcher;
}

/*initialize the dataset parameters*/
int initializeDateset()
{
	DIR *dpdf;
	struct dirent *epdf;
	FILE* file; 
	int dum1,dum2,dum3,dum4; //for reading from a file
	char filename[1000];

	/*how many seasons are in the dataset?*/
	dpdf = opendir(dataset);
	if (dpdf != NULL)
	{
		while (epdf = readdir(dpdf))
		{
			if (strncmp(epdf->d_name,"season_",7)==0 && epdf->d_type == 4)
			{
				printf("Season directory %i %s \n",seasons,epdf->d_name);
				strcpy(season[seasons++],epdf->d_name);
			}
		}
	}else{
		fprintf(stderr,"Dataset directory not found\n");
		return -1;
	}
	for (int i=0;i<seasons;i++)sprintf(season[i],"season_%02i",i);
	for (int i=0;i<seasons;i++)printf("Season ordered %i %s \n",i,season[i]);
	/*are there at least two?*/
	if (seasons < 2)
	{
		fprintf(stderr,"At least two directories with the prefix season_ have to be present in the datasets directory\n");
		return -1;	
	}
	
	/*check the files*/
	dum4 = 0;
	printf("Dataset seems to be OK: %i seasons and %i locations\n",seasons,numLocations);
}

int main(int argc, char ** argv) 
{
	srand (time(NULL));
	strcpy(dataset,argv[1]);
	initializeDateset();
	distance_factor = 1.0;
	int totalTests = 0;
	int numPictures = 0;
	int locations[seasons];
	int key = 0;
	//locations[0] = 18737; 
	//locations[1] = 18590;
	for (int i = 0;i<10;i++) locations[i] = i;
	int locationScan = 0;
	do{
		delete detector;
		delete descriptor;
		char filename[100];
		Mat im[seasons];
		Mat img[seasons];
		Mat descriptors[seasons];
		vector<KeyPoint> keypoints[seasons];
		for (int s = 0;s<seasons;s++)
		{
			sprintf(filename,"%s/%s/%09i.bmp",dataset,season[s],locations[s]);
			printf("%s/%s/%09i.bmp",dataset,season[s],locations[s]);
			im[s] =  imread(filename, CV_LOAD_IMAGE_COLOR);
			img[s] = imread(filename, CV_LOAD_IMAGE_GRAYSCALE);
			if (img[s].empty())
			{
				printf("Can't read image %s\n",filename);
				return -1;
			}
		}
		printf("\n");

		detector = new StarFeatureDetector(45,0,10,8,5);
		descriptor = new GriefDescriptorExtractor(32);
		KeyPoint kp;
		Mat dp;

		for (int s = 0;s<seasons;s++)
		{
			detector->detect(img[s], keypoints[s]);
			descriptor->compute(img[s],keypoints[s],descriptors[s]);
		}

		int estimated = 0;	
		int groundTruth = 0;
		for (int b=1;b<seasons;b++){

			Mat descriptors1,descriptors2;
			vector<KeyPoint> keypoints1,keypoints2;
			descriptors1 = descriptors[0];
			descriptors2 = descriptors[b];
			keypoints1 = keypoints[0];	
			keypoints2 = keypoints[b];	

			vector<DMatch> matches, inliers_matches;
			int sumDev,auxMax,histMax;
			sumDev = auxMax = histMax = 0;

			// matching descriptors
			matches.clear();
			inliers_matches.clear();
			if (descriptors1.rows*descriptors2.rows > 0) distinctiveMatch(descriptors1, descriptors2, matches);

			if (matches.size() > 0){
				//histogram assembly
				int numBins = 100; 
				int histogram[numBins];
				int bestHistogram[numBins];
				memset(bestHistogram,0,sizeof(int)*numBins);
				histMax = 0;
				int maxS,domDir;
				for (int s = 0;s<granularity;s++){
					memset(histogram,0,sizeof(int)*numBins);
					for( size_t i = 0; i < matches.size(); i++ )
					{
						int i1 = matches[i].queryIdx;
						int i2 = matches[i].trainIdx;
						if ((fabs(keypoints1[i1].pt.y-keypoints2[i2].pt.y))<VERTICAL_LIMIT){
							int devx = (int)(keypoints1[i1].pt.x-keypoints2[i2].pt.x + numBins/2*granularity);
							int index = (devx+s)/granularity;
							if (index > -1 && index < numBins) histogram[index]++;
						}
					}
					for (int i = 0;i<numBins;i++){
						if (histMax < histogram[i]){
							histMax = histogram[i];
							maxS = s;
							domDir = i;
							memcpy(bestHistogram,histogram,sizeof(int)*numBins);
						}
					}
				}

				auxMax=0;
				for (int i =0;i<numBins;i++){
					if (auxMax < bestHistogram[i] && bestHistogram[i] != histMax){
						auxMax = bestHistogram[i];
					}
				}

				sumDev = 0;
				for( size_t i = 0; i < matches.size(); i++ ){
					int i1 = matches[i].queryIdx;
					int i2 = matches[i].trainIdx;

					if ((int)((keypoints1[i1].pt.x-keypoints2[i2].pt.x + numBins/2*granularity+maxS)/granularity) == domDir && fabs(keypoints1[i1].pt.y-keypoints2[i2].pt.y)<VERTICAL_LIMIT)
					{
						sumDev += keypoints1[i1].pt.x-keypoints2[i2].pt.x;
						inliers_matches.push_back(matches[i]);
					}
				}
				estimated = (sumDev/histMax);
			}else{
				difference = 1000;
				estimated = 0;
			}
			offsetX = estimated;
			offsetY = 0;
			do{
				//printf("DIFF: %i %i\n",(sumDev/histMax),-(offsetX[ims+numLocations*a]-offsetX[ims+numLocations*b]));
				Mat imA,imB,imC,img_matches,img_matches_transposed;
				vector<KeyPoint> kpA,kpB;
				KeyPoint kp;
				kpA.clear();
				kpB.clear();
				for (int s=0;s<keypoints1.size();s++){
					kp = keypoints1[s];
					kp.pt.x = keypoints1[s].pt.y;
					kp.pt.y = keypoints1[s].pt.x;
					kpA.push_back(kp);
				}
				for (int s=0;s<keypoints2.size();s++){
					kp = keypoints2[s];
					kp.pt.x = keypoints2[s].pt.y+offsetY;
					kp.pt.y = keypoints2[s].pt.x+offsetX;
					kpB.push_back(kp);
				}
				cv::transpose(im[0], imA);
				cv::transpose(im[b], imB);
				cv::transpose(im[b], imC);
				if (offsetX != 0){
					if (offsetX > 0){
						imC(cv::Rect(0, 0, img->rows, img->cols-offsetX)).copyTo(imB(cv::Rect(0,offsetX,img->rows,img->cols-offsetX)));
						imC(cv::Rect(0, img->cols-offsetX, img->rows, offsetX)).copyTo(imB(cv::Rect(0,0,img->rows,offsetX)));
					}else{
						imC(cv::Rect(0, -offsetX, img->rows, img->cols+offsetX)).copyTo(imB(cv::Rect(0,0,img->rows,img->cols+offsetX)));
						imC(cv::Rect(0, 0, img->rows, -offsetX)).copyTo(imB(Rect(0,img->cols+offsetX,img->rows,-offsetX)));
					}
				}
				if (displayStyle == 1) imA = imA/2+imB/2;
				namedWindow("matches", 1);
				if (kpA.size() >0 && kpB.size()>0 && inliers_matches.size() >0 && displayStyle == 2){
					drawMatches(imA, kpA, imB, kpB, inliers_matches, img_matches, Scalar(0, 0, 255), Scalar(0, 0, 255), vector<char>(), 0);
				}else{
					kpA.clear();
					kpB.clear();
					matches.clear();
					drawMatches(imA, kpA, imB, kpB, matches, img_matches, Scalar(0, 0, 255), Scalar(0, 0, 255), vector<char>(), 0);
				}
				cv::transpose(img_matches,img_matches_transposed);
				imshow("matches", img_matches_transposed);
				printf("Location %03i vs location %03i. Proposed offset: %i Manual offset: %i. Scan: %i Features: %i Pressed key: %i \n",locations[0],locations[1],offsetX,offsetX-estimated,locationScan,histMax,key);
				if (locationScan <= 0){
				       	key = waitKey(1)%256;
				        //key = 'a';	
				}else {
					key = waitKey(1);
					key = 0;
					if (histMax > autoBestMatch){
						autoBestMatch = histMax;
						autoLocation = locations[1];
					}
					locationScan--;
					if (locationScan < 2)
					{
						locations[1] = autoLocation;	
					}else{
						locations[1]++;
					}
				}
				if (key == 83) offsetX++;
				if (key == 81) offsetX--;
				if (key == 82) locations[1]++;
				if (key == 84) locations[1]--;
				if (key == '1')
				{
					locations[0]++;
					locations[1]++;
				}
				if (key == '2')
				{
					locations[0]+=10;
					locations[1]+=10;
				}
				if (key == '3')
				{
					locations[0]+=50;
					locations[1]+=50;
				}
				if (key == 'a'){
					locationScan = 50;
					autoBestMatch = 0;
				}
				if (key == 32) displayStyle=(displayStyle+1)%3;
			}while (key !=27 && key != 10 && key != 82  && key != 84 && key != '1' && key != '2' && key != '3' && locationScan == 0);
			totalTests++;
			if (key == 10 || key == 'a')
			{
				printf("Saved %03i vs %03i -  %i %i \n",locations[0],locations[1],offsetX,offsetY);
				char retez[1000];
				sprintf(retez,"%s/%s/annotation.txt",dataset,season[b]);
				output = fopen(retez,"a");	
				fprintf(output,"%03i %03i %i %i \n",locations[0],locations[1],offsetX,offsetY);
				fclose(output);
				locations[0]+=1;//50;
				locations[1]+=1;// 25;
				//locations[0]++;
				//locations[1]++;
			}
		}
	}while (key != 27);
	return 0;
}
