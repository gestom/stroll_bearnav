#include "CMovAvg.h"

CMovAvg::CMovAvg(int idd,float param)
{
	tau=(unsigned int) param;
	id = idd;
	order = 0;
	firstTime = -1;
	lastTime = -1;
	max_dif = -1;
	measurements = 0;
	type = TT_MOV_AVG;
}

CMovAvg::CMovAvg(string f_id,float param){
  tau=(int) param;
	if(tau<=0){
		tau = 60*60*24;
	}
	fid = f_id;
	order = 0;
	firstTime = -1;
	lastTime = -1;
	max_dif = -1;
	measurements = 0;
	type = TT_MOV_AVG;
}

void CMovAvg::init(int iMaxPeriod,int elements,int numClasses)
{
	string line;
	ifstream f(fname);
	max_dif = 1800;
	score = 0.0;
}

CMovAvg::~CMovAvg()
{
}

// adds new state observations at given times
int CMovAvg::add(uint32_t time_in,float state)
{
	times.push_back(time_in);
	states.push_back(state);


}

void CMovAvg::update(int modelOrder,unsigned int* times,float* signal,int length)
{

}

/*text representation of the fremen model*/
void CMovAvg::print(bool verbose)
{
}

void CMovAvg::setParam(float param)
{
	tau = param;
}

float CMovAvg::estimate(uint32_t time)
{
	if(time-lastTime>max_dif || lastTime == -1){
		score = 0.0;
		double W = 0.0;
		double E = 0.0;

		for (size_t i = 0; i < times.size(); i++) {
			uint32_t t1 = times[i];
			double wi = exp(-1*(((double)time-(double)t1)/(double)tau));
			int state = states[i];
			W += wi;
			E += wi*(double)state;
		}
		if(W==0.0){
			score = 0.0;
		}else{
			score = E/W;
		}
	}
	return score;
}

float CMovAvg::predict(uint32_t time)
{
	if(time-lastTime>max_dif || lastTime == -1){
		score = 0.0;
		double W = 0.0;
		double E = 0.0;

		for (size_t i = 0; i < times.size(); i++) {
			uint32_t t1 = times[i];
			double wi = exp(-1*(((double)time-(double)t1)/(double)tau));
			int state = states[i];
			W += wi;
			E += wi*(double)state;
		}
		if(W==0.0){
			score = 0.0;
		}else{
			score = E/W;
		}
	}
	return score;
}
int CMovAvg::save(const char* name,bool lossy)
{
	FILE* file = fopen(name,"w");
	save(file);
	fclose(file);
	return 0;
}

int CMovAvg::load(const char* name)
{
	FILE* file = fopen(name,"r");
	load(file);
	fclose(file);
	return 0;
}


int CMovAvg::save(FILE* file,bool lossy)
{
	double array[10000];
	int len = exportToArray(array,10000);
	fwrite(array,sizeof(double),len,file);
	return 0;
}

int CMovAvg::load(FILE* file)
{
	double *array = (double*)malloc(MAX_TEMPORAL_MODEL_SIZE*sizeof(double));
	int len = fread(array,sizeof(double),MAX_TEMPORAL_MODEL_SIZE,file);
	importFromArray(array,len);
	free(array);
	return 0;
}


int CMovAvg::exportToArray(double* array,int maxLen)
{
	int pos = 0;
	array[pos++] = type;
	array[pos++] = score;
	array[pos++] = measurements;
	return pos;
}

int CMovAvg::importFromArray(double* array,int len)
{
	int pos = 0;
	type = (ETemporalType)array[pos++];
	if (type != TT_SUM) fprintf(stderr,"Error loading the model, type mismatch.\n");
	score = array[pos++];
	measurements = array[pos++];
	update(0);
	return pos;

}
