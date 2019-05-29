#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <sys/time.h>
#include <assert.h>
#include <float.h>
#include "xmmintrin.h"

#define MINSNPS_B 5
#define MAXSNPS_E 20

double gettime(void);
float randpval (void);



double gettime(void)
{
	struct timeval ttime;
	gettimeofday(&ttime , NULL);
	return ttime.tv_sec + ttime.tv_usec * 0.000001;
}

float randpval (void)
{
	int vr = rand();
	int vm = rand()%vr;
	float r = ((float)vm)/(float)vr;
	assert(r>=0.0f && r<=1.00001f);
	return r;
}

float max(float a, float b) {
	if (a>b)
		return a;
	return b;
}

float min(float a, float b) {
	if (a<b)
		return a;
	return b;
}
	
float sum(float a,float b){
		return(a+b);
}

typedef struct{
		float mVec;
		float nVec;
		float LVec;
		float RVec;
		float CVec;
		float FVec;
} ness_data;	

	 int main(int argc, char ** argv)
	{
	 assert(argc==2);
	 double timeTotalMainStart = gettime();
	 float avgF = 0.0f;
	 float maxF = 0.0f;
	 float minF = FLT_MAX;
	 unsigned int N = (unsigned int)atoi(argv[1]);
	 unsigned int iters = 10;
	 srand(1);

	 //Should get better results
	 ness_data * all_data = (ness_data *)_mm_malloc(sizeof(ness_data)*N,16);

	 assert(all_data!=NULL);


/*
	 float * mVec = (float*)_mm_malloc(sizeof(float)*N,16);
	 assert(mVec!=NULL);
	 float * nVec = (float*)_mm_malloc(sizeof(float)*N,16);
	 assert(nVec!=NULL);
	 float * LVec = (float*)_mm_malloc(sizeof(float)*N,16);
	 assert(LVec!=NULL);
	 float * RVec = (float*)_mm_malloc(sizeof(float)*N,16);
	 assert(RVec!=NULL);
	 float * CVec = (float*)_mm_malloc(sizeof(float)*N,16);
	 assert(CVec!=NULL);
	 float * FVec = (float*)_mm_malloc(sizeof(float)*N,16);
	 assert(FVec!=NULL);
*/


	for(unsigned int i=0;i<N;i++)
	{
	all_data[i].mVec  = (float)(MINSNPS_B+rand()%MAXSNPS_E);
	all_data[i].nVec = (float)(MINSNPS_B+rand()%MAXSNPS_E);
	all_data[i].LVec = randpval()*all_data[i].mVec;
	all_data[i].RVec = randpval()*all_data[i].nVec;
	all_data[i].CVec = randpval()*all_data[i].mVec*all_data[i].nVec;
	all_data[i].FVec = 0.0;

	assert(all_data[i].mVec>=MINSNPS_B && all_data[i].mVec<=(MINSNPS_B+MAXSNPS_E));
	assert(all_data[i].nVec>=MINSNPS_B && all_data[i].nVec<=(MINSNPS_B+MAXSNPS_E));
	assert(all_data[i].LVec>=0.0f && all_data[i].LVec<=1.0f*all_data[i].mVec);
	assert(all_data[i].RVec>=0.0f && all_data[i].RVec<=1.0f*all_data[i].nVec);
	assert(all_data[i].CVec>=0.0f && all_data[i].CVec<=1.0f*all_data[i].mVec*all_data[i].nVec);
	}


	__m128 variable,variable1,variable2,variable3,variable4,variable5,variable6;


	__m128 scale1 = _mm_set_ps1(0.01f);
	__m128 scale2 = _mm_set_ps1(1.0f);
	__m128 scale3 = _mm_set_ps1(2.0f);

	__m128 maxg;
	__m128 ming;
	__m128 sumg;

	double timeOmegaTotalStart = gettime();
	for(unsigned int j=0;j<iters;j++)
	{

		maxg= _mm_set_ps(0.0f,0.0f,0.0f,0.0f);
		ming= _mm_set_ps(FLT_MAX,FLT_MAX,FLT_MAX,FLT_MAX);
		sumg= _mm_set_ps(0.0f,0.0f,0.0f,0.0f);

		for(unsigned int i=0;i<N; i+=4)//check this later for any changes!!!!!!!!!!!!!!!!!!!!!!!!!!
		{

			__m128 LVecss= _mm_set_ps(all_data[i+3].LVec, all_data[i+2].LVec, all_data[i+1].LVec, all_data[i].LVec);
			__m128 RVecss= _mm_set_ps(all_data[i+3].RVec, all_data[i+2].RVec, all_data[i+1].RVec, all_data[i].RVec);
			__m128 mVecss= _mm_set_ps(all_data[i+3].mVec, all_data[i+2].mVec, all_data[i+1].mVec, all_data[i].mVec);
			__m128 nVecss= _mm_set_ps(all_data[i+3].nVec, all_data[i+2].nVec, all_data[i+1].nVec, all_data[i].nVec);
			__m128 CVecss= _mm_set_ps(all_data[i+3].CVec, all_data[i+2].CVec, all_data[i+1].CVec, all_data[i].CVec);
			__m128 FVecss= _mm_set_ps(all_data[i+3].FVec, all_data[i+2].FVec, all_data[i+1].FVec, all_data[i].FVec);

			variable= _mm_add_ps(LVecss, RVecss);//!
			variable1= _mm_div_ps( _mm_mul_ps(mVecss, _mm_sub_ps(mVecss,scale2))  ,  scale3);//!
			variable2= _mm_div_ps( _mm_mul_ps(nVecss, _mm_sub_ps(nVecss,scale2))  ,  scale3);//!
			variable3= _mm_div_ps(variable,_mm_add_ps(variable1,variable2));//!
			variable4=_mm_sub_ps(CVecss,_mm_sub_ps(LVecss,RVecss));//!
			variable5=_mm_mul_ps(mVecss,nVecss);//!
			variable6=_mm_div_ps(variable4,variable5);//!

			FVecss = _mm_div_ps(variable3, _mm_add_ps(variable6, scale1));//!

			maxg= _mm_max_ps(FVecss,maxg);
			ming= _mm_min_ps(FVecss,ming);
			sumg= _mm_add_ps(FVecss,sumg);
			
		}
	}

	double timeOmegaTotal = gettime()-timeOmegaTotalStart;
	double timeTotalMainStop = gettime();

	float maxl[4];
	float minl[4];
	float suml[4];

	_mm_store_ps(maxl, maxg);
	_mm_store_ps(minl, ming);
	_mm_store_ps(suml, sumg);

	maxF = maxl[0];
    maxF = maxl[1] > maxF ? maxl[1] : maxF;
    maxF = maxl[2] > maxF ? maxl[2] : maxF;
    maxF = maxl[3] > maxF ? maxl[3] : maxF;

    minF = minl[0];
    minF = minl[1] < minF ? minl[1] : minF;
    minF = minl[2] < minF ? minl[2] : minF;
    minF = minl[3] < minF ? minl[3] : minF;

    avgF = suml[0] + suml[1] + suml[2] + suml[3];

/*
	maxF = max(max(max(maxl[0], maxl[1]), maxl[2]), maxl[3]);
	minF = min(min(min(minl[0], minl[1]), minl[2]), minl[3]);
	avgF = sum(sum(sum(suml[0], suml[1]), suml[2]), suml[3]);
*/

	printf("Omega time %fs - Total time %fs - Min %e - Max %e - Avg %e\n",
	timeOmegaTotal/iters, timeTotalMainStop-timeTotalMainStart, (double)minF, (double)maxF,(double)avgF/N);
	_mm_free(all_data);
	
}