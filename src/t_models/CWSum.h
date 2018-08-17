#ifndef CW_SUM_H
#define CW_SUM_H

#include <stdint.h>
#include <stdio.h>
#include <iostream>
#include <algorithm>
#include <math.h>
#include <stdint.h>
#include <string>
#include <vector>
#include <fstream>
#include <sstream>
#include <stdlib.h>
#include "CTemporal.h"

#define	MAX_TEMPORAL_MODEL_SIZE 10000
#define MAX_ID_LENGTH 100

using namespace std;


class CWSum: public CTemporal
{
	public:

		CWSum(int idd);
    CWSum(string f_id, float param);
    ~CWSum();

    //adds a serie of measurements to the data
    int add(uint32_t time,float state);

    //initialize
    void init(int maxPeriod,int elements,int numActivities);

    //estimates the probability for the given times
    float estimate(uint32_t time);
    float predict(uint32_t time);

    void update(int maxOrder,unsigned int* times = NULL,float* signal = NULL,int length = 0);
    void print(bool verbose=true);
		void setParam(float param);

    int exportToArray(double* array,int maxLen);
    int importFromArray(double* array,int len);
    int save(FILE* file,bool lossy = false);
    int load(FILE* file);
    int save(const char* name,bool lossy = false);
    int load(const char* name);
    void prepare(char* fname);

		int id;
		double score;
		float w;

};

#endif
