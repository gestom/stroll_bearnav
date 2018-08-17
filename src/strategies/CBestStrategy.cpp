#include "CBestStrategy.h"
#include <ros/ros.h>

using namespace std;
using namespace cv;


CBestStrategy::CBestStrategy(float n_init){
  n = (int)n_init;
  if(n<=0){
    n=100;
  }
}

CBestStrategy::CBestStrategy(){

}

CBestStrategy::~CBestStrategy(){

}

void CBestStrategy::filterFeatures(vector<KeyPoint> *keypoints, Mat *descriptors, vector<KeyPoint> *tmp, Mat *tmp_mat, vector<double> score){
  ROS_ERROR("BEST size %d n %d\n",tmp->size(), n);

  if(n>=tmp->size()){
    for (size_t i = 0; i < tmp->size(); i++) {
      keypoints->push_back(tmp->at(i));
      descriptors->push_back(tmp_mat->row(i));
    }
    return;
  }

  ftr_stc features_stcs[(int)tmp->size()];
  CStrategy::set_and_sort(features_stcs, score);
  //
  // Mat tmp_mat = descriptors->clone();
  // descriptors->release();

  // vector<KeyPoint> tmp(*keypoints);
  // keypoints->clear();
  int last = tmp->size()-1;
  for(int i = 0; i<n; i++){
      keypoints->push_back(tmp->at(features_stcs[last-i].index));
      descriptors->push_back(tmp_mat->row(features_stcs[last-i].index));
  }
}
