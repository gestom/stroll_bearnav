#include <sys/time.h>
#include <stdlib.h>
#include <opencv2/opencv.hpp>
#include <opencv2/core/core.hpp>
#include <opencv2/nonfree/features2d.hpp>
#include <stdio.h>
#include <opencv2/highgui/highgui.hpp>
#include <fstream>
#include "grief/grief.h"

#define CROSSCHECK false 
#define VERTICAL_LIMIT 100
#define MAX_SEASONS 100
#define MAX_LOCATIONS 1000
#define WINDOW_SIZE 48 

char fileInfo[1000];
int numExchange = 10;
int runs = 0;
bool save=false;
bool draw=false; 
bool matchFail=false; 
using namespace std;
using namespace cv;
unsigned int n;
float distance_factor = 0.7;
int griefDescriptorLength= 512;
char dataset[1000];
char season[1000][1000]; 
int matchingTests = 0;
int matchingFailures = 0;
typedef struct{
	int id;
	int value;
}TRating;

TRating griefRating[1024];

int time0,time1,time2,time3,time4,time5,time6;

int compare(const void * a, const void * b)
{
  if ( (*(TRating*)a).value <  (*(TRating*)b).value ) return +1;
  if ( (*(TRating*)a).value >  (*(TRating*)b).value ) return -1;
  return 0;
}

int generateNew()
{
	int x1[512];
	int y1[512];
	int x2[512];
	int y2[512];
	int xWindow = WINDOW_SIZE;
	int yWindow = WINDOW_SIZE;
	FILE* file = fopen("tools/grief/test_pairs.txt","r+");
	for (int i = 0;i<512;i++){
		fscanf(file,"%i %i %i %i\n",&x1[i],&y1[i],&x2[i],&y2[i]);
	}
	fclose(file);

	file = fopen("tools/grief/pair_stats.txt","w");
	for (int i = 0;i<griefDescriptorLength;i++){
		fprintf(file,"%i %i %i %i %i\n",x1[i],y1[i],x2[i],y2[i],griefRating[i].value);
	}
	fclose(file);

	int pairs = 0;
	int sum = 0;
	qsort (griefRating,griefDescriptorLength,sizeof(TRating),compare);
	for (int i = 0;i<griefDescriptorLength;i++){
		 printf("Ratio %i %i %i %i %i %i\n",griefRating[i].id,griefRating[i].value,x1[i],y1[i],x2[i],y2[i]);
		 sum+=griefRating[i].value;
	}
	sum=sum/griefDescriptorLength;

	//exchange 10 comparisons by 10 random ones
	for (int i = griefDescriptorLength-numExchange;i<griefDescriptorLength;i++){
		int id = griefRating[i].id;
		x1[id] = rand()%xWindow-xWindow/2;
		y1[id] = rand()%yWindow-yWindow/2;
		x2[id] = rand()%xWindow-xWindow/2;
		y2[id] = rand()%yWindow-yWindow/2;
	}

	file = fopen("tools/grief/test_pairs.txt","w");
	for (int i = 0;i<512;i++){
		fprintf(file,"%i %i %i %i\n",x1[i],y1[i],x2[i],y2[i]);
	}
	//printf("%i %i %.3f\n",matchingFailures,matchingTests,(float)matchingFailures/matchingTests); 
	printf("Population fitness: %i %.3f\n",sum,(float)matchingFailures/matchingTests*100.0);
	fclose(file);
}

int getTime()
{
  struct  timeval currentTime;
  gettimeofday(&currentTime, NULL);
  return currentTime.tv_sec*1000 + currentTime.tv_usec/1000;
}

namespace cv{

	class CV_EXPORTS FakeFeatureDetector : public FeatureDetector
	{
		public:
			// bytes is a length of descriptor in bytes. It can be equal 16, 32 or 64 bytes.
			FakeFeatureDetector(){}


		protected:
			virtual void detectImpl( const Mat& image, vector<KeyPoint>& keypoints, const Mat& mask=Mat() ) const;
			//virtual void detectImpl(const Mat& image, vector<KeyPoint>& keypoints, Mat& descriptors) const;
			//typedef void(*PixelTestFn)(const Mat&, const vector<KeyPoint>&, Mat&);
	};
}

