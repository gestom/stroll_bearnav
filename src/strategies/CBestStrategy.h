#ifndef CBESTSTRATEGY_H
#define CBESTSTRATEGY_H


#include <vector>
#include "CStrategy.h"
#include <stdlib.h>

using namespace std;
using namespace cv;

class CBestStrategy: public CStrategy{
  public:
    CBestStrategy(float n_init);
    CBestStrategy();
		~CBestStrategy();

    void filterFeatures(vector<KeyPoint> *keypoints, Mat *descriptors, vector<KeyPoint> *keypoints_out, Mat *descriptors_out, vector<double> score);

    int n = 100;
};

#endif
