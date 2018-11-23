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


#define CROSSCHECK false
#define VERTICAL_LIMIT 100
bool norm2 =true;
bool upright =true;

bool update=false;
bool save=false;
bool draw=false; 
bool drawAll=false; 
bool normalizeSift=false; 
using namespace std;
using namespace cv;
unsigned int n;
float distance_factor = 0.7;
const int granularity = 20;
const int width = 500;
const int height = 100;
int histogram2D[width*2+granularity][height*2+granularity];
bool hist2D = false;
int difference = 0;
FILE *output = NULL;
int maxFeatures=1600;
int minFeatures=99;
int numFeatures=maxFeatures;

int numFails[1600/100+1];
int numFeats[1600/100+1];
char season[1000][1000]; 
int seasons = 0;
char dataset[1000];
char descriptorName[1000];
char detectorName[1000];
char fileInfo[1000];

FeatureDetector     *detector = NULL;
DescriptorExtractor *descriptor = NULL;

/*benchmarking time*/
float timeDetection = 0;
float timeDescription = 0;
float timeMatching = 0;
int totalExtracted = 0;
int totalMatched = 0;

//reads the detections from a file- in our case, used to read Super-pixel grid bounding boxes
namespace cv{

	class CV_EXPORTS FakeFeatureDetector : public FeatureDetector
	{
		public:
			FakeFeatureDetector(){}
		protected:
			virtual void detectImpl( const Mat& image, vector<KeyPoint>& keypoints, const Mat& mask=Mat() ) const;
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
		kp.pt.x = (a+c)/2.0;
		kp.pt.y = (b+d)/2.0;
		kp.angle = -1;
		kp.octave = 1;
		kp.response = 1;
		kp.size = sqrt((d-b)*(c-a));
		keypoints.push_back(kp);
	}
	fclose(file);
}

/*for benchmarking purposes*/
int getElapsedTime()
{
  static int lastTime;
  struct  timeval currentTime;
  gettimeofday(&currentTime, NULL);
  int timeNow = currentTime.tv_sec*1000 + currentTime.tv_usec/1000;
  int result = timeNow - lastTime;
  lastTime = timeNow; 
  return result; 
}

/*to select most responsive features*/
bool compare_response(KeyPoint first, KeyPoint second)
{
	  if (first.response > second.response) return true; else return false;
}

/*matching scheme*/
void distinctiveMatch(const Mat& descriptors1, const Mat& descriptors2, vector<DMatch>& matches, bool norm2= true, bool crossCheck=false)
{
   DescriptorMatcher *descriptorMatcher;
   vector<vector<DMatch> > allMatches1to2, allMatches2to1;

   if (norm2)
      descriptorMatcher = new BFMatcher(cv::NORM_L2,  false);
   else 
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
   delete descriptorMatcher;
}

int *seq1;
int *seq2;
int *offsetX;
int *offsetY;
int numLocations = 0;

void rootSift(Mat *m)
{
	for (int r = 0;r<m->rows;r++)
	{
		float n1 = 0;	
		for (int c = 0;c<m->cols;c++) n1+=fabs(m->at<float>(r,c));
		for (int c = 0;c<m->cols;c++) m->at<float>(r,c)=sqrt(fabs(m->at<float>(r,c)/n1));
	}
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
		while ((epdf = readdir(dpdf)))
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
	for (int i = 0;i<seasons;i++){
		sprintf(filename,"%s/%s/displacements.txt",dataset,season[i]);
		file = fopen(filename,"r");
		if (file==NULL)
		{
			fprintf(stderr,"File %s not found - look at the examples on how to fill the datasets.\n",filename);
			return -1;
		}
		dum3 = 0;
		while (feof(file)==0)
		{
			if (fscanf(file,"%i %i\n",&dum1,&dum2)!=2)
			{
				fprintf(stderr,"File %s corrupt at line %i!\n",filename,dum3);
				return -1;
			}
			dum3++;
		}
		if (i==0) dum4 = dum3;
		if (dum3 != dum4)
		{
				fprintf(stderr,"Files from %s and %s dataset do not have the same number of lines %i %i!\n",season[0],season[1],dum4,dum3);
				return -1;
		}
		fclose(file);
	}
	numLocations = dum4;
	printf("Dataset seems to be OK: %i seasons and %i locations\n",seasons,numLocations);
	/*allocate variables*/
	offsetX = (int*)malloc(sizeof(int)*numLocations*seasons);
	offsetY = (int*)malloc(sizeof(int)*numLocations*seasons);

	/*read offsets*/
	for (int i = 0;i<seasons;i++){
		sprintf(filename,"%s/%s/displacements.txt",dataset,season[i]);
		file = fopen(filename,"r");
		dum3 = 0;
		while (feof(file) == 0)
		{
			dum4 = fscanf(file,"%i\t%i\n",&dum1,&dum2);
			offsetX[i*numLocations+dum3] = -dum1;
			offsetY[i*numLocations+dum3] = -dum2;
			dum3++;
		}
		fclose(file);
	}
	return 0;
}