void FakeFeatureDetector::detectImpl( const Mat& image, vector<KeyPoint>& keypoints, const Mat& mask ) const
{
	FILE *file = fopen(fileInfo,"r");
	keypoints.clear();
	float a,b,c,d,x,y;
	KeyPoint kp;
	while (feof(file) == 0)
	{
		fscanf(file,"%f,%f,%f,%f\n",&a,&b,&c,&d);
		//kp.pt.x = (a+c)/2.0;
		//kp.pt.y = (b+d)/2.0;
		kp.pt.x = (a+c)/2.0;
		kp.pt.y = (b+d)/2.0;
		kp.angle = 0;
		kp.octave = 1;
		kp.response = 2;
		kp.size = 1.0/fmin((c-a)/2,(d-b)/2);
		keypoints.push_back(kp);
	}
	//fprintf(stdout,"MOTOFOKO %s %i\n",fileInfo,keypoints.size());
	fclose(file);
}

//feature matching - this can combine 'ratio' and 'cross-check' 
void distinctiveMatch(const Mat& descriptors1, const Mat& descriptors2, vector<DMatch>& matches, bool crossCheck=false)
{
	Ptr<DescriptorMatcher> descriptorMatcher;
	vector<vector<DMatch> > allMatches1to2, allMatches2to1;

	descriptorMatcher = new BFMatcher(cv::NORM_HAMMING, false);
	descriptorMatcher->knnMatch(descriptors1, descriptors2, allMatches1to2, 2);

	if (!crossCheck)
	{
		for(unsigned int i=0; i < allMatches1to2.size(); i++)
		{
			if (allMatches1to2[i].size() == 2)
			{ 
				if (allMatches1to2[i][0].distance < allMatches1to2[i][1].distance * distance_factor)
				{
					DMatch match = DMatch(allMatches1to2[i][0].queryIdx, allMatches1to2[i][0].trainIdx, allMatches1to2[i][0].distance);
					matches.push_back(match);
				}
			}         
			else if (allMatches1to2[i].size() == 1)
			{
				DMatch match = DMatch(allMatches1to2[i][0].queryIdx, allMatches1to2[i][0].trainIdx, allMatches1to2[i][0].distance);
				matches.push_back(match);
				cout << "ERROR" << endl;
			}

		}
	}
	else
	{
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

	}

}


