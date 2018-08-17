#include "CFrelement.h"
#include <ros/ros.h>


using namespace std;

int fremenSort(const void* i,const void* j)
{
	 if (((SFrelement*)i)->amplitude < ((SFrelement*)j)->amplitude) return +1;
	 return -1;
}

CFrelement::CFrelement(int idi)
{
	id = idi;
	//initialization of the frequency set
	storedGain = 0.0;
	predictGain = 0.5;
	order = 2;
	firstTime = -1;
	lastTime = -1;
	measurements = 0;
	type = TT_FREMEN;
}

CFrelement::CFrelement(string f_id)
{
	fid = f_id;
	//initialization of the frequency set
	storedGain = 0.0;
	predictGain = 0.5;
	order = 2;
	firstTime = -1;
	lastTime = -1;
	measurements = 0;
	type = TT_FREMEN;
}

void CFrelement::init(int iMaxPeriod,int elements,int numClasses)
{
	iMaxPeriod = 86400;
	maxPeriod = iMaxPeriod;
	numElements = 1;//maxPeriod/3600/6;
	numClasses = 0;
	storedFrelements = (SFrelement*)malloc(sizeof(SFrelement)*numElements);
	predictFrelements = (SFrelement*)malloc(sizeof(SFrelement)*numElements);
	for (int i=0;i<numElements;i++) storedFrelements[i].amplitude = storedFrelements[i].phase = 0;
	for (int i=0;i<numElements;i++) storedFrelements[i].period = (maxPeriod)/(i+1);
}

CFrelement::~CFrelement()
{
	free(storedFrelements);
	free(predictFrelements);
}

// adds new state observations at given times
int CFrelement::add(uint32_t time,float state)
{
	if (state < 0) state =0;
	if (measurements == 0)
	{
		for (int i = 0;i<numElements;i++){
			storedFrelements[i].realStates  = 0;
			storedFrelements[i].imagStates  = 0;
			storedFrelements[i].realBalance = 0;
			storedFrelements[i].imagBalance = 0;
		}
		firstTime = time;
	}else{
		if (time - lastTime < shortestTime) shortestTime = time-lastTime;
	}
	lastTime = time;

	//update the gains accordingly
	storedGain = (storedGain*measurements+state)/(measurements+1);

	float angle = 0;
	//recalculate the spectral components
	for (int i = 0;i<numElements;i++)
	{
		angle = 2*M_PI*(float)time/storedFrelements[i].period;
		storedFrelements[i].realStates   += state*cos(angle);
		storedFrelements[i].imagStates   += state*sin(angle);
		storedFrelements[i].realBalance  += cos(angle);
		storedFrelements[i].imagBalance  += sin(angle);
	}
	measurements++;

	return 0;
}

/*not required in incremental version*/
void CFrelement::update(int modelOrder,unsigned int* times,float* signal,int length)
{
	//establish amplitudes and phase shifts
	float re,im;
	//int duration = lastTime - firstTime;
	predictGain = storedGain;
	for (int i = 0;i<numElements;i++)
	{
		re = storedFrelements[i].realStates-storedGain*storedFrelements[i].realBalance;
		im = storedFrelements[i].imagStates-storedGain*storedFrelements[i].imagBalance;
		//if (1.5*storedFrelements[i].period <= duration && storedFrelements[i].period > shortestTime*2) storedFrelements[i].amplitude = sqrt(re*re+im*im)/measurements; else storedFrelements[i].amplitude = 0;
		storedFrelements[i].amplitude = sqrt(re*re+im*im)/measurements;
		if (storedFrelements[i].amplitude < FREMEN_AMPLITUDE_THRESHOLD) storedFrelements[i].amplitude = 0;
		storedFrelements[i].phase = atan2(im,re);
	}
	//sort the spectral component
	qsort(storedFrelements,numElements,sizeof(SFrelement),fremenSort);

	if (modelOrder > numElements) modelOrder = numElements;
	order = modelOrder;
	for (int i = 0;i<order;i++) predictFrelements[i] = storedFrelements[i];

	/*if given this info, the model tries to determine optimal model order to prevent overfit*/
	if (times != NULL && signal != NULL && length > 0){
		int bestOrder = 0;
		for (int o = 0;o<=modelOrder;o++){
			float error = 0;
			float minError = length*10;
			order = 0;
			for (int i = 0;i<length;i++)
			{
				order = o;
				error += fabs(estimate(times[i])-signal[i]);
			}
			if (error < minError)
			{
				minError = error;
				bestOrder = o;
			}
		}
		order = bestOrder;
	}
}