/*initialize detector*/
void initializeDetector(char *nameI)
{
	char name[100];
	strcpy(name,nameI);
	/*modifiers*/
	if (strncmp("up-",name,3)==0)	{upright = true;strcpy(name,&nameI[3]);}
	if (strncmp("norm-",name,5)==0)	{upright = false;strcpy(name,&nameI[5]);}

	/*detectors*/
	if (strcmp("sift",  name)==0)  	detector = new SIFT(0,3,0.0,10,1.6);
	if (strcmp("surf",  name)==0)  	detector = new SURF(0);
	if (strcmp("star",  name)==0)  	detector = new StarFeatureDetector(45,0,10,8,5);
	if (strcmp("brisk", name)==0) 	detector = new BRISK(0,4);
	if (strcmp("orb",   name)==0) 	detector = new OrbFeatureDetector(maxFeatures,1.2f,8,31,0,2,0,31); 
	if (strcmp("fast",  name)==0)	detector = new FastFeatureDetector(0,true);

	/*new ones*/
	if (strcmp("mser",  name)==0)	detector = new MSER(2);
	if (strcmp("gftt",  name)==0)	detector = new GFTTDetector(1600,0.01,1,3,false,0.04);
	if (strcmp("fake",  name)==0)	detector = new FakeFeatureDetector();
}

/*initialize detector*/
void initializeDescriptor(char *nameI)
{
	char name[100];
	strcpy(name,nameI);
	/*modifiers*/
	if (strncmp("root-",name,5)==0){normalizeSift= true;strcpy(name,&nameI[5]);}
	/*descriptors*/
	if (strcmp("sift",  name)==0)  	{norm2=true;descriptor = new SIFT(0,3,0.0,10,1.6);}
	if (strcmp("surf",  name)==0)   {norm2=true;descriptor = new SURF(0);}
	if (strcmp("brisk", name)==0)   {norm2=false;descriptor = new BRISK(0,4);}
	if (strcmp("brief", name)==0)   {norm2=false;descriptor = new BriefDescriptorExtractor(64);}
	if (strcmp("grief", name)==0)   {norm2=false;descriptor = new GriefDescriptorExtractor(64);}
	if (strcmp("orb",   name)==0)   {norm2=false;descriptor = new OrbFeatureDetector(maxFeatures,1.2f,8,31,0,2,0,31);} 
	if (strcmp("freak",  name)==0)	{norm2=false;descriptor = new FREAK();}
}