int main(int argc, char ** argv) 
{
	srand (time(NULL));
	int detectorThreshold = 0;
	distance_factor = 1.0;
	// process command line args
	if (argc > 2 && strcmp(argv[2],"draw")==0) draw = true;
	if (argc > 2 && strcmp(argv[2],"save")==0) save = true;

	char filename[100];
	int numSeasons = 0;
	int numLocations = 0;
	int numDisplacements = 0;
	bool supervised = false;
	Mat tmpIm; 
	int x,y; 
	FILE *displacements;

	/*load dataset parameters, check dataset consistency*/
	/*check the number of seasons and check for existance of the displacement files*/
	int timer = getTime();

	do{
		sprintf(filename,"%s/season_%02i/%09i.bmp",argv[1],numSeasons,numLocations);
		tmpIm =  imread(filename, CV_LOAD_IMAGE_COLOR);
		if (tmpIm.data != NULL)
		{
			sprintf(filename,"%s/season_%02i/displacements.txt",argv[1],numSeasons);
			if (fopen(filename,"r") != NULL) numDisplacements++;
			x = tmpIm.cols;
			y = tmpIm.rows;
			numSeasons++;
		}
	}
	while (numSeasons < MAX_SEASONS && tmpIm.data != NULL);
	if (numDisplacements > 0 && numDisplacements < numSeasons){
		printf("WARNING: Dataset is annotated only partially (check if the ""displacements.txt"" files exist in every ""season_nn"" directory). Ignoring hand annotation data.\n");
	}
	supervised = (numDisplacements == numSeasons);

	/*check the number of locations*/
	do{
		sprintf(filename,"%s/season_%02i/%09i.bmp",argv[1],0,numLocations++);
		tmpIm =  imread(filename, CV_LOAD_IMAGE_COLOR);
	}while (numLocations < MAX_LOCATIONS && tmpIm.data != NULL);
	numLocations--;
	printf("Dataset: %ix%i images from %i seasons and %i places, annotated %i, loadTime %i\n",x,y,numSeasons,numLocations,numDisplacements,getTime()-timer);


	int offsetX[numSeasons*numLocations];
	int offsetY[numSeasons*numLocations];

	/*check the dataset consistency*/
	for (int i=0;i<numSeasons;i++)
	{
		for (int j=0;j<numLocations;j++)
		{
			sprintf(filename,"%s/season_%02i/%09i.bmp",argv[1],i,j);
			tmpIm =  imread(filename, CV_LOAD_IMAGE_COLOR);
			if (tmpIm.data == NULL) {
				fprintf(stderr,"ERROR: Image %s could not be loaded. \n",filename);
				exit(EXIT_FAILURE);
			}		
			if (tmpIm.cols != x || tmpIm.rows != y) {
				fprintf(stderr,"ERROR: Incosistent dataset image dimensions. Image %s is %ix%i, but we expect %ix%i.\n",filename,tmpIm.cols,tmpIm.rows,x,y);
				exit(EXIT_FAILURE);
			}
		}
		if (supervised){
			sprintf(filename,"%s/season_%02i/displacements.txt",argv[1],i);
			displacements = fopen(filename,"r");
			int aX,aY,aR;
			for (int j = 0;j<numLocations;j++){
				aR = fscanf(displacements,"%i %i\n",&aX,&aY);
				offsetX[j+i*numLocations] = aX;
				offsetY[j+i*numLocations] = aY;
				if (aR != 2){
					fprintf(stderr,"ERROR: Error reading %s file on line %i.\n",filename,j);
					exit(EXIT_FAILURE);
				}
				if (abs(aX) > x || abs(aY) > y) fprintf(stderr,"WARNING: %s file on line %i indicates displacements %i %i, but image dimensions are just %i %i.\n",filename,j,aX,aY,x,y);
			}	
			fclose(displacements);
		}
	}
	printf("Dataset consistency check OK. Time %i ms.\n",getTime()-timer);


	Mat im[numSeasons];
	Mat img[numSeasons];
	Mat global = Mat(WINDOW_SIZE,WINDOW_SIZE,CV_64F);
	global = global*0;
	Mat submat,griefw;

	//GRIEF evolution step 1: reset the comparison ratings
	vector<DMatch> matches, inliers_matches,working_matches;
	int matcher = 0;
	for (int i = 0;i<1024;i++){
		griefRating[i].value=0;
		griefRating[i].id=i;
	}

	for (int location = 0;location<numLocations;location++){
		for (int i=0;i<numSeasons;i++){
			sprintf(filename,"%s/season_%02i/%09i.bmp",argv[1],i,location);
			im[i] =  imread(filename, CV_LOAD_IMAGE_COLOR);
			img[i] = imread(filename, CV_LOAD_IMAGE_GRAYSCALE);
			if(img[i].empty())
			{
				printf("Can't read image %s\n",filename);
				return -1;
			}
		}

		// detecting keypoints and generating descriptors
		Mat descriptors[numSeasons];
		vector<KeyPoint> keypoints[numSeasons];
		KeyPoint kp;
		Mat dp;

		cout << "Detecting STAR features and extracting genetically modified BRIEF descriptors"  << endl;
		//StarFeatureDetector detector(45,detectorThreshold,10,8,5);		//TODO make this selectable
		SURF  detector(0);
		//FakeFeatureDetector detector;		//TODO make this selectable
		//BRISK detector(0,4);
		GriefDescriptorExtractor extractor(griefDescriptorLength/8);

		time0 = getTime();
		for (int i = 0;i<numSeasons;i++){
			sprintf(fileInfo,"%s/season_%02i/spgrid_regions_%09i.txt",argv[1],i,location);
			detector.detect(img[i], keypoints[i]);
			for (unsigned int j = 0;j<keypoints[i].size();j++) keypoints[i][j].angle = -1;
			extractor.compute(img[i], keypoints[i], descriptors[i]);
			printf("Location %i season %i, extracted %i\n",location,i,(int)keypoints[i].size());
		}
		time2 = time1 = getTime();


		// matching the extracted features
		for (int ik = 0;ik<numSeasons;ik++){
			for (int jk = ik+1;jk<numSeasons;jk++){
				time3 = getTime();
				matches.clear();
				inliers_matches.clear();
				/*if not empty*/
				if (descriptors[ik].rows*descriptors[jk].rows > 0) distinctiveMatch(descriptors[ik], descriptors[jk], matches, CROSSCHECK);
				time4 = getTime();

				/*are there any tentative correspondences ?*/
				int sumDev = 0;
				int numPoints = 0;

				int histMax = 0;
				int auxMax=0;
				int manualDir = 0; 
				int histDir = 0;
				int numBins = 100; 
				int granularity = 20;
				int maxS = 0;
				int domDir = 0;
				vector<unsigned char> mask;
				if (matches.size() > 0){
					//histogram assembly
					int histogram[numBins];
					int bestHistogram[numBins];
					memset(bestHistogram,0,sizeof(int)*numBins);
					for (int s = 0;s<granularity;s++){
						memset(histogram,0,sizeof(int)*numBins);
						for( size_t i = 0; i < matches.size(); i++ )
						{
							int i1 = matches[i].queryIdx;
							int i2 = matches[i].trainIdx;
							if ((fabs(keypoints[ik][i1].pt.y-keypoints[jk][i2].pt.y))<VERTICAL_LIMIT){
								int devx = (int)(keypoints[ik][i1].pt.x-keypoints[jk][i2].pt.x + numBins/2*granularity);
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


					for (int i =0;i<numBins;i++){
						if (auxMax < bestHistogram[i] && bestHistogram[i] != histMax){
							auxMax = bestHistogram[i];
						}
					}

					/*calculate dominant direction*/
					for( size_t i = 0; i < matches.size(); i++ )
					{
						int i1 = matches[i].queryIdx;
						int i2 = matches[i].trainIdx;
						if ((int)((keypoints[ik][i1].pt.x-keypoints[jk][i2].pt.x+numBins/2*granularity+maxS)/granularity)==domDir && fabs(keypoints[ik][i1].pt.y-keypoints[jk][i2].pt.y)<VERTICAL_LIMIT)
						{
							sumDev += keypoints[ik][i1].pt.x-keypoints[jk][i2].pt.x;
							numPoints++;
						}
					}
					histDir = (sumDev/numPoints);
					manualDir = offsetX[location+ik*numLocations] - offsetX[location+jk*numLocations];
					if (fabs(manualDir - histDir) > 35) matchFail = true; else matchFail = false;
					float realDir = histDir;
					int strength = 1;
					//if (matchFail) strength = 100;
					if (matchFail && supervised) realDir = manualDir;

					/*rate individual comparisons*/
					for( size_t i = 0; i < matches.size(); i++ ){
						char eff = 0;
						int i1 = matches[i].queryIdx;
						int i2 = matches[i].trainIdx;
						if ((abs(keypoints[ik][i1].pt.x-keypoints[jk][i2].pt.x-realDir)< 35 ) && fabs(keypoints[ik][i1].pt.y-keypoints[jk][i2].pt.y)<VERTICAL_LIMIT)
						{
							inliers_matches.push_back(matches[i]);
							eff = -strength;
						}else{
							eff = +strength;
						}
						for (int o = 0;o<griefDescriptorLength/8;o++){
							unsigned char b = descriptors[ik].at<uchar>(i1,o)^descriptors[jk].at<uchar>(i2,o);
							unsigned char oo = 128;
							for (int p = 0;p<8;p++){
								if (oo&b)  griefRating[8*o+p].value+=eff; else griefRating[8*o+p].value-=eff;
								oo=oo/2;
							}
						}
					}
					//if (histMax > 0) printf("\nDirection histogram %i %i %i\n",-(sumDev/histMax),histMax,auxMax); else printf("\nDirection histogram 1000 0 0\n");
					if (histMax > 0 || supervised) printf("%04i %02i%02i %i %i %i %i\n",location,ik+1,jk+1,histDir,manualDir,histMax,auxMax); else printf("%04i %02i%02i 1000 -1000 0 0\n",location,ik+1,jk+1);
				}else{
					printf("%04i %02i%02i 1000 -1000 0 0\n",location,ik+1,jk+1);
					matchFail = true;
				}

				/*double minVal; 
				double maxVal; 
				Point minLoc; 
				Point maxLoc;
				minMaxLoc( global, &minVal, &maxVal, &minLoc, &maxLoc );
				global = (global-minVal)/(maxVal-minVal)*255;
				global.convertTo(submat, CV_8U);
				imwrite("heh.bmp",submat);*/

				if (matchFail) matchingFailures++;
				matchingTests++;	
				//for drawing the results
				if (draw&&matchFail)
				{
					Mat imA,imB,img_matches,img_matches_transposed;
					vector<KeyPoint> kpA,kpB;
					KeyPoint kp;
					kpA.clear();
					kpB.clear();
					for (int s=0;s<keypoints[ik].size();s++){
						kp = keypoints[ik][s];
						kp.pt.x = keypoints[ik][s].pt.y;
						kp.pt.y = keypoints[ik][s].pt.x;
						kpA.push_back(kp);
					}
					for (int s=0;s<keypoints[jk].size();s++){
						kp = keypoints[jk][s];
						kp.pt.x = keypoints[jk][s].pt.y;
						kp.pt.y = keypoints[jk][s].pt.x;
						kpB.push_back(kp);
					}
					cv::transpose(im[ik], imA);
					cv::transpose(im[jk], imB);
					namedWindow("matches", 1);
					drawMatches(imA, kpA, imB, kpB, inliers_matches, img_matches, Scalar(0, 0, 255), Scalar(0, 0, 255), vector<char>(), 2);
					cv::transpose(img_matches,img_matches_transposed);
					imshow("matches", img_matches_transposed);
					waitKey(0);
				}
				//end drawing
			}
		}
	}
	generateNew();
	return 0;
}
