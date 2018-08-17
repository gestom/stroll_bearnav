#ifndef CQUANTILSTRATEGY_H
#define CQUANTILSTRATEGY_H


#include <vector>
#include "CStrategy.h"
#include <stdlib.h>

using namespace std;
using namespace cv;

class CQuantilStrategy: public CStrategy{
  public:
    CQuantilStrategy(float p_init);
    CQuantilStrategy();
		~CQuantilStrategy();

    void filterFeatures(vector<KeyPoint> *keypoints, Mat *descriptors, vector<KeyPoint> *tmp, Mat *tmp_mat, vector<double> score);

    double p = 0.5;
};

#endif
