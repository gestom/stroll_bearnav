#include "CWSum.h"

CWSum::CWSum(int idd)
{
	id = idd;
	order = 0;
	firstTime = -1;
	lastTime = -1;
	measurements = 0;
	type = TT_W_SUM;
}

CWSum::CWSum(string f_id, float param){
	score = 0.0;
	w = param;
	fid = f_id;
	order = 0;
	firstTime = -1;
	lastTime = -1;
	measurements = 0;
	type = TT_W_SUM;
}

void CWSum::init(int iMaxPeriod,int elements,int numClasses)
{
	score = 0.0;

}

CWSum::~CWSum()
{
}

// adds new state observations at given times
int CWSum::add(uint32_t time,float state)
{
	int s = (int)state;
	if(s<0){
		score += w*s;
	}else{
		score += s;
	}
	lastTime = time;
	measurements++;
}

void CWSum::update(int modelOrder,unsigned int* times,float* signal,int length)
{

}

/*text representation of the fremen model*/
void CWSum::print(bool verbose)
{
}

void CWSum::setParam(float param)
{

}

float CWSum::estimate(uint32_t time)
{
	return score;
}

float CWSum::predict(uint32_t time)
{
	return score;
}
int CWSum::save(const char* name,bool lossy)
{
	FILE* file = fopen(name,"w");
	save(file);
	fclose(file);
	return 0;
}

int CWSum::load(const char* name)
{
	FILE* file = fopen(name,"r");
	load(file);
	fclose(file);
	return 0;
}


int CWSum::save(FILE* file,bool lossy)
{
	double array[10000];
	int len = exportToArray(array,10000);
	fwrite(array,sizeof(double),len,file);
	return 0;
}

int CWSum::load(FILE* file)
{
	double *array = (double*)malloc(MAX_TEMPORAL_MODEL_SIZE*sizeof(double));
	int len = fread(array,sizeof(double),MAX_TEMPORAL_MODEL_SIZE,file);
	importFromArray(array,len);
	free(array);
	return 0;
}


int CWSum::exportToArray(double* array,int maxLen)
{
	int pos = 0;
	array[pos++] = type;
	array[pos++] = score;
	array[pos++] = measurements;
	return pos;
}

int CWSum::importFromArray(double* array,int len)
{
	int pos = 0;
	type = (ETemporalType)array[pos++];
	if (type != TT_SUM) fprintf(stderr,"Error loading the model, type mismatch.\n");
	score = array[pos++];
	measurements = array[pos++];
	update(0);
	return pos;

}