/*text representation of the fremen model*/
void CFrelement::print(bool verbose)
{
	std::cout << "Model: " << fid << " Prior: " << predictGain << " Size: " << measurements << " ";
	if (verbose){
		for (int i = 0;i<order;i++){
			std::cout << "Frelement " << i << " " << predictFrelements[i].amplitude << " " << predictFrelements[i].phase << " " << predictFrelements[i].period << " ";
			/*FILE *f;
			f = fopen("/home/eliska/fremen.txt", "a");
			fprintf(f, "%f\n", predictFrelements[i].period);
			fclose(f);*/
		}
	}
	std::cout << endl;
}
void CFrelement::setParam(float param)
{

}

float CFrelement::estimate(uint32_t time)
{
	float saturation = 0.01;
	float estimate =  predictGain;
	for (int i = 0;i<order;i++) estimate+=2*predictFrelements[i].amplitude*cos(time/predictFrelements[i].period*2*M_PI-predictFrelements[i].phase);
	if (estimate > 1.0-saturation) estimate =  1.0-saturation;
	if (estimate < 0.0+saturation) estimate =  0.0+saturation;
	return estimate;
}


float CFrelement::predict(uint32_t time)
{
	float saturation = 0.01;
	float estimate =  predictGain;
	for (int i = 0;i<order;i++) estimate+=2*predictFrelements[i].amplitude*cos(time/predictFrelements[i].period*2*M_PI-predictFrelements[i].phase);
	if (estimate > 1.0-saturation) estimate =  1.0-saturation;
	if (estimate < 0.0+saturation) estimate =  0.0+saturation;
	return estimate;
}

int CFrelement::save(const char* name,bool lossy)
{
	FILE* file = fopen(name,"w");
	double array[10000];
	int len = exportToArray(array,10000);
	fwrite(array,sizeof(double),len,file);
	fclose(file);
	return 0;
}

int CFrelement::importFromArray(double* array,int len)
{
	int pos = 0;
	type = (ETemporalType)array[pos++];
	if (type != TT_FREMEN) fprintf(stderr,"Error loading the model, type mismatch.\n");
	order = array[pos++];
	id = array[pos++];
	storedGain = array[pos++];
	predictGain = array[pos++];
	numElements = array[pos++];
	measurements = array[pos++];
	shortestTime = array[pos++];
	memcpy(&firstTime,&array[pos++],sizeof(double));
	memcpy(&lastTime,&array[pos++],sizeof(double));
	for (int i = 0;i<numElements;i++){
		storedFrelements[i].realStates = array[pos++];
		storedFrelements[i].imagStates = array[pos++];
		storedFrelements[i].realBalance = array[pos++];
		storedFrelements[i].imagBalance = array[pos++];
		storedFrelements[i].period = array[pos++];
	}
	update(order);
	return 0;
}

int CFrelement::exportToArray(double* array,int maxLen)
{
	int pos = 0;
	array[pos++] = type;
	array[pos++] = order;
	array[pos++] = id;
	array[pos++] = storedGain;
	array[pos++] = predictGain;
	array[pos++] = numElements;
	array[pos++] = measurements;
	array[pos++] = shortestTime;
	memcpy(&array[pos++],&firstTime,sizeof(double));
	memcpy(&array[pos++],&lastTime,sizeof(double));
	for (int i = 0;i<numElements;i++){
		array[pos++] = storedFrelements[i].realStates;
		array[pos++] = storedFrelements[i].imagStates;
		array[pos++] = storedFrelements[i].realBalance;
		array[pos++] = storedFrelements[i].imagBalance;
		array[pos++] = storedFrelements[i].period;
	}
	return pos;
}

int CFrelement::load(const char* name)
{
	FILE* file = fopen(name,"r");
	double *array = (double*)malloc(MAX_TEMPORAL_MODEL_SIZE*sizeof(double));
	int len = fread(array,sizeof(double),MAX_TEMPORAL_MODEL_SIZE,file);
	importFromArray(array,len);
	free(array);
	fclose(file);
	return 0;
}


int CFrelement::save(FILE* file,bool lossy)
{
	int frk = numElements;
	fwrite(&frk,sizeof(uint32_t),1,file);
	fwrite(&storedGain,sizeof(float),1,file);
	fwrite(storedFrelements,sizeof(SFrelement),numElements,file);
	return 0;
}

int CFrelement::load(FILE* file)
{
	int frk = numElements;
	fwrite(&frk,sizeof(uint32_t),1,file);
	fwrite(&storedGain,sizeof(float),1,file);
	fwrite(storedFrelements,sizeof(SFrelement),numElements,file);
	return 0;
}
