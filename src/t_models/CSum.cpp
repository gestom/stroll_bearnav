#include "CSum.h"

CSum::CSum(int idd)
{
	id = idd;
	order = 0;
	firstTime = -1;
	lastTime = -1;
	measurements = 0;
	type = TT_SUM;
}

CSum::CSum(string f_id){
	score = 0.0;

	fid = f_id;
	order = 0;
	firstTime = -1;
	lastTime = -1;
	measurements = 0;
	type = TT_SUM;
}

void CSum::init(int iMaxPeriod,int elements,int numClasses)
{

	score = 0.0;
}

CSum::~CSum()
{
}

// adds new state observations at given times
int CSum::add(uint32_t time,float state)
{
	score += (int)state;
	lastTime = time;
	measurements++;
	return 0;
}

void CSum::update(int modelOrder,unsigned int* times,float* signal,int length)
{

}

/*text representation of the fremen model*/
void CSum::print(bool verbose)
{
}

void CSum::setParam(float param)
{

}

float CSum::estimate(uint32_t time)
{
	return score;
}

float CSum::predict(uint32_t time)
{
	return score;
}
int CSum::save(const char* name,bool lossy)
{
	FILE* file = fopen(name,"w");
	save(file);
	fclose(file);
	return 0;
}

int CSum::load(const char* name)
{
	FILE* file = fopen(name,"r");
	load(file);
	fclose(file);
	return 0;
}


int CSum::save(FILE* file,bool lossy)
{
	double array[10000];
	int len = exportToArray(array,10000);
	fwrite(array,sizeof(double),len,file);
	return 0;
}

int CSum::load(FILE* file)
{
	double *array = (double*)malloc(MAX_TEMPORAL_MODEL_SIZE*sizeof(double));
	int len = fread(array,sizeof(double),MAX_TEMPORAL_MODEL_SIZE,file);
	importFromArray(array,len);
	free(array);
	return 0;
}


int CSum::exportToArray(double* array,int maxLen)
{
	int pos = 0;
	array[pos++] = type;
	array[pos++] = score;
	array[pos++] = measurements;
	return pos;
}

int CSum::importFromArray(double* array,int len)
{
	int pos = 0;
	type = (ETemporalType)array[pos++];
	if (type != TT_SUM) fprintf(stderr,"Error loading the model, type mismatch.\n");
	score = array[pos++];
	measurements = array[pos++];
	update(0);
	return pos;

}