int main(int argc, char ** argv) 
{
	srand (time(NULL));
	strcpy(dataset,argv[3]);
	strcpy(descriptorName,argv[2]);
	strcpy(detectorName,argv[1]);
	initializeDateset();
	if (argc > 4 && strcmp(argv[4],"draw")==0) drawAll = true;
	if (argc > 4 && strcmp(argv[4],"save")==0) save = true;
	if (argc > 4 && strcmp(argv[4],"update")==0) update = true;
	if (update) output = fopen("newcon.txt","w+");
	distance_factor = 1.0;
	memset(numFails,0,(maxFeatures/100+1)*sizeof(int));	
	memset(numFeats,0,(maxFeatures/100+1)*sizeof(int));	
	int totalTests = 0;
	int numPictures = 0;
	for (int ims=0;ims<numLocations;ims++)
	{
		delete detector;
		delete descriptor;
		char filename[100];
		Mat im[seasons];
		Mat img[seasons];
		Mat descriptors[seasons];
		vector<KeyPoint> keypoints[seasons];
		for (int s = 0;s<seasons;s++)
		{
			sprintf(filename,"%s/%s/%09i.bmp",dataset,season[s],ims);
			im[s] =  imread(filename, CV_LOAD_IMAGE_COLOR);
			img[s] = imread(filename, CV_LOAD_IMAGE_GRAYSCALE);
			if (img[s].empty())
			{
				printf("Can't read image %s\n",filename);
				return -1;
			}
		}
		initializeDetector(detectorName);
		initializeDescriptor(descriptorName);
		KeyPoint kp;
		Mat dp;

		for (int s = 0;s<seasons;s++)
		{
			/*detection*/
			getElapsedTime();
			sprintf(fileInfo,"%s/%s/spgrid_regions_%09i_740.txt",dataset,season[s],ims);
			detector->detect(img[s], keypoints[s]);
			
			timeDetection += getElapsedTime();

			sort(keypoints[s].begin(),keypoints[s].end(),compare_response);
			/*extraction*/
			getElapsedTime();
			if (upright) for (unsigned int j = 0;j<keypoints[s].size();j++) keypoints[s][j].angle = -1;

			/*exceptional case: SIFT/ORB does not work well because the sift does not provide an octave with a feature*/
			if (strstr(detectorName,"sift")!=NULL && strcmp(descriptorName,"orb")==0)
			{
				/*providing a fake octave*/
				for (unsigned int j = 0;j<keypoints[s].size();j++) keypoints[s][j].octave = 1;
			}

			descriptor->compute(img[s],keypoints[s],descriptors[s]);
			if (normalizeSift) rootSift(&descriptors[s]);	
			timeDescription += getElapsedTime();
			totalExtracted += descriptors[s].rows;
			numPictures++;
		}

		for (int nFeatures=maxFeatures;nFeatures>=minFeatures;nFeatures-=100)
		{
			for (int a=0;a<seasons;a++){
				for (int b=a+1;b<seasons;b++){

					Mat descriptors1,descriptors2;
					vector<KeyPoint> keypoints1,keypoints2;
					descriptors1 = descriptors[a];
					descriptors2 = descriptors[b];
					keypoints1 = keypoints[a];	
					keypoints2 = keypoints[b];	
					numFeatures = nFeatures;

					//use all features when numFeatures is 0 
					if (numFeatures > 0){
						int numRemove = max(0,descriptors1.rows-numFeatures); 
						descriptors1.pop_back(numRemove);
						keypoints1.erase(keypoints1.begin()+descriptors1.rows,keypoints1.end());	
						numRemove = max(0,descriptors2.rows-numFeatures); 
						descriptors2.pop_back(numRemove);
						keypoints2.erase(keypoints2.begin()+descriptors2.rows,keypoints2.end());	
					}
					vector<DMatch> matches, inliers_matches;
					int sumDev,auxMax,histMax;
					sumDev = auxMax = histMax = 0;
					numFeats[numFeatures/100]+=(descriptors1.rows+descriptors2.rows)/2;

					// matching descriptors
					matches.clear();
					inliers_matches.clear();
					getElapsedTime();
					if (descriptors1.rows*descriptors2.rows > 0) distinctiveMatch(descriptors1, descriptors2, matches, norm2, CROSSCHECK);

					//benchmark unrestricted detector sets only 
					timeMatching += getElapsedTime();
					totalMatched += descriptors1.rows*descriptors2.rows;

					if (matches.size() > 0){

						//histogram assembly

						if (hist2D){
							int numBins = 100; 
							int iX = 0;
							int iY = 0;

							for(int i = 0; i < width*2+granularity; i++ )
							{
								for(int j = 0; j < height*2+granularity; j++ ) histogram2D[i][j]=0;
							}

							//create the histogram
							for(size_t i = 0; i < matches.size(); i++ )
							{
								int i1 = matches[i].queryIdx;
								int i2 = matches[i].trainIdx;
								int iXO = ((int)(keypoints1[i1].pt.x-keypoints2[i2].pt.x + numBins/2*granularity)/granularity);
								int iYO = ((int)(keypoints1[i1].pt.y-keypoints2[i2].pt.y + numBins/2*granularity)/granularity);
								for(int ii = 0; ii < 1+0*granularity; ii++ ){
									for(int ij = 0; ij < 1+0*granularity; ij++ ){
										iX = iXO+ii;
										iY = iYO+ij;
										if (iX > -1 && iX < width*2 && iY > -1 && iY < height*2) histogram2D[iX][iY]++;
									}
								}
							}

							//find histogram maximum
							histMax = 0;
							for (int i = 0;i<width*2;i++){
								for (int j = 0;j<height*2;j++){
									if (histMax < histogram2D[i][j]){
										histMax = histogram2D[i][j];
										iX = i;
										iY = i;
									}
								}
							}
							printf("Max: %i %i %i\n",histMax,iX,iY);
							sumDev = iX-width-granularity/2;
							sumDev = sumDev*histMax;
							auxMax = 0;

							for( size_t i = 0; i < matches.size(); i++ ){
								int i1 = matches[i].queryIdx;
								int i2 = matches[i].trainIdx;

								if ((int)((keypoints1[i1].pt.x-keypoints2[i2].pt.x + numBins/2*granularity)/granularity) == iX && (int)((keypoints1[i1].pt.y-keypoints2[i2].pt.y + numBins/2*granularity)/granularity) == iY )
								{
									sumDev += keypoints1[i1].pt.x-keypoints2[i2].pt.x;
									inliers_matches.push_back(matches[i]);
								}
							}

						}else{
							//histogram assembly
							int numBins = 100; 
							int histogram[numBins];
							int bestHistogram[numBins];
							memset(bestHistogram,0,sizeof(int)*numBins);
							histMax = 0;
							int maxS,domDir;
							maxS = domDir = 0;
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
							//if (histMax > 0) printf("\nDirection histogram %i %i %i\n",-(sumDev/histMax),histMax,auxMax); else printf("\nDirection histogram 1000 0 0\n");

						}
						if (histMax > 0) difference = (sumDev/histMax)+(offsetX[ims+numLocations*a]-offsetX[ims+numLocations*b]); else difference = 1000;
						//if (histMax > 0) printf("\nDirection histogram %i %i %i\n",-(sumDev/histMax),histMax,auxMax); else printf("\nDirection histogram 1000 0 0\n");
						//if (histMax > 0) printf("%05i %05i %i %i %i %i %i\n",imNum1,imNum2,difference,-(sumDev/histMax),-offsetX[ims],histMax,auxMax); else printf("%05i %05i 1000 1000 %i 0 0\n",imNum1,imNum2,offsetX[ims]);
						if (drawAll==false && update) draw = (abs(difference) > 35); else draw = drawAll;
						//if (update && draw == false) fprintf(output,"Offset %i %i %i %i\n",imNum1,imNum2,offsetX[ims],offsetY[ims]);
						//if (histMax > 0) fprintf(output,"%05i %05i %i %i %i %i %i\n",imNum1,imNum2,-(sumDev/histMax)+offset[ims],-(sumDev/histMax),-offset[ims],histMax,auxMax); else fprintf(output,"%05i %05i 1000 1000 %i 0 0\n",imNum1,imNum2,offset[ims]);
					}else{
						difference = 1000;
						printf("%05i %05i 1000 1000 %i 0 0\n",ims,ims,offsetX[ims]);
						draw = update;
					}
					if (fabs(difference) > 35) numFails[numFeatures/100]++;
					printf("Report: %s %s %09i : %i : %i : %.f - ",season[a],season[b],totalTests,numFails[numFeatures/100],fabs(difference) > 35,fabs(difference));
					if (drawAll || save || (draw&&(fabs(difference) > 35)))
					{
						if (fabs(difference) > 35) printf("DIFF: %i %i\n",(sumDev/histMax),-(offsetX[ims+numLocations*a]-offsetX[ims+numLocations*b]));
						Mat imA,imB,img_matches,img_matches_transposed;
						vector<KeyPoint> kpA,kpB;
						KeyPoint kp;
						kpA.clear();
						kpB.clear();
						for (unsigned int s=0;s<keypoints1.size();s++){
							kp = keypoints1[s];
							kp.pt.x = keypoints1[s].pt.y;
							kp.pt.y = keypoints1[s].pt.x;
							kpA.push_back(kp);
						}
						for (unsigned int s=0;s<keypoints2.size();s++){
							kp = keypoints2[s];
							kp.pt.x = keypoints2[s].pt.y;
							kp.pt.y = keypoints2[s].pt.x;
							kpB.push_back(kp);
						}
						cv::transpose(im[a], imA);
						cv::transpose(im[b], imB);
						namedWindow("matches", 1);
						Scalar color(0,0,255);
						if (kpA.size() >0 && kpB.size()>0 && inliers_matches.size() >0)
						{
							if (fabs(difference) <= 35) color = Scalar(0,255,0);
						}else{
							kpA.push_back(kp);
							kpB.push_back(kp);
						}
						drawMatches(imA, kpA, imB, kpB, inliers_matches, img_matches, color, color, vector<char>(), 2);
						cv::transpose(img_matches,img_matches_transposed);
						if (save){
							sprintf(filename,"%s/%09i-%02i-%02i-A.bmp",dataset,totalTests,a,b);
							char description[1000];
							sprintf(description,"Successes: %03i Failures: %03i",totalTests+1-numFails[numFeatures/100],numFails[numFeatures/100]);
							line(img_matches_transposed,cvPoint(15,20),cvPoint(358,20),Scalar(0,0,0),32,0);
							putText(img_matches_transposed,description,cvPoint(10,28), FONT_HERSHEY_SIMPLEX, 0.75,color,2.5);

							imwrite(filename,img_matches_transposed);
							imA = 0*imA;
							imB = 0*imB; 
							drawMatches(imA, kpA, imB, kpB, inliers_matches, img_matches, color, color, vector<char>(), 2);
							cv::transpose(img_matches,img_matches_transposed);
							sprintf(filename,"%s/%09i-%02i-%02i-B.bmp",dataset,totalTests,a,b);
							imwrite(filename,img_matches_transposed);
							//imshow("matches", img_matches_transposed);
							//waitKey(0);
						}else{
							imshow("matches", img_matches_transposed);
							waitKey(0);
						}
					}
					printf("Season %02i vs %02i, features %04i of %04i, location %03i of %03i \n",a,b,nFeatures,maxFeatures,ims+1,numLocations);
					totalTests++;
				}
			}
		}
	}
	if (update) fclose(output);
	int numPairs = numLocations*seasons*(seasons-1)/2;
	printf("%i %i\n",totalTests,numLocations*seasons*(seasons-1)/2);
	numFails[0] = numPairs;
	char report[100];
	sprintf(report,"%s/results/%s_%s.histogram",dataset,detectorName,descriptorName);
	FILE* summary = fopen(report,"w+");
	for (int n=0;n<=maxFeatures/100;n++){
	       	fprintf(summary,"%02i %.4f Detections: %i Times: %.3f %.3f %.3f Extracted: %i %i \n",n,100.0*numFails[n]/numPairs,numFeats[n]/numPairs,timeDetection/numPictures,timeDescription/totalExtracted*1000,timeMatching/totalMatched*1000000,totalExtracted/numPictures,totalMatched/totalTests);
	       	printf("%02i %.4f Detections: %i Times: %.3f %.3f %.3f Extracted: %i %i \n",n,100.0*numFails[n]/numPairs,numFeats[n]/numPairs,timeDetection/numPictures,timeDescription/totalExtracted*1000,timeMatching/totalMatched*1000000,totalExtracted/numPictures,totalMatched/totalTests);
	}
	fclose(summary);
	delete seq1;
	delete seq2;
	delete offsetX;
	delete offsetY;
	return 0;
}
