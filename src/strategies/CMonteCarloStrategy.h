#ifndef CMONTECARLOSTRATEGY_H
#define CMONTECARLOSTRATEGY_H

#include <vector>
#include "CStrategy.h"
#include <stdlib.h>

using namespace std;

class CMonteCarloStrategy: public CStrategy{
  public:
    CMonteCarloStrategy(float n_init);
    CMonteCarloStrategy();
		~CMonteCarloStrategy();

    void filterFeatures(vector<KeyPoint> *keypoints, Mat *descriptors, vector<KeyPoint> *tmp, Mat *tmp_mat, vector<double> score);

    int n = 100;
};

#endif
