#include "CQuantilStrategy.h"
#include <iostream>
#include <ros/ros.h>

using namespace std;
using namespace cv;


CQuantilStrategy::CQuantilStrategy(float p_init){
  p = p_init;
  if(p<0 || p>1){
    p=0.5;
  }
}

CQuantilStrategy::CQuantilStrategy(){

}

CQuantilStrategy::~CQuantilStrategy(){

}

void CQuantilStrategy::filterFeatures(vector<KeyPoint> *keypoints, Mat *descriptors, vector<KeyPoint> *tmp, Mat *tmp_mat, vector<double> score){
  int size = tmp->size();
  ROS_ERROR("QUANTILE size %d",size);


  ftr_stc features_stcs[size];
  CStrategy::set_and_sort(features_stcs, score);
  double index = (size*p);
  double zero = 0.0;
  double dif = index - (int)index;
  double k = 0.0;
  //possibility to change to fmod((double)keypoints.size(),p)==0.0
  if(fmod((double)score.size(),p)==0.0){
    k = (features_stcs[(int)index].stc+features_stcs[(int)index+1].stc)/2;
  }else{
    k = features_stcs[(int)index].stc;
  }
  for(int i = 0 ;i<size;i++){
    if(score[i]>=k){
      keypoints->push_back(tmp->at(i));
      descriptors->push_back(tmp_mat->row(i));
    }
  }
}
