#include <ros/ros.h>
#include "CMonteCarloStrategy.h"

using namespace std;


CMonteCarloStrategy::CMonteCarloStrategy(float n_init){
  n = (int)n_init;
  if(n<=0){
    n=100;
  }
}

CMonteCarloStrategy::CMonteCarloStrategy(){

}

CMonteCarloStrategy::~CMonteCarloStrategy(){

}

double find_min(vector<double> stcs, int size){
  double min = stcs[0];
  for(int i = 1;i<size;++i){
    if(min>stcs[i]){
      min = stcs[i];
    }
  }
  return min+1;
}

void CMonteCarloStrategy::filterFeatures(vector<KeyPoint> *keypoints, Mat *descriptors, vector<KeyPoint> *tmp, Mat *tmp_mat, vector<double> score){
  int size=score.size();
  if (size==0){
    return;
  }if(n>=size){
    for(int i = 0; i<size;++i){
      keypoints->push_back(tmp->at(i));
      descriptors->push_back(tmp_mat->row(i));
    }
  }


  double min = find_min(score,size);
  double add = 0.0;
  if(min<0.0){
    add = - min;
  }if(min==0){
    add = 0.001;
  }
  int count = 0;
  double all = 0.0;
  double mnt_crl[size];
  for (int i = 0; i<size;++i){
    all += add + score[i];

    mnt_crl[i] = all;
    score[i] = 0.0;
  }

  srand (0);
  int more_picked = 0;

  for (int i = 0; i<n;++i){
    double r_next = 0.0 + double((all*rand())/(RAND_MAX + 1.0) );
    bool picked = false;
    double diff = 0.0;
    for(int j = 0; j<size ;j++){
      if(mnt_crl[j]>r_next){

        if(picked){
          // i--;
          mnt_crl[j]-= diff;

        }else{
          picked = true;
          if(j>0){
            diff = mnt_crl[j]-mnt_crl[j-1];
          }else{
            diff = mnt_crl[j];
          }
          all-=diff;
          keypoints->push_back(tmp->at(j));
          descriptors->push_back(tmp_mat->row(j));
        }
      }
    }
  }

}
