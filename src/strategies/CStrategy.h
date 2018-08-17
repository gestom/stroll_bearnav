#ifndef CSTRATEGY_H
#define CSTRATEGY_H

#include <opencv2/features2d.hpp>
using namespace cv;
using namespace std;
#include <vector>


struct ftr_stc{
  int index;
  double stc;
};

typedef enum{
	TT_BEST	= 0,
	TT_MONTE_CARLO,
	TT_QUANTIL
}EStrategyType;

class CStrategy
{
  public:
    virtual void filterFeatures(vector<KeyPoint> *keypoints, Mat *descriptors, vector<KeyPoint> *keypoints_out, Mat *descriptors_out, vector<double> score) = 0;

    void set_and_sort(ftr_stc* ftr_stcs, vector<double> score);

};


CStrategy* spawnStrategy(const char* type, float param);
CStrategy* spawnStrategy(EStrategyType type,float param);

#endif
