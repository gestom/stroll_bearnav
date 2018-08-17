#ifndef CTEMPORAL_H
#define CTEMPORAL_H

#include <stdint.h>
#include <stdio.h>
#include <iostream>
#include <algorithm>
#include <math.h>
#include <string.h>
#include <stdint.h>

#define	MAX_TEMPORAL_MODEL_SIZE 10000
#define MAX_ID_LENGTH 100

using namespace std;

typedef enum{
	TT_SUM	= 0,
	TT_W_SUM,
	TT_MOV_AVG,
	TT_MEAN,
	TT_FREMEN,
	TT_NUMBER
}ETemporalType;

class CTemporal
{
	public:

		//adds a serie of measurements to the data
		virtual int add(uint32_t time,float state) = 0;

		//initialize
		virtual void init(int maxPeriod,int elements,int numActivities) = 0;

		//estimates the probability for the given times
		virtual float estimate(uint32_t time) = 0;
		virtual float predict(uint32_t time) = 0;

		virtual void update(int maxOrder,unsigned int* times = NULL,float* signal = NULL,int length = 0) = 0;
		virtual void print(bool verbose=true) = 0;
		virtual void setParam(float param)=0;

		virtual int exportToArray(double* array,int maxLen) = 0;
		virtual int importFromArray(double* array,int len) = 0;
		virtual int save(FILE* file,bool lossy = false) = 0;
		virtual int load(FILE* file) = 0;
		virtual int save(const char* name,bool lossy = false) = 0;
		virtual int load(const char* name) = 0;

		string fid;
		int measurements,order;
		int64_t firstTime;
		int64_t  lastTime;
		int numElements;
		int maxPeriod;
		ETemporalType type;

};

CTemporal* spawnTemporalModel(const char* type,int maxPeriod,int elements,int numClasses);
CTemporal* spawnTemporalModel(const char* type, string f_id, float param);
CTemporal* spawnTemporalModel(ETemporalType type,int maxPeriod,int elements,int numClasses);
CTemporal* spawnTemporalModel(ETemporalType type, string f_id, float param);
#endif
