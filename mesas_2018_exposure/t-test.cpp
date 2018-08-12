#include <stdlib.h>
#include <iostream>
#include <stdio.h>
#include <libalglib/statistics.h>

using namespace alglib;


int maina(int argc, char **argv)
{
	printf("%.3f\n",1.0-alglib::studenttdistribution(atoi(argv[1]),atoi(argv[2])));
	return 0;
}

float a,b;
double xA[1000];
int numData = 0;

int main(int argc, char **argv)
{
	float thr = 0.05;
	if (argc > 1) thr = atof(argv[1]);
	while(feof(stdin)==0){
		fscanf(stdin,"%f %f\n",&a,&b);
		xA[numData++]=a-b;
	}
	double both,left,right;
	real_1d_array x;
        x.setcontent(numData,xA);	
	alglib::studentttest1(x,numData,0,both,left,right);
	//printf("%.3lf %.3lf %.3lf\n",left,right,both);
	if (right < thr) printf("First value is higher (p(higher)=%.3f)\n",1-right);
	else if (left < thr) printf("First value is smaller (p(smaller)=%.3f)\n",1-left);
	else printf("First value is similar (p(similar)=%.3f,%.3f,%.3f)\n",both,right,left);
	return 0;
}
